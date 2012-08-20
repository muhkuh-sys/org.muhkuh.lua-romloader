/***************************************************************************
 *   Copyright (C) 2010 by Christoph Thelen                                *
 *   doc_bacardi@users.sourceforge.net                                     *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/


#include "usb.h"

#include "netx_io_areas.h"
#include "options.h"

#include "monitor.h"

/*-----------------------------------*/


#define true (1==1)
#define false (1==0)

#define ARRAYSIZE(a) (sizeof(a)/sizeof((a)[0]))

static unsigned char aucPacketRx[MONITOR_USB_MAX_PACKET_SIZE];

/*-----------------------------------*/


typedef enum
{
	USB_FIFO_MODE_Stream       = 0,
	USB_FIFO_MODE_StreamZLP    = 1,
	USB_FIFO_MODE_Packet       = 2,
	USB_FIFO_MODE_Transaction  = 3
} USB_FIFO_MODE_T;


/*-----------------------------------*/


static void enum_write_config_data(void)
{
	const unsigned long *pulCnt;
	const unsigned long *pulEnd;
	volatile unsigned long *pulDst;


	/* Get a pointer to the start and end of the enumeration data. */
	pulCnt = g_t_options.t_usb_settings.uCfg.aul;
	pulEnd = g_t_options.t_usb_settings.uCfg.aul + (sizeof(USB_CONFIGURATION_T)/sizeof(unsigned long));

	/* Get a pointer to the enumeration ram. */
	pulDst = (volatile unsigned long*)HOSTADDR(usb_dev_enum_ram);

	/* Copy the data. */
	while( pulCnt<pulEnd )
	{
		*(pulDst++) = *(pulCnt++);
	}
}


/*-----------------------------------*/


void usb_deinit(void)
{
#if 0
	HOSTDEF(ptUsbDevCtrlArea);


	/* Reset the USB core. */
	ptUsbDevCtrlArea->ulUsb_dev_cfg = HOSTMSK(usb_dev_cfg_usb_dev_reset);
	/* Release reset and disable the core with JTAG. */
	ptUsbDevCtrlArea->ulUsb_dev_cfg = 0;
#endif
}



void usb_init(void)
{
	HOSTDEF(ptUsbDevCtrlArea);
	unsigned long ulValue;


	usb_reset_fifo();

	/* Set disconnect timeout to max. */
	ulValue  = HOSTMSK(usb_dev_cfg_usb_core_enable);
	ulValue |= 3 << HOSTSRT(usb_dev_cfg_disconn_timeout);
	ptUsbDevCtrlArea->ulUsb_dev_cfg = ulValue;

	/* Disable all IRQs. */
	ptUsbDevCtrlArea->ulUsb_dev_irq_mask = 0;
	/* Acknowledge any pending IRQs. */
	ptUsbDevCtrlArea->ulUsb_dev_irq_raw = 0xffffffffU;

	enum_write_config_data();
}



void usb_reset_fifo(void)
{
	HOSTDEF(ptUsbDevFifoCtrlArea);
	unsigned long ulValue;


	/* Reset all FIFOs. */
	ptUsbDevFifoCtrlArea->ulUsb_dev_fifo_ctrl_conf = HOSTMSK(usb_dev_fifo_ctrl_conf_reset);

	/* Set the FIFO modes and release the reset. */
	ulValue  = USB_FIFO_MODE_Transaction << HOSTSRT(usb_dev_fifo_ctrl_conf_mode_interrupt);
	ulValue |= USB_FIFO_MODE_StreamZLP   << HOSTSRT(usb_dev_fifo_ctrl_conf_mode_uart_rx);
	ulValue |= USB_FIFO_MODE_StreamZLP   << HOSTSRT(usb_dev_fifo_ctrl_conf_mode_uart_tx);
	ulValue |= USB_FIFO_MODE_Packet      << HOSTSRT(usb_dev_fifo_ctrl_conf_mode_jtag_rx);
	ulValue |= USB_FIFO_MODE_Packet      << HOSTSRT(usb_dev_fifo_ctrl_conf_mode_jtag_tx);
	ptUsbDevFifoCtrlArea->ulUsb_dev_fifo_ctrl_conf = ulValue;
}



void usb_loop(void)
{
	HOSTDEF(ptUsbDevCtrlArea);
	unsigned long ulValue;
	unsigned long ulFillLevel;
	unsigned char *pucCnt;
	unsigned char *pucEnd;


	/* Wait for a new packet. */
	ulValue  = ptUsbDevCtrlArea->ulUsb_dev_irq_raw;
	ulValue &= HOSTMSK(usb_dev_irq_raw_jtag_rx_packet_received);
	if( ulValue!=0 )
	{
		/* Acknowledge the IRQ. */
		ptUsbDevCtrlArea->ulUsb_dev_irq_raw = HOSTMSK(usb_dev_irq_raw_jtag_rx_packet_received);

		/* Get the UART RX input fill level. */
		ulFillLevel = usb_get_rx_fill_level();
		if( ulFillLevel>0 )
		{
			/* Is the fill level valid? */
			if( ulFillLevel>MONITOR_USB_MAX_PACKET_SIZE )
			{
				/* Reset the FIFO. */
				usb_reset_fifo();
			}
			else
			{
				/* Copy the complete packet to the buffer. */
				pucCnt = aucPacketRx;
				pucEnd = aucPacketRx + ulFillLevel;
				do
				{
					*(pucCnt++) = usb_get_byte();
				} while( pucCnt<pucEnd );

				monitor_process_packet(aucPacketRx, ulFillLevel, MONITOR_USB_MAX_PACKET_SIZE);
			}
		}
	}
}



void usb_send_packet(const unsigned char *pucPacket, size_t sizPacket)
{
	HOSTDEF(ptUsbDevCtrlArea);
	HOSTDEF(ptUsbDevFifoArea);
	HOSTDEF(ptUsbDevFifoCtrlArea);
	const unsigned char *pucCnt;
	const unsigned char *pucEnd;
	unsigned long ulValue;


	if( sizPacket>0 )
	{
		/* Wait until any running transfers are finished. */
		do
		{
			ulValue  = ptUsbDevFifoCtrlArea->ulUsb_dev_fifo_ctrl_jtag_ep_tx_stat;
			ulValue &= HOSTMSK(usb_dev_fifo_ctrl_jtag_ep_tx_stat_transaction_active);
		} while( ulValue!=0 );

		/* Fill the FIFO with the packet data. */
		pucCnt = pucPacket;
		pucEnd = pucCnt + sizPacket;
		while( pucCnt<pucEnd )
		{
			ptUsbDevFifoArea->ulUsb_dev_jtag_tx_data = *(pucCnt++);
		}

		/*
		 * Start a new transfer.
		 */
		/* Set the packet length. */
		ulValue  = sizPacket << HOSTSRT(usb_dev_fifo_ctrl_jtag_ep_tx_len_transaction_len);
		/* Do not send ZLPs. This connection is using a custom driver. */
		ulValue |= HOSTMSK(usb_dev_fifo_ctrl_jtag_ep_tx_len_transaction_no_zlp);
		ptUsbDevFifoCtrlArea->ulUsb_dev_fifo_ctrl_jtag_ep_tx_len = ulValue;

		/* Wait until the packet is sent. */
		do
		{
			ulValue  = ptUsbDevCtrlArea->ulUsb_dev_irq_raw;
			ulValue &= HOSTMSK(usb_dev_irq_raw_jtag_tx_packet_sent);
		} while( ulValue==0 );
		/* Acknowledge the IRQ. */
		ptUsbDevCtrlArea->ulUsb_dev_irq_raw = HOSTMSK(usb_dev_irq_raw_jtag_tx_packet_sent);
	}
}



unsigned long usb_get_rx_fill_level(void)
{
	HOSTDEF(ptUsbDevFifoCtrlArea);
	unsigned long ulFillLevel;


	ulFillLevel   = ptUsbDevFifoCtrlArea->ulUsb_dev_fifo_ctrl_jtag_ep_rx_len;
	ulFillLevel  &= HOSTMSK(usb_dev_fifo_ctrl_jtag_ep_rx_len_packet_len);
	ulFillLevel >>= HOSTSRT(usb_dev_fifo_ctrl_jtag_ep_rx_len_packet_len);
	return ulFillLevel;
}



unsigned long usb_get_tx_fill_level(void)
{
	HOSTDEF(ptUsbDevFifoCtrlArea);
	unsigned long ulFillLevel;


	ulFillLevel   = ptUsbDevFifoCtrlArea->ulUsb_dev_fifo_ctrl_jtag_ep_tx_len;
	ulFillLevel  &= HOSTMSK(usb_dev_fifo_ctrl_jtag_ep_tx_len_packet_len);
	ulFillLevel >>= HOSTSRT(usb_dev_fifo_ctrl_jtag_ep_tx_len_packet_len);
	return ulFillLevel;
}



unsigned char usb_get_byte(void)
{
	HOSTDEF(ptUsbDevFifoArea);
	unsigned char ucData;


	/* Get a byte from the FIFO. */
	ucData = (unsigned char)(ptUsbDevFifoArea->ulUsb_dev_jtag_rx_data);
	return ucData;
}


/*-----------------------------------*/

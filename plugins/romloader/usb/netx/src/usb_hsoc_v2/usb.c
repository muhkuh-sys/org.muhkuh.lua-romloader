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

#if ASIC_TYP==ASIC_TYP_NETX56
#      define USB_FIFO_DEPTH_JTag_TX 64
#endif


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
	ulValue |= USB_FIFO_MODE_Transaction << HOSTSRT(usb_dev_fifo_ctrl_conf_mode_jtag_rx);
	ulValue |= USB_FIFO_MODE_Transaction << HOSTSRT(usb_dev_fifo_ctrl_conf_mode_jtag_tx);
	ptUsbDevFifoCtrlArea->ulUsb_dev_fifo_ctrl_conf = ulValue;
}



void usb_loop(void)
{
	HOSTDEF(ptUsbDevFifoArea);
	HOSTDEF(ptUsbDevFifoCtrlArea);
	unsigned long ulValue;
	unsigned long ulFillLevel;
	unsigned long ulReceivedData;
	unsigned char *pucCnt;
	unsigned char *pucEnd;
	unsigned long ulTransactionSize;


	/* Receive a new transaction. */
	ulReceivedData = 0;
	pucCnt = aucPacketRx;

	/* Expect the maximum transaction size. */
	ulTransactionSize = 0x2000;

	do
	{
		/* Get the RX input fill level. */
		ulFillLevel   = ptUsbDevFifoCtrlArea->ulUsb_dev_fifo_ctrl_jtag_ep_rx_stat;
		ulFillLevel  &= HOSTMSK(usb_dev_fifo_ctrl_jtag_ep_rx_stat_fill_level);
		ulFillLevel >>= HOSTSRT(usb_dev_fifo_ctrl_jtag_ep_rx_stat_fill_level);
		if( ulFillLevel>0 )
		{
			/* Get the new size of the transaction. */
			ulReceivedData += ulFillLevel;

			/* Is the updated size of the transaction still smaller than the buffer size? */
			if( ulReceivedData>MONITOR_USB_MAX_PACKET_SIZE )
			{
				/* No, the transaction overflows the buffer. */
				break;
			}
			else
			{
				/* Copy the complete packet to the buffer. */
				pucEnd = pucCnt + ulFillLevel;
				do
				{
					*(pucCnt++) = (unsigned char)(ptUsbDevFifoArea->ulUsb_dev_jtag_rx_data);
				} while( pucCnt<pucEnd );
			}
		}

		/* Is the transaction finished? */
		ulValue   = ptUsbDevFifoCtrlArea->ulUsb_dev_fifo_ctrl_jtag_ep_rx_len;
		ulValue  &= HOSTMSK(usb_dev_fifo_ctrl_jtag_ep_rx_len_transaction_len);
		ulValue >>= HOSTSRT(usb_dev_fifo_ctrl_jtag_ep_rx_len_transaction_len);
		if( ulValue!=0 )
		{
			ulTransactionSize = ulValue;
		}
	} while( ulReceivedData<ulTransactionSize );

	if( ulReceivedData<ulTransactionSize )
	{
		/* TODO: Discard the transaction. */
		while(1) {};
	}
	else
	{
		monitor_process_packet(aucPacketRx, ulTransactionSize, MONITOR_USB_MAX_PACKET_SIZE);
	}
}



void usb_send_packet(const unsigned char *pucPacket, size_t sizPacket)
{
	HOSTDEF(ptUsbDevFifoArea);
	HOSTDEF(ptUsbDevFifoCtrlArea);
	const unsigned char *pucCnt;
	const unsigned char *pucEnd;
	unsigned long ulValue;
	size_t sizLeft;
	size_t sizChunk;
	
	
	if( sizPacket>0 )
	{
		/* Set the packet length. */
		ulValue  = sizPacket << HOSTSRT(usb_dev_fifo_ctrl_jtag_ep_tx_len_transaction_len);
		ptUsbDevFifoCtrlArea->ulUsb_dev_fifo_ctrl_jtag_ep_tx_len = ulValue;

		sizLeft = sizPacket;
		pucCnt = pucPacket;
		while( sizLeft!=0 )
		{
			/* Wait for free bytes in the FIFO. */
			do
			{
				ulValue   = ptUsbDevFifoCtrlArea->ulUsb_dev_fifo_ctrl_jtag_ep_tx_stat;
				ulValue  &= HOSTMSK(usb_dev_fifo_ctrl_jtag_ep_tx_stat_fill_level);
				ulValue >>= HOSTSRT(usb_dev_fifo_ctrl_jtag_ep_tx_stat_fill_level);
				sizChunk = (size_t)(USB_FIFO_DEPTH_JTag_TX-ulValue);
			} while( sizChunk==0 );

			if( sizChunk>sizLeft )
			{
				sizChunk = sizLeft;
			}

			/* Write data to the FIFO. */
			pucEnd = pucCnt + sizChunk;
			do
			{
				ptUsbDevFifoArea->ulUsb_dev_jtag_tx_data = *(pucCnt++);
			} while( pucCnt<pucEnd );

			sizLeft -= sizChunk;
		}

		/* Wait until the transaction is finished. */
		do
		{
			ulValue  = ptUsbDevFifoCtrlArea->ulUsb_dev_fifo_ctrl_jtag_ep_tx_stat;
			ulValue &= HOSTMSK(usb_dev_fifo_ctrl_jtag_ep_tx_stat_transaction_active);
		} while( ulValue!=0 );
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

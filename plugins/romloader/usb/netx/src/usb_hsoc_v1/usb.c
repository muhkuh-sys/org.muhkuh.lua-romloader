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
	USB_FIFO_MODE_Packet            = 0,
	USB_FIFO_MODE_Streaming         = 1
} USB_FIFO_MODE_T;

typedef enum
{
	USB_FIFO_ACK_Manual             = 0,
	USB_FIFO_ACK_Auto               = 1
} USB_FIFO_ACK_T;

typedef enum
{
	USB_FIFO_Control_Out            = 0,
	USB_FIFO_Control_In             = 1,
	USB_FIFO_JTag_TX                = 2,
	USB_FIFO_JTag_RX                = 3,
	USB_FIFO_Uart_TX                = 4,
	USB_FIFO_Uart_RX                = 5,
	USB_FIFO_Interrupt_In           = 6
} USB_FIFO_T;


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
	HOSTDEF(ptUsbDevFifoCtrlArea);
	unsigned long ulValue;


#if 0
	/* Reset the USB core. */
	ptUsbDevCtrlArea->ulUsb_dev_cfg = HOSTMSK(usb_dev_cfg_usb_dev_reset);
	/* Release reset and enable the core with JTAG. */
	ptUsbDevCtrlArea->ulUsb_dev_cfg = HOSTMSK(usb_dev_cfg_usb_core_enable) | HOSTMSK(usb_dev_cfg_usb_to_jtag_enable);
#endif
	/* Set IRQ mask to 0. */
	ptUsbDevCtrlArea->ulUsb_dev_irq_mask = 0;

	/* ACK all IRQs. */
	ptUsbDevCtrlArea->ulUsb_dev_irq_raw = 0xffffffffU;

	/* Reset all FIFOs. */
	usb_reset_fifo();

	ptUsbDevFifoCtrlArea->ulUsb_dev_fifo_ctrl_out_handshake = HOSTMSK(usb_dev_fifo_ctrl_out_handshake_ack);
	ptUsbDevFifoCtrlArea->ulUsb_dev_fifo_ctrl_out_handshake = 0;

	/* Set the FIFO modes.
	   NOTE: This differs from the default configuration for the UART_TX FIFO.
	         It is set to packet mode here. */
	ulValue  = USB_FIFO_MODE_Packet    << (HOSTSRT(usb_dev_fifo_ctrl_conf_mode)+USB_FIFO_Control_Out);
	ulValue |= USB_FIFO_MODE_Streaming << (HOSTSRT(usb_dev_fifo_ctrl_conf_mode)+USB_FIFO_Control_In);
	ulValue |= USB_FIFO_MODE_Streaming << (HOSTSRT(usb_dev_fifo_ctrl_conf_mode)+USB_FIFO_JTag_TX);
	ulValue |= USB_FIFO_MODE_Packet    << (HOSTSRT(usb_dev_fifo_ctrl_conf_mode)+USB_FIFO_JTag_RX);
	ulValue |= USB_FIFO_MODE_Packet    << (HOSTSRT(usb_dev_fifo_ctrl_conf_mode)+USB_FIFO_Uart_TX);
	ulValue |= USB_FIFO_MODE_Packet    << (HOSTSRT(usb_dev_fifo_ctrl_conf_mode)+USB_FIFO_Uart_RX);
	ulValue |= USB_FIFO_MODE_Streaming << (HOSTSRT(usb_dev_fifo_ctrl_conf_mode)+USB_FIFO_Interrupt_In);
	/* Set the auto ACK modes.
	   NOTE: This differs from the default configuration for the UART_TX FIFO.
	         It is set to manual mode here. */
	ulValue |= USB_FIFO_ACK_Auto   << (HOSTSRT(usb_dev_fifo_ctrl_conf_auto_out_ack)+USB_FIFO_Control_Out);
	ulValue |= USB_FIFO_ACK_Manual << (HOSTSRT(usb_dev_fifo_ctrl_conf_auto_out_ack)+USB_FIFO_Control_In);
	ulValue |= USB_FIFO_ACK_Manual << (HOSTSRT(usb_dev_fifo_ctrl_conf_auto_out_ack)+USB_FIFO_JTag_TX);
	ulValue |= USB_FIFO_ACK_Auto   << (HOSTSRT(usb_dev_fifo_ctrl_conf_auto_out_ack)+USB_FIFO_JTag_RX);
	ulValue |= USB_FIFO_ACK_Manual << (HOSTSRT(usb_dev_fifo_ctrl_conf_auto_out_ack)+USB_FIFO_Uart_TX);
	ulValue |= USB_FIFO_ACK_Auto   << (HOSTSRT(usb_dev_fifo_ctrl_conf_auto_out_ack)+USB_FIFO_Uart_RX);
	ulValue |= USB_FIFO_ACK_Manual << (HOSTSRT(usb_dev_fifo_ctrl_conf_auto_out_ack)+USB_FIFO_Interrupt_In);
	ptUsbDevFifoCtrlArea->ulUsb_dev_fifo_ctrl_conf = ulValue;

	enum_write_config_data();
}



void usb_reset_fifo(void)
{
	HOSTDEF(ptUsbDevFifoCtrlArea);


	ptUsbDevFifoCtrlArea->ulUsb_dev_fifo_ctrl_conf = DFLT_VAL_NX10_usb_dev_fifo_ctrl_conf_auto_out_ack | DFLT_VAL_NX10_usb_dev_fifo_ctrl_conf_mode | HOSTMSK(usb_dev_fifo_ctrl_conf_reset);
	ptUsbDevFifoCtrlArea->ulUsb_dev_fifo_ctrl_conf = DFLT_VAL_NX10_usb_dev_fifo_ctrl_conf_auto_out_ack | DFLT_VAL_NX10_usb_dev_fifo_ctrl_conf_mode;
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
	ulValue &= HOSTMSK(usb_dev_irq_raw_uart_rx_packet_received);
	if( ulValue!=0 )
	{
		/* Acknowledge the IRQ. */
		ptUsbDevCtrlArea->ulUsb_dev_irq_raw = HOSTMSK(usb_dev_irq_raw_uart_rx_packet_received);

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
	unsigned long ulRawIrq;


	pucCnt = pucPacket;
	pucEnd = pucCnt + sizPacket;
	while( pucCnt<pucEnd )
	{
		ptUsbDevFifoArea->aulUsb_dev_fifo[USB_FIFO_Uart_TX] = *(pucCnt++);
	}

	/* Trigger packet send. */
	ptUsbDevFifoCtrlArea->ulUsb_dev_fifo_ctrl_in_handshake = 1<<USB_FIFO_Uart_TX;
	ptUsbDevFifoCtrlArea->ulUsb_dev_fifo_ctrl_in_handshake = 0;

	/* Wait until the FIFO is empty. */
	/* NOTE: without this, the transfer fails under very rare conditions.
	         I have no idea if this check is enough and the irq_raw condition can
	         be skipped, so I choose the safe side and keep both.
	*/
	do
	{
		ulRawIrq  = ptUsbDevFifoCtrlArea->aulUsb_dev_fifo_ctrl_status[USB_FIFO_Uart_TX];
		ulRawIrq &= HOSTMSK(usb_dev_fifo_ctrl_status4_in_fill_level)|HOSTMSK(usb_dev_fifo_ctrl_status4_out_fill_level);
	} while( ulRawIrq!=0 );

	/* Wait until the packet is sent. */
	do
	{
		ulRawIrq  = ptUsbDevCtrlArea->ulUsb_dev_irq_raw;
		ulRawIrq &= HOSTMSK(usb_dev_irq_raw_uart_tx_packet_sent);
	} while( ulRawIrq==0 );

	/* Acknowledge the IRQ. */
	ptUsbDevCtrlArea->ulUsb_dev_irq_raw = HOSTMSK(usb_dev_irq_raw_uart_tx_packet_sent);
}



unsigned long usb_get_rx_fill_level(void)
{
	HOSTDEF(ptUsbDevFifoCtrlArea);
	unsigned long ulFillLevel;


	ulFillLevel   = ptUsbDevFifoCtrlArea->aulUsb_dev_fifo_ctrl_status[USB_FIFO_Uart_RX];
	ulFillLevel  &= HOSTMSK(usb_dev_fifo_ctrl_status0_out_fill_level);
	ulFillLevel >>= HOSTSRT(usb_dev_fifo_ctrl_status0_out_fill_level);
	return ulFillLevel;
}



unsigned long usb_get_tx_fill_level(void)
{
	HOSTDEF(ptUsbDevFifoCtrlArea);
	unsigned long ulFillLevel;


	ulFillLevel   = ptUsbDevFifoCtrlArea->aulUsb_dev_fifo_ctrl_status[USB_FIFO_Uart_TX];
	ulFillLevel  &= HOSTMSK(usb_dev_fifo_ctrl_status0_in_fill_level);
	ulFillLevel >>= HOSTSRT(usb_dev_fifo_ctrl_status0_in_fill_level);
	return ulFillLevel;
}



unsigned char usb_get_byte(void)
{
	HOSTDEF(ptUsvDevFifoArea);
	unsigned char ucData;


	/* Get a byte from the FIFO. */
	ucData = (unsigned char)ptUsvDevFifoArea->aulUsb_dev_fifo[USB_FIFO_Uart_RX];
	return ucData;
}


/*-----------------------------------*/

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

#include "usbmonitor_commands.h"
#include "usbmon.h"

/*-----------------------------------*/


#define true (1==1)
#define false (1==0)

#define ARRAYSIZE(a) (sizeof(a)/sizeof((a)[0]))


/*-----------------------------------*/


typedef enum
{
	USB_FIFO_MODE_Packet		= 0,
	USB_FIFO_MODE_Streaming		= 1
} USB_FIFO_MODE_T;

typedef enum
{
	USB_FIFO_ACK_Manual		= 0,
	USB_FIFO_ACK_Auto		= 1
} USB_FIFO_ACK_T;

typedef enum
{
	USB_FIFO_Control_Out		= 0,
	USB_FIFO_Control_In		= 1,
	USB_FIFO_JTag_TX		= 2,
	USB_FIFO_JTag_RX		= 3,
	USB_FIFO_Uart_TX		= 4,
	USB_FIFO_Uart_RX		= 5,
	USB_FIFO_Interrupt_In		= 6
} USB_FIFO_T;


/*-----------------------------------*/


static unsigned char aucPacketRx[64];


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
	/* Reset the USB core. */
	ptUsbDevCtrlArea->ulUsb_dev_cfg = HOSTMSK(usb_dev_cfg_usb_dev_reset);
	/* Release reset and disable the core with JTAG. */
	ptUsbDevCtrlArea->ulUsb_dev_cfg = 0;
#endif
}


void usb_init(void)
{
	unsigned long ulValue;

#if 0
	/* Reset the USB core. */
	ptUsbDevCtrlArea->ulUsb_dev_cfg = HOSTMSK(usb_dev_cfg_usb_dev_reset);
	/* Release reset and enable the core with JTAG. */
	ptUsbDevCtrlArea->ulUsb_dev_cfg = HOSTMSK(usb_dev_cfg_usb_core_enable) | HOSTMSK(usb_dev_cfg_usb_to_jtag_enable);
#endif
	/* Set irq mask to 0. */
	ptUsbDevCtrlArea->ulUsb_dev_irq_mask = 0;

	/* Reset all FIFOs. */
	ptUsbDevFifoCtrlArea->ulUsb_dev_fifo_ctrl_conf = DFLT_VAL_NX10_usb_dev_fifo_ctrl_conf_auto_out_ack | DFLT_VAL_NX10_usb_dev_fifo_ctrl_conf_mode | HOSTMSK(usb_dev_fifo_ctrl_conf_reset);
	/* Release the reset. */
	ptUsbDevFifoCtrlArea->ulUsb_dev_fifo_ctrl_conf = DFLT_VAL_NX10_usb_dev_fifo_ctrl_conf_auto_out_ack | DFLT_VAL_NX10_usb_dev_fifo_ctrl_conf_mode;

	ptUsbDevFifoCtrlArea->ulUsb_dev_fifo_ctrl_out_handshake = HOSTMSK(usb_dev_fifo_ctrl_out_handshake_ack);
	ptUsbDevFifoCtrlArea->ulUsb_dev_fifo_ctrl_out_handshake = 0;

	/* Set the fifo modes.
	   NOTE: This differs from the default config for the UART_TX fifo.
	         It is set to packet mode here. */
	ulValue  = USB_FIFO_MODE_Packet    << (HOSTSRT(usb_dev_fifo_ctrl_conf_mode)+USB_FIFO_Control_Out);
	ulValue |= USB_FIFO_MODE_Streaming << (HOSTSRT(usb_dev_fifo_ctrl_conf_mode)+USB_FIFO_Control_In);
	ulValue |= USB_FIFO_MODE_Streaming << (HOSTSRT(usb_dev_fifo_ctrl_conf_mode)+USB_FIFO_JTag_TX);
	ulValue |= USB_FIFO_MODE_Packet    << (HOSTSRT(usb_dev_fifo_ctrl_conf_mode)+USB_FIFO_JTag_RX);
	ulValue |= USB_FIFO_MODE_Packet    << (HOSTSRT(usb_dev_fifo_ctrl_conf_mode)+USB_FIFO_Uart_TX);
	ulValue |= USB_FIFO_MODE_Packet    << (HOSTSRT(usb_dev_fifo_ctrl_conf_mode)+USB_FIFO_Uart_RX);
	ulValue |= USB_FIFO_MODE_Streaming << (HOSTSRT(usb_dev_fifo_ctrl_conf_mode)+USB_FIFO_Interrupt_In);
	/* Set the auto ack modes.
	   NOTE: This differs from the default config for the UART_TX fifo.
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


void usb_send_byte(unsigned char ucData)
{
	/* Write the status to the fifo. */
	ptUsvDevFifoArea->aulUsb_dev_fifo[USB_FIFO_Uart_TX] = ucData;
}


void usb_send_packet(void)
{
	unsigned long ulRawIrq;


	/* Trigger packet send. */
	ptUsbDevFifoCtrlArea->ulUsb_dev_fifo_ctrl_in_handshake = 1<<USB_FIFO_Uart_TX;
	ptUsbDevFifoCtrlArea->ulUsb_dev_fifo_ctrl_in_handshake = 0;

	/* Wait until the packet is sent. */
	do
	{
		ulRawIrq  = ptUsbDevCtrlArea->ulUsb_dev_irq_raw;
		ulRawIrq &= HOSTMSK(usb_dev_irq_raw_uart_tx_packet_sent);
	} while( ulRawIrq==0 );

	/* Acknowledge the irq. */
	ptUsbDevCtrlArea->ulUsb_dev_irq_raw = HOSTMSK(usb_dev_irq_raw_uart_tx_packet_sent);
}


unsigned long usb_get_rx_fill_level(void)
{
	unsigned long ulFillLevel;


	ulFillLevel   = ptUsbDevFifoCtrlArea->aulUsb_dev_fifo_ctrl_status[USB_FIFO_Uart_RX];
	ulFillLevel  &= HOSTMSK(usb_dev_fifo_ctrl_status0_out_fill_level);
	ulFillLevel >>= HOSTSRT(usb_dev_fifo_ctrl_status0_out_fill_level);
	return ulFillLevel;
}


unsigned long usb_get_tx_fill_level(void)
{
	unsigned long ulFillLevel;


	ulFillLevel   = ptUsbDevFifoCtrlArea->aulUsb_dev_fifo_ctrl_status[USB_FIFO_Uart_TX];
	ulFillLevel  &= HOSTMSK(usb_dev_fifo_ctrl_status0_in_fill_level);
	ulFillLevel >>= HOSTSRT(usb_dev_fifo_ctrl_status0_in_fill_level);
	return ulFillLevel;
}


unsigned char usb_get_byte(void)
{
	unsigned char ucData;


	/* Get a byte from the fifo. */
	ucData = (unsigned char)ptUsvDevFifoArea->aulUsb_dev_fifo[USB_FIFO_Uart_RX];
	return ucData;
}


/*-----------------------------------*/


unsigned char usb_call_console_get(unsigned int uiHandle __attribute__((unused)))
{
	unsigned long ulFillLevel;
	unsigned char ucData;


	/* Wait for a byte in the fifo. */
	do
	{
		ulFillLevel = usb_get_rx_fill_level();
	} while( ulFillLevel!=0 );

	/* Get a byte from the fifo. */
	ucData = usb_get_byte();
	return ucData;
}


void usb_call_console_put(unsigned int uiHandle __attribute__((unused)), unsigned int uiChar)
{
	unsigned long ulFillLevel;


	/* Add the byte to the fifo. */
	usb_send_byte((unsigned char)uiChar);

	/* Reached the maximum packet size? */
	ulFillLevel = usb_get_tx_fill_level();
	if( ulFillLevel>=sizeof(USBMON_PACKET_MESSAGE_T) )
	{
		/* Yes -> send the packet. */
		usb_send_packet();

		/* Start a new packet. */
		usb_send_byte(USBMON_STATUS_CallMessage);
	}
}


unsigned int usb_call_console_peek(unsigned int uiHandle __attribute__((unused)))
{
	unsigned long ulFillLevel;


	ulFillLevel = usb_get_rx_fill_level();
	return (ulFillLevel==0) ? 0U : 1U;
}


void usb_call_console_flush(unsigned int uiHandle __attribute__((unused)))
{
	/* Flush all waiting data. */
	usb_send_packet();

	/* Start the new message packet. */
	usb_send_byte(USBMON_STATUS_CallMessage);
}


/*-----------------------------------*/


void usb_loop(void)
{
	unsigned long ulFillLevel;
	unsigned char *pucCnt;
	unsigned char *pucEnd;


	/* start loopback */
	while(1)
	{
		/* wait for a character in the input fifo */

		/* get the Uart RX input fill level */
		ulFillLevel = usb_get_rx_fill_level();
		if( ulFillLevel>0 )
		{
			/* Is the fill level valid? */
			if( ulFillLevel>64 )
			{
				ulFillLevel = 64;
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

				usbmon_process_packet(aucPacketRx, ulFillLevel);
			}
		}
	}
}


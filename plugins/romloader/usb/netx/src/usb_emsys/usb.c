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

#include <stddef.h>

#include "netx_io_areas.h"
#include "options.h"

#include "usbmonitor_commands.h"

#include "usb_descriptors.h"
#include "usb_io.h"
#include "usb_main.h"

/*-----------------------------------*/


#define true (1==1)
#define false (1==0)

#define ARRAYSIZE(a) (sizeof(a)/sizeof((a)[0]))


/*-----------------------------------*/

static size_t sizSendData;
static unsigned char aucSendData[64];

/*-----------------------------------*/


void usb_deinit(void)
{
#if 0
	unsigned long ulCnt;


	for(ulCnt=0; ulCnt<4; ++ulCnt)
	{
		/* Select pipe. */
		ptUsbCoreArea->ulPIPE_SEL = ulCnt;
		/* Deactivate pipe. */
		ptUsbCoreArea->ulPIPE_CTRL = 0;
		/* Invalidate the data buffer. */
		ptUsbCoreArea->ulPIPE_DATA_TBYTES = 0;
	}

	/* Clear the termination. */
	ptUsbCoreArea->ulUsb_core_ctrl = 0;
#endif
}


void usb_init(void)
{
	/* NOTE: This is a kick-start. The core must start in running mode! */

	globalState = USB_State_Configured;

	/* Configuration 0 is set. */
	currentConfig = 0;
#if 0
	/* Soft reset. */
	ptUsbCoreArea->ulUsb_core_ctrl = 1;
	/* Release reset and set ID Function. */
	ptUsbCoreArea->ulUsb_core_ctrl = 8;

	/* Set ID pull-up and read connector ID value. */
	ptUsbCoreArea->ulPORT_CTRL = MSK_USB_PORT_CTRL_ID_PU;
	ptUsbCoreArea->ulPORT_CTRL = 0;
#endif
	sizSendData = 0;

	tOutTransactionNeeded = USB_SetupTransaction_NoOutTransaction;
	tReceiveEpState = USB_ReceiveEndpoint_Running;
	tSendEpState = USB_SendEndpoint_Idle;

	usb_descriptors_init();
}


void usb_send_byte(unsigned char ucData)
{
	/* Is still space left in the output buffer? */
	if( sizSendData<sizeof(aucSendData) )
	{
		/* Write the byte to the output packet. */
		aucSendData[sizSendData] = ucData;
		++sizSendData;
	}
}


void usb_send_packet(void)
{
	HOSTDEF(ptUsbCoreArea);
	unsigned long ulPipeEvent;


	/* Write the packet data to the FIFO. */
	usb_io_write_fifo(Usb_Ep1_Buffer>>2, sizSendData, aucSendData);
	/* Send the packet. */
	usb_io_sendDataPacket(1, sizSendData);

	/* Wait until the packet is sent. */
	do
	{
		ulPipeEvent = ptUsbCoreArea->ulPIPE_EV;
	} while( (ulPipeEvent&(1<<1))==0 );

	/* Clear the event. */
	ptUsbCoreArea->ulPIPE_EV = (1<<1);
#if 0
	/* Was the last packet a complete packet? */
	if( sizSendData==Usb_Ep1_PacketSize )
	{
		/* Yes -> send a 0 byte packet. */
		usb_io_sendDataPacket(1, 0);

		/* Wait until the packet is sent. */
		do
		{
			ulPipeEvent = ptUsbCoreArea->ulPIPE_EV;
		} while( (ulPipeEvent&(1<<1))==0 );

		/* Clear the event. */
		ptUsbCoreArea->ulPIPE_EV = (1<<1);
	}
#endif
	/* No more packet data waiting. */
	sizSendData = 0;
}


unsigned long usb_get_rx_fill_level(void)
{
	return 0;
}


unsigned long usb_get_tx_fill_level(void)
{
	return sizSendData;
}


#if 0
unsigned char usb_get_byte(void)
{
	unsigned char ucData;


	/* Get a byte from the FIFO. */
	ucData = (unsigned char)ptUsvDevFifoArea->aulUsb_dev_fifo[USB_FIFO_Uart_RX];
	return ucData;
}
#endif


/*-----------------------------------*/


unsigned char usb_call_console_get(void)
{
	return 0;
}


void usb_call_console_put(unsigned int uiChar)
{
	unsigned long ulFillLevel;


	/* Add the byte to the FIFO. */
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


unsigned int usb_call_console_peek(void)
{
	unsigned long ulFillLevel;


	ulFillLevel = usb_get_rx_fill_level();
	return (ulFillLevel==0) ? 0U : 1U;
}


void usb_call_console_flush(void)
{
	/* Flush all waiting data. */
	usb_send_packet();

	/* Start the new message packet. */
	usb_send_byte(USBMON_STATUS_CallMessage);
}


void usb_loop(void)
{
	HOSTDEF(ptUsbCoreArea);
	unsigned long ulPortEvent;


	while(1)
	{
		if( globalState==USB_State_Powered )
		{
			/* Set termination. */
			ptUsbCoreArea->ulPORT_CTRL = 0x180;
			/* Wait for reset event. */
			ulPortEvent = ptUsbCoreArea->ulMAIN_EV;
			if( (ulPortEvent&MSK_USB_MAIN_EV_GPORT_EV)!=0 )
			{
				usb_handleReset();
			}
		}
		else
		{
			/* Handle enumeration. */
			usb_pingpong();
		}
	}
}


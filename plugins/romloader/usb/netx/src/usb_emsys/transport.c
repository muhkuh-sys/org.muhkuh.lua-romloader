/***************************************************************************
 *   Copyright (C) 2012 by Christoph Thelen                                *
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


#include "transport.h"

#include "monitor.h"
#include "monitor_commands.h"
#include "netx_io_areas.h"

#include "usb.h"


static unsigned char aucPacketTx[MONITOR_USB_MAX_PACKET_SIZE];

static size_t sizPacketTxFill;
static size_t sizPacketTxFillLast;


void transport_init(void)
{
	monitor_init();

	sizPacketTxFill = 0;
	sizPacketTxFillLast = 0;

	usb_init();
}


void transport_loop(void)
{
	usb_loop();
}



void transport_send_packet(void)
{
	usb_send_packet(aucPacketTx, sizPacketTxFill);

	/* Remember the packet size for resends. */
	sizPacketTxFillLast = sizPacketTxFill;

	sizPacketTxFill = 0;
}



void transport_send_byte(unsigned char ucData)
{
	if( sizPacketTxFill<MONITOR_USB_MAX_PACKET_SIZE )
	{
		aucPacketTx[sizPacketTxFill] = ucData;
		++sizPacketTxFill;
	}
}



void transport_resend_packet(void)
{
	/* Restore the last packet size. */
	sizPacketTxFill = sizPacketTxFillLast;

	/* Send the buffer again. */
	transport_send_packet();
}



unsigned char transport_call_console_get(void)
{
	return 0;
}


void transport_call_console_put(unsigned int uiChar)
{
	/* Add the byte to the buffer. */
	if( sizPacketTxFill<MONITOR_USB_MAX_PACKET_SIZE )
	{
		aucPacketTx[sizPacketTxFill] = (unsigned char)uiChar;
		++sizPacketTxFill;
	}

	/* Reached the maximum packet size? */
	if( sizPacketTxFill>=MONITOR_USB_MAX_PACKET_SIZE )
	{
		transport_call_console_flush();
	}
}


unsigned int transport_call_console_peek(void)
{
	unsigned long ulFillLevel;


	ulFillLevel = usb_get_rx_fill_level();
	return (ulFillLevel==0) ? 0U : 1U;
}


void transport_call_console_flush(void)
{
	usb_send_packet(aucPacketTx, sizPacketTxFill);
	sizPacketTxFillLast = sizPacketTxFill;

	/* Start a new packet. */
	aucPacketTx[0] = MONITOR_STATUS_CallMessage;
	sizPacketTxFill = 1;
}

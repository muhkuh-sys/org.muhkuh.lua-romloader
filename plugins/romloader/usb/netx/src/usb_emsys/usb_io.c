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

#include "usb_io.h"
#include "usb_globals.h"
#include "netx_io_areas.h"


void usb_io_read_fifo(unsigned int uiDwOffset, unsigned int uiByteCount, unsigned char *pucBuffer)
{
	const unsigned char *pucSc;
	unsigned char *pucDc, *pucDe;


	pucSc = (const unsigned char*)(HOSTADR(USB_FIFO_BASE) + (uiDwOffset<<2));
	pucDc = pucBuffer;
	pucDe = pucDc + uiByteCount;

	while( pucDc<pucDe ) {
		*pucDc++ = *pucSc++;
	}
}


void usb_io_write_fifo(unsigned int uiDwOffset, unsigned int uiByteCount, const unsigned char *pucBuffer)
{
	/*
	  NOTE: the AHBL switch can not write bytes to registers or all other
	  bytes in the DWORD are set to the same value.
	*/
	const unsigned char *pucSc;
	unsigned long *pulDc, *pulDe;
	unsigned int uiByteCnt;
	unsigned long ulValue;


	/* Round up the number of bytes to a multiple of 32 bits. */
	uiByteCount += 3;

	pucSc = pucBuffer;
	pulDc = (unsigned long*)(HOSTADR(USB_FIFO_BASE) + (uiDwOffset<<2));
	pulDe = pulDc + (uiByteCount>>2);

	uiByteCnt = 0;
	ulValue = 0;
	while( pulDc<pulDe ) {
		ulValue >>= 8;
		ulValue  |= (*pucSc++)<<24;
		uiByteCnt++;
		if( (uiByteCnt&3)==0 ) {
			*pulDc++ = ulValue;
		}
	}
}


void usb_io_sendDataPacket(unsigned int uiPipeNr, unsigned int uiPacketSize)
{
	HOSTDEF(ptUsbCoreArea);


	ptUsbCoreArea->ulPIPE_SEL = uiPipeNr;
	ptUsbCoreArea->ulPIPE_CTRL = MSK_USB_PIPE_CTRL_ACT | DEF_USB_PIPE_CTRL_TPID_IN;
	ptUsbCoreArea->ulPIPE_DATA_PTR = uiPipeNr<<5U;
	ptUsbCoreArea->ulPIPE_DATA_TBYTES = MSK_USB_PIPE_DATA_TBYTES_DBV|uiPacketSize;
}

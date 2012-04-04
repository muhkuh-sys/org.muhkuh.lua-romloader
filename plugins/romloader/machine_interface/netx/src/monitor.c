/***************************************************************************
 *   Copyright (C) 2011 by Christoph Thelen                                *
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


#include "monitor.h"

#include <stddef.h>
#include <string.h>

#include "serial_vectors.h"
#include "monitor_commands.h"
#include "transport.h"


/*-----------------------------------*/


typedef union
{
	unsigned char *puc;
	unsigned short *pus;
	unsigned long *pul;
	unsigned long ul;
} ADR_T;


typedef void (*PFN_MONITOR_CALL_T)(unsigned long ulR0);


/*-----------------------------------*/


static void send_status(MONITOR_STATUS_T tStatus)
{
	/* Write the status to the fifo. */
	transport_send_byte(tStatus);

	/* Send the packet. */
	transport_send_packet();
}


static void command_read_memory(unsigned long ulAddress, unsigned long ulSize, MONITOR_ACCESSSIZE_T tAccessSize)
{
	ADR_T uAdrCnt;
	ADR_T uAdrEnd;
	unsigned long ulValue;
	int iCnt;


//	uprintf("read 0x%04x->0x%08x\n", ulSize, ulAddress);

	/* Get the start address. */
	uAdrCnt.ul = ulAddress;
	/* Get the end address. */
	uAdrEnd.ul = ulAddress + ulSize;

	/* Write status "Ok" to the fifo. */
	transport_send_byte(MONITOR_STATUS_Ok);

	/* Write data bytes to the fifo. */
	do
	{
		/* Get the next data element in the requested access width. */
		switch(tAccessSize)
		{
		case MONITOR_ACCESSSIZE_Byte:
			ulValue = *(uAdrCnt.puc++);
			break;

		case MONITOR_ACCESSSIZE_Word:
			ulValue = *(uAdrCnt.pus++);
			break;

		case MONITOR_ACCESSSIZE_Long:
			ulValue = *(uAdrCnt.pul++);
			break;
		}

		/* Add the data byte-by-byte to the fifo. */
		iCnt = 1<<tAccessSize;
		do
		{
			transport_send_byte((unsigned char)(ulValue & 0xffU));
			ulValue >>= 8;
			--iCnt;
		} while( iCnt>0 );
	} while( uAdrCnt.ul<uAdrEnd.ul);

	/* Send the packet. */
	transport_send_packet();
}


static void command_write_memory(const unsigned char *pucData, unsigned long ulAddress, unsigned long ulDataSize, MONITOR_ACCESSSIZE_T tAccessSize)
{
	const unsigned char *pucCnt;
	const unsigned char *pucEnd;
	ADR_T uAdrDst;
	unsigned long ulValue;


//	uprintf("write 0x%04x->0x%08x\n", ulDataSize, ulAddress);
//	hexdump(pucData, ulDataSize);

	/* Get the source start address. */
	pucCnt = pucData;
	/* Get the source end address. */
	pucEnd = pucData + ulDataSize;
	/* Get the destination end address. */
	uAdrDst.ul = ulAddress;

	/* Write data bytes to memory. */
	do
	{
		/* Get the next data element in the requested access width. */
		switch(tAccessSize)
		{
		case MONITOR_ACCESSSIZE_Byte:
			*(uAdrDst.puc++) = *(pucCnt++);
			break;

		case MONITOR_ACCESSSIZE_Word:
			ulValue  = *(pucCnt++);
			ulValue |= (unsigned long)(*(pucCnt++) << 8U);
			*(uAdrDst.pus++) = (unsigned short)ulValue;
			break;

		case MONITOR_ACCESSSIZE_Long:
			ulValue  = *(pucCnt++);
			ulValue |= (unsigned long)(*(pucCnt++) << 8U);
			ulValue |= (unsigned long)(*(pucCnt++) << 16U);
			ulValue |= (unsigned long)(*(pucCnt++) << 24U);
			*(uAdrDst.pul++) = ulValue;
			break;
		}
	} while( pucCnt<pucEnd);

	/* Send the status packet. */
	send_status(MONITOR_STATUS_Ok);
}



static void command_call(unsigned long ulAddress, unsigned long ulR0)
{
	PFN_MONITOR_CALL_T ptCall;


	ptCall = (PFN_MONITOR_CALL_T)ulAddress;

	/* Send the status packet. */
	send_status(MONITOR_STATUS_Ok);

	/* Start the new message packet. */
	transport_send_byte(MONITOR_STATUS_CallMessage);

	/* Call the routine. */
	ptCall(ulR0);

	/* Flush any remaining bytes in the fifo. */
	transport_send_packet();

	/* The call finished, notify the PC. */
	send_status(MONITOR_STATUS_CallFinished);
}


static unsigned long get_unaligned_dword(const unsigned char *pucBuffer)
{
	unsigned long ulValue;


	ulValue  = pucBuffer[0];
	ulValue |= (unsigned long)(pucBuffer[1]<<8U);
	ulValue |= (unsigned long)(pucBuffer[2]<<16U);
	ulValue |= (unsigned long)(pucBuffer[3]<<24U);

	return ulValue;
}


static const SERIAL_V2_COMM_UI_FN_T tCallConsole =
{
	.fn =
	{
		.fnGet = transport_call_console_get,
		.fnPut = transport_call_console_put,
		.fnPeek = transport_call_console_peek,
		.fnFlush = transport_call_console_flush
	}
};


void monitor_init(void)
{
	/* Set the vectors. */
	memcpy(&tSerialV2Vectors, &tCallConsole, sizeof(SERIAL_V2_COMM_UI_FN_T));
}


void monitor_process_packet(const unsigned char *pucPacket, unsigned long ulPacketSize)
{
	MONITOR_COMMAND_T tCmd;
	unsigned long ulDataSize;
	unsigned long ulAddress;
	MONITOR_ACCESSSIZE_T tAccessSize;
	unsigned long ulR0;


	/* Get the command and the data size from the first byte. */
	tCmd = (MONITOR_COMMAND_T)(pucPacket[0]&0x3fU);
	tAccessSize = (MONITOR_ACCESSSIZE_T)(pucPacket[0]>>6U);
	ulDataSize = pucPacket[1];


	if( tCmd==MONITOR_COMMAND_Execute )
	{
		/* Get the address. */
		ulAddress = get_unaligned_dword(pucPacket + 1);

		if( ulPacketSize!=9U )
		{
			send_status(MONITOR_STATUS_InvalidPacketSize);
		}
		else
		{
			ulR0 = get_unaligned_dword(pucPacket + 5);
			command_call(ulAddress, ulR0);
		}
	}
	else if( tCmd==MONITOR_COMMAND_Read )
	{
		/* Get the address. */
		ulAddress = get_unaligned_dword(pucPacket + 2);

		if( ulPacketSize!=6 )
		{
			send_status(MONITOR_STATUS_InvalidPacketSize);
		}
		else if( ulDataSize>MONITOR_MAX_PACKET_SIZE-6 )
		{
			send_status(MONITOR_STATUS_InvalidSizeParameter);
		}
		else
		{
			command_read_memory(ulAddress, ulDataSize, tAccessSize);
		}
	}
	else if( tCmd==MONITOR_COMMAND_Write )
	{
		/* Get the address. */
		ulAddress = get_unaligned_dword(pucPacket + 2);

		if( ulPacketSize!=(6+ulDataSize) )
		{
			send_status(MONITOR_STATUS_InvalidPacketSize);
		}
		else if( ulDataSize>MONITOR_MAX_PACKET_SIZE-6 )
		{
			send_status(MONITOR_STATUS_InvalidSizeParameter);
		}
		else
		{
			command_write_memory(pucPacket+6, ulAddress, ulDataSize, tAccessSize);
		}
	}
	else
	{
		send_status(MONITOR_STATUS_InvalidCommand);
	}
}


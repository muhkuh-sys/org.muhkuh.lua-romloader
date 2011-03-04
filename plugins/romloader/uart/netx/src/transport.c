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


#include "transport.h"

#include <stddef.h>

#include "monitor.h"
#include "monitor_commands.h"
#include "serial_vectors.h"
#include "systime.h"
#include "uart.h"

/* This is only for debug messages. */
#include "uprintf.h"



#define MAX_PACKET_SIZE 256

#define UART_BUFFER_NO_TIMEOUT 0
#define UART_BUFFER_TIMEOUT 1

unsigned char aucStreamBuffer[MAX_PACKET_SIZE];
size_t sizStreamBufferHead;
size_t sizStreamBufferFill;

unsigned char aucPacketInputBuffer[MAX_PACKET_SIZE];

unsigned char aucPacketOutputBuffer[MAX_PACKET_SIZE];
size_t sizPacketOutputFill;


static unsigned short crc16(unsigned short usCrc, unsigned char ucData)
{
	unsigned int uiCrc;


	uiCrc  = (usCrc >> 8U) | ((usCrc & 0xffU) << 8U);
	uiCrc ^= ucData;
	uiCrc ^= (uiCrc & 0xffU) >> 4U;
	uiCrc ^= (uiCrc & 0x0fU) << 12U;
	uiCrc ^= ((uiCrc & 0xffU) << 4U) << 1U;

	return (unsigned short)uiCrc;
}


void transport_init(void)
{
	/* Init the stream buffer. */
	sizStreamBufferHead = 0;
	sizStreamBufferFill = 0;

	sizPacketOutputFill = 0;
}


static int uart_buffer_fill(size_t sizRequestedFillLevel, unsigned int uiTimeoutFlag)
{
	size_t sizWritePosition;
	int iResult;
	unsigned long ulTimeout;
	unsigned int uiHasData;


	uprintf("uart_buffer_fill: sizRequestedFillLevel=%d, uiTimeoutFlag=%d\n", sizRequestedFillLevel, uiTimeoutFlag);

	uprintf("sizStreamBufferHead = %d\n", sizStreamBufferHead);
	uprintf("sizStreamBufferFill = %d\n", sizStreamBufferFill);

	/* Get the write position. */
	sizWritePosition = sizStreamBufferHead + sizStreamBufferFill;
	if( sizWritePosition>=MAX_PACKET_SIZE )
	{
		sizWritePosition -= MAX_PACKET_SIZE;
	}

	iResult = 0;

	if( uiTimeoutFlag==UART_BUFFER_NO_TIMEOUT )
	{
		/* Fillup the buffer to the requested level. */
		while( sizStreamBufferFill<sizRequestedFillLevel )
		{
			/* Write the new byte to the buffer. */
			aucStreamBuffer[sizWritePosition] = SERIAL_GET();

			uprintf("Buffer[0x%04x] = 0x%02x\n", sizWritePosition, aucStreamBuffer[sizWritePosition]);

			/* Increase the write position. */
			++sizWritePosition;
			if( sizWritePosition>=MAX_PACKET_SIZE )
			{
				sizWritePosition -= MAX_PACKET_SIZE;
			}
			/* Increase the fill level. */
			++sizStreamBufferFill;
		}
	}
	else
	{
		/* Fillup the buffer to the requested level. */
		while( sizStreamBufferFill<sizRequestedFillLevel )
		{
			/* Wait for new data. */
			ulTimeout = systime_get_ms();
			uiHasData = 0;
			do
			{
				if( systime_elapsed(ulTimeout, 500)!=0 )
				{
					break;
				}

				uiHasData = SERIAL_PEEK();
			} while( uiHasData==0 );

			if( uiHasData==0 )
			{
				iResult = -1;
				break;
			}

			/* Write the new byte to the buffer. */
			aucStreamBuffer[sizWritePosition] = SERIAL_GET();
			/* Increase the write position. */
			++sizWritePosition;
			if( sizWritePosition>=MAX_PACKET_SIZE )
			{
				sizWritePosition -= MAX_PACKET_SIZE;
			}
			/* Increase the fill level. */
			++sizStreamBufferFill;
		}
	}

	return iResult;
}


static unsigned char uart_buffer_get(void)
{
	unsigned char ucByte;


	ucByte = aucStreamBuffer[sizStreamBufferHead];

	uprintf("Get from %d = 0x%02x\n", sizStreamBufferHead, ucByte);

	++sizStreamBufferHead;
	if( sizStreamBufferHead>=MAX_PACKET_SIZE )
	{
		sizStreamBufferHead -= MAX_PACKET_SIZE;
	}

	--sizStreamBufferFill;

	return ucByte;
}


static unsigned char uart_buffer_peek(size_t sizOffset)
{
	size_t sizReadPosition;
	unsigned char ucByte;


	sizReadPosition = sizStreamBufferHead + sizOffset;
	if( sizReadPosition>=MAX_PACKET_SIZE )
	{
		sizReadPosition -= MAX_PACKET_SIZE;
	}

	ucByte = aucStreamBuffer[sizReadPosition];
	uprintf("peek at offset %d = 0x%02x\n", sizOffset, ucByte);

	return ucByte;
}


void transport_loop(void)
{
	unsigned char ucByte;
	size_t sizPacket;
	unsigned short usCrc16;
	size_t sizCrcPosition;
	int iResult;


	/* Collect a complete packet. */

	/* Wait for the start character. */
	do
	{
		uart_buffer_fill(1, UART_BUFFER_NO_TIMEOUT);
		ucByte = uart_buffer_get();
	} while( ucByte!=MONITOR_STREAM_PACKET_START );

	uprintf("Startchar\n");

	/* Get the size of the data packet in bytes. */
	uart_buffer_fill(2, UART_BUFFER_NO_TIMEOUT);
	sizPacket  = uart_buffer_peek(0);
	sizPacket |= (size_t)(uart_buffer_peek(1) << 8U);

	uprintf("Size: 0x%08x\n", sizPacket);

	/* Is the packet's size valid? */
	if( sizPacket>=4 && sizPacket<=MAX_PACKET_SIZE )
	{
		/* Yes, the packet size is valid. */

		iResult = uart_buffer_fill(sizPacket, UART_BUFFER_TIMEOUT);
		if( iResult==0 )
		{
			/* Loop over all bytes and build the crc16 checksum. */
			/* NOTE: the size information also contains the size itself and the checksum. */
			usCrc16 = 0;
			sizCrcPosition = 0;
			while( sizCrcPosition<sizPacket )
			{
				usCrc16 = crc16(usCrc16, uart_buffer_peek(sizCrcPosition));
				++sizCrcPosition;
			}

			if( usCrc16!=0 )
			{
				uprintf("crc failed: %04x\n", usCrc16);
			}
			else
			{
				/* Ok, the CRC matches! */

				/* TODO: process the packet. */

				uprintf("Received packet (%d bytes): ", sizPacket);
				sizCrcPosition = 0;
				while( sizCrcPosition<sizPacket )
				{
					aucPacketInputBuffer[sizCrcPosition] = uart_buffer_get();
					++sizCrcPosition;
				}

				monitor_process_packet(aucPacketInputBuffer, sizPacket);
			}
		}
	}
}












void transport_send_byte(unsigned char ucData)
{
	if( sizPacketOutputFill<MAX_PACKET_SIZE )
	{
		aucPacketOutputBuffer[sizPacketOutputFill] = ucData;
		++sizPacketOutputFill;
	}
}


void transport_send_packet(void)
{
	size_t sizPacket;
	const unsigned char *pucCnt;
	const unsigned char *pucEnd;
	unsigned char ucData;
	unsigned short usCrc;


	/* Get the packet size without the start character. */
	sizPacket  = sizPacketOutputFill;
	/* Add the size of the length field. */
	sizPacket += sizeof(unsigned short);
	/* Add the size of the CRC16 field. */
	sizPacket += sizeof(unsigned short);

	/* Send the start character. */
	uart_put(MONITOR_STREAM_PACKET_START);

	/* Send the size. */
	uart_put(sizPacket&0xffU);
	uart_put(sizPacket>>8U);

	/* Send the packet and build the CRC16. */
	pucCnt = aucPacketOutputBuffer;
	pucEnd = pucCnt + sizPacketOutputFill;
	usCrc = 0;
	while( pucCnt<pucEnd )
	{
		ucData = *(pucCnt++);
		uart_put(ucData);
		usCrc = crc16(usCrc, ucData);
	}

	/* Send the CRC16. */
	uart_put(usCrc&0xffU);
	uart_put(usCrc>>8U);

	sizPacketOutputFill = 0;
}


unsigned char transport_call_console_get(void)
{
	return 0;
}


void transport_call_console_put(unsigned int uiChar)
{

}

unsigned int transport_call_console_peek(void)
{
	return 0;
}


void transport_call_console_flush(void)
{
}


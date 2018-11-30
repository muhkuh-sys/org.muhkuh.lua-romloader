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
#include <string.h>

#include "monitor.h"
#include "monitor_commands.h"
#include "serial_vectors.h"
#include "systime.h"



#define UART_BUFFER_NO_TIMEOUT 0
#define UART_BUFFER_TIMEOUT 1

#define MONITOR_MAX_PACKET_SIZE_UART 2048

/* Resend a packet after 1000ms if no ACK was received. */
#define UART_RESEND_TIMEOUT_MS 1000


unsigned char aucStreamBuffer[MONITOR_MAX_PACKET_SIZE_UART];
size_t sizStreamBufferHead;
size_t sizStreamBufferFill;

//unsigned char aucPacketInputBuffer[MONITOR_MAX_PACKET_SIZE_UART];

unsigned char aucPacketOutputBuffer[MONITOR_MAX_PACKET_SIZE_UART];
size_t sizPacketOutputFill;
size_t sizPacketOutputFillLast;

static unsigned char ucSequence;


/* This is a very nice routine for the CITT XModem CRC from
 * http://www.eagleairaust.com.au/code/crc16.htm or now
 * https://web.archive.org/web/20130106081206/http://www.eagleairaust.com.au/code/crc16.htm
 */
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
	monitor_init();

	/* Initialize the stream buffer. */
	sizStreamBufferHead = 0;
	sizStreamBufferFill = 0;

	sizPacketOutputFill = 0;
	sizPacketOutputFillLast = 0;

	ucSequence = 0;
}



static int uart_buffer_fill(size_t sizRequestedFillLevel, unsigned int uiTimeoutFlag)
{
	size_t sizWritePosition;
	int iResult;
	unsigned long ulTimeout;
	unsigned int uiHasData;


//	uprintf("uart_buffer_fill: sizRequestedFillLevel=%d, uiTimeoutFlag=%d\n", sizRequestedFillLevel, uiTimeoutFlag);

//	uprintf("sizStreamBufferHead = %d\n", sizStreamBufferHead);
//	uprintf("sizStreamBufferFill = %d\n", sizStreamBufferFill);

	/* Get the write position. */
	sizWritePosition = sizStreamBufferHead + sizStreamBufferFill;
	if( sizWritePosition>=MONITOR_MAX_PACKET_SIZE_UART )
	{
		sizWritePosition -= MONITOR_MAX_PACKET_SIZE_UART;
	}

	iResult = 0;

	if( uiTimeoutFlag==UART_BUFFER_NO_TIMEOUT )
	{
		/* Fill-up the buffer to the requested level. */
		while( sizStreamBufferFill<sizRequestedFillLevel )
		{
			/* Write the new byte to the buffer. */
			aucStreamBuffer[sizWritePosition] = (unsigned char)(SERIAL_V1_GET());

//			uprintf("Buffer[0x%04x] = 0x%02x\n", sizWritePosition, aucStreamBuffer[sizWritePosition]);

			/* Increase the write position. */
			++sizWritePosition;
			if( sizWritePosition>=MONITOR_MAX_PACKET_SIZE_UART )
			{
				sizWritePosition -= MONITOR_MAX_PACKET_SIZE_UART;
			}
			/* Increase the fill level. */
			++sizStreamBufferFill;
		}
	}
	else
	{
		/* Fill-up the buffer to the requested level. */
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

				uiHasData = SERIAL_V1_PEEK();
			} while( uiHasData==0 );

			if( uiHasData==0 )
			{
				iResult = -1;
				break;
			}

			/* Write the new byte to the buffer. */
			aucStreamBuffer[sizWritePosition] = (unsigned char)(SERIAL_V1_GET());
			/* Increase the write position. */
			++sizWritePosition;
			if( sizWritePosition>=MONITOR_MAX_PACKET_SIZE_UART )
			{
				sizWritePosition -= MONITOR_MAX_PACKET_SIZE_UART;
			}
			/* Increase the fill level. */
			++sizStreamBufferFill;
		}
	}

	return iResult;
}



static void transport_uart_send_ack(void)
{
	unsigned int uiCnt;
	unsigned short usCrc16;
	unsigned char aucACK[7];


	aucACK[0] = MONITOR_STREAM_PACKET_START;
	aucACK[1] = 2U;  /* The low byte of the data size. */
	aucACK[2] = 0U;  /* The high byte of the data size. */
	aucACK[3] = ucSequence;
	aucACK[4] = MONITOR_PACKET_TYP_ACK;

	/* Generate the CRC for the packet. */
	usCrc16 = 0;
	for(uiCnt=1; uiCnt<5; ++uiCnt)
	{
		usCrc16 = crc16(usCrc16, aucACK[uiCnt]);
	}
	aucACK[5] = (unsigned char)(usCrc16 >> 8U);
	aucACK[6] = (unsigned char)(usCrc16 & 0xff);

	for(uiCnt=0; uiCnt<sizeof(aucACK); ++uiCnt)
	{
		SERIAL_V1_PUT(aucACK[uiCnt]);
	}

	/* Wait until the ACK is out. */
	SERIAL_V1_FLUSH();
}



unsigned char transport_buffer_get(void)
{
	unsigned char ucByte;


	ucByte = aucStreamBuffer[sizStreamBufferHead];

//	uprintf("Get from %d = 0x%02x\n", sizStreamBufferHead, ucByte);

	++sizStreamBufferHead;
	if( sizStreamBufferHead>=MONITOR_MAX_PACKET_SIZE_UART )
	{
		sizStreamBufferHead -= MONITOR_MAX_PACKET_SIZE_UART;
	}

	--sizStreamBufferFill;

	return ucByte;
}


unsigned char transport_buffer_peek(unsigned int sizOffset)
{
	unsigned int sizReadPosition;
	unsigned char ucByte;


	sizReadPosition = sizStreamBufferHead + sizOffset;
	if( sizReadPosition>=MONITOR_MAX_PACKET_SIZE_UART )
	{
		sizReadPosition -= MONITOR_MAX_PACKET_SIZE_UART;
	}

	ucByte = aucStreamBuffer[sizReadPosition];
//	uprintf("peek at offset %d = 0x%02x\n", sizOffset, ucByte);

	return ucByte;
}


void transport_buffer_skip(unsigned int sizSkip)
{
	sizStreamBufferHead += sizSkip;
	if( sizStreamBufferHead>=MONITOR_MAX_PACKET_SIZE_UART )
	{
		sizStreamBufferHead -= MONITOR_MAX_PACKET_SIZE_UART;
	}

	sizStreamBufferFill -= sizSkip;
}



typedef enum ACK_RESULT_ENUM
{
	ACK_RESULT_SequenceAckd       =  0,
	ACK_RESULT_OperationCanceled  = -1,
	ACK_RESULT_Timeout            = -2
} ACK_RESULT_T;


static ACK_RESULT_T transport_uart_wait_for_ack(unsigned char ucRequiredSequenceNumber, unsigned long ulTimeoutMs)
{
	ACK_RESULT_T tResult;
	int iResult;
	unsigned long ulTimerHandle;
	unsigned char ucByte;
	unsigned int sizPacket;
	unsigned short usCrc;
	unsigned int sizCrcPosition;
	MONITOR_PACKET_TYP_T tPacketTyp;
	unsigned char ucSequenceNumber;
	int iIsElapsed;


	tResult = ACK_RESULT_Timeout;
//	ptRingBufferReceive = &(ptDevice->tRingBufferReceive.tRingBuffer);
//	sizRingbufferReceiveTotal = ptRingBufferReceive->sizTotal;

	ulTimerHandle = systime_get_ms();
	do
	{
		/* Wait for the start character. */
		iResult = uart_buffer_fill(1, UART_BUFFER_TIMEOUT);
		if( iResult==0 )
		{
			ucByte = transport_buffer_get();
			if( ucByte==MONITOR_STREAM_PACKET_START )
			{
				/* Wait for the size. */
				iResult = uart_buffer_fill(2, UART_BUFFER_TIMEOUT);
				if( iResult==0 )
				{
					sizPacket  = transport_buffer_peek(0);
					sizPacket |= (unsigned int)(transport_buffer_peek(1) << 8U);
					if( sizPacket!=0 && sizPacket<=MONITOR_MAX_PACKET_SIZE_UART-4)
					{
						/* Get the size, data and CRC16.
						 * The complete data has this structure:
						 *   2 bytes for the size of the packet
						 *   sizPacket bytes of data
						 *   2 bytes for the CRC16
						 */
						iResult = uart_buffer_fill(2+sizPacket+2, UART_BUFFER_TIMEOUT);
						if( iResult==0 )
						{
							/* Loop over all bytes and build the CRC16 checksum. */
							usCrc = 0;
							sizCrcPosition = 0;
							while( sizCrcPosition<sizPacket+4 )
							{
								ucByte = transport_buffer_peek(sizCrcPosition);
								usCrc = crc16(usCrc, ucByte);
								++sizCrcPosition;
							}

							if( usCrc==0 )
							{
								/* OK, the CRC matches! */

								/* Get the packet type. */
								tPacketTyp = (MONITOR_PACKET_TYP_T)transport_buffer_peek(3);

								/* Is this an ACK packet with the correct sequence number? */
								if( sizPacket==2 && tPacketTyp==MONITOR_PACKET_TYP_ACK )
								{
									ucSequenceNumber = transport_buffer_peek(2);
									if( ucSequenceNumber==ucRequiredSequenceNumber )
									{
										tResult = ACK_RESULT_SequenceAckd;
										break;
									}
								}
								else if( sizPacket==2 && tPacketTyp==MONITOR_PACKET_TYP_CancelOperation )
								{
									tResult = ACK_RESULT_OperationCanceled;
									break;
								}

								/* Skip the complete packet. It is processed or ignored now. */
								transport_buffer_skip(2+sizPacket+2);
							}
						}
					}
				}
			}
		}

		iIsElapsed = systime_elapsed(ulTimerHandle, ulTimeoutMs);
	} while( iIsElapsed==0 );

	return tResult;
}



void transport_loop(void)
{
	unsigned char ucByte;
	unsigned int sizPacket;
	unsigned short usCrc16;
	unsigned int sizCrcPosition;
	int iResult;
	unsigned char ucPacketSequenceNumber;


	/* Collect a complete packet. */

	/* Wait for the start character. */
	do
	{
		uart_buffer_fill(1, UART_BUFFER_NO_TIMEOUT);
		ucByte = transport_buffer_get();
	} while( ucByte!=MONITOR_STREAM_PACKET_START );

//	uprintf("Startchar\n");

	/* Get the size of the data packet in bytes. */
	uart_buffer_fill(2, UART_BUFFER_NO_TIMEOUT);
	sizPacket  = transport_buffer_peek(0);
	sizPacket |= (unsigned int)(transport_buffer_peek(1) << 8U);

//	uprintf("Size: 0x%08x\n", sizPacket);

	/* Is the packet's size valid? */
	if( sizPacket==0 )
	{
		/* Get the magic "*#". This is the knock sequence of the old ROM code. */
		iResult = uart_buffer_fill(4, UART_BUFFER_TIMEOUT);
		if( iResult==0 && transport_buffer_peek(2)=='*' && transport_buffer_peek(3)=='#' )
		{
			/* Discard the size and magic. */
			transport_buffer_skip(4);

			/* Send magic cookie and version info. */
			monitor_send_magic(MONITOR_MAX_PACKET_SIZE_UART);
		}
	}
	else if( sizPacket<=MONITOR_MAX_PACKET_SIZE_UART-4 )
	{
		/* Yes, the packet size is valid. */

		/* Get the data and CRC16. */
		iResult = uart_buffer_fill(sizPacket+4, UART_BUFFER_TIMEOUT);
		if( iResult==0 )
		{
			/* Loop over all bytes and build the CRC16 checksum. */
			/* NOTE: the size is just for the user data, but the CRC16 includes the size. */
			usCrc16 = 0;
			sizCrcPosition = 0;
			while( sizCrcPosition<sizPacket+4 )
			{
				ucByte = transport_buffer_peek(sizCrcPosition);
				usCrc16 = crc16(usCrc16, ucByte);
				++sizCrcPosition;
			}

			if( usCrc16==0 )
			{
				/* OK, the CRC matches! */

				ucPacketSequenceNumber = transport_buffer_peek(2);
				if( ucSequence==ucPacketSequenceNumber )
				{
					/* Send an ACK packet here. */
					transport_uart_send_ack();

					/* Increase the sequence number. */
					++ucSequence;

					/* Skip the size and the sequence. */
					transport_buffer_skip(2+1);

					/* The maximum packet size is reduced by 6 bytes:
					 *  1 start char
					 *  2 size bytes
					 *  1 sequence number
					 *  2 CRC bytes
					 */
					monitor_process_packet(sizPacket-1U, MONITOR_MAX_PACKET_SIZE_UART-6U);
				}
				else
				{
					/* Skip over the complete packet. It is already copied. */
					transport_buffer_skip(sizPacket+4);
				}
			}
/*
			else
			{
				uprintf("crc failed: %04x\n", usCrc16);
			}
*/
		}
/*
		else
		{
			uprintf("Failed to fill buffer: %d\n", iResult);
		}
*/
	}
/*
	else
	{
		uprintf("size %d exceeds maximum %d\n", sizPacket, MONITOR_MAX_PACKET_SIZE-4);
	}
*/
}



void transport_send_byte(unsigned char ucData)
{
	if( sizPacketOutputFill<MONITOR_MAX_PACKET_SIZE_UART )
	{
		aucPacketOutputBuffer[sizPacketOutputFill] = ucData;
		++sizPacketOutputFill;
	}
/*
	else
	{
		uprintf("discarding byte 0x%02x\n", ucData);
	}
*/
}



void transport_send_bytes(const unsigned char *pucData, unsigned int sizData)
{
	/* Does the data still fit into the output buffer? */
	if( (sizPacketOutputFill+sizData)>MONITOR_MAX_PACKET_SIZE_UART )
	{
		/* No -> copy only what still fits. */
		sizData = MONITOR_MAX_PACKET_SIZE_UART - sizPacketOutputFill;
	}

	/* Copy the data to the buffer. */
	memcpy(aucPacketOutputBuffer + sizPacketOutputFill, pucData, sizData);

	sizPacketOutputFill += sizData;
}



int transport_send_packet(TRANSPORT_SEND_PACKET_ACK_T tRequireAck)
{
	const unsigned char *pucCnt;
	const unsigned char *pucEnd;
	unsigned char ucData;
	unsigned int sizData;
	unsigned short usCrc;
	ACK_RESULT_T tResult;
	int iResult;


	do
	{
//		uprintf("send packet\n");

		/* Send the start character. */
		SERIAL_V1_PUT(MONITOR_STREAM_PACKET_START);
//		uprintf("%02x ", MONITOR_STREAM_PACKET_START);

		usCrc = 0;

		/* The size of the data includes the sequence number. */
		sizData = sizPacketOutputFill + 1U;

		/* Send the size. */
		ucData = (unsigned char)( sizData        & 0xffU);
		usCrc = crc16(usCrc, ucData);
		SERIAL_V1_PUT(ucData);
//		uprintf("%02x ", ucData);
		ucData = (unsigned char)((sizData >> 8U) & 0xffU);
		usCrc = crc16(usCrc, ucData);
		SERIAL_V1_PUT(ucData);
//		uprintf("%02x ", ucData);

		/* Send the sequence number. */
		usCrc = crc16(usCrc, ucSequence);
		SERIAL_V1_PUT(ucSequence);

		/* Send the packet and build the CRC16. */
		pucCnt = aucPacketOutputBuffer;
		pucEnd = pucCnt + sizPacketOutputFill;
		while( pucCnt<pucEnd )
		{
			ucData = *(pucCnt++);
			SERIAL_V1_PUT(ucData);
//			uprintf("%02x ", ucData);
			usCrc = crc16(usCrc, ucData);
		}

		/* Send the CRC16. */
		ucData = (unsigned char)(usCrc>>8U);
		SERIAL_V1_PUT(ucData);
//		uprintf("%02x ", ucData);
		ucData = (unsigned char)(usCrc&0xffU);
		SERIAL_V1_PUT(ucData);
//		uprintf("%02x ", ucData);

//		uprintf("\n\n");

		if( tRequireAck==TRANSPORT_SEND_PACKET_WITHOUT_ACK )
		{
			tResult = ACK_RESULT_SequenceAckd;
		}
		else
		{
			/* Wait for ACK. */
			tResult = transport_uart_wait_for_ack(ucSequence, UART_RESEND_TIMEOUT_MS);
			if( tResult==ACK_RESULT_SequenceAckd )
			{
				/* Increase the sequence number. */
				++ucSequence;
			}
		}
	} while( tResult==ACK_RESULT_Timeout );

	sizPacketOutputFill = 0;

	if( tResult==ACK_RESULT_SequenceAckd )
	{
		iResult = 0;
	}
	else
	{
		iResult = -1;
	}

	return iResult;
}



unsigned char transport_call_console_get(void)
{
	return 0;
}



void transport_call_console_flush(void)
{
	/* Send the packet. */
	transport_send_packet(TRANSPORT_SEND_PACKET_WITH_ACK);

	/* Start a new packet. */
	transport_send_byte(MONITOR_PACKET_TYP_CallMessage);
}



void transport_call_console_put(unsigned int uiChar)
{
	/* Add the byte to the FIFO. */
	transport_send_byte((unsigned char)uiChar);

	/* Reached the maximum packet size?
	 * The maximum size is reduced by...
	 *   2 bytes size
	 *   1 byte sequence number
	 *   1 byte packet type
	 *   2 bytes CRC
	 */
	if( sizPacketOutputFill>=MONITOR_MAX_PACKET_SIZE_UART-6 )
	{
		transport_call_console_flush();
	}

}



unsigned int transport_call_console_peek(void)
{
	return 0;
}

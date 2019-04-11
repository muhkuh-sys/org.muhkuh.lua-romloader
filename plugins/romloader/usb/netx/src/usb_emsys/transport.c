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

#include <string.h>

#include "monitor.h"
#include "monitor_commands.h"
#include "netx_io_areas.h"
#include "systime.h"
#include "usb.h"


typedef enum ACK_RESULT_ENUM
{
	ACK_RESULT_SequenceAckd       =  0,
	ACK_RESULT_OperationCanceled  = -1,
	ACK_RESULT_Timeout            = -2
} ACK_RESULT_T;


/* Resend a packet after 1000ms if no ACK was received. */
#define USB_RESEND_TIMEOUT_MS 1000


static unsigned char aucPacketTx[MONITOR_MAX_PACKET_SIZE_USB];

static unsigned int sizPacketTxFill;
static unsigned int sizPacketRxHead;

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



static void transport_send_ack(void)
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

	usb_send_packet(aucACK, sizeof(aucACK));
}



void transport_init(void)
{
	monitor_init();

	/* Reserve 4 bytes for...
	 *   1 byte stream start
	 *   2 bytes data size
	 *   1 byte sequence number
	 */
	sizPacketTxFill = 4;

	sizPacketRxHead = 0;
	sizPacketRxFill = 0;

	ucSequence = 0;

	usb_init();
}


void transport_loop(void)
{
	unsigned int sizPacket;
	unsigned short usCrc16;
	unsigned int sizCrcPosition;
	unsigned char ucPacketSequenceNumber;


	usb_loop();
	if( sizPacketRxFill!=0U )
	{
		/* Does the packet start with the identifier? */
		if( aucPacketRx[0]==MONITOR_STREAM_PACKET_START )
		{
			/* The minimum packet size is 5 bytes, which is the knock sequence. */
			if( sizPacketRxFill>=5U )
			{
				sizPacket  =  (unsigned int)(aucPacketRx[1]);
				sizPacket |= ((unsigned int)(aucPacketRx[2])) << 8U;
				if( sizPacket==0U )
				{
					if( aucPacketRx[3]=='*' && aucPacketRx[4]=='#' )
					{
						/* Discard the size and magic. */
						transport_buffer_skip(5);

						/* This is a knock sequence. Respond with the magic cookie and version info. */
						monitor_send_magic(MONITOR_MAX_PACKET_SIZE_USB);
					}
				}
				/* Does the data fit into the received packet?
				 * The packet has these extra fields which are not part of the data:
				 *   1 byte stream start
				 *   2 bytes data size
				 *   2 bytes CRC16
				 */
				else if( sizPacket+5U==sizPacketRxFill )
				{
					/* Loop over all bytes and build the CRC16 checksum. */
					/* NOTE: the size is just for the user data, but the CRC16 includes the size. */
					usCrc16 = 0;
					sizCrcPosition = 1U;
					while( sizCrcPosition<sizPacketRxFill )
					{
						usCrc16 = crc16(usCrc16, aucPacketRx[sizCrcPosition]);
						++sizCrcPosition;
					}

					if( usCrc16==0 )
					{
						/* OK, the CRC matches! */

						/* Get the sequence number. */
						ucPacketSequenceNumber = aucPacketRx[3];
						if( ucSequence==ucPacketSequenceNumber )
						{
							/* Send an ACK packet here. */
							transport_send_ack();

							/* Increase the sequence number. */
							++ucSequence;

							/* Move the read pointer after the sequence number. */
							sizPacketRxHead = 4U;
							sizPacketRxFill -= 4U;

							/* Process the rest of the packet. */
							monitor_process_packet(sizPacketRxFill - 2U, MONITOR_MAX_PACKET_SIZE_USB - 6U);
						}
					}
				}
			}
		}

		/* Acknowledge the flag. */
		sizPacketRxFill = 0U;
	}
}



static ACK_RESULT_T transport_wait_for_ack(unsigned char ucRequiredSequenceNumber, unsigned long ulTimeoutMs)
{
	ACK_RESULT_T tResult;
	unsigned long ulTimerHandle;
	unsigned char ucPacketSequenceNumber;
	MONITOR_PACKET_TYP_T tPacketTyp;
	unsigned int sizData;
	int iIsElapsed;
	unsigned int uiCnt;
	unsigned short usCrc16;


	tResult = ACK_RESULT_Timeout;

	ulTimerHandle = systime_get_ms();
	do
	{
		usb_loop();
		if( sizPacketRxFill!=0U )
		{
			if( sizPacketRxFill==7 )
			{
				sizData  =  (unsigned int)(aucPacketRx[1]);
				sizData |= ((unsigned int)(aucPacketRx[2])) << 8U;
				if( sizData==2 )
				{
					/* Build the CRC. */
					usCrc16 = 0;
					for(uiCnt=1; uiCnt<sizPacketRxFill; ++uiCnt)
					{
						usCrc16 = crc16(usCrc16, aucPacketRx[uiCnt]);
					}
					if( usCrc16==0 )
					{
						/* Get the packet type. */
						tPacketTyp = (MONITOR_PACKET_TYP_T)aucPacketRx[4];

						ucPacketSequenceNumber = aucPacketRx[3];

						/* Is this an ACK packet with the correct sequence number? */
						if(tPacketTyp==MONITOR_PACKET_TYP_ACK && ucPacketSequenceNumber==ucRequiredSequenceNumber )
						{
							tResult = ACK_RESULT_SequenceAckd;

							/* Acknowledge the flag. */
							sizPacketRxFill = 0U;
							usb_received_packet_is_processed();

							break;
						}
						else if( tPacketTyp==MONITOR_PACKET_TYP_CancelOperation )
						{
							tResult = ACK_RESULT_OperationCanceled;

							/* Acknowledge the flag. */
							sizPacketRxFill = 0U;
							usb_received_packet_is_processed();

							break;
						}
					}
				}
			}
		}

		iIsElapsed = systime_elapsed(ulTimerHandle, ulTimeoutMs);
	} while( iIsElapsed==0 );

	return tResult;
}



int transport_send_packet(TRANSPORT_SEND_PACKET_ACK_T tRequireAck)
{
	unsigned int sizData;
	unsigned int uiCnt;
	unsigned short usCrc16;
	ACK_RESULT_T tResult;
	int iResult;


	/* Set the packet start. */
	aucPacketTx[0] = MONITOR_STREAM_PACKET_START;
	/* Set the data size. */
	sizData = sizPacketTxFill - 3U;
	aucPacketTx[1] =  sizData        & 0xffU;
	aucPacketTx[2] = (sizData >> 8U) & 0xffU;
	/* Write the sequence number to the start of the buffer. */
	aucPacketTx[3] = ucSequence;

	/* Build the CRC. */
	usCrc16 = 0;
	for(uiCnt=1; uiCnt<sizPacketTxFill; ++uiCnt)
	{
		usCrc16 = crc16(usCrc16, aucPacketTx[uiCnt]);
	}
	aucPacketTx[sizPacketTxFill++] = (unsigned char)(usCrc16 >> 8U);
	aucPacketTx[sizPacketTxFill++] = (unsigned char)(usCrc16 & 0xff);

	do
	{
		/* Send the packet. */
		usb_send_packet(aucPacketTx, sizPacketTxFill);

		if( tRequireAck==TRANSPORT_SEND_PACKET_WITHOUT_ACK )
		{
			tResult = ACK_RESULT_SequenceAckd;
		}
		else
		{
			/* Wait for ACK. */
			tResult = transport_wait_for_ack(ucSequence, USB_RESEND_TIMEOUT_MS);
			if( tResult==ACK_RESULT_SequenceAckd )
			{
				/* Increase the sequence number. */
				++ucSequence;
			}
		}
	} while( tResult==ACK_RESULT_Timeout );

	/* Reserve one byte for the sequence number. */
	sizPacketTxFill = 4;

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



void transport_send_byte(unsigned char ucData)
{
	if( sizPacketTxFill<MONITOR_MAX_PACKET_SIZE_USB )
	{
		aucPacketTx[sizPacketTxFill] = ucData;
		++sizPacketTxFill;
	}
}



void transport_send_bytes(const unsigned char *pucData, unsigned int sizData)
{
	/* Does the data still fit into the output buffer? */
	if( (sizPacketTxFill+sizData)>MONITOR_MAX_PACKET_SIZE_USB )
	{
		/* No -> copy only what still fits. */
		sizData = MONITOR_MAX_PACKET_SIZE_USB - sizPacketTxFill;
	}

	/* Copy the data to the buffer. */
	memcpy(aucPacketTx + sizPacketTxFill, pucData, sizData);

	sizPacketTxFill += sizData;
}



unsigned char transport_buffer_get(void)
{
	unsigned char ucByte;


	ucByte = aucPacketRx[sizPacketRxHead];

	++sizPacketRxHead;
	if( sizPacketRxHead>=MONITOR_MAX_PACKET_SIZE_USB )
	{
		sizPacketRxHead -= MONITOR_MAX_PACKET_SIZE_USB;
	}

	--sizPacketRxFill;
	if( sizPacketRxFill==0U )
	{
		usb_received_packet_is_processed();
	}

	return ucByte;
}



unsigned char transport_buffer_peek(unsigned int sizOffset)
{
	unsigned int sizReadPosition;
	unsigned char ucByte;


	sizReadPosition = sizPacketRxHead + sizOffset;
	if( sizReadPosition>=MONITOR_MAX_PACKET_SIZE_USB )
	{
		sizReadPosition -= MONITOR_MAX_PACKET_SIZE_USB;
	}

	ucByte = aucPacketRx[sizReadPosition];

	return ucByte;
}



void transport_buffer_skip(unsigned int sizSkip)
{
	sizPacketRxHead += sizSkip;
	if( sizPacketRxHead>=MONITOR_MAX_PACKET_SIZE_USB )
	{
		sizPacketRxHead -= MONITOR_MAX_PACKET_SIZE_USB;
	}

	sizPacketRxFill -= sizSkip;
	if( sizPacketRxFill==0U )
	{
		usb_received_packet_is_processed();
	}
}



unsigned char transport_call_console_get(void)
{
	return 0;
}


void transport_call_console_put(unsigned int uiChar)
{
	/* Add the byte to the buffer. */
	transport_send_byte((unsigned char)uiChar);

	/* Reached the maximum packet size? */
	if( sizPacketTxFill>=MONITOR_MAX_PACKET_SIZE_USB )
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
	transport_send_packet(TRANSPORT_SEND_PACKET_WITH_ACK);

	/* Start a new packet. */
	transport_send_byte(MONITOR_PACKET_TYP_CallMessage);
}

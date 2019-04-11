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
#include "transport_extension.h"

#include <stddef.h>
#include <string.h>

#include "monitor.h"
#include "monitor_commands.h"
#include "serial_vectors.h"
#include "systime.h"

#include "usb.h"


#define UART_BUFFER_NO_TIMEOUT 0
#define UART_BUFFER_TIMEOUT 1

#define MONITOR_MAX_PACKET_SIZE_UART 0x0700

/* Resend a packet after 1000ms if no ACK was received. */
#define UART_RESEND_TIMEOUT_MS 1000


typedef int (*PFN_TRANSPORT_FILL_BUFFER_T)(size_t sizRequestedFillLevel, unsigned int uiTimeoutFlag);
typedef int (*PFN_TRANSPORT_SEND_PACKET_T)(TRANSPORT_SEND_PACKET_ACK_T tRequireAck);
typedef void (*PFN_TRANSPORT_SEND_ACK_T)(void);


unsigned char aucStreamBuffer[MONITOR_MAX_PACKET_SIZE_UART];
size_t sizStreamBufferHead;
size_t sizStreamBufferFill;

//unsigned char aucPacketInputBuffer[MONITOR_MAX_PACKET_SIZE_UART];

unsigned char aucPacketOutputBuffer[MONITOR_MAX_PACKET_SIZE_UART];
size_t sizPacketOutputFill;
size_t sizPacketOutputFillLast;


static PFN_TRANSPORT_FILL_BUFFER_T s_pfnFillBuffer;
static PFN_TRANSPORT_SEND_PACKET_T s_pfnSendPacket;
static PFN_TRANSPORT_SEND_ACK_T s_pfnSendAck;


static unsigned char ucSequence;



/* This is a very nice routine for the CITT XModem CRC from http://www.eagleairaust.com.au/code/crc16.htm. */
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



static int transport_buffer_fill_usbcdc(size_t sizRequestedFillLevel, unsigned int uiTimeoutFlag)
{
	int iResult;
	unsigned long ulTimeout;


	iResult = 0;

	if( uiTimeoutFlag==UART_BUFFER_NO_TIMEOUT )
	{
		/* Fill-up the buffer to the requested level. */
		while( sizStreamBufferFill<sizRequestedFillLevel )
		{
			/* Process incoming packets and send waiting data. */
			usb_pingpong();
		}
	}
	else
	{
		ulTimeout = systime_get_ms();

		/* Fill-up the buffer to the requested level. */
		while( sizStreamBufferFill<sizRequestedFillLevel )
		{
			/* Wait for new data. */
			if( systime_elapsed(ulTimeout, 500)!=0 )
			{
				iResult = -1;
				break;
			}

			/* Process incoming packets and send waiting data. */
			usb_pingpong();
		}
	}

	return iResult;
}



static int transport_buffer_fill_uart(size_t sizRequestedFillLevel, unsigned int uiTimeoutFlag)
{
	HOSTDEF(ptUart0Area);
	size_t sizWritePosition;
	int iResult;
	unsigned long ulTimeout;
	unsigned long ulValue;


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
			/* Wait for data in the FIFO. */
			ulValue  = ptUart0Area->ulUartfr;
			ulValue &= HOSTMSK(uartfr_RXFE);
			if( ulValue==0 )
			{
				/* Write the new byte to the buffer. */
				aucStreamBuffer[sizWritePosition] = (unsigned char)(ptUart0Area->ulUartdr);
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
	}
	else
	{
		/* Fill-up the buffer to the requested level. */
		ulTimeout = systime_get_ms();
		while( sizStreamBufferFill<sizRequestedFillLevel )
		{
			/* Wait for new data. */
			if( systime_elapsed(ulTimeout, 500)!=0 )
			{
				iResult = -1;
				break;
			}

			/* Get the current fill level. */
			ulValue  = ptUart0Area->ulUartfr;
			ulValue &= HOSTMSK(uartfr_RXFE);
			if( ulValue==0 )
			{
				/* Write the new byte to the buffer. */
				aucStreamBuffer[sizWritePosition] = (unsigned char)(ptUart0Area->ulUartdr);
				/* Increase the write position. */
				++sizWritePosition;
				if( sizWritePosition>=MONITOR_MAX_PACKET_SIZE_UART )
				{
					sizWritePosition -= MONITOR_MAX_PACKET_SIZE_UART;
				}
				/* Increase the fill level. */
				++sizStreamBufferFill;

				ulTimeout = systime_get_ms();
			}
		}
	}

	return iResult;
}



static void transport_uart_send_ack(void)
{
	HOSTDEF(ptUart0Area);
	unsigned int uiCnt;
	unsigned long ulValue;
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
		do
		{
			ulValue  = ptUart0Area->ulUartfr;
			ulValue &= HOSTMSK(uartfr_TXFE);
		} while( ulValue==0 );
		ptUart0Area->ulUartdr = aucACK[uiCnt];
	}

	/* Wait until the ACK is out. */
	do
	{
		ulValue  = ptUart0Area->ulUartfr;
		ulValue &= HOSTMSK(uartfr_TXFE);
	} while( ulValue==0 );
}



static void transport_usbcdc_send_ack(void)
{
	HOSTDEF(ptUsbCoreArea);
	unsigned int uiCnt;
	unsigned long ulValue;
	unsigned short usCrc16;
	unsigned long *pulUsbFifo;
	unsigned int sizUsbTransferLength;
	union BUF_UNION
	{
		unsigned char auc[8];
		unsigned long aul[2];
	} uBuf;


	/* Write the complete packet in DWORD chunks to the USB FIFO.
	 * The USB FIFO area does not handle byte writes correctly.
	 * All other bytes in the DWORD would get the same value.
	 */

	uBuf.auc[0] = MONITOR_STREAM_PACKET_START;
	uBuf.auc[1] = 2U;  /* The low byte of the data size. */
	uBuf.auc[2] = 0U;  /* The high byte of the data size. */
	uBuf.auc[3] = ucSequence;
	uBuf.auc[4] = MONITOR_PACKET_TYP_ACK;

	/* Generate the CRC for the packet. */
	usCrc16 = 0;
	for(uiCnt=1; uiCnt<5; ++uiCnt)
	{
		usCrc16 = crc16(usCrc16, uBuf.auc[uiCnt]);
	}
	uBuf.auc[5] = (unsigned char)(usCrc16 >> 8U);
	uBuf.auc[6] = (unsigned char)(usCrc16 & 0xff);
	uBuf.auc[7] = 0;
	sizUsbTransferLength = 7U;

	pulUsbFifo = (unsigned long*)(HOSTADR(USB_FIFO_BASE) + 0x800U);
	*(pulUsbFifo++) = uBuf.aul[0];
	*(pulUsbFifo++) = uBuf.aul[1];

	/* Send the packet in the FIFO. */
	ptUsbCoreArea->ulPIPE_SEL = 3;
	ptUsbCoreArea->ulPIPE_CTRL = MSK_USB_PIPE_CTRL_ACT | DEF_USB_PIPE_CTRL_TPID_IN;
	ptUsbCoreArea->ulPIPE_DATA_PTR = 0x0800 / sizeof(unsigned long);
	ptUsbCoreArea->ulPIPE_DATA_TBYTES = MSK_USB_PIPE_DATA_TBYTES_DBV|(sizUsbTransferLength);

	/* Wait until the packet is sent. */
	do
	{
		ulValue  = ptUsbCoreArea->ulPIPE_DATA_TBYTES;
		ulValue &= MSK_USB_PIPE_DATA_TBYTES_DBV;
	} while( ulValue!=0 );

	/* Need a ZLP? */
	if( (sizUsbTransferLength&0x3fU)==0U )
	{
		ptUsbCoreArea->ulPIPE_CTRL = MSK_USB_PIPE_CTRL_ACT | DEF_USB_PIPE_CTRL_TPID_IN;
		ptUsbCoreArea->ulPIPE_DATA_PTR = 0x0800 / sizeof(unsigned long);
		ptUsbCoreArea->ulPIPE_DATA_TBYTES = MSK_USB_PIPE_DATA_TBYTES_DBV;

		/* Wait until the packet is sent. */
		do
		{
			ulValue  = ptUsbCoreArea->ulPIPE_DATA_TBYTES;
			ulValue &= MSK_USB_PIPE_DATA_TBYTES_DBV;
		} while( ulValue!=0 );
	}

}



unsigned char transport_buffer_get(void)
{
	unsigned char ucByte;


	ucByte = aucStreamBuffer[sizStreamBufferHead];

	++sizStreamBufferHead;
	if( sizStreamBufferHead>=MONITOR_MAX_PACKET_SIZE_UART )
	{
		sizStreamBufferHead -= MONITOR_MAX_PACKET_SIZE_UART;
	}

	--sizStreamBufferFill;

	/* need to reactivate the endpoint? */
	if( tReceiveEpState==USB_ReceiveEndpoint_Blocked && (MONITOR_MAX_PACKET_SIZE_UART-sizStreamBufferFill)>=Usb_Ep2_PacketSize )
	{
		usb_activateInputPipe();
	}

	return ucByte;
}


unsigned char transport_buffer_peek(size_t sizOffset)
{
	size_t sizReadPosition;
	unsigned char ucByte;


	sizReadPosition = sizStreamBufferHead + sizOffset;
	if( sizReadPosition>=MONITOR_MAX_PACKET_SIZE_UART )
	{
		sizReadPosition -= MONITOR_MAX_PACKET_SIZE_UART;
	}

	ucByte = aucStreamBuffer[sizReadPosition];

	return ucByte;
}


void transport_buffer_skip(size_t sizSkip)
{
	sizStreamBufferHead += sizSkip;
	if( sizStreamBufferHead>=MONITOR_MAX_PACKET_SIZE_UART )
	{
		sizStreamBufferHead -= MONITOR_MAX_PACKET_SIZE_UART;
	}

	sizStreamBufferFill -= sizSkip;

	// need to reactivate the endpoint?
	if( tReceiveEpState==USB_ReceiveEndpoint_Blocked && (MONITOR_MAX_PACKET_SIZE_UART-sizStreamBufferFill)>=Usb_Ep2_PacketSize )
	{
		usb_activateInputPipe();
	}
}



typedef enum ACK_RESULT_ENUM
{
	ACK_RESULT_SequenceAckd       =  0,
	ACK_RESULT_OperationCanceled  = -1,
	ACK_RESULT_Timeout            = -2
} ACK_RESULT_T;



static ACK_RESULT_T transport_wait_for_ack(unsigned char ucRequiredSequenceNumber, unsigned long ulTimeoutMs)
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

	ulTimerHandle = systime_get_ms();
	do
	{
		/* Wait for the start character. */
		iResult = s_pfnFillBuffer(1, UART_BUFFER_TIMEOUT);
		if( iResult==0 )
		{
			ucByte = transport_buffer_get();
			if( ucByte==MONITOR_STREAM_PACKET_START )
			{
				/* Wait for the size. */
				iResult = s_pfnFillBuffer(2, UART_BUFFER_TIMEOUT);
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
						iResult = s_pfnFillBuffer(2+sizPacket+2, UART_BUFFER_TIMEOUT);
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
	size_t sizPacket;
	unsigned short usCrc16;
	size_t sizCrcPosition;
	int iResult;
	unsigned char ucPacketSequenceNumber;


	/* Collect a complete packet. */

	/* Wait for the start character. */
	do
	{
		s_pfnFillBuffer(1, UART_BUFFER_NO_TIMEOUT);
		ucByte = transport_buffer_get();
	} while( ucByte!=MONITOR_STREAM_PACKET_START );

	/* Get the size of the data packet in bytes. */
	s_pfnFillBuffer(2, UART_BUFFER_NO_TIMEOUT);
	sizPacket  = transport_buffer_peek(0);
	sizPacket |= (size_t)(transport_buffer_peek(1) << 8U);

	/* Is the packet's size valid? */
	if( sizPacket==0 )
	{
		/* Get the magic "*#". This is the knock sequence of the old ROM code. */
		iResult = s_pfnFillBuffer(4, UART_BUFFER_TIMEOUT);
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

		/* Get the size, data and CRC16. */
		iResult = s_pfnFillBuffer(sizPacket+4, UART_BUFFER_TIMEOUT);
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
					s_pfnSendAck();

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
		}
	}
}



void transport_send_byte(unsigned char ucData)
{
	if( sizPacketOutputFill<MONITOR_MAX_PACKET_SIZE_UART )
	{
		aucPacketOutputBuffer[sizPacketOutputFill] = ucData;
		++sizPacketOutputFill;
	}
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



static int transport_send_packet_usbcdc(TRANSPORT_SEND_PACKET_ACK_T tRequireAck)
{
	HOSTDEF(ptUsbCoreArea);
	const unsigned char *pucCnt;
	const unsigned char *pucEnd;
	unsigned char ucData;
	unsigned short usCrc;
	unsigned int sizData;
	unsigned long ulValue;
	unsigned int uiBitPos;
	unsigned long *pulUsbFifo;
	size_t sizUsbTransferLength;
	ACK_RESULT_T tResult;
	int iResult;


	/* Write the complete packet in DWORD chunks to the USB FIFO.
	 * The USB FIFO area does not handle byte writes correctly.
	 * All other bytes in the DWORD would get the same value.
	 */

	do
	{
		/* Send the start character. */
		ulValue  = MONITOR_STREAM_PACKET_START;

		/* The size of the data includes the sequence number. */
		sizData = sizPacketOutputFill + 1U;

		/* Send the size. */
		ucData = (unsigned char)( sizData        & 0xffU);
		ulValue |= ((unsigned long)ucData) << 8U;
		usCrc = crc16(0, ucData);
		ucData = (unsigned char)((sizData >> 8U) & 0xffU);
		ulValue |= ((unsigned long)ucData) << 16U;
		usCrc = crc16(usCrc, ucData);

		/* Send the sequence. */
		ulValue |= ((unsigned long)ucSequence) << 24U;
		usCrc = crc16(usCrc, ucSequence);

		pulUsbFifo = (unsigned long*)(HOSTADR(USB_FIFO_BASE) + 0x800U);
		*(pulUsbFifo++) = ulValue;

		/* Send the packet and build the CRC16. */
		pucCnt = aucPacketOutputBuffer;
		pucEnd = aucPacketOutputBuffer + sizPacketOutputFill;
		uiBitPos = 0U;
		ulValue = 0U;
		while( pucCnt<pucEnd )
		{
			ucData = *(pucCnt++);
			ulValue |= ((unsigned long)ucData) << uiBitPos;
			usCrc = crc16(usCrc, ucData);
			uiBitPos += 8U;
			if( uiBitPos>24U )
			{
				*(pulUsbFifo++) = ulValue;
				uiBitPos = 0U;
				ulValue = 0U;
			}
		}

		/* Send the CRC16. */
		ucData = (unsigned char)(usCrc>>8U);
		ulValue |= ((unsigned long)ucData) << uiBitPos;
		uiBitPos += 8U;
		if( uiBitPos>24U )
		{
			*(pulUsbFifo++) = ulValue;
			uiBitPos = 0;
			ulValue = 0;
		}
		ucData = (unsigned char)(usCrc&0xffU);
		ulValue |= ((unsigned long)ucData) << uiBitPos;
		*(pulUsbFifo++) = ulValue;
		sizUsbTransferLength = sizPacketOutputFill + 6U;


		/* Send the packet in the FIFO. */
		ptUsbCoreArea->ulPIPE_SEL = 3;
		ptUsbCoreArea->ulPIPE_CTRL = MSK_USB_PIPE_CTRL_ACT | DEF_USB_PIPE_CTRL_TPID_IN;
		ptUsbCoreArea->ulPIPE_DATA_PTR = 0x0800 / sizeof(unsigned long);
		ptUsbCoreArea->ulPIPE_DATA_TBYTES = MSK_USB_PIPE_DATA_TBYTES_DBV|(sizUsbTransferLength);

		/* Wait until the packet is sent. */
		do
		{
			ulValue  = ptUsbCoreArea->ulPIPE_DATA_TBYTES;
			ulValue &= MSK_USB_PIPE_DATA_TBYTES_DBV;
		} while( ulValue!=0 );

		/* Need a ZLP? */
		if( (sizUsbTransferLength&0x3fU)==0U )
		{
			ptUsbCoreArea->ulPIPE_CTRL = MSK_USB_PIPE_CTRL_ACT | DEF_USB_PIPE_CTRL_TPID_IN;
			ptUsbCoreArea->ulPIPE_DATA_PTR = 0x0800 / sizeof(unsigned long);
			ptUsbCoreArea->ulPIPE_DATA_TBYTES = MSK_USB_PIPE_DATA_TBYTES_DBV;

			/* Wait until the packet is sent. */
			do
			{
				ulValue  = ptUsbCoreArea->ulPIPE_DATA_TBYTES;
				ulValue &= MSK_USB_PIPE_DATA_TBYTES_DBV;
			} while( ulValue!=0 );
		}

		if( tRequireAck==TRANSPORT_SEND_PACKET_WITHOUT_ACK )
		{
			tResult = ACK_RESULT_SequenceAckd;
		}
		else
		{
			/* Wait for ACK. */
			tResult = transport_wait_for_ack(ucSequence, UART_RESEND_TIMEOUT_MS);
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



static int transport_send_packet_uart(TRANSPORT_SEND_PACKET_ACK_T tRequireAck)
{
	HOSTDEF(ptUart0Area);
	unsigned long ulValue;
	const unsigned char *pucCnt;
	const unsigned char *pucEnd;
	unsigned char ucData;
	unsigned short usCrc;
	unsigned int sizData;
	ACK_RESULT_T tResult;
	int iResult;


	do
	{
		/* Send the start character. */
		do
		{
			ulValue  = ptUart0Area->ulUartfr;
			ulValue &= HOSTMSK(uartfr_TXFE);
		} while( ulValue==0 );
		ptUart0Area->ulUartdr = MONITOR_STREAM_PACKET_START;

		/* The size of the data includes the sequence number. */
		sizData = sizPacketOutputFill + 1U;

		/* Send the size. */
		ucData = (unsigned char)( sizData        & 0xffU);
		do
		{
			ulValue  = ptUart0Area->ulUartfr;
			ulValue &= HOSTMSK(uartfr_TXFE);
		} while( ulValue==0 );
		ptUart0Area->ulUartdr = (unsigned long)ucData;
		usCrc = crc16(0, ucData);
		ucData = (unsigned char)((sizData >> 8U) & 0xffU);
		do
		{
			ulValue  = ptUart0Area->ulUartfr;
			ulValue &= HOSTMSK(uartfr_TXFE);
		} while( ulValue==0 );
		ptUart0Area->ulUartdr = (unsigned long)ucData;
		usCrc = crc16(usCrc, ucData);

		/* Send the sequence number. */
		ucData = ucSequence;
		do
		{
			ulValue  = ptUart0Area->ulUartfr;
			ulValue &= HOSTMSK(uartfr_TXFE);
		} while( ulValue==0 );
		ptUart0Area->ulUartdr = (unsigned long)ucData;
		usCrc = crc16(usCrc, ucData);

		/* Send the packet and build the CRC16. */
		pucCnt = aucPacketOutputBuffer;
		pucEnd = aucPacketOutputBuffer + sizPacketOutputFill;
		while( pucCnt<pucEnd )
		{
			do
			{
				ulValue  = ptUart0Area->ulUartfr;
				ulValue &= HOSTMSK(uartfr_TXFE);
			} while( ulValue==0 );

			ucData = *(pucCnt++);
			ptUart0Area->ulUartdr = (unsigned long)ucData;
			usCrc = crc16(usCrc, ucData);
		}

		/* Send the CRC16. */
		ucData = (unsigned char)(usCrc>>8U);
		do
		{
			ulValue  = ptUart0Area->ulUartfr;
			ulValue &= HOSTMSK(uartfr_TXFE);
		} while( ulValue==0 );
		ptUart0Area->ulUartdr = (unsigned long)ucData;
		ucData = (unsigned char)(usCrc&0xffU);
		do
		{
			ulValue  = ptUart0Area->ulUartfr;
			ulValue &= HOSTMSK(uartfr_TXFE);
		} while( ulValue==0 );
		ptUart0Area->ulUartdr = (unsigned long)ucData;

		if( tRequireAck==TRANSPORT_SEND_PACKET_WITHOUT_ACK )
		{
			tResult = ACK_RESULT_SequenceAckd;
		}
		else
		{
			/* Wait for ACK. */
			tResult = transport_wait_for_ack(ucSequence, UART_RESEND_TIMEOUT_MS);
			if( tResult==ACK_RESULT_SequenceAckd )
			{
				/* Increase the sequence number. */
				++ucSequence;
			}
		}
	} while( tResult==ACK_RESULT_Timeout );

	/* The package data was processed. */
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



int transport_send_packet(TRANSPORT_SEND_PACKET_ACK_T tRequireAck)
{
	return s_pfnSendPacket(tRequireAck);
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


void transport_set_vectors(unsigned long ulDevice)
{
	if( ulDevice==0 )
	{
		s_pfnFillBuffer = transport_buffer_fill_uart;
		s_pfnSendPacket = transport_send_packet_uart;
		s_pfnSendAck = transport_uart_send_ack;
	}
	else if( ulDevice==1 )
	{
		s_pfnFillBuffer = transport_buffer_fill_usbcdc;
		s_pfnSendPacket = transport_send_packet_usbcdc;
		s_pfnSendAck = transport_usbcdc_send_ack;
	}
}



tUsbCdc_BufferState usb_cdc_buf_rec_put(unsigned char *pucBuffer, unsigned int uiBufferLen, unsigned int uiMaxPacketSize)
{
	tUsbCdc_BufferState tResult;
	size_t sizWritePosition;
	unsigned int uiBufferLeft;
	unsigned char *pucCnt, *pucEnd;


	/* 0 bytes always succeed. */
	if( uiBufferLen==0 )
	{
		tResult = tUsbCdc_BufferState_Ok;
	}
	else
	{
		/* Test the number of free bytes in the buffer. */
		uiBufferLeft = MONITOR_MAX_PACKET_SIZE_UART - sizStreamBufferFill;
		if( uiBufferLen>uiBufferLeft )
		{
			tResult = tUsbCdc_BufferState_Rejected;
		}
		else
		{
			/* Get the write position. */
			sizWritePosition = sizStreamBufferHead + sizStreamBufferFill;
			if( sizWritePosition>=MONITOR_MAX_PACKET_SIZE_UART )
			{
				sizWritePosition -= MONITOR_MAX_PACKET_SIZE_UART;
			}

			/* Copy the bytes into the buffer. */
			pucCnt = pucBuffer;
			pucEnd = pucBuffer + uiBufferLen;
			do
			{
				/* Write the new byte to the buffer. */
				aucStreamBuffer[sizWritePosition] = *(pucCnt++);
				/* Increase the write position. */
				++sizWritePosition;
				if( sizWritePosition>=MONITOR_MAX_PACKET_SIZE_UART )
				{
					sizWritePosition -= MONITOR_MAX_PACKET_SIZE_UART;
				}
			} while( pucCnt<pucEnd );

			/* Increase the fill level. */
			sizStreamBufferFill += uiBufferLen;

			uiBufferLeft = MONITOR_MAX_PACKET_SIZE_UART - sizStreamBufferFill;
			if( uiBufferLeft<uiMaxPacketSize )
			{
				// ok, but the buffer can't hold another packet
				tResult = tUsbCdc_BufferState_Full;
			}
			else
			{
				// ok
				tResult = tUsbCdc_BufferState_Ok;
			}
		}
	}

	return tResult;
}

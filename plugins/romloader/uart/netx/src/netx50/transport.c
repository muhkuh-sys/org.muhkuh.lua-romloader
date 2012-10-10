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

#include "monitor.h"
#include "monitor_commands.h"
#include "serial_vectors.h"
#include "systime.h"

#include "usb.h"


#define UART_BUFFER_NO_TIMEOUT 0
#define UART_BUFFER_TIMEOUT 1

#define MONITOR_MAX_PACKET_SIZE_UART 256

typedef int (*PFN_TRANSPORT_FILL_BUFFER_T)(size_t sizRequestedFillLevel, unsigned int uiTimeoutFlag);
typedef void (*PFN_TRANSPORT_SEND_PACKET_T)(void);


unsigned char aucStreamBuffer[MONITOR_MAX_PACKET_SIZE_UART];
size_t sizStreamBufferHead;
size_t sizStreamBufferFill;

unsigned char aucPacketInputBuffer[MONITOR_MAX_PACKET_SIZE_UART];

unsigned char aucPacketOutputBuffer[MONITOR_MAX_PACKET_SIZE_UART];
size_t sizPacketOutputFill;
size_t sizPacketOutputFillLast;


static PFN_TRANSPORT_FILL_BUFFER_T s_pfnFillBuffer;
static PFN_TRANSPORT_SEND_PACKET_T s_pfnSendPacket;


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
}



static int transport_buffer_fill_usb_cdc(size_t sizRequestedFillLevel, unsigned int uiTimeoutFlag)
{
	/* Fill-up the buffer to the requested level. */
	while( sizStreamBufferFill<sizRequestedFillLevel )
	{
		/* Process incoming packets and send waiting data. */
		usb_pingpong();
	}

	return 0;
}



static int transport_buffer_fill_uart(size_t sizRequestedFillLevel, unsigned int uiTimeoutFlag)
{
	size_t sizWritePosition;
	int iResult;
	unsigned long ulTimeout;
	HOSTDEF(ptUart0Area);
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



static unsigned char uart_buffer_get(void)
{
	unsigned char ucByte;


	ucByte = aucStreamBuffer[sizStreamBufferHead];

	++sizStreamBufferHead;
	if( sizStreamBufferHead>=MONITOR_MAX_PACKET_SIZE_UART )
	{
		sizStreamBufferHead -= MONITOR_MAX_PACKET_SIZE_UART;
	}

	--sizStreamBufferFill;

	// need to reactivate the endpoint?
	if( tReceiveEpState==USB_ReceiveEndpoint_Blocked && (MONITOR_MAX_PACKET_SIZE_UART-sizStreamBufferFill)>=Usb_Ep2_PacketSize )
	{
		usb_activateInputPipe();
	}

	return ucByte;
}


static unsigned char uart_buffer_peek(size_t sizOffset)
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


static void uart_buffer_skip(size_t sizSkip)
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


void transport_loop(void)
{
	unsigned char ucByte;
	size_t sizPacket;
	unsigned short usCrc16;
	size_t sizCrcPosition;
	int iResult;
	unsigned char *pucBuffer;


	/* Collect a complete packet. */

	/* Wait for the start character. */
	do
	{
		s_pfnFillBuffer(1, UART_BUFFER_NO_TIMEOUT);
		ucByte = uart_buffer_get();
	} while( ucByte!=MONITOR_STREAM_PACKET_START );

	/* Get the size of the data packet in bytes. */
	s_pfnFillBuffer(2, UART_BUFFER_NO_TIMEOUT);
	sizPacket  = uart_buffer_peek(0);
	sizPacket |= (size_t)(uart_buffer_peek(1) << 8U);

	/* Is the packet's size valid? */
	if( sizPacket==0 )
	{
		/* Get the magic "*#". This is the knock sequence of the old ROM code. */
		iResult = s_pfnFillBuffer(4, UART_BUFFER_TIMEOUT);
		if( iResult==0 && uart_buffer_peek(2)=='*' && uart_buffer_peek(3)=='#' )
		{
			/* Discard the size and magic. */
			for(sizCrcPosition=0; sizCrcPosition<4; ++sizCrcPosition)
			{
				uart_buffer_get();
			}

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
			usCrc16 = crc16(0, uart_buffer_peek(0));
			usCrc16 = crc16(usCrc16, uart_buffer_peek(1));
			pucBuffer = aucPacketInputBuffer;
			sizCrcPosition = 2;
			while( sizCrcPosition<sizPacket+4 )
			{
				ucByte = uart_buffer_peek(sizCrcPosition);
				*(pucBuffer++) = ucByte;
				usCrc16 = crc16(usCrc16, ucByte);
				++sizCrcPosition;
			}

			if( usCrc16==0 )
			{
				/* OK, the CRC matches! */

				/* Skip over the complete packet. It is already copied. */
				uart_buffer_skip(sizPacket+4);

				monitor_process_packet(aucPacketInputBuffer, sizPacket, MONITOR_MAX_PACKET_SIZE_UART-5U);
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



static void transport_send_packet_usb_cdc(void)
{
	const unsigned char *pucCnt;
	const unsigned char *pucEnd;
	unsigned char ucData;
	unsigned short usCrc;


	/* Send the start character. */
	usb_cdc_buf_send_put(MONITOR_STREAM_PACKET_START);

	/* Send the size. */
	ucData = (unsigned char)( sizPacketOutputFill        & 0xffU);
	usb_cdc_buf_send_put(ucData);
	usCrc = crc16(0, ucData);
	ucData = (unsigned char)((sizPacketOutputFill >> 8U) & 0xffU);
	usb_cdc_buf_send_put(ucData);
	usCrc = crc16(usCrc, ucData);


	/* Send the packet and build the CRC16. */
	pucCnt = aucPacketOutputBuffer;
	pucEnd = aucPacketOutputBuffer + sizPacketOutputFill;
	while( pucCnt<pucEnd )
	{
		ucData = *(pucCnt++);
		usb_cdc_buf_send_put(ucData);
		usCrc = crc16(usCrc, ucData);
	}

	/* Send the CRC16. */
	ucData = (unsigned char)(usCrc>>8U);
	usb_cdc_buf_send_put(ucData);
	ucData = (unsigned char)(usCrc&0xffU);
	usb_cdc_buf_send_put(ucData);

	usb_cdc_buf_send_flush();

	/* Remember the packet size for resends. */
	sizPacketOutputFillLast = sizPacketOutputFill;

	sizPacketOutputFill = 0;
}



static void transport_send_packet_uart(void)
{
	HOSTDEF(ptUart0Area);
	unsigned long ulValue;
	const unsigned char *pucCnt;
	const unsigned char *pucEnd;
	unsigned char ucData;
	unsigned short usCrc;


	/* Send the start character. */
	do
	{
		ulValue  = ptUart0Area->ulUartfr;
		ulValue &= HOSTMSK(uartfr_TXFF);
	} while( ulValue!=0 );
	ptUart0Area->ulUartdr = MONITOR_STREAM_PACKET_START;

	/* Send the size. */
	ucData = (unsigned char)( sizPacketOutputFill        & 0xffU);
	do
	{
		ulValue  = ptUart0Area->ulUartfr;
		ulValue &= HOSTMSK(uartfr_TXFF);
	} while( ulValue!=0 );
	ptUart0Area->ulUartdr = (unsigned long)ucData;
	usCrc = crc16(0, ucData);
	ucData = (unsigned char)((sizPacketOutputFill >> 8U) & 0xffU);
	do
	{
		ulValue  = ptUart0Area->ulUartfr;
		ulValue &= HOSTMSK(uartfr_TXFF);
	} while( ulValue!=0 );
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
			ulValue &= HOSTMSK(uartfr_TXFF);
		} while( ulValue!=0 );

		ucData = *(pucCnt++);
		ptUart0Area->ulUartdr = (unsigned long)ucData;
		usCrc = crc16(usCrc, ucData);
	}

	/* Send the CRC16. */
	ucData = (unsigned char)(usCrc>>8U);
	do
	{
		ulValue  = ptUart0Area->ulUartfr;
		ulValue &= HOSTMSK(uartfr_TXFF);
	} while( ulValue!=0 );
	ptUart0Area->ulUartdr = (unsigned long)ucData;
	ucData = (unsigned char)(usCrc&0xffU);
	do
	{
		ulValue  = ptUart0Area->ulUartfr;
		ulValue &= HOSTMSK(uartfr_TXFF);
	} while( ulValue!=0 );
	ptUart0Area->ulUartdr = (unsigned long)ucData;

	/* Remember the packet size for resends. */
	sizPacketOutputFillLast = sizPacketOutputFill;

	sizPacketOutputFill = 0;
}



void transport_send_packet(void)
{
	s_pfnSendPacket();
}



void transport_resend_packet(void)
{
	/* Restore the last packet size. */
	sizPacketOutputFill = sizPacketOutputFillLast;

	/* Send the buffer again. */
	transport_send_packet();
}



unsigned char transport_call_console_get(void)
{
	return 0;
}



void transport_call_console_put(unsigned int uiChar)
{
	/* Add the byte to the FIFO. */
	transport_send_byte((unsigned char)uiChar);

	/* Reached the maximum packet size? */
	if( sizPacketOutputFill>=MONITOR_MAX_PACKET_SIZE_UART-5 )
	{
		/* Yes -> send the packet. */
		transport_send_packet();

		/* Start a new packet. */
		transport_send_byte(MONITOR_STATUS_CallMessage);
	}

}



unsigned int transport_call_console_peek(void)
{
	return 0;
}



void transport_call_console_flush(void)
{
	/* Send the packet. */
	transport_send_packet();

	/* Start a new packet. */
	transport_send_byte(MONITOR_STATUS_CallMessage);
}


void transport_set_vectors(unsigned long ulDevice)
{
	if( ulDevice==0 )
	{
		s_pfnFillBuffer = transport_buffer_fill_uart;
		s_pfnSendPacket = transport_send_packet_uart;
	}
	else if( ulDevice==1 )
	{
		s_pfnFillBuffer = transport_buffer_fill_usb_cdc;
		s_pfnSendPacket = transport_send_packet_usb_cdc;
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

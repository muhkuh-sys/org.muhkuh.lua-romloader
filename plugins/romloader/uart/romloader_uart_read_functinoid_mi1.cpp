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


#include <stdio.h>

#include "romloader_uart_read_functinoid_mi1.h"

/* Load- and entry points for the monitor firmware. */
#include "netx/targets/uartmon_netx56_monitor_run.h"

/* Data of the monitor firmware. */
#include "netx/targets/uartmon_netx56_monitor.h"


#define UART_BASE_TIMEOUT_MS 500
#define UART_CHAR_TIMEOUT_MS 50


#define MI1_MONITOR_COMMAND_MSK     0x0fU
#define MI1_MONITOR_COMMAND_SRT     0U
#define MI1_MONITOR_ACCESSSIZE_MSK  0x30U
#define MI1_MONITOR_ACCESSSIZE_SRT  4U
#define MI1_MONITOR_STATUS_MSK      0x3fU
#define MI1_MONITOR_STATUS_SRT      0U
#define MI1_MONITOR_SEQUENCE_MSK    0xc0U
#define MI1_MONITOR_SEQUENCE_SRT    6U

typedef enum
{
	MI1_MONITOR_COMMAND_Read                    = 0x00,
	MI1_MONITOR_COMMAND_Write                   = 0x01,
	MI1_MONITOR_COMMAND_Execute                 = 0x02,
	MI1_MONITOR_COMMAND_Magic                   = 0xff
} MI1_MONITOR_COMMAND_T;

typedef enum
{
	MI1_MONITOR_STATUS_Ok                        = 0x00,
	MI1_MONITOR_STATUS_CallMessage               = 0x01,
	MI1_MONITOR_STATUS_CallFinished              = 0x02,
	MI1_MONITOR_STATUS_InvalidCommand            = 0x03,
	MI1_MONITOR_STATUS_InvalidPacketSize         = 0x04,
	MI1_MONITOR_STATUS_InvalidSizeParameter      = 0x05
} MI1_MONITOR_STATUS_T;

typedef enum
{
	MI1_MONITOR_ACCESSSIZE_Byte                  = 0,
	MI1_MONITOR_ACCESSSIZE_Word                  = 1,
	MI1_MONITOR_ACCESSSIZE_Long                  = 2
} MI1_MONITOR_ACCESSSIZE_T;



romloader_uart_read_functinoid_mi1::romloader_uart_read_functinoid_mi1(romloader_uart_device *ptDevice, char *pcPortName)
{
	m_ptUartDev = ptDevice;
	m_pcPortName = pcPortName;

	packet_ringbuffer_init();
}



unsigned long romloader_uart_read_functinoid_mi1::read_data32(unsigned long ulNetxAddress)
{
	unsigned char aucCommand[6];
	UARTSTATUS_T tResult;
	unsigned long ulValue;
	bool fOk;


	/* Expect error. */
	fOk = false;

	ulValue = 0;

	aucCommand[0] = MI1_MONITOR_COMMAND_Read | (MI1_MONITOR_ACCESSSIZE_Long<<6);
	aucCommand[1] = 4;
	aucCommand[2] =  ulNetxAddress      & 0xffU;
	aucCommand[3] = (ulNetxAddress>>8 ) & 0xffU;
	aucCommand[4] = (ulNetxAddress>>16) & 0xffU;
	aucCommand[5] = (ulNetxAddress>>24) & 0xffU;
	tResult = execute_command(aucCommand, 6);
	if( tResult==UARTSTATUS_OK )
	{
		if( m_sizPacketInputBuffer==4+5 )
		{
			ulValue = m_aucPacketInputBuffer[3] |
				m_aucPacketInputBuffer[4]<<8 |
				m_aucPacketInputBuffer[5]<<16 |
				m_aucPacketInputBuffer[6]<<24;
			fOk = true;
		}
	}

	return ulValue;
}



bool romloader_uart_read_functinoid_mi1::write_image(unsigned long ulNetxAddress, const unsigned char *pucBUFFER_IN, size_t sizBUFFER_IN)
{
	bool fOk;
	size_t sizChunk;
	UARTSTATUS_T tResult;
	const size_t sizMaxPacketSizeClient = 256;
	unsigned char aucCommand[sizMaxPacketSizeClient];
	long lBytesProcessed;


	/* Be optimistic. */
	fOk = true;

	lBytesProcessed = 0;
	do
	{
		sizChunk = sizBUFFER_IN;
		if( sizChunk>sizMaxPacketSizeClient-11 )
		{
			sizChunk = sizMaxPacketSizeClient-11;
		}

		aucCommand[0] = MI1_MONITOR_COMMAND_Write |
				(MI1_MONITOR_ACCESSSIZE_Byte<<MI1_MONITOR_ACCESSSIZE_SRT);
		aucCommand[1] = sizChunk;
		aucCommand[2] = (unsigned char)( ulNetxAddress       & 0xffU);
		aucCommand[3] = (unsigned char)((ulNetxAddress>> 8U) & 0xffU);
		aucCommand[4] = (unsigned char)((ulNetxAddress>>16U) & 0xffU);
		aucCommand[5] = (unsigned char)((ulNetxAddress>>24U) & 0xffU);
		memcpy(aucCommand+6, pucBUFFER_IN, sizChunk);
		tResult = execute_command(aucCommand, sizChunk+6);
		if( tResult!=UARTSTATUS_OK )
		{
			fprintf(stderr, "failed to execute command!\n");
			fOk = false;
			break;
		}
		else
		{
			if( m_sizPacketInputBuffer!=4+1 )
			{
				fprintf(stderr, "answer to write_data08 has wrong packet size of %d!\n", m_sizPacketInputBuffer);
				fOk = false;
				break;
			}
			else
			{
				pucBUFFER_IN += sizChunk;
				sizBUFFER_IN -= sizChunk;
				ulNetxAddress += sizChunk;
				lBytesProcessed += sizChunk;
			}
		}
	} while( sizBUFFER_IN!=0 );
	
	return fOk;
}



bool romloader_uart_read_functinoid_mi1::call(unsigned long ulNetxAddress, unsigned long ulParameterR0)
{
	bool fOk;
	UARTSTATUS_T tResult;
	unsigned char aucCommand[9];
	unsigned char ucStatus;


	/* Construct the command packet. */
	aucCommand[0x00] = MI1_MONITOR_COMMAND_Execute;
	aucCommand[0x01] = (unsigned char)( ulNetxAddress      & 0xffU);
	aucCommand[0x02] = (unsigned char)((ulNetxAddress>>8 ) & 0xffU);
	aucCommand[0x03] = (unsigned char)((ulNetxAddress>>16) & 0xffU);
	aucCommand[0x04] = (unsigned char)((ulNetxAddress>>24) & 0xffU);
	aucCommand[0x05] = (unsigned char)( ulParameterR0      & 0xffU);
	aucCommand[0x06] = (unsigned char)((ulParameterR0>>8 ) & 0xffU);
	aucCommand[0x07] = (unsigned char)((ulParameterR0>>16) & 0xffU);
	aucCommand[0x08] = (unsigned char)((ulParameterR0>>24) & 0xffU);

	printf("Executing call command:\n");
	hexdump(aucCommand, 9);
	tResult = execute_command(aucCommand, 9);
	if( tResult!=UARTSTATUS_OK )
	{
		fprintf(stderr, "failed to execute command!");
		fOk = false;
	}
	else
	{
		if( m_sizPacketInputBuffer!=4+1 )
		{
			fprintf(stderr, "answer to call command has wrong packet size of %d!", m_sizPacketInputBuffer);
			fOk = false;
		}
		else
		{
			fOk = true;
		}
	}
	
	return fOk;
}



int romloader_uart_read_functinoid_mi1::update_device(ROMLOADER_CHIPTYP tChiptyp)
{
	int iResult;
	bool fOk;
	
	
	/* Expect failure. */
	iResult = -1;

	switch(tChiptyp)
	{
	case ROMLOADER_CHIPTYP_NETX56:
		fprintf(stderr, "update netx56.\n");

		fOk = write_image(MONITOR_DATA_START_NETX56, auc_uartmon_netx56_monitor, sizeof(auc_uartmon_netx56_monitor));
		{
			fOk = call(MONITOR_EXEC_NETX56, 0);
			printf("start: %d\n", fOk);
			if( fOk==true )
			{
				/* The ROM code was updated. */
				iResult = 0;
			}
		}
		break;


	case ROMLOADER_CHIPTYP_NETX10:
	case ROMLOADER_CHIPTYP_NETX50:
	case ROMLOADER_CHIPTYP_NETX500:
	case ROMLOADER_CHIPTYP_NETX100:
	default:
		/* No idea how to update this one! */
		fprintf(stderr, "%s(%p): No strategy to update chip type %d!\n", m_pcPortName, this, tChiptyp);
		break;
	}
	
	return iResult;
}



romloader_uart_read_functinoid_mi1::UARTSTATUS_T romloader_uart_read_functinoid_mi1::execute_command(const unsigned char *aucCommand, size_t sizCommand)
{
	UARTSTATUS_T tResult;
	unsigned char ucStatus;
	unsigned int uiRetryCnt;


	uiRetryCnt = 10;
	do
	{
		tResult = send_packet(aucCommand, sizCommand);
		if( tResult!=UARTSTATUS_OK )
		{
			fprintf(stderr, "! execute_command: send_packet failed with errorcode %d\n", tResult);
		}
		else
		{
			tResult = receive_packet();
			if( tResult!=UARTSTATUS_OK )
			{
				fprintf(stderr, "! execute_command: receive_packet failed with errorcode %d\n", tResult);
			}
			else
			{
				if( m_sizPacketInputBuffer<5 )
				{
					fprintf(stderr, "Error: received no user data!\n");
					tResult = UARTSTATUS_MISSING_USERDATA;
				}
				else
				{
					ucStatus = m_aucPacketInputBuffer[2];
					if( ucStatus==0 )
					{
						tResult = UARTSTATUS_OK;
					}
					else
					{
						fprintf(stderr, "Error: status is not ok: %d\n", ucStatus);
						tResult = UARTSTATUS_COMMAND_EXECUTION_FAILED;
					}
				}
			}
		}

		if( tResult!=UARTSTATUS_OK )
		{
			--uiRetryCnt;
			if( uiRetryCnt==0 )
			{
				fprintf(stderr, "Retried 10 times and nothing happened. Giving up!\n");
				break;
			}
			else
			{
				fprintf(stderr, "***************************************\n");
				fprintf(stderr, "*                                     *\n");
				fprintf(stderr, "*            retry                    *\n");
				fprintf(stderr, "*                                     *\n");
				fprintf(stderr, "***************************************\n");
			}
		}

	} while( tResult!=UARTSTATUS_OK );

	return tResult;
}


void romloader_uart_read_functinoid_mi1::packet_ringbuffer_init(void)
{
	m_sizPacketRingBufferHead = 0;
	m_sizPacketRingBufferFill = 0;
}


romloader_uart_read_functinoid_mi1::UARTSTATUS_T romloader_uart_read_functinoid_mi1::packet_ringbuffer_fill(size_t sizRequestedFillLevel)
{
	size_t sizWritePosition;
	UARTSTATUS_T tResult;
	size_t sizReceiveCnt;
	size_t sizChunk;
	size_t sizRead;


	/* Expect success. */
	tResult = UARTSTATUS_OK;

	/* Does the buffer contain enough data? */
	if( m_sizPacketRingBufferFill<sizRequestedFillLevel )
	{
		/* No -> receive the remaining amount of bytes. */
		sizReceiveCnt = sizRequestedFillLevel - m_sizPacketRingBufferFill;
		do
		{
			/* Get the write position. */
			sizWritePosition = m_sizPacketRingBufferHead + m_sizPacketRingBufferFill;
			if( sizWritePosition>=sizMaxPacketSizeMI1 )
			{
				sizWritePosition -= sizMaxPacketSizeMI1;
			}

			/* Get the size of the remaining continuous buffer space. */
			/* This is the maximum chunk which can be read in one piece. */
			sizChunk = sizMaxPacketSizeMI1 - sizWritePosition;
			/* Limit the chunk size to the requested size. */
			if( sizChunk>sizReceiveCnt )
			{
				sizChunk = sizReceiveCnt;
			}

			/* Receive the chunk. */
			sizRead = m_ptUartDev->RecvRaw(m_aucPacketRingBuffer+sizWritePosition, sizChunk, UART_BASE_TIMEOUT_MS + sizChunk*UART_CHAR_TIMEOUT_MS);

			m_sizPacketRingBufferFill += sizRead;
			sizReceiveCnt -= sizRead;

			if( sizRead!=sizChunk )
			{
//				fprintf(stderr, "TIMEOUT: requested %d bytes, got only %d.\n", sizChunk, sizRead);
				tResult = UARTSTATUS_TIMEOUT;
				break;
			}
		} while( sizReceiveCnt>0 );
	}

	return tResult;
}


unsigned char romloader_uart_read_functinoid_mi1::packet_ringbuffer_get(void)
{
	unsigned char ucByte;


	ucByte = m_aucPacketRingBuffer[m_sizPacketRingBufferHead];

	++m_sizPacketRingBufferHead;
	if( m_sizPacketRingBufferHead>=sizMaxPacketSizeMI1 )
	{
		m_sizPacketRingBufferHead -= sizMaxPacketSizeMI1;
	}

	--m_sizPacketRingBufferFill;

	return ucByte;
}


int romloader_uart_read_functinoid_mi1::packet_ringbuffer_peek(size_t sizOffset)
{
	size_t sizReadPosition;


	sizReadPosition = m_sizPacketRingBufferHead + sizOffset;
	if( sizReadPosition>=sizMaxPacketSizeMI1 )
	{
		sizReadPosition -= sizMaxPacketSizeMI1;
	}

	return m_aucPacketRingBuffer[sizReadPosition];
}


romloader_uart_read_functinoid_mi1::UARTSTATUS_T romloader_uart_read_functinoid_mi1::send_packet(const unsigned char *pucData, size_t sizData)
{
	unsigned char aucBuffer[sizMaxPacketSizeMI1];
	UARTSTATUS_T tResult;
	unsigned short usCrc;
	const unsigned char *pucCnt;
	const unsigned char *pucEnd;
	size_t sizSend;
	size_t sizPacket;


	/* The maximum packet size is reduced by the start char (1 byte), the size information (2 bytes) and the crc (2 bytes). */
	if( sizData<=sizMaxPacketSizeMI1-5 )
	{
		/* Set the packet start character. */
		aucBuffer[0] = MONITOR_STREAM_PACKET_START;
		/* Set the size of the user data. */
		aucBuffer[1] = (unsigned char)(sizData & 0xff);
		aucBuffer[2] = (unsigned char)(sizData >> 8);

		/* Copy the user data. */
		memcpy(aucBuffer+3, pucData, sizData);

		/* Generate the CRC. */
		usCrc = 0;
		pucCnt = aucBuffer + 1;
		pucEnd = aucBuffer + 1 + 2 + sizData;
		while( pucCnt<pucEnd )
		{
			usCrc = crc16(usCrc, *pucCnt);
			++pucCnt;
		}

		/* Append the CRC. */
		aucBuffer[sizData+3] = (usCrc>>8 ) & 0xffU;
		aucBuffer[sizData+4] =  usCrc      & 0xffU;

		/* Send the buffer. */
		sizPacket = sizData + 5;
		sizSend = m_ptUartDev->SendRaw(aucBuffer, sizPacket, UART_BASE_TIMEOUT_MS + sizPacket*UART_CHAR_TIMEOUT_MS);
		if( sizSend==sizPacket )
		{
			tResult = UARTSTATUS_OK;
		}
		else
		{
			fprintf(stderr, "! send_packet: failed to send the packet!\n");
			tResult = UARTSTATUS_SEND_FAILED;
		}
	}
	else
	{
		fprintf(stderr, "! send_packet: packet too large: %d bytes!\n", sizData);
		tResult = UARTSTATUS_PACKET_TOO_LARGE;
	}

	return tResult;
}


romloader_uart_read_functinoid_mi1::UARTSTATUS_T romloader_uart_read_functinoid_mi1::receive_packet(void)
{
	unsigned int uiRetries;
	unsigned char ucData;
	bool fFound;
	UARTSTATUS_T tResult;
	size_t sizData;
	size_t sizPacket;
	unsigned short usCrc;
	size_t sizCnt;


	/* Wait for the start character. */
	fFound = false;
	uiRetries = 10;
	do
	{
		tResult = packet_ringbuffer_fill(1);
		if( tResult==UARTSTATUS_OK )
		{
			ucData = packet_ringbuffer_get();
			if( ucData==MONITOR_STREAM_PACKET_START )
			{
				fFound = true;
				break;
			}
			else
			{
				fprintf(stderr, "WARNING: Skipping char 0x%02x.\n", ucData);
			}
		}
		--uiRetries;
	} while( uiRetries>0 );

	if( fFound!=true )
	{
		fprintf(stderr, "receive_packet: no start char found!\n");
		tResult = UARTSTATUS_FAILED_TO_SYNC;
	}
	else
	{
		/* Get the size information. */
		tResult = packet_ringbuffer_fill(2);
		if( tResult==UARTSTATUS_OK )
		{
			sizData  = packet_ringbuffer_peek(0);
			sizData |= packet_ringbuffer_peek(1) << 8;

			/* The complete packet consists of the size info, the user data and the CRC.*/
			sizPacket = 2 + sizData + 2;

			/* Get the rest of the packet. */
			tResult = packet_ringbuffer_fill(sizPacket);
			if( tResult==UARTSTATUS_OK )
			{
				/* Generate the CRC. */
				usCrc = 0;
				sizCnt = 0;
				do
				{
					ucData = packet_ringbuffer_peek(sizCnt);
					usCrc = crc16(usCrc, ucData);
					++sizCnt;
				} while( sizCnt<sizPacket );
				if( usCrc==0 )
				{
					/* Get the complete packet. */
					sizCnt = 0;
					do
					{
						m_aucPacketInputBuffer[sizCnt] = packet_ringbuffer_get();
						++sizCnt;
					} while( sizCnt<sizPacket );

					m_sizPacketInputBuffer = sizPacket;
				}
				else
				{
					fprintf(stderr, "! receive_packet: CRC failed.\n");

					/* Debug: get the complete packet and dump it. */
					printf("packet size: 0x%08x bytes\n", sizPacket);
					sizCnt = 0;
					do
					{
						m_aucPacketInputBuffer[sizCnt] = packet_ringbuffer_get();
						++sizCnt;
					} while( sizCnt<sizPacket );
					printf("Packet data:\n");
					hexdump(m_aucPacketInputBuffer, sizPacket);

					tResult = UARTSTATUS_CRC_MISMATCH;
				}
			}
		}
	}

	return tResult;
}



void romloader_uart_read_functinoid_mi1::hexdump(const unsigned char *pucData, unsigned long ulSize)
{
	const unsigned char *pucDumpCnt, *pucDumpEnd;
	unsigned long ulAddressCnt;
	size_t sizBytesLeft;
	size_t sizChunkSize;
	size_t sizChunkCnt;


	// show a hexdump of the data
	pucDumpCnt = pucData;
	pucDumpEnd = pucData + ulSize;
	ulAddressCnt = 0;
	while( pucDumpCnt<pucDumpEnd )
	{
		// get number of bytes for the next line
		sizChunkSize = 16;
		sizBytesLeft = pucDumpEnd - pucDumpCnt;
		if( sizChunkSize>sizBytesLeft )
		{
			sizChunkSize = sizBytesLeft;
		}

		// start a line in the dump with the address
		printf("%08lX: ", ulAddressCnt);
		// append the data bytes
		sizChunkCnt = sizChunkSize;
		while( sizChunkCnt!=0 )
		{
			printf("%02X ", *(pucDumpCnt++));
			--sizChunkCnt;
		}
		// next line
		printf("\n");
		ulAddressCnt += sizChunkSize;
	}
}

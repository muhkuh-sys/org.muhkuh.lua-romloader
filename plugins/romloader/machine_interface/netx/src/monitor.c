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

#include "asic_types.h"
#include "serial_vectors_bridge.h"
#include "monitor_commands.h"
#include "../../../romloader_def.h"
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


static const unsigned char aucMagic[8] =
{
	/* Magic */
	'M', 'O', 'O', 'H',

	/* Version */
	MONITOR_VERSION_MINOR & 0xff,
	MONITOR_VERSION_MINOR >> 8,
	MONITOR_VERSION_MAJOR & 0xff,
	MONITOR_VERSION_MAJOR >> 8

	/* NOTE: The next 3 bytes define the chip type and the maximum packet size. */
	/* Chip type */
	/* MaxPacketSize 0..7 */
	/* MaxPacketSize 8..15 */
};

static unsigned char ucChiptype;

static unsigned char ucSequenceCurrent;
static unsigned char ucSequenceLast;

/*-----------------------------------*/


static void send_status(MONITOR_STATUS_T tStatus)
{
	unsigned char ucStatus;


	/* Write the status to the FIFO. */
	ucStatus  = (unsigned char)tStatus;
	ucStatus |= (unsigned char)(ucSequenceCurrent << MONITOR_SEQUENCE_SRT);
	transport_send_byte(ucStatus);

	/* Send the packet. */
	transport_send_packet();
}


static void command_read_memory(unsigned long ulAddress, unsigned long ulSize, MONITOR_ACCESSSIZE_T tAccessSize)
{
	unsigned char ucStatus;
	ADR_T uAdrCnt;
	ADR_T uAdrEnd;
	unsigned long ulValue;
	int iCnt;


	/* Get the start address. */
	uAdrCnt.ul = ulAddress;
	/* Get the end address. */
	uAdrEnd.ul = ulAddress + ulSize;

	/* Write status "OK" to the FIFO. */
	ucStatus  = (unsigned char)MONITOR_STATUS_Ok;
	ucStatus |= (unsigned char)(ucSequenceCurrent << MONITOR_SEQUENCE_SRT);
	transport_send_byte(ucStatus);

	/* Write data bytes to the FIFO. */
	do
	{
		/* Initialize the output value. */
		ulValue = 0;

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

		/* Add the data byte-by-byte to the FIFO. */
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

	/* Flush any remaining bytes in the FIFO. */
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


static void next_sequence_number(void)
{
	ucSequenceLast = ucSequenceCurrent;
	ucSequenceCurrent = (unsigned char)((ucSequenceCurrent + 1U) & (MONITOR_SEQUENCE_MSK>>MONITOR_SEQUENCE_SRT));
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


#define ADR_NETX_VERSION_56       0x080f0008
#define VAL_NETX_VERSION_56A      0x00006003
#define VAL_NETX_VERSION_56B      0x00106003

#define ADR_NETX_VERSION_500_100  0x00200008
#define VAL_NETX_VERSION_500      0x00001000
#define VAL_NETX_VERSION_100      0x00003002

void monitor_init(void)
{
#if ASIC_TYP==ASIC_TYP_NETX500 || ASIC_TYP==ASIC_TYP_NETX56
	unsigned long ulNetxVersion;
#endif


	/* Set the vectors. */
	memcpy(&tSerialV2Vectors, &tCallConsole, sizeof(SERIAL_V2_COMM_UI_FN_T));

	/* Initialize the sequence numbers. */
	ucSequenceCurrent = 0x00U;
	ucSequenceLast = 0xffU;

	/* Set the chip type */
#if ASIC_TYP==ASIC_TYP_NETX500
	ulNetxVersion = *((unsigned long*)ADR_NETX_VERSION_500_100);
	if (ulNetxVersion == VAL_NETX_VERSION_500)
	{
		ucChiptype = (unsigned char)ROMLOADER_CHIPTYP_NETX500;
	}
	else if (ulNetxVersion == VAL_NETX_VERSION_100)
	{
		ucChiptype = (unsigned char)ROMLOADER_CHIPTYP_NETX100;
	}
#elif ASIC_TYP==ASIC_TYP_NETX50
	ucChiptype = (unsigned char)ROMLOADER_CHIPTYP_NETX50;
#elif ASIC_TYP==ASIC_TYP_NETX10
	ucChiptype = (unsigned char)ROMLOADER_CHIPTYP_NETX10;
#elif ASIC_TYP==ASIC_TYP_NETX56
	ulNetxVersion = *((unsigned long*)ADR_NETX_VERSION_56);
	if (ulNetxVersion == VAL_NETX_VERSION_56A)
	{
		ucChiptype = (unsigned char)ROMLOADER_CHIPTYP_NETX56;
	}
	else if (ulNetxVersion == VAL_NETX_VERSION_56B)
	{
		ucChiptype = (unsigned char)ROMLOADER_CHIPTYP_NETX56B;
	}
#else
#       error "Unknown ASIC type!"
#endif
}


void monitor_process_packet(const unsigned char *pucPacket, unsigned long ulPacketSize, unsigned short usMaxpacketSize)
{
	unsigned char ucCommand;
	MONITOR_COMMAND_T tCmd;
	unsigned long ulDataSize;
	unsigned long ulAddress;
	MONITOR_ACCESSSIZE_T tAccessSize;
	unsigned char ucSequence;
	unsigned long ulR0;


	ucCommand = pucPacket[0];
	if( ucCommand==MONITOR_COMMAND_Magic )
	{
		monitor_send_magic(usMaxpacketSize);
	}
	else
	{
		/* Get the command and the data size from the first byte. */
		tCmd = (MONITOR_COMMAND_T)((ucCommand&MONITOR_COMMAND_MSK)>>MONITOR_COMMAND_SRT);
		tAccessSize = (MONITOR_ACCESSSIZE_T)((ucCommand&MONITOR_ACCESSSIZE_MSK)>>MONITOR_ACCESSSIZE_SRT);
		ucSequence = (ucCommand&MONITOR_SEQUENCE_MSK)>>MONITOR_SEQUENCE_SRT;
		ulDataSize  = ((unsigned long)pucPacket[1]);
		ulDataSize |= ((unsigned long)pucPacket[2]) << 8U;


		if( ucSequence==ucSequenceCurrent )
		{
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
				ulAddress = get_unaligned_dword(pucPacket + 3U);

				if( ulPacketSize!=7 )
				{
					send_status(MONITOR_STATUS_InvalidPacketSize);
				}
				else if( ulDataSize>usMaxpacketSize-1U )
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
				ulAddress = get_unaligned_dword(pucPacket + 3U);

				if( ulPacketSize!=(7U+ulDataSize) )
				{
					send_status(MONITOR_STATUS_InvalidPacketSize);
				}
				else
				{
					command_write_memory(pucPacket+7U, ulAddress, ulDataSize, tAccessSize);
				}
			}
			else
			{
				send_status(MONITOR_STATUS_InvalidCommand);
			}

			next_sequence_number();
		}
		else if( ucSequence==ucSequenceLast )
		{
			/* This is the last transfer's sequence number.
			 * Send the last packet again.
			 */
			transport_resend_packet();
		}
		else
		{
			send_status(MONITOR_STATUS_InvalidSequenceNumber);
		}
	}
}



void monitor_send_magic(unsigned short usMaxpacketSize)
{
	unsigned char ucStatus;
	size_t sizCnt;


	/* Send the status "OK". */
	ucStatus  = (unsigned char)MONITOR_STATUS_Ok;
	ucStatus |= (unsigned char)(ucSequenceCurrent << MONITOR_SEQUENCE_SRT);
	transport_send_byte(ucStatus);

	/* Send the complete magic sequence. */
	for(sizCnt=0; sizCnt<sizeof(aucMagic); ++sizCnt)
	{
		transport_send_byte(aucMagic[sizCnt]);
	}

	/* Add the chip type */
	transport_send_byte(ucChiptype);

	/* Add the maximum packet size. */
	transport_send_byte((unsigned char)( usMaxpacketSize     &0xffU));
	transport_send_byte((unsigned char)((usMaxpacketSize>>8U)&0xffU));

	/* Send the packet. */
	transport_send_packet();

	next_sequence_number();
}


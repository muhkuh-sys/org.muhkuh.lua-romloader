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

#include "netx_io_areas.h"
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


static unsigned char ucChiptype;

//static unsigned char ucSequenceCurrent;
//static unsigned char ucSequenceLast;

/*-----------------------------------*/


static void send_status(MONITOR_STATUS_T tStatus)
{
	/* Write the status to the FIFO. */
	transport_send_byte(MONITOR_PACKET_TYP_Status);
	transport_send_byte(tStatus);

	/* Send the packet. */
	transport_send_packet(TRANSPORT_SEND_PACKET_WITH_ACK);
}


static void command_read_memory(unsigned long ulAddress, unsigned long ulSize, MONITOR_ACCESSSIZE_T tAccessSize)
{
	ADR_T uAdrCnt;
	ADR_T uAdrEnd;
	unsigned char ucValue;
	unsigned short usValue;
	unsigned long ulValue;


	/* Get the start address. */
	uAdrCnt.ul = ulAddress;
	/* Get the end address. */
	uAdrEnd.ul = ulAddress + ulSize;

	/* Skip...
	 *   1 byte packet type
	 *   2 bytes data size
	 *   4 bytes source address
	 *   2 bytes CRC
	 */
	transport_buffer_skip(9U);

	transport_send_byte(MONITOR_PACKET_TYP_ReadData);

	/* Write data bytes to the FIFO. */
	switch(tAccessSize)
	{
	case MONITOR_ACCESSSIZE_Byte:
		while( uAdrCnt.ul<uAdrEnd.ul)
		{
			ucValue = *(uAdrCnt.puc++);
			transport_send_byte(ucValue);
		}
		break;

	case MONITOR_ACCESSSIZE_Word:
		while( uAdrCnt.ul<uAdrEnd.ul)
		{
			usValue = *(uAdrCnt.pus++);
			transport_send_byte((unsigned char)( usValue       & 0xffU));
			transport_send_byte((unsigned char)((usValue>> 8U) & 0xffU));
		}
		break;

	case MONITOR_ACCESSSIZE_Long:
		while( uAdrCnt.ul<uAdrEnd.ul)
		{
			ulValue = *(uAdrCnt.pul++);
			transport_send_byte((unsigned char)( ulValue       & 0xffU));
			transport_send_byte((unsigned char)((ulValue>> 8U) & 0xffU));
			transport_send_byte((unsigned char)((ulValue>>16U) & 0xffU));
			transport_send_byte((unsigned char)((ulValue>>24U) & 0xffU));
		}
		break;

	case MONITOR_ACCESSSIZE_Any:
		transport_send_bytes(uAdrCnt.puc, ulSize);
		break;
	}

	/* Send the packet. */
	transport_send_packet(TRANSPORT_SEND_PACKET_WITH_ACK);
}



static void command_write_memory(unsigned long ulAddress, unsigned long ulDataSize, MONITOR_ACCESSSIZE_T tAccessSize)
{
	unsigned int uiOffsetCnt;
	unsigned int uiOffsetEnd;
	ADR_T uAdrDst;
	unsigned char ucValue;
	unsigned short usValue;
	unsigned long ulValue;


	/* Skip...
	 *   1 byte packet type
	 *   2 bytes data size
	 *   4 bytes destination address
	 */
	transport_buffer_skip(7U);

	/* Get the start offset in the receive buffer. */
	uiOffsetCnt = 0U;
	/* Get the end offset in the receive buffer. */
	uiOffsetEnd = ulDataSize;
	/* Get the destination end address. */
	uAdrDst.ul = ulAddress;

	/* Map the "ANY" access to the best fitting. */
	if( tAccessSize==MONITOR_ACCESSSIZE_Any )
	{
		if( (ulAddress&3U)==0U )
		{
			tAccessSize = MONITOR_ACCESSSIZE_Long;
		}
		else if( (ulAddress&1U)==0U )
		{
			tAccessSize = MONITOR_ACCESSSIZE_Word;
		}
		else
		{
			tAccessSize = MONITOR_ACCESSSIZE_Byte;
		}
	}

	/* Write data bytes to memory. */
	switch(tAccessSize)
	{
	case MONITOR_ACCESSSIZE_Byte:
	case MONITOR_ACCESSSIZE_Any:
		while( uiOffsetCnt<uiOffsetEnd )
		{
			ucValue = transport_buffer_peek(uiOffsetCnt++);
			*(uAdrDst.puc++) = ucValue;
		}
		break;

	case MONITOR_ACCESSSIZE_Word:
		while( uiOffsetCnt<uiOffsetEnd )
		{
			usValue  =                  transport_buffer_peek(uiOffsetCnt++);
			usValue |= (unsigned short)(transport_buffer_peek(uiOffsetCnt++) << 8U);
			*(uAdrDst.pus++) = usValue;
		}
		break;

	case MONITOR_ACCESSSIZE_Long:
		while( uiOffsetCnt<uiOffsetEnd )
		{
			ulValue  =                 transport_buffer_peek(uiOffsetCnt++);
			ulValue |= (unsigned long)(transport_buffer_peek(uiOffsetCnt++) <<  8U);
			ulValue |= (unsigned long)(transport_buffer_peek(uiOffsetCnt++) << 16U);
			ulValue |= (unsigned long)(transport_buffer_peek(uiOffsetCnt++) << 24U);
			*(uAdrDst.pul++) = ulValue;
		}
		break;
	}

	/* Skip the data and CRC. */
	transport_buffer_skip(ulDataSize + 2U);

	/* Send the status packet. */
	send_status(MONITOR_STATUS_Ok);
}



static void command_call(unsigned long ulAddress, unsigned long ulR0)
{
	PFN_MONITOR_CALL_T ptCall;


	ptCall = (PFN_MONITOR_CALL_T)ulAddress;

	/* Skip...
	 *  1 byte packet type
	 *  4 bytes address
	 *  4 bytes r0 value
	 *  2 bytes CRC
	 */
	transport_buffer_skip(11U);

	/* Send the status packet. */
	send_status(MONITOR_STATUS_Ok);

	/* Start the new message packet. */
	transport_send_byte(MONITOR_PACKET_TYP_CallMessage);

	/* Call the routine. */
	ptCall(ulR0);

	/* Flush any remaining bytes in the FIFO. */
	transport_send_packet(TRANSPORT_SEND_PACKET_WITH_ACK);

	/* The call finished, notify the PC. */
	send_status(MONITOR_STATUS_CallFinished);
}


static unsigned long get_unaligned_dword(unsigned int uiOffset)
{
	unsigned long ulValue;


	ulValue  =                 transport_buffer_peek(uiOffset);
	ulValue |= (unsigned long)(transport_buffer_peek(uiOffset+1)<< 8U);
	ulValue |= (unsigned long)(transport_buffer_peek(uiOffset+2)<<16U);
	ulValue |= (unsigned long)(transport_buffer_peek(uiOffset+3)<<24U);

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

#define ADR_NETX_VERSION_56             0x080f0008
#define VAL_NETX_VERSION_56A            0x00006003
#define VAL_NETX_VERSION_56B            0x00106003
#define ADR_NETX_VERSION_4000           0x04100020
#define VAL_NETX_VERSION_4000_RELAXED   0x00108004
#define VAL_NETX_VERSION_4000_FULL      0x0010b004

#define ADR_NETX_VERSION_500_100        0x00200008
#define VAL_NETX_VERSION_500            0x00001000
#define VAL_NETX_VERSION_100            0x00003002

void monitor_init(void)
{
#if ASIC_TYP==ASIC_TYP_NETX500 || ASIC_TYP==ASIC_TYP_NETX56 || ASIC_TYP==ASIC_TYP_NETX4000_RELAXED
	unsigned long ulNetxVersion;
#endif


	/* Set the vectors. */
	memcpy(&tSerialV2Vectors, &tCallConsole, sizeof(SERIAL_V2_COMM_UI_FN_T));

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
#elif ASIC_TYP==ASIC_TYP_NETX4000_RELAXED
	ulNetxVersion = *((unsigned long*)ADR_NETX_VERSION_4000);
	if (ulNetxVersion == VAL_NETX_VERSION_4000_RELAXED)
	{
		ucChiptype = (unsigned char)ROMLOADER_CHIPTYP_NETX4000_RELAXED;
	}
	else if (ulNetxVersion == VAL_NETX_VERSION_4000_FULL)
	{
		/* On the netx 4000 full/small, an OTP fuse bit indicates the package type. */
		NX4000_DEF_ptRAPSysctrlArea
		if ((ptRAPSysctrlArea->aulRAP_SYSCTRL_OTP_CONFIG_[0] & 0x1UL) == 0UL)
		{
			ucChiptype = (unsigned char)ROMLOADER_CHIPTYP_NETX4000_FULL;
		}
		else
		{
			ucChiptype = (unsigned char)ROMLOADER_CHIPTYP_NETX4100_SMALL;
		}
	}
#else
#       error "Unknown ASIC type!"
#endif
}


void monitor_process_packet(unsigned long ulPacketSize, unsigned short usMaxpacketSize)
{
	unsigned char ucCommand;
	MONITOR_PACKET_TYP_T tPacketTyp;
	unsigned long ulDataSize;
	unsigned long ulAddress;
	MONITOR_ACCESSSIZE_T tAccessSize;
//	unsigned char ucSequence;
	unsigned long ulR0;


	ucCommand = transport_buffer_peek(0);
	if( ucCommand==MONITOR_PACKET_TYP_CommandMagic )
	{
		monitor_send_magic(usMaxpacketSize);
	}
	else
	{
		/* Get the packet type from the first byte. */
		tPacketTyp = (MONITOR_PACKET_TYP_T)((ucCommand&MONITOR_PACKET_TYP_MSK)>>MONITOR_PACKET_TYP_SRT);

		if( tPacketTyp==MONITOR_PACKET_TYP_CommandExecute )
		{
			if( ulPacketSize!=9U )
			{
				send_status(MONITOR_STATUS_InvalidPacketSize);
			}
			else
			{
				/* Get the address. */
				ulAddress = get_unaligned_dword(1U);

				ulR0 = get_unaligned_dword(5U);
				command_call(ulAddress, ulR0);
			}
		}
		else
		{
			/* Get the access size. */
			tAccessSize = (MONITOR_ACCESSSIZE_T)((ucCommand&MONITOR_ACCESSSIZE_MSK)>>MONITOR_ACCESSSIZE_SRT);

			/* Read the data size. */
			ulDataSize  =  (unsigned long)transport_buffer_peek(1);
			ulDataSize |= ((unsigned long)transport_buffer_peek(2)) << 8U;

			/* Get the address. */
			ulAddress = get_unaligned_dword(3U);

			if( tPacketTyp==MONITOR_PACKET_TYP_CommandRead )
			{
				if( ulPacketSize!=7U )
				{
					send_status(MONITOR_STATUS_InvalidPacketSize);
				}
				else if( ulDataSize>(usMaxpacketSize-1U) )
				{
					send_status(MONITOR_STATUS_InvalidSizeParameter);
				}
				else
				{
					command_read_memory(ulAddress, ulDataSize, tAccessSize);
				}
			}
			else if( tPacketTyp==MONITOR_PACKET_TYP_CommandWrite )
			{
				if( ulPacketSize!=(7U+ulDataSize) )
				{
					send_status(MONITOR_STATUS_InvalidPacketSize);
				}
				else
				{
					command_write_memory(ulAddress, ulDataSize, tAccessSize);
				}
			}
			else
			{
				send_status(MONITOR_STATUS_InvalidCommand);
			}
		}
	}
}



static const unsigned char aucMagic[9] =
{
	MONITOR_STATUS_Ok,

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



void monitor_send_magic(unsigned short usMaxpacketSize)
{
	/* Send the fixed part of the magic sequence. */
	transport_send_bytes(aucMagic, sizeof(aucMagic));

	/* Add the chip type */
	transport_send_byte(ucChiptype);

	/* Add the maximum packet size. */
	transport_send_byte((unsigned char)( usMaxpacketSize     &0xffU));
	transport_send_byte((unsigned char)((usMaxpacketSize>>8U)&0xffU));

	/* Send the packet. */
	transport_send_packet(TRANSPORT_SEND_PACKET_WITHOUT_ACK);
}


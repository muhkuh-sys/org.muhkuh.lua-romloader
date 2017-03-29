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
#include <stdint.h>


#include "monitor.h"
#include "monitor_commands.h"
#include "serial_vectors.h"
//#include "systime.h"

#include "globals.h"

/*-------------------------------------------------------------------------*/

#define DPM_BOOT_NETX_RECEIVED_CMD      0x01
#define DPM_BOOT_NETX_SEND_CMD          0x02

#define DPM_BOOT_HOST_SEND_CMD          0x01
#define DPM_BOOT_HOST_RECEIVED_CMD      0x02

#define SRT_HANDSHAKE_REG_ARM_DATA      16
#define SRT_HANDSHAKE_REG_PC_DATA       24

/* The ID "MMON" shows that the monitor is running. */
#define BOOT_ID_MONITOR 0x4e4f4d4d

#define DPM_NETX_TO_HOST_BUFFERSIZE     0x0200
#define DPM_HOST_TO_NETX_BUFFERSIZE     0x0400

typedef struct HBOOT_DPM_STRUCT
{
	volatile uint32_t ulDpmBootId;
	volatile uint32_t ulDpmByteSize;
	volatile uint32_t ulSdramExtGeneralCtrl;
	volatile uint32_t ulSdramExtTimingCtrl;
	volatile uint32_t ulSdramExtByteSize;
	volatile uint32_t ulSdramHifGeneralCtrl;
	volatile uint32_t ulSdramHifTimingCtrl;
	volatile uint32_t ulSdramHifByteSize;
	volatile uint32_t aulReserved_20[22];
	volatile uint32_t ulNetxToHostDataSize;
	volatile uint32_t ulHostToNetxDataSize;
	volatile uint32_t ulHandshake;
	volatile uint32_t aulReserved_84[31];
	volatile uint8_t aucNetxToHostData[DPM_NETX_TO_HOST_BUFFERSIZE];
	volatile uint8_t aucHostToNetxData[DPM_HOST_TO_NETX_BUFFERSIZE];
} HBOOT_DPM_T;

uint8_t aucNetMonToImageData[1]; // ???
uint8_t aucImageToNetMonData[1]; // ???

static HBOOT_DPM_T tDpm __attribute__ ((section (".dpm")));

/*-------------------------------------------------------------------------*/

size_t sizStreamBufferHead;
size_t sizStreamBufferFill;

size_t sizPacketOutputFill;
size_t sizPacketOutputFillLast;

size_t sizPacketOutputEmpty; // ???
size_t sizPacketOutputEmptyFirst; // ???

/*-------------------------------------------------------------------------*/


static void dpm_init(void)
{
	HOSTDEF_PT_DPM_AREA;
//HOSTDEF(ptDpmArea);
	unsigned long ulValue;
	unsigned long ulNetxAdr;


	/* Setup the DPM at the new location. */
	tDpm.ulDpmBootId = BOOT_ID_MONITOR;
	tDpm.ulDpmByteSize = 0x0100 + sizeof(HBOOT_DPM_T);
	tDpm.ulSdramExtGeneralCtrl = 0;
	tDpm.ulSdramExtTimingCtrl = 0;
	tDpm.ulSdramExtByteSize = 0;
	tDpm.ulSdramHifGeneralCtrl = 0;
	tDpm.ulSdramHifTimingCtrl = 0;
	tDpm.ulSdramHifByteSize = 0;
	tDpm.ulNetxToHostDataSize = 0;
	tDpm.ulHostToNetxDataSize = 0;
	tDpm.ulHandshake = 0;


	/* Setup the map for the new DPM location. */
        /* DPM mapping:
         * 0x0000 - 0x00ff : DPM configuration area
         * 0x0100 - 0x017f : dpm_mb_start
         * 0x0180 - 0x01ff : intramhs_dpm_mirror
         * 0x0200 - 0x07ff : dpm_mb_start+0x0100
         */
#if ASIC_TYP==ASIC_TYP_NETX56
	ptDpmArea->asDpm_win[0].ulEnd = 0x017fU + 1;
	ulNetxAdr = (unsigned long)&tDpm;
	ulValue  = (ulNetxAdr-0x0100U) & HOSTMSK(dpm_win1_map_win_map);
	ulValue |= ulNetxAdr & HOSTMSK(dpm_win1_map_win_page);
	ulValue |= HOSTMSK(dpm_win1_map_byte_area)|HOSTMSK(dpm_win1_map_read_ahead);
	ptDpmArea->asDpm_win[0].ulMap = ulValue;

	ptDpmArea->asDpm_win[1].ulEnd = 0x01ffU + 1;
	ulNetxAdr = HOSTADDR(intramhs_dpm_mirror);
	ulValue  = (ulNetxAdr-0x0180U) & HOSTMSK(dpm_win1_map_win_map);
	ulValue |= ulNetxAdr & HOSTMSK(dpm_win1_map_win_page);
	ulValue |= HOSTMSK(dpm_win1_map_byte_area);
	ptDpmArea->asDpm_win[1].ulMap = ulValue;

	ptDpmArea->asDpm_win[2].ulEnd = 0x07ffU + 1;
	ulNetxAdr = ((unsigned long)&tDpm)+0x100;
	ulValue  = (ulNetxAdr-0x0200U) & HOSTMSK(dpm_win1_map_win_map);
	ulValue |= ulNetxAdr & HOSTMSK(dpm_win1_map_win_page);
	ulValue |= HOSTMSK(dpm_win1_map_byte_area)|HOSTMSK(dpm_win1_map_read_ahead);
	ptDpmArea->asDpm_win[2].ulMap = ulValue;

	ptDpmArea->asDpm_win[3].ulEnd = 0U;
	ptDpmArea->asDpm_win[3].ulMap = 0U;
#elif ASIC_TYP==ASIC_TYP_NETX4000_RELAXED
	ptDpmArea->ulDpm_win1_end = 0x017fU + 1; //ulDpm_win1_end
	ulNetxAdr = (unsigned long) &tDpm;
	ulValue = (ulNetxAdr - 0x0100U) & HOSTMSK(dpm_win1_map_win_map);
	ulValue |= ulNetxAdr & HOSTMSK(dpm_win1_map_win_page);
	ulValue |= HOSTMSK(dpm_win1_map_byte_area)|HOSTMSK(dpm_win1_map_read_ahead);
	ptDpmArea->ulDpm_win1_map = ulValue;

	ptDpmArea->ulDpm_win2_end = 0x01ffU + 1;
	ulNetxAdr = HOSTADDR(intramhs_dpm_mirror);
	ulValue = (ulNetxAdr - 0x0180U) & HOSTMSK(dpm_win1_map_win_map);
	ulValue |= ulNetxAdr & HOSTMSK(dpm_win1_map_win_page);
	ulValue |= HOSTMSK(dpm_win1_map_byte_area);
	ptDpmArea->ulDpm_win2_map = ulValue;

	ptDpmArea->ulDpm_win3_end = 0x07ffU + 1;
	ulNetxAdr = ((unsigned long) &tDpm) + 0x100;
	ulValue = (ulNetxAdr - 0x0200U) & HOSTMSK(dpm_win1_map_win_map);
	ulValue |= ulNetxAdr & HOSTMSK(dpm_win1_map_win_page);
	ulValue |= HOSTMSK(dpm_win1_map_byte_area)|HOSTMSK(dpm_win1_map_read_ahead);
	ptDpmArea->ulDpm_win3_map = ulValue;

	ptDpmArea->ulDpm_win4_end = 0U;
	ptDpmArea->ulDpm_win4_map = 0U;
#endif
}

void queue_init(void) 
{
	uiStartPoint = 0;
	uiEndPoint = 0;
	uiSize = 0;
}


static unsigned long mailbox_purr(unsigned long ulMask)
{
	HOSTDEF_PT_HANDSHAKE_ARM_MIRROR_AREA //HOSTDEF(ptHandshakeDtcmArmMirrorArea);
	unsigned long ulValue;
	unsigned long ulHostPart;
	unsigned long ulNetxPart;


	ulValue = PT_HANDSCHAKE_ARM_MIRROR_AREA_HANDSHAKE_REG; //ptHandshakeDtcmArmMirrorArea->aulHandshakeReg[0]; //PT_HANDSCHAKE_ARM_MIRROR_AREA_HANDSHAKE_REG;

	ulHostPart  = ulValue >> SRT_HANDSHAKE_REG_PC_DATA;
	ulHostPart &= 3;
	ulNetxPart  = ulValue >> SRT_HANDSHAKE_REG_ARM_DATA;
	ulNetxPart &= 3;

	/* Check for harmony. */
	ulValue  = ulHostPart ^ ulNetxPart;
	/* Mask */
	ulValue &= ulMask;

	return ulValue;
}


/*-------------------------------------------------------------------------*/

void transport_init(void)
{
	/* Initialize the DPM. */
	dpm_init();

	monitor_init();

	queue_init();

	transport_send_byte(MONITOR_STATUS_Ok);
	transport_send_packet();

	/* Initialize the stream buffer. */
	sizStreamBufferHead = 0;
	sizStreamBufferFill = 0;

	sizPacketOutputFill = 0;
	sizPacketOutputFillLast = 0;
}



void transport_loop(void)
{
	HOSTDEF_PT_HANDSHAKE_ARM_MIRROR_AREA //HOSTDEF(ptHandshakeDtcmArmMirrorArea);
	unsigned long ulValue;
	unsigned long ulPacketSize;


	/* Wait for a packet. */
	do
	{
		ulValue = mailbox_purr(DPM_BOOT_NETX_RECEIVED_CMD);
	} while( ulValue==0 );

	/* Acknowledge the packet. */
	PT_HANDSCHAKE_ARM_MIRROR_AREA_HANDSHAKE_REG /*ptHandshakeDtcmArmMirrorArea->aulHandshakeReg[0]*/ ^=	DPM_BOOT_NETX_RECEIVED_CMD << SRT_HANDSHAKE_REG_ARM_DATA;

	/* Is the packet's size valid? */
	ulPacketSize = tDpm.ulHostToNetxDataSize;
	if( ulPacketSize==0 )
	{
		/* Send magic cookie and version info. */
		monitor_send_magic(DPM_NETX_TO_HOST_BUFFERSIZE);
	}
	else if(ulPacketSize<=DPM_HOST_TO_NETX_BUFFERSIZE )
	{
		/* Yes, the packet size is valid. */
		monitor_process_packet(tDpm.aucHostToNetxData, ulPacketSize, DPM_NETX_TO_HOST_BUFFERSIZE);
	}
}



/**
 * Receive byte??
 * @todo could be made more beautiful with return by param.
 * @return: 0 if queue full (not handled yet).
 */


uint8_t transport_dequeue(void) 
{
	char ucData = 0;
	if( uiSize>=1 ) 
	{
		ucData = ucQueue[uiEndPoint++];
		uiSize--;
	}
	return ucData;
}


void transport_enqueue(unsigned char ucData) 
{
	if( uiSize<=QUEUE_LENGTH ) 
	{
		ucQueue[uiStartPoint++] = ucData;
	}
	uiSize++;
}


void transport_send_byte(unsigned char ucData)
{
	if( sizPacketOutputFill<DPM_HOST_TO_NETX_BUFFERSIZE )
	{
		tDpm.aucNetxToHostData[sizPacketOutputFill] = ucData;
		++sizPacketOutputFill;
	}
/*
	else
	{
		uprintf("discarding byte 0x%02x\n", ucData);
	}
*/
}

void transport_send_packet(void) 
{
	HOSTDEF_PT_HANDSHAKE_ARM_MIRROR_AREA //HOSTDEF(ptHandshakeDtcmArmMirrorArea);
	unsigned long ulValue;


	/*--- check if the sent esc ---*/
	unsigned long ulPacketSize;
	
	tDpm.ulHostToNetxDataSize =	sizPacketOutputFill;
	
	/* Is the packet's size valid? */
	ulPacketSize = tDpm.ulHostToNetxDataSize;
	if( ulPacketSize==0 )
	{

	} 
	else if ( ulPacketSize<=DPM_HOST_TO_NETX_BUFFERSIZE ) 
	{

		ulValue  = PT_HANDSCHAKE_ARM_MIRROR_AREA_HANDSHAKE_REG; //ptHandshakeDtcmArmMirrorArea->aulHandshakeReg[0];
		ulValue  = ulValue >> 16;
		ulValue &= 0x8000;

		if ( ulValue==0x8000 )
		{

//			/* Acknowledge the packet. */
//			ptHandshakeDtcmArmMirrorArea->aulHandshakeReg[0] ^=
//			DPM_BOOT_NETX_RECEIVED_CMD
//					<< SRT_HANDSHAKE_REG_ARM_DATA;

			transport_enqueue(0x2B); // ESC REQ

			transport_acknowledge_escape_command();
			/* We don't set back the flag because host should do this */
			/* Wait for a packet. */

		}
//		else 
//		{
		
		/* Send the packet. */
		tDpm.ulNetxToHostDataSize = sizPacketOutputFill;

		/* Toggle the 'packet send' flag. */
		PT_HANDSCHAKE_ARM_MIRROR_AREA_HANDSHAKE_REG /*ptHandshakeDtcmArmMirrorArea->aulHandshakeReg[0]*/ ^=	DPM_BOOT_NETX_SEND_CMD << SRT_HANDSHAKE_REG_ARM_DATA;

		/* Wait for host ACK. */
		do
		{
			ulValue = mailbox_purr(DPM_BOOT_NETX_SEND_CMD);
		} while( ulValue!=0 );

		/* Remember the packet size for resends. */
		sizPacketOutputFillLast = sizPacketOutputFill;

		sizPacketOutputFill = 0;
//		}
	}
}

/**
 * escape command expresses as msb of handshake
 *
 * @todo get rid of the magic numbers
 */
int transport_acknowledge_escape_command(void) 
{
	HOSTDEF_PT_HANDSHAKE_ARM_MIRROR_AREA //HOSTDEF(ptHandshakeDtcmArmMirrorArea);

	uint32_t ulValue;
	int iResult;
	uint32_t ulHostPart;
	uint32_t ulNetxPart;


	ulValue = PT_HANDSCHAKE_ARM_MIRROR_AREA_HANDSHAKE_REG; // ptHandshakeDtcmArmMirrorArea->aulHandshakeReg[0];

	ulHostPart  = ulValue >> SRT_HANDSHAKE_REG_PC_DATA;
	ulHostPart |= 0x80;
	/* not needed yet */
	ulNetxPart  = ulValue >> SRT_HANDSHAKE_REG_ARM_DATA;
	ulNetxPart &= 0xFF;
	ulNetxPart |= 0x80;

	ulValue  = 0;
	ulValue |= ulHostPart << SRT_HANDSHAKE_REG_PC_DATA;
	ulValue |= ulNetxPart << SRT_HANDSHAKE_REG_ARM_DATA;

	PT_HANDSCHAKE_ARM_MIRROR_AREA_HANDSHAKE_REG /*ptHandshakeDtcmArmMirrorArea->aulHandshakeReg[0]*/ = ulValue;

	return iResult = 0;
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
	uint8_t uiReceivedByte = transport_dequeue();
	return uiReceivedByte;
}



void transport_call_console_put(unsigned int uiChar) 
{

	/* Add the byte to the FIFO. */
	transport_send_byte((unsigned char)uiChar);

	/* Reached the maximum packet size? */
	if( sizPacketOutputFill>=DPM_NETX_TO_HOST_BUFFERSIZE ) 
	{
		/* Yes -> send the packet. */
		transport_send_packet();

		/* Start a new packet. */
		transport_send_byte(MONITOR_STATUS_CallMessage);
	}

}



unsigned int transport_call_console_peek(void)
{
	return uiSize;
}



void transport_call_console_flush(void)
{
	/* Send the packet. */
	transport_send_packet();

	/* Start a new packet. */
	transport_send_byte(MONITOR_STATUS_CallMessage);
}



void transport_netMon_to_netX_send_byte()
{

}



void transport_netMon_to_netX()
{

}


/**
 * @todo: get rid of the magic numbers
 */
int transport_is_ready_to_execute(void)
{
	HOSTDEF_PT_HANDSHAKE_ARM_MIRROR_AREA //HOSTDEF(ptHandshakeDtcmArmMirrorArea);

	uint32_t ulValue;
	int iResult;
	uint32_t ulHostPart;
	uint32_t ulNetxPart;

	ulValue = PT_HANDSCHAKE_ARM_MIRROR_AREA_HANDSHAKE_REG; // ptHandshakeDtcmArmMirrorArea->aulHandshakeReg[0];

	ulHostPart = ulValue >> SRT_HANDSHAKE_REG_PC_DATA;
	ulHostPart &= 0x40;

	ulNetxPart = ulValue >> SRT_HANDSHAKE_REG_ARM_DATA;

	if (ulHostPart == 0x40)
	{
		/* set back the netx flag */
		/* IS THIS NEEDED???*/
		ulNetxPart &= 0xBF;

		ulValue  = 0;
		ulValue |= ulHostPart << SRT_HANDSHAKE_REG_PC_DATA;
		ulValue |= ulNetxPart << SRT_HANDSHAKE_REG_ARM_DATA;

		PT_HANDSCHAKE_ARM_MIRROR_AREA_HANDSHAKE_REG /*ptHandshakeDtcmArmMirrorArea->aulHandshakeReg[0]*/ = ulValue;
		return 1;
	}
	return iResult = 0;
}

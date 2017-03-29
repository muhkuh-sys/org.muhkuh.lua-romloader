/***************************************************************************
 *   Copyright (C) 2015 by Christoph Thelen                                *
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


#include "main.h"

#include <string.h>

#include "monitor_commands.h"
#include "rdy_run.h"
#include "serial_vectors.h"
#include "systime.h"
#include "transport.h"




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




void dpm_monitor(void)
{

HOSTDEF_PT_HANDSHAKE_ARM_MIRROR_AREA;
/* Acknowledge the packet. */
//PT_HANDSCHAKE_ARM_MIRROR_AREA_HANDSHAKE_REG /*ptHandshakeDtcmArmMirrorArea->aulHandshakeReg[0]*/ ^=	DPM_BOOT_NETX_RECEIVED_CMD << SRT_HANDSHAKE_REG_ARM_DATA;
int lVal = PT_HANDSCHAKE_ARM_MIRROR_AREA_HANDSHAKE_REG; // = 0x03030000;
PT_HANDSCHAKE_ARM_MIRROR_AREA_HANDSHAKE_REG = 0x02030000;
PT_HANDSCHAKE_ARM_MIRROR_AREA_HANDSHAKE_REG = 0x03030000;


	systime_init();

	transport_init();
	while(1)
	{
		transport_loop();
	}
}


/*-----------------------------------*/


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

#include "rdy_run.h"
#include "systime.h"
#include "monitor_commands.h"
#include "serial_vectors.h"

/*-----------------------------------*/

void dpm_monitor(void)
{
#if ASIC_TYP==56
	unsigned long ulRomId;
#endif
	BLINKI_HANDLE_T tBlinkiHandle;

	systime_init();

#if 0
/* ASIC_TYP==56 */
	ulRomId = aulRomId[2];
	if( ulRomId==ROM_CODE_ID_NETX56 )
	{
		ulConsoleDevice = aulConsoleDevices_netx56[0];
	}
	else if( ulRomId==ROM_CODE_ID_NETX56B )
	{
		ulConsoleDevice = aulConsoleDevices_netx56b[0];
	}
	else
	{
		ulConsoleDevice = (unsigned long)CONSOLE_DEVICE_NONE;
	}
	if( ulConsoleDevice!=((unsigned long)CONSOLE_DEVICE_USB) && ulConsoleDevice!=((unsigned long)CONSOLE_DEVICE_UART0) )
	{
		while(1) {};
	}
	transport_set_vectors(ulConsoleDevice);
#endif

	rdy_run_setLEDs(RDYRUN_OFF);
	rdy_run_blinki_init(&tBlinkiHandle, 0x00000055, 0x00000150);
	while(1)
	{
		rdy_run_blinki(&tBlinkiHandle);
	}
}


/*-----------------------------------*/


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


#include <string.h>
#include "asic_types.h"
#include "bootstrap_serial_vectors.h"

#if ASIC_TYP==ASIC_TYP_NETX50
#       include "uartmon_netx50_monitor_run.h"
#       define MONITOR_DATA_START MONITOR_DATA_START_NETX50
#       define MONITOR_DATA_END   MONITOR_DATA_END_NETX50
#       define MONITOR_EXEC       MONITOR_EXEC_NETX50
#elif ASIC_TYP==ASIC_TYP_NETX10
#       include "uartmon_netx10_monitor_run.h"
#       define MONITOR_DATA_START MONITOR_DATA_START_NETX10
#       define MONITOR_DATA_END   MONITOR_DATA_END_NETX10
#       define MONITOR_EXEC       MONITOR_EXEC_NETX10
#else
#       error "no host define set!"
#endif


typedef void (*PFN_START_ADR_T)(void) __attribute__ ((noreturn));

void bootstrap(void) __attribute__ ((noreturn));
void bootstrap(void)
{
	unsigned char *pucCnt;
	unsigned char *pucEnd;


	/* Receive the complete monitor code. */
	pucCnt = (unsigned char*)MONITOR_DATA_START;
	pucEnd = (unsigned char*)MONITOR_DATA_END;
	do
	{
		*(pucCnt++) = (unsigned char)SERIAL_GET();
	} while( pucCnt<pucEnd );

	/* Run the monitor code. */
	((PFN_START_ADR_T)(MONITOR_EXEC))();
}


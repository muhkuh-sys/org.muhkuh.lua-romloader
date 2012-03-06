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
#include "bootstrap_serial_vectors.h"

#if ASIC_TYP==500
#       include "uartmon_netx500_monitor_run.h"
#elif ASIC_TYP==50
#       include "uartmon_netx50_monitor_run.h"
#elif ASIC_TYP==10
#       include "uartmon_netx10_monitor_run.h"
#else
#       error "no host define set!"
#endif


typedef void (*PFN_START_ADR_T)(void) __attribute__ ((noreturn));

void bootstrap(void) __attribute__ ((noreturn));
void bootstrap(void)
{
	unsigned char *pucCnt;
	unsigned char *pucEnd;


#if ASIC_TYP==100 || ASIC_TYP==500
	typedef void (*PFN_SERIAL_V1_INIT_T)(void);

	/* Check the output handlers. */
	if( tSerialV1Vectors.fn.fnPut==NULL )
	{
		/* NOTE:
		   On netX500 and netX100 the romcode disables the UART before the call and reenables it when the call
		   returns. This means we have to init the uart here.
		*/

		/* Reinit the romcode uart routines, they are deactivated right before the 'CALL' command enters the user's code.
		   NOTE: the routine is thumb-code, bit #0 of the address must be set to switch the mode.
		*/
		((PFN_SERIAL_V1_INIT_T)(0x002015f4|1))();

		// set the vectors to the romcode
		// NOTE: all routines are thumb-code, bit #0 of the address must be set to switch the mode
		tSerialV1Vectors.fn.fnGet   = (PFN_SERIAL_GET_T)(0x00201664|1);
		tSerialV1Vectors.fn.fnPut   = (PFN_SERIAL_PUT_T)(0x00201646|1);
		tSerialV1Vectors.fn.fnPeek  = (PFN_SERIAL_PEEK_T)(0x002016b0|1);
		tSerialV1Vectors.fn.fnFlush = (PFN_SERIAL_FLUSH_T)(0x002016ba|1);
	}
#endif

	pucCnt = (unsigned char*)MONITOR_DATA_START;
	pucEnd = (unsigned char*)MONITOR_DATA_END;
	do
	{
		*(pucCnt++) = (unsigned char)SERIAL_GET();
	} while( pucCnt<pucEnd );
	((PFN_START_ADR_T)(MONITOR_EXEC))();
}


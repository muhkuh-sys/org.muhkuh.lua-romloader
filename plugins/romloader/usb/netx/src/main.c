/***************************************************************************
 *   Copyright (C) 2010 by Christoph Thelen                                *
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

#include "netx_io_areas.h"
#include "options.h"
#include "systime.h"
#include "usb.h"
#include "usbmon.h"


/*-----------------------------------*/


static void write_one_value(unsigned char *pucBuffer, unsigned long ulValue)
{
	unsigned int uiDigit;
	int iCnt;


	/* Dump a complete 32bit number, that's 8 digits. */
	iCnt = 8;
	do
	{
		/* Get the digit. */
		uiDigit = ulValue >> 28;

		/* Convert the digit to ASCII. */
		uiDigit |= '0';
		if( uiDigit>'9' )
		{
			uiDigit += 'a'-'9'-1;
		}

		/* Write the digit to the buffer. */
		*(pucBuffer++) = (unsigned char)uiDigit;

		/* Move to the next digit. */
		ulValue <<= 4;
		--iCnt;
	} while( iCnt>0 );
}


static void gen_usb_string(unsigned int uiOffset, unsigned long ulParameter0, unsigned long ulParameter1)
{
	unsigned char *pucCnt;


	pucCnt  = g_t_options.t_usb_settings.uCfg.auc;
	pucCnt += uiOffset;
	write_one_value(pucCnt, ulParameter0);

	pucCnt += 8;
	write_one_value(pucCnt, ulParameter1);
}


void usb_monitor(unsigned long ulParameter0, unsigned long ulParameter1, unsigned long ulParameter2, unsigned long ulParameter3)
{
	options_set_default();
	systime_init();

	/* TODO: save the current configuration. */

	usb_deinit();
	systime_delay(200);

	/* Convert the parameters to a device and serial string. */
	gen_usb_string(20+8, ulParameter0, ulParameter1);
	gen_usb_string(38+8, ulParameter2, ulParameter3);

	usb_init();
	usbmon_loop();

	usb_deinit();

	/* TODO: restore the old configuration. */
}


/*-----------------------------------*/


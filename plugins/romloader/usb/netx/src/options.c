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


#include <string.h>

#include "options.h"


static const OPTIONS_T t_default_options =
{
	.t_usb_settings.uCfg.auc =
	{
		/* device descriptor */
		0x39, 0x19,             /* vendor id */
		0x0c, 0x00,             /* product id */
		0x02, 0x00,             /* device release number */
		/* config descriptor */
		0x00, 0x00,             /* conf. characteristics, max. power consumption */

		/* manufacturer string  */
		34, 0x03,
		'H', 'i', 'l', 's', 'c', 'h', 'e', 'r', 
		' ', '1', '2', '3', '4', '5', '6', '7',

		/* device string */
		34, 0x03,
		'H', 'S', 'o', 'C', 'T', ' ', 'T', 'e', 
		's', 't', 'b', 'e', 'n', 'c', 'h', '#',

		/* S/N string */
		34, 0x03,
		'S', '/', 'N', ':', ' ', '1', '2', '3',
		'4', '5', '6', '7', '8', '9', '0', '1'
	},

#if ASIC_DEBUGMSG==1
	/* TODO: Always enable this block once the debug messages are
	 * included in the normal build.
	 */
	.t_debug_settings =
	{
		.ul_main    = 0xffffffff
	}
#endif
};


OPTIONS_T g_t_options;


void options_set_default(void)
{
	memcpy(&g_t_options, &t_default_options, sizeof(OPTIONS_T));
}


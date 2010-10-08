/***************************************************************************
 *   Copyright (C) 2005, 2006, 2007, 2008, 2009 by Hilscher GmbH           *
 *                                                                         *
 *   Author: Christoph Thelen (cthelen@hilscher.com)                       *
 *                                                                         *
 *   Redistribution or unauthorized use without expressed written          *
 *   agreement from the Hilscher GmbH is forbidden.                        *
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


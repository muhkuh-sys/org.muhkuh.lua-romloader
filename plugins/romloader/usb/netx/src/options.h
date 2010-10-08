/***************************************************************************
 *   Copyright (C) 2005, 2006, 2007, 2008, 2009 by Hilscher GmbH           *
 *                                                                         *
 *   Author: Christoph Thelen (cthelen@hilscher.com)                       *
 *                                                                         *
 *   Redistribution or unauthorized use without expressed written          *
 *   agreement from the Hilscher GmbH is forbidden.                        *
 ***************************************************************************/


#ifndef __OPTIONS_H__
#define __OPTIONS_H__


typedef struct
{
	union
	{
		unsigned char auc[16*sizeof(unsigned long)];
		unsigned long aul[16];
	} uCfg;
} USB_CONFIGURATION_T;

typedef struct
{
	unsigned long ul_main;
} DEBUG_CONFIGURATION_T;


typedef struct
{
	USB_CONFIGURATION_T t_usb_settings;

#if ASIC_DEBUGMSG==1
	/* TODO: Always enable this block once the debug messages are
	 * included in the normal build.
	 */
	/* debug config */
	DEBUG_CONFIGURATION_T t_debug_settings;
#endif
} OPTIONS_T;


extern OPTIONS_T g_t_options;

void options_set_default(void);

#endif	/* __OPTIONS_H__ */


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

#include "netx_io_areas.h"


#if ASIC_TYP==ASIC_TYP_NETX10
#       define USB_VENDOR_ID  0x1939
#       define USB_PRODUCT_ID 0x000c
#       define USB_RELEASE_ID 0x0002
#elif ASIC_TYP==ASIC_TYP_NETX56
#       define USB_VENDOR_ID  0x1939
#       define USB_PRODUCT_ID 0x0018
#       define USB_RELEASE_ID 0x0002
#else
#       error "Unknown ASIC_TYP"
#endif

static const OPTIONS_T t_default_options =
{
	.t_usb_settings.uCfg.auc =
	{
		/* device descriptor */
		(USB_VENDOR_ID &0xffU), ((USB_VENDOR_ID >>8U)&0xffU),             /* vendor id */
		(USB_PRODUCT_ID&0xffU), ((USB_PRODUCT_ID>>8U)&0xffU),             /* product id */
		(USB_RELEASE_ID&0xffU), ((USB_RELEASE_ID>>8U)&0xffU),             /* device release number */
		/* config descriptor */
		0xC0, 0x00,             /* conf. characteristics, max. power consumption */

		/* manufacturer string  */
		28, 0x03,
		'H', 'i', 'l', 's', 'c', 'h', 'e', 'r',
		' ', 'G', 'm', 'b', 'H',   0,   0,   0,

		/* device string */
		28, 0x03,
		'H', 'B', 'o', 'o', 't', ' ', 'm', 'o',
		'n', 'i', 't', 'o', 'r',   0,   0,   0,

		/* S/N string */
		34, 0x03,
		'0', '0', '0', '0', '0', '0', '0', '0',
		'0', '0', '0', '0', '0', '0', '0', '2'
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


typedef enum
{
	CHIP_SUBTYP_NETX50              = 0,
	CHIP_SUBTYP_NETX51              = 1,
	CHIP_SUBTYP_NETX52              = 2
} CHIP_SUBTYP_T;


void options_set_default(void)
{
#if ASIC_TYP==ASIC_TYP_NETX56
	HOSTDEF(ptAsicCtrlArea);
	unsigned long ulValue;
	unsigned long ulChipSubType;
	CHIP_SUBTYP_T tChipSubTyp;
#endif


	memcpy(&g_t_options, &t_default_options, sizeof(OPTIONS_T));

#if ASIC_TYP==ASIC_TYP_NETX56
	/* Is this a netX52? */
	ulValue = ptAsicCtrlArea->ulSample_at_nres;
	ulChipSubType  = (ulValue&HOSTMSK(sample_at_nres_sar_mem_a18))>> HOSTSRT(sample_at_nres_sar_mem_a18);
	ulChipSubType |= (ulValue&HOSTMSK(sample_at_nres_sar_mem_a19))>>(HOSTSRT(sample_at_nres_sar_mem_a19)-1);
	tChipSubTyp = (CHIP_SUBTYP_T)ulChipSubType;
	if(tChipSubTyp==CHIP_SUBTYP_NETX52)
	{
		/* Set product ID 0x0019 for netX52 boards. */
		g_t_options.t_usb_settings.uCfg.auc[2] = 0x19U;
	}
#endif
}


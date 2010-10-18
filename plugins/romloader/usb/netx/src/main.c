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
#include "uart.h"
#include "usb.h"
#include "usbmon.h"


/*-----------------------------------*/

extern const unsigned char aucUuid[32];

#if ASIC_DEBUGMSG!=0
static const UART_CONFIGURATION_T tUartCfg_nxhx10_etm =
{
	.uc_rx_mmio = 20U,
	.uc_tx_mmio = 21U,
	.uc_rts_mmio = 0xffU,
	.uc_cts_mmio = 0xffU,
	.us_baud_div = UART_BAUDRATE_DIV(UART_BAUDRATE_115200)
};
#endif


void usb_monitor(void)
{
	options_set_default();
	systime_init();

	/* TODO: save the current configuration. */

#if ASIC_DEBUGMSG!=0
	uart_init(0, &tUartCfg_nxhx10_etm);
#endif

	usb_deinit();
	systime_delay(200);

	/* Convert the parameters to a device and serial string. */
	memcpy(g_t_options.t_usb_settings.uCfg.auc+20+8, aucUuid, 16);
	memcpy(g_t_options.t_usb_settings.uCfg.auc+38+8, aucUuid+16, 16);

	usb_init();
	usbmon_loop();

	usb_deinit();

	/* TODO: restore the old configuration. */
}


/*-----------------------------------*/


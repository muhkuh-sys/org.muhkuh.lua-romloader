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

#include "systime.h"
#include "monitor_commands.h"
#include "transport.h"

/* DEBUG: this is only for the debug messages. */
#include "uprintf.h"
#include "serial_vectors.h"

#include "uart.h"

/*-----------------------------------*/


static const UART_CONFIGURATION_T tUartCfg_nxhx10_etm =                                                                                                                                                                                     
{                                                                                                                                                                                                                                           
	.uc_rx_mmio = 20U,                                                                                                                                                                                                                  
	.uc_tx_mmio = 21U,                                                                                                                                                                                                                  
	.uc_rts_mmio = 0xffU,                                                                                                                                                                                                               
	.uc_cts_mmio = 0xffU,                                                                                                                                                                                                               
	.us_baud_div = UART_BAUDRATE_DIV(UART_BAUDRATE_115200)                                                                                                                                                                              
};



void uart_monitor(void)
{
	systime_init();

	uart_init(&tUartCfg_nxhx10_etm);
	tSerialVectors.fn.fnGet = uart_get;
	tSerialVectors.fn.fnPut = uart_put;
	tSerialVectors.fn.fnPeek = uart_peek;
	tSerialVectors.fn.fnFlush = uart_flush;

	uprintf("Hallo!\n");

	transport_init();

	while(1)
	{
		transport_loop();
	}
}


/*-----------------------------------*/


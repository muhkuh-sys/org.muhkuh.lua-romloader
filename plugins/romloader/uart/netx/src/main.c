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
#include "serial_vectors.h"
#include "uart.h"

/*-----------------------------------*/

#if ASIC_TYP==100 || ASIC_TYP==500
	static const UART_CONFIGURATION_T tUartCfg =
	{
		.us_baud_div = UART_BAUDRATE_DIV(UART_BAUDRATE_115200)
	};
#elif ASIC_TYP==10
	static const UART_CONFIGURATION_T tUartCfg =
	{
		.uc_rx_mmio = 20U,
		.uc_tx_mmio = 21U,
		.uc_rts_mmio = 0xffU,
		.uc_cts_mmio = 0xffU,
		.us_baud_div = UART_BAUDRATE_DIV(UART_BAUDRATE_115200)
	};
#endif


SERIAL_V1_COMM_UI_FN_T tSerialV1Vectors;


#if ASIC_TYP==50
static const SERIAL_V1_COMM_UI_FN_T tSerialNetx50UsbVectors =
{
	.aul =
	{
		0x08202361U,
		0x082023bdU,
		0x08202355U,
		0x08202419U
	}
};


static unsigned int netx50_usb_peek(void)
{
	/* This is the routine for usb communication. Call this on a regular basis to get new USB packets. */
	typedef void (*PFN_SERIAL_V1_CYCLIC_T)(void);


	((PFN_SERIAL_V1_CYCLIC_T)(0x08201ff8U|1))();
	return ((PFN_SERIAL_V1_PEEK_T)(0x08202355U|1))();
}
#endif


void uart_monitor(void)
{
	systime_init();

#if ASIC_TYP==100 || ASIC_TYP==500 || ASIC_TYP==10
	/* The netX500 and netX100 romcode uart put routine converts LF (0x0a)
	 * to CR LF (0x0d 0x0a). It is not possible to send binary data with
	 * it. Replace the vectors with custom routines.
	 * 
	 * The netX10 romcode uses areas in bank0 around offset 0x8180. This is
	 * outside the RAM area reserved for the monitor code.
	 */

	/* Initialize the UART. */
	uart_init(&tUartCfg);

	/* Set the new vectors. */
	tSerialV1Vectors.fn.fnGet   = uart_get;
	tSerialV1Vectors.fn.fnPut   = uart_put;
	tSerialV1Vectors.fn.fnPeek  = uart_peek;
	tSerialV1Vectors.fn.fnFlush = uart_flush;
#elif ASIC_TYP==50
	/* Copy the ROM code vectors to an internal buffer. */
	memcpy(&tSerialV1Vectors, &tSerialV2Vectors, sizeof(SERIAL_V2_COMM_FN_T));

	/* Compare vectors to netx50 USB. This one needs special treatment. */
	if( memcmp(&tSerialV1Vectors, &tSerialNetx50UsbVectors, sizeof(SERIAL_V1_COMM_UI_FN_T))==0 )
	{
		tSerialV1Vectors.fn.fnPeek = netx50_usb_peek;
	}
#endif

	transport_init();

	while(1)
	{
		transport_loop();
	}
}


/*-----------------------------------*/


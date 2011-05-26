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

#if ASIC_TYP==10
static const UART_CONFIGURATION_T tUartCfg =
{
	.uc_rx_mmio = 20U,
	.uc_tx_mmio = 21U,
	.uc_rts_mmio = 0xffU,
	.uc_cts_mmio = 0xffU,
	.us_baud_div = UART_BAUDRATE_DIV(UART_BAUDRATE_115200)
};
#elif ASIC_TYP==50
static const UART_CONFIGURATION_T tUartCfg =
{
	.uc_rx_mmio = 34U,
	.uc_tx_mmio = 35U,
	.uc_rts_mmio = 0xffU,
	.uc_cts_mmio = 0xffU,
	.us_baud_div = UART_BAUDRATE_DIV(UART_BAUDRATE_115200)
};
#elif ASIC_TYP==100 || ASIC_TYP==500
static const UART_CONFIGURATION_T tUartCfg =
{
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
		tSerialV1Vectors.fn.fnGet   = (PFN_SERIAL_V1_GET_T)(0x00201664|1);
		tSerialV1Vectors.fn.fnPut   = (PFN_SERIAL_V1_PUT_T)(0x00201646|1);
		tSerialV1Vectors.fn.fnPeek  = (PFN_SERIAL_V1_PEEK_T)(0x002016b0|1);
		tSerialV1Vectors.fn.fnFlush = (PFN_SERIAL_V1_FLUSH_T)(0x002016ba|1);
	}
#endif

	uart_init(&tUartCfg);
	uprintf("Hallo!\n");

	/* Copy the V1 vectors to an internal buffer. */
	memcpy(&tSerialV1Vectors, &tSerialV2Vectors, sizeof(SERIAL_V2_COMM_FN_T));

#if ASIC_TYP==50
	/* Compare vectors to netx50 USB. This one needs special treatment. */
	if( memcmp(&tSerialV2Vectors, &tSerialNetx50UsbVectors, sizeof(SERIAL_V1_COMM_UI_FN_T))==0 )
	{
		uprintf("found netx50 USB\n");
		tSerialV1Vectors.fn.fnPeek = netx50_usb_peek;
	}
	else
	{
		uprintf("not netX50 USB.\n");
	}
#endif

	transport_init();

	while(1)
	{
		transport_loop();
	}
}


/*-----------------------------------*/


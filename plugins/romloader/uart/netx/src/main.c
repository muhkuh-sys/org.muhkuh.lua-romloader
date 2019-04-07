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

#include "asic_types.h"
#include "monitor_commands.h"
#include "serial_vectors.h"
#include "systime.h"
#include "transport.h"
#include "uart.h"

#if ASIC_TYP==ASIC_TYP_NETX50 || ASIC_TYP==ASIC_TYP_NETX56 || ASIC_TYP==ASIC_TYP_NETX4000_RELAXED || ASIC_TYP==ASIC_TYP_NETX4000
#       include "transport_extension.h"
#endif

#if ASIC_TYP==ASIC_TYP_NETX50
#       include "netx50/usb.h"
#endif

/*-----------------------------------*/

#if ASIC_TYP==ASIC_TYP_NETX500
	static const UART_CONFIGURATION_T tUartCfg =
	{
		.us_baud_div = UART_BAUDRATE_DIV(UART_BAUDRATE_115200)
	};
#elif ASIC_TYP==ASIC_TYP_NETX56
	typedef enum
	{
		CONSOLE_DEVICE_NONE             = 0,
		CONSOLE_DEVICE_UART0            = 1,
		CONSOLE_DEVICE_UART1            = 2,
		CONSOLE_DEVICE_USB              = 3,
		CONSOLE_DEVICE_DCC              = 4,
		CONSOLE_DEVICE_SIMU             = 5
	} CONSOLE_DEVICE_T;

	extern const unsigned long aulConsoleDevices_netx56[2];
	extern const unsigned long aulConsoleDevices_netx56b[2];
	extern const unsigned long aulRomId[3];

#       define ROM_CODE_ID_NETX56  0x00006003
#       define ROM_CODE_ID_NETX56B 0x00106003

#elif ASIC_TYP==ASIC_TYP_NETX4000_RELAXED || ASIC_TYP==ASIC_TYP_NETX4000
typedef enum ENUM_CONSOLE_DEVICE
{
	CONSOLE_DEVICE_NONE             = 0,
	CONSOLE_DEVICE_UART0            = 1,
	CONSOLE_DEVICE_UART1            = 2,
	CONSOLE_DEVICE_UART2            = 3,
	CONSOLE_DEVICE_USBDEV           = 4,
	CONSOLE_DEVICE_ETH              = 5,
	CONSOLE_DEVICE_DCC              = 6,
	CONSOLE_DEVICE_RAPUART0         = 7,
	CONSOLE_DEVICE_RAPUART1         = 8,
	CONSOLE_DEVICE_RAPUART2         = 9,
	CONSOLE_DEVICE_RAPUART3         = 10
} CONSOLE_DEVICE_T;

#endif


SERIAL_V1_COMM_UI_FN_T tSerialV1Vectors;


#if ASIC_TYP==ASIC_TYP_NETX50
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

typedef enum
{
	CONSOLE_DEVICE_UART             = 0,
	CONSOLE_DEVICE_USB              = 1
} CONSOLE_DEVICE_T;
#endif


void uart_monitor(void)
{
#if ASIC_TYP==ASIC_TYP_NETX56
	unsigned long ulRomId;
	unsigned long ulConsoleDevice;
#endif
#if ASIC_TYP==ASIC_TYP_NETX50
	unsigned long ulConsoleDevice;
#endif
#if ASIC_TYP==ASIC_TYP_NETX4000_RELAXED || ASIC_TYP==ASIC_TYP_NETX4000
	const unsigned long * const pulRomId = (const unsigned long * const)(0x04100020U);
	const unsigned long * const pulConsoleDeviceRC4 = (const unsigned long * const)(0x000226c8U);
	const unsigned long * const pulConsoleDeviceRC5 = (const unsigned long * const)(0x00023400U);
	unsigned long ulValue;
	unsigned long ulConsoleDevice;
#endif


	systime_init();

#if ASIC_TYP==ASIC_TYP_NETX500
	/* Both ASICs in this group can not use the ROM routines for UART
	 * communication.
	 * 
	 * The netX500 and netX100 ROM code UART put routine converts LF (0x0a)
	 * to CR LF (0x0d 0x0a). It is not possible to send binary data with
	 * it. Replace the vectors with custom routines.
	 */

	/* Initialize the UART. */
	uart_init(&tUartCfg);

	/* Set the new vectors. */
	tSerialV1Vectors.fn.fnGet   = uart_get;
	tSerialV1Vectors.fn.fnPut   = uart_put;
	tSerialV1Vectors.fn.fnPeek  = uart_peek;
	tSerialV1Vectors.fn.fnFlush = uart_flush;
#elif ASIC_TYP==ASIC_TYP_NETX10
	/* The netX10 ROM code uses areas in bank0 around offset 0x8180. This
	 * is outside the RAM area reserved for the monitor code.
	 */

	/* Set the new vectors. */
	tSerialV1Vectors.fn.fnGet   = uart_get;
	tSerialV1Vectors.fn.fnPut   = uart_put;
	tSerialV1Vectors.fn.fnPeek  = uart_peek;
	tSerialV1Vectors.fn.fnFlush = uart_flush;
#elif ASIC_TYP==ASIC_TYP_NETX50
	/* Compare vectors to netx50 USB. This one needs special treatment. */
	if( memcmp(&tSerialV2Vectors, &tSerialNetx50UsbVectors, sizeof(SERIAL_V2_COMM_FN_T))==0 )
	{
		/* USB CDC */
		usb_init();
		ulConsoleDevice = (unsigned long)CONSOLE_DEVICE_USB;
	}
	else
	{
		/* UART */

		/* In this mode the USB endpoints are in the state "not configured". */
		tReceiveEpState = USB_EndpointState_Unconfigured;

		/* Copy the ROM code vectors to an internal buffer. */
		memcpy(&tSerialV1Vectors, &tSerialV2Vectors, sizeof(SERIAL_V2_COMM_FN_T));
		ulConsoleDevice = (unsigned long)CONSOLE_DEVICE_UART;
	}
	transport_set_vectors(ulConsoleDevice);
#elif ASIC_TYP==ASIC_TYP_NETX56
	ulRomId = aulRomId[2];
	if( ulRomId==ROM_CODE_ID_NETX56 )
	{
		ulConsoleDevice = aulConsoleDevices_netx56[0];
	}
	else if( ulRomId==ROM_CODE_ID_NETX56B )
	{
		ulConsoleDevice = aulConsoleDevices_netx56b[0];
	}
	else
	{
		ulConsoleDevice = (unsigned long)CONSOLE_DEVICE_NONE;
	}
	if( ulConsoleDevice!=((unsigned long)CONSOLE_DEVICE_USB) && ulConsoleDevice!=((unsigned long)CONSOLE_DEVICE_UART0) )
	{
		while(1) {};
	}
	transport_set_vectors(ulConsoleDevice);
#elif ASIC_TYP==ASIC_TYP_NETX4000_RELAXED || ASIC_TYP==ASIC_TYP_NETX4000
	ulConsoleDevice = CONSOLE_DEVICE_NONE;
	ulValue = *pulRomId;
	if( ulValue==0x00108004U )
	{
		/* Get the current console device. */
		ulConsoleDevice = *pulConsoleDeviceRC4;
	}
	else if( ulValue==0x0010b004U )
	{
		ulConsoleDevice = *pulConsoleDeviceRC5;
	}

	/* This update code is working with a UART or a virtual COM port.
	 * If the ROM console device is something different, something
	 * terrible happened.
	 */
	if( ulConsoleDevice!=CONSOLE_DEVICE_UART0 &&
	    ulConsoleDevice!=CONSOLE_DEVICE_UART1 &&
	    ulConsoleDevice!=CONSOLE_DEVICE_UART2 &&
	    ulConsoleDevice!=CONSOLE_DEVICE_USBDEV &&
	    ulConsoleDevice!=CONSOLE_DEVICE_RAPUART0 &&
	    ulConsoleDevice!=CONSOLE_DEVICE_RAPUART1 &&
	    ulConsoleDevice!=CONSOLE_DEVICE_RAPUART2 &&
	    ulConsoleDevice!=CONSOLE_DEVICE_RAPUART3 )
	{
		while(1) {};
	}

	transport_set_vectors(ulConsoleDevice);

#else
#       error "Unknown ASIC_TYP!"
#endif

	transport_init();

	while(1)
	{
		transport_loop();
	}
}


/*-----------------------------------*/


/***************************************************************************
 *   Copyright (C) 2010 by Hilscher GmbH                                   *
 *                                                                         *
 *   Author: Christoph Thelen (cthelen@hilscher.com)                       *
 *                                                                         *
 *   Redistribution or unauthorized use without expressed written          *
 *   agreement from the Hilscher GmbH is forbidden.                        *
 ***************************************************************************/


#include "main.h"

#include <string.h>

#include "netx_io_areas.h"
#include "netx_test.h"
#include "options.h"
#include "serial_vectors.h"
#include "systime.h"
#include "uprintf.h"
#include "uart.h"
#include "usb.h"


#if ASIC_DEBUGMSG==1
	#define DEBUGZONE(n)  (g_t_options.t_debug_settings.ul_main&(0x00000001<<(n)))

	//
	// These defines must match the ZONE_* defines
	//
	#define DBG_ZONE_ERROR		0
	#define DBG_ZONE_WARNING	1
	#define DBG_ZONE_FUNCTION	2
	#define DBG_ZONE_INIT		3
	#define DBG_ZONE_VERBOSE	7

	#define ZONE_ERROR		DEBUGZONE(DBG_ZONE_ERROR)
	#define ZONE_WARNING		DEBUGZONE(DBG_ZONE_WARNING)
	#define ZONE_FUNCTION		DEBUGZONE(DBG_ZONE_FUNCTION)
	#define ZONE_INIT		DEBUGZONE(DBG_ZONE_INIT)
	#define ZONE_VERBOSE		DEBUGZONE(DBG_ZONE_VERBOSE)

	#define DEBUGMSG(cond,...) ((void)((cond)?(uprintf(__VA_ARGS__)),1:0))
#else
	#define DEBUGMSG(cond,...) ((void)0)
#endif


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


static const UART_CONFIGURATION_T tUartCfg =
{
	.uc_rx_mmio = 20U,
	.uc_tx_mmio = 21U,
	.uc_rts_mmio = 0xffU,
	.uc_cts_mmio = 0xffU,
	.us_baud_div = UART_BAUDRATE_DIV(UART_BAUDRATE_115200)
};


TEST_RESULT_T test(unsigned long ulParameter0, unsigned long ulParameter1, unsigned long ulParameter2, unsigned long ulParameter3)
{
	TEST_RESULT_T tResult;


	options_set_default();
	systime_init();


	uart_init(0, &tUartCfg);
	tSerialVectors.fn.fnGet   = uart_get;
	tSerialVectors.fn.fnPut   = uart_put;
	tSerialVectors.fn.fnPeek  = uart_peek;
	tSerialVectors.fn.fnFlush = uart_flush;


	/* say hi! */
	DEBUGMSG(ZONE_VERBOSE, ".*** USB Monitor ***\n");
	DEBUGMSG(ZONE_VERBOSE, "\n\n");

	/* show compile settings */
	DEBUGMSG(ZONE_VERBOSE, ".system:\n");
	DEBUGMSG(ZONE_VERBOSE, ".  ASIC_TYP   = %d\n", ASIC_TYP);
	DEBUGMSG(ZONE_VERBOSE, ".  parameter0 = 0x%08x\n", ulParameter0);
	DEBUGMSG(ZONE_VERBOSE, ".  parameter1 = 0x%08x\n", ulParameter1);
	DEBUGMSG(ZONE_VERBOSE, ".  parameter2 = 0x%08x\n", ulParameter2);
	DEBUGMSG(ZONE_VERBOSE, ".  parameter3 = 0x%08x\n", ulParameter3);
	DEBUGMSG(ZONE_VERBOSE, "\n\n");

	DEBUGMSG(ZONE_VERBOSE, " Disable usb core.\n");
	usb_deinit();

	DEBUGMSG(ZONE_VERBOSE, " Delay for 200ms...\n");
	systime_delay(200);

	/* Convert the parameters to a device and serial string. */
	hexdump(g_t_options.t_usb_settings.uCfg.auc, sizeof(USB_CONFIGURATION_T));
	gen_usb_string(20+8, ulParameter0, ulParameter1);
	gen_usb_string(38+8, ulParameter2, ulParameter3);
	hexdump(g_t_options.t_usb_settings.uCfg.auc, sizeof(USB_CONFIGURATION_T));

	DEBUGMSG(ZONE_VERBOSE, " Init the USB core.\n");
	if( usb_init()==0 )
	{
		uprintf("! Enum RAM data verification failed!");
		while(1) {};
	}
	else
	{
		usb_loop();
	}

	while(1) {};

	tResult = TEST_RESULT_OK;

	return tResult;
}


/*-----------------------------------*/


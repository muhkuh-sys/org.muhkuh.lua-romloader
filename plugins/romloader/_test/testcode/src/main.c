/***************************************************************************
 *   Copyright (C) 2008 by Christoph Thelen                                *
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

#include "netx_consoleapp.h"
#include "systime.h"
#include "serial_vectors.h"

#include "main.h"

/*-----------------------------------*/

static void delay_print(unsigned int uiMessages, unsigned long ulDelayMs) {
	unsigned int uiCnt;
	unsigned long ulTimer;
	int iElapsed;

	/* print a lot of messages */
	uprintf(". counting from 0 to %d and %dms delay.\n", uiMessages, ulDelayMs);
	for (uiCnt = 0; uiCnt < uiMessages; ++uiCnt) {
		if (ulDelayMs != 0) {
			ulTimer = systime_get_ms();
			do {
				iElapsed = systime_elapsed(ulTimer, ulDelayMs);
			} while (iElapsed == 0);
		}
		uprintf("%% %d/%d\n", uiCnt, uiMessages);
	}

}

static void msleep(unsigned long ulDelayMs) {
	unsigned int uiCnt;
	unsigned long ulTimer;
	int iElapsed;

	if (ulDelayMs != 0) {
		ulTimer = systime_get_ms();
		do {
			iElapsed = systime_elapsed(ulTimer, ulDelayMs);
		} while (iElapsed == 0);
	}

}

/* This is your main function for the test,
 your control starts here. */
NETX_CONSOLEAPP_RESULT_T netx_consoleapp_main(
		NETX_CONSOLEAPP_PARAMETER_T *ptTestParam) {
	/* this is the result of the test */
	NETX_CONSOLEAPP_RESULT_T tTestResult;
	/* this is the input parameter from the xml file */
	unsigned long ulParameter;

	/* Init all modules. */
	systime_init();

	/* the input parameter is  */
	ulParameter = (unsigned long) ptTestParam->pvInitParams;

	/* say hi */

	char *tmp[25];
	while (1) {
		uprintf("ABC\n");

		int i = 0;
		if (0 != SERIAL_PEEK()) {
			while (0 != SERIAL_PEEK()) {
				if(0x2B == SERIAL_GET()){
					uprintf("ask for ESC\n");
				}

			}
			uprintf("GOT >>>%s<<< FROM HOST\n", tmp);
			break;
		}
	}

	uprintf(". *** test skeleton start ***\n");
	uprintf(". Parameter Address: 0x%08x\n", (unsigned long) ptTestParam);
	uprintf(". Parameter: 0x%08x\n", ulParameter);

	/* Print a very long line (longer than 1 USB packet). */
	uprintf(
			"012345678901234567890123456789012345678901234567890123456789012345678901234567\n");
	uprintf(
			"000000000011111111112222222222333333333344444444445555555555666666666677777777\n");
//	char c = NULL;
//	while (0 != 0) {
//
//
//
//		msleep(100);
//
//	}
	uprintf("GOT SOMETHING\n");
////	/* Print messages with a specific delay. */
//	delay_print(100,    0);
//	delay_print(  8,  500);
//	delay_print(  4, 1000);
//	delay_print(  2, 2000);

	/* write parameter to return message */
	ptTestParam->pvReturnMessage = (void*) ulParameter;

	/* test result is "ok" */
	tTestResult = NETX_CONSOLEAPP_RESULT_OK;

	return tTestResult;
}

/*-----------------------------------*/

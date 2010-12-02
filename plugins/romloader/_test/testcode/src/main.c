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


/* all netx io areas as structures */
#include "netx_io_areas.h"

#include "netx_consoleapp.h"
#include "uprintf.h"

#include "main.h"

/*-----------------------------------*/

/* This is your main function for the test,
   your control starts here. */
NETX_CONSOLEAPP_RESULT_T netx_consoleapp_main(NETX_CONSOLEAPP_PARAMETER_T *ptTestParam)
{
	/* this is the result of the test */
	NETX_CONSOLEAPP_RESULT_T tTestResult;
	/* this is the input parameter from the xml file */
	unsigned long ulParameter;
	int iMax, iCnt;


	/* the input parameter is  */
	ulParameter = (unsigned long)ptTestParam->pvInitParams;

	/* say hi */
	uprintf(". *** test skeleton start ***\n");
	uprintf(". Parameter Address: 0x%08x\n", (unsigned long)ptTestParam);
	uprintf(". Parameter: 0x%08x\n", ulParameter);

	/* print a lot of messages */
	iMax = 100;
	uprintf(". counting from 0 to %d\n", iMax);
	for(iCnt=0; iCnt<iMax; ++iCnt)
	{
		uprintf("%% %d/%d\n", iCnt, iMax);
	}

	/* write parameter to return message */
	ptTestParam->pvReturnMessage = (void*)ulParameter;

	/* test result is "ok" */
	tTestResult = NETX_CONSOLEAPP_RESULT_OK;

	return tTestResult;
}

/*-----------------------------------*/

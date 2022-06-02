/***************************************************************************
 *   Copyright (C) 2012 by Christoph Thelen                                *
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

#include <stdio.h>

#include <sys/stat.h>

#include "romloader_uart_read_functinoid_hboot3.h"


#ifdef _MSC_VER
#       define snprintf _snprintf
#endif

#if defined(_MSC_VER)
#       define SLEEP_MS(ms) Sleep(ms)
#elif defined(__GNUC__) || defined(__MINGW32__) || defined(__MINGW64__)
#       include <unistd.h>
#       define SLEEP_MS(ms) usleep(ms*1000)
#endif



romloader_uart_read_functinoid_hboot3::romloader_uart_read_functinoid_hboot3(romloader_uart_device *ptDevice, char *pcPortName)
{
	m_ptDevice = ptDevice;
	m_pcPortName = 	pcPortName;
}

bool romloader_uart_read_functinoid_hboot3::read_data32(uint32_t ulAddress, uint32_t *pulValue)
{
	union
	{
		uint8_t auc[32];
		char ac[32];
	} uCmd;
	union
	{
		uint8_t *puc;
		char *pc;
	} uResponse;
	size_t sizCmd;
	bool fOk;
	int iResult;
	uint32_t ulReadbackAddress;
	uint32_t ulValue;


	sizCmd = snprintf(uCmd.ac, 32, "D %x ++3\n", ulAddress);
	/* Send the command with 500ms second timeout. */
	if( m_ptDevice->SendRaw(uCmd.auc, sizCmd, 500)!=sizCmd )
	{
		/* Failed to send the command to the device. */
		fprintf(stderr, "Failed to send the command to the device.\n");
		fOk = false;
	}
	else
	{
		/* Receive one line. This is the command echo. */
		fOk = m_ptDevice->GetLine(&uResponse.puc, "\r\n", 2000);
		if( fOk==false )
		{
			fprintf(stderr, "failed to receive command echo!\n");
		}
		else
		{
			sizCmd = strlen(uResponse.pc);
			free(uResponse.puc);

			/* Receive the rest of the output until the command prompt. This is the command result. */
			fOk = m_ptDevice->GetLine(&uResponse.puc, "\r\n000 >", 2000);
			if( fOk==false )
			{
				fprintf(stderr, "failed to get command response!\n");
			}
			else
			{
				iResult = sscanf(uResponse.pc, "%08x: %08x", &ulReadbackAddress, &ulValue);
				if( iResult==2 && ulAddress==ulReadbackAddress )
				{
					if( pulValue!=NULL )
					{
						*pulValue = ulValue;
					}
				}
				else
				{
					fprintf(stderr, "The command response is invalid!\n");
					fOk = false;
				}
				sizCmd = strlen(uResponse.pc);
				hexdump(uResponse.puc, sizCmd);
				free(uResponse.puc);
			}
		}
	}

	return fOk;
}


/*
Send the command "s 0 4" and check if the status code 000 or 014 are returned.

Secure boot disabled:
s 0 4

begin 666 00000000_00000004.bin
$\/\)(   `
`
end

000 >

Secure boot enabled:
s 0 4

014 >



*/
CONSOLE_MODE_T romloader_uart_read_functinoid_hboot3::detect_console_mode(void)
{
	union
	{
		uint8_t auc[32];
		char ac[32];
	} uCmd;
	union
	{
		uint8_t *puc;
		char *pc;
	} uResponse;
	size_t sizCmd;
	bool fOk;
	int iMatch;
	CONSOLE_MODE_T tConsoleMode;

	/* Expect failure. */
	tConsoleMode = CONSOLE_MODE_Unknown;
	
	sizCmd = snprintf(uCmd.ac, 32, "s 0 4\n");
	/* Send the command with 500ms second timeout. */
	if( m_ptDevice->SendRaw(uCmd.auc, sizCmd, 500)!=sizCmd )
	{
		/* Failed to send the command to the device. */
		fprintf(stderr, "Failed to send the command to the device.\n");
	}
	else
	{
		/* Receive one line. This is the command echo. */
		fOk = m_ptDevice->GetLine(&uResponse.puc, "\r\n", 2000);
		if( fOk==false )
		{
			fprintf(stderr, "failed to receive command echo!\n");
		}
		else
		{
			free(uResponse.puc);

			/* Receive the rest of the output until the command prompt. This is the command result. */
			iMatch = m_ptDevice->GetLine2(&uResponse.puc, "\r\n000 >", "\r\n014 >", 2000);

			switch (iMatch) {
				case 1:
					tConsoleMode = CONSOLE_MODE_Open;
					fprintf(stderr, "The console is in open mode.\n");
					//sizCmd = strlen(uResponse.pc);
					//hexdump(uResponse.puc, sizCmd);
					free(uResponse.puc);
					break;
					
				case 2:
					tConsoleMode = CONSOLE_MODE_Secure;
					fprintf(stderr, "The console is in secure mode.\n");
					//sizCmd = strlen(uResponse.pc);
					//hexdump(uResponse.puc, sizCmd);
					free(uResponse.puc);
					break;
				
				default:
					fprintf(stderr, "failed to get command response!\n");
					fprintf(stderr, "The console is in an unknown mode.\n");
					break;
			}
		}
	}

	return tConsoleMode;
}

void romloader_uart_read_functinoid_hboot3::hexdump(const uint8_t *pucData, uint32_t ulSize)
{
	const uint8_t *pucDumpCnt, *pucDumpEnd;
	uint32_t ulAddressCnt;
	size_t sizBytesLeft;
	size_t sizChunkSize;
	size_t sizChunkCnt;


	// show a hexdump of the data
	pucDumpCnt = pucData;
	pucDumpEnd = pucData + ulSize;
	ulAddressCnt = 0;
	while( pucDumpCnt<pucDumpEnd )
	{
		// get number of bytes for the next line
		sizChunkSize = 16;
		sizBytesLeft = pucDumpEnd - pucDumpCnt;
		if( sizChunkSize>sizBytesLeft )
		{
			sizChunkSize = sizBytesLeft;
		}

		// start a line in the dump with the address
		printf("%08X: ", ulAddressCnt);
		// append the data bytes
		sizChunkCnt = sizChunkSize;
		while( sizChunkCnt!=0 )
		{
			printf("%02X ", *(pucDumpCnt++));
			--sizChunkCnt;
		}
		// next line
		printf("\n");
		ulAddressCnt += sizChunkSize;
	}
}


/* Reset the netx 90 via console command. */
bool romloader_uart_read_functinoid_hboot3::netx90_reset(void)
{
	size_t sizLine;
	uint32_t ulLoadAddress;
	union
	{
		uint8_t auc[64];
		char ac[64];
	} uBuffer;
	union
	{
		uint8_t *puc;
		char *pc;
	} uResponse;
	unsigned int uiTimeoutMs;
	bool fOk;

	/* Be optimistic. */
	fOk = true;

	uiTimeoutMs = 500;

	/* Send the USIP command */
	fprintf(stderr, "Sending RESET command to netx 90\n");
	sizLine = snprintf(uBuffer.ac, sizeof(uBuffer), "reset\n");
	if( m_ptDevice->SendRaw(uBuffer.auc, sizLine, uiTimeoutMs)!=sizLine )
	{
		fprintf(stderr, "%s(%p): Failed to send the RESET command!\n", m_pcPortName, this);
		fOk = false;
	}
	else 	
	{
		/* Receive one line. This is the command echo. */
		fOk = m_ptDevice->GetLine(&uResponse.puc, "\r\n", 2000);
		if( fOk!=true )
		{
			fprintf(stderr, "failed to receive command echo!\n");
		}
		else
		{
			free(uResponse.puc);

			printf("Waiting for response\n");
			fOk = m_ptDevice->GetLine(&uResponse.puc, "Resetting the chip.\r\n", uiTimeoutMs);
			if( fOk!=true )
			{
				fprintf(stderr, "Failed to get response.\n");
			}
			else
			{
				printf("Response: '%s'\n", uResponse.pc);
				free(uResponse.puc);
				/* wait for RESET to execute.
				   Note: Parameter to SLEEP_MS must be <1000, 
				   if SLEEP_MS is implemented using usleep */
				SLEEP_MS(500);
			}
		}
	}
	return fOk;
}




/* Actually, just start the built-in machine interface */
int romloader_uart_read_functinoid_hboot3::update_device(ROMLOADER_CHIPTYP tChiptyp)
{
	bool fOk;
	int iResult;

	size_t sizData;
	const uint8_t * pucData;

	/* Expect failure. */
	iResult = -1;

	switch(tChiptyp)
	{
	case ROMLOADER_CHIPTYP_NETX90B:
	case ROMLOADER_CHIPTYP_NETX90C:
	case ROMLOADER_CHIPTYP_NETX90D:
		fprintf(stderr, "Attempting to enter the machine interface..\n");
		fOk = netx90_reset();
		if( fOk==true )
		{
			iResult = 0;
		}

		break;



	case ROMLOADER_CHIPTYP_NETX50:
	case ROMLOADER_CHIPTYP_NETX500:
	case ROMLOADER_CHIPTYP_NETX100:
	default:
		/* No idea how to update this one! */
		fprintf(stderr, "%s(%p): No strategy to update chip type %d!\n", m_pcPortName, this, tChiptyp);
		break;
	}
	
	return iResult;
}



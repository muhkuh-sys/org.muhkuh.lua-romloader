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


#include "romloader_uart_read_functinoid_hboot1.h"

/* Load- and entry points for the bootstrap firmware. */
#include "uartmon_netx10_bootstrap_run.h"

/* Data of the bootstrap firmware. */
#include "uartmon_netx10_bootstrap.h"

/* Data of the monitor firmware. */
#include "uartmon_netx10_monitor.h"


#ifdef _MSC_VER
#       define snprintf _snprintf
#endif


romloader_uart_read_functinoid_hboot1::romloader_uart_read_functinoid_hboot1(romloader_uart_device *ptDevice, char *pcPortName)
{
	m_ptDevice = ptDevice;
	m_pcPortName = 	pcPortName;
}

bool romloader_uart_read_functinoid_hboot1::read_data32(uint32_t ulAddress, uint32_t *pulValue)
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
			fOk = m_ptDevice->GetLine(&uResponse.puc, "\r\n>", 2000);
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



void romloader_uart_read_functinoid_hboot1::hexdump(const uint8_t *pucData, uint32_t ulSize)
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



bool romloader_uart_read_functinoid_hboot1::netx10_load_code(const uint8_t *pucNetxCode, size_t sizNetxCode)
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
	uuencoder tUuencoder;
	UUENCODER_PROGRESS_INFO_T tProgressInfo;


	/* Be optimistic. */
	fOk = true;

	uiTimeoutMs = 100;

	/* Construct the command. */
	sizLine = snprintf(uBuffer.ac, sizeof(uBuffer), "l %lx\n", BOOTSTRAP_DATA_START_NETX10);
	if( m_ptDevice->SendRaw(uBuffer.auc, sizLine, 500)!=sizLine )
	{
		fprintf(stderr, "%s(%p): Failed to send command!\n", m_pcPortName, this);
		fOk = false;
	}
	else if( m_ptDevice->GetLine(&uResponse.puc, "\r\n", 500)!=true )
	{
		fprintf(stderr, "%s(%p): Failed to get command echo!\n", m_pcPortName, this);
		fOk = false;
	}
	else
	{
		free(uResponse.puc);

		printf("Uploading firmware...\n");
		tUuencoder.set_data(pucNetxCode, sizNetxCode);

		/* Send the data line by line with a delay of 10ms. */
		do
		{
			sizLine = tUuencoder.process(uBuffer.ac, sizeof(uBuffer));
			if( sizLine!=0 )
			{
				uiTimeoutMs = 100;
				tUuencoder.get_progress_info(&tProgressInfo);
				printf("%05ld/%05ld (%d%%)\n", tProgressInfo.sizProcessed, tProgressInfo.sizTotal, tProgressInfo.uiPercent);
				if( m_ptDevice->SendRaw(uBuffer.auc, sizLine, 500)!=sizLine )
				{
					fprintf(stderr, "%s(%p): Failed to send uue data!\n", m_pcPortName, this);
					fOk = false;
					break;
				}

				// FIXME: The delay is not necessary for a USB connection. Detect USB/UART and enable the delay for UART.
				//SLEEP_MS(10);
			}
		} while( tUuencoder.isFinished()==false );

		if( fOk==true )
		{
			fOk = m_ptDevice->GetLine(&uResponse.puc, "\r\n>", 2000);
			if( fOk==true )
			{
				printf("Response: '%s'\n", uResponse.pc);
				free(uResponse.puc);
			}
			else
			{
				fprintf(stderr, "Failed to get response.\n");
			}
		}
		else
		{
			fprintf(stderr, "%s(%p): Failed to upload the firmware!\n", m_pcPortName, this);
		}
	}

	return fOk;
}



bool romloader_uart_read_functinoid_hboot1::netx10_start_code(void)
{
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
	size_t sizLine;
	bool fOk;
	uint32_t ulExecAddress;


	/* Construct the command. */
	sizLine = sprintf(uBuffer.ac, "g %lx 0\n", BOOTSTRAP_EXEC_NETX10);
	printf("Start command: '%s'\n", uBuffer.ac);
	if( m_ptDevice->SendRaw(uBuffer.auc, sizLine, 500)!=sizLine )
	{
		fprintf(stderr, "%s(%p): Failed to send command!\n", m_pcPortName, this);
		fOk = false;
	}
	else if( m_ptDevice->GetLine(&uResponse.puc, "\r\n", 2000)!=true )
	{
		fprintf(stderr, "%s(%p): Failed to get command echo!\n", m_pcPortName, this);
		fOk = false;
	}
	else
	{
		printf("Response: '%s'\n", uResponse.pc);
		free(uResponse.puc);
		fOk = true;
	}

	return fOk;
}




int romloader_uart_read_functinoid_hboot1::update_device(ROMLOADER_CHIPTYP tChiptyp)
{
	bool fOk;
	int iResult;


	fprintf(stderr, "update device.\n");

	/* Expect failure. */
	iResult = -1;

	switch(tChiptyp)
	{
	case ROMLOADER_CHIPTYP_NETX10:
		fprintf(stderr, "update netx10.\n");

		fOk = netx10_load_code(auc_uartmon_netx10_bootstrap, sizeof(auc_uartmon_netx10_bootstrap));
		if( fOk==true )
		{
			fOk = netx10_start_code();
			if( fOk==true )
			{
				if( m_ptDevice->SendRaw(auc_uartmon_netx10_monitor, sizeof(auc_uartmon_netx10_monitor), 500)!=sizeof(auc_uartmon_netx10_monitor) )
				{
					fprintf(stderr, "%s(%p): Failed to send command!\n", m_pcPortName, this);
				}
				else
				{
					/* The ROM code is now HBOOT_SOFT */
					iResult = 0;
				}
			}
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



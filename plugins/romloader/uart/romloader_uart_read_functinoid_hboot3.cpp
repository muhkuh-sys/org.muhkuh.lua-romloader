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


/*
000 >usip v
waiting for UUE upload...
Received 0x000004b8 bytes.
Upload OK. Restarting the system to run the image.


Send command "usip v"
Wait for message "waiting for UUE upload..."
Send boot image
Wait for message "Upload OK. Restarting the system to run the image."
send knock sequence for MI
*/

bool romloader_uart_read_functinoid_hboot3::netx90_load_usip(const uint8_t *pucNetxImage, size_t sizNetxImage)
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

	/* Send the USIP command */
	sizLine = snprintf(uBuffer.ac, sizeof(uBuffer), "usip v\n");
	if( m_ptDevice->SendRaw(uBuffer.auc, sizLine, 500)!=sizLine )
	{
		fprintf(stderr, "%s(%p): Failed to send the USIP command!\n", m_pcPortName, this);
		fOk = false;
	}
	else if( m_ptDevice->GetLine(&uResponse.puc, "waiting for UUE upload...\r\n", 500)!=true )
	{
		fprintf(stderr, "%s(%p): Failed to get the 'waiting for UUE upload' message!\n", m_pcPortName, this);
		fOk = false;
	}
	else
	{
		free(uResponse.puc);

		printf("Uploading boot image...\n");
		tUuencoder.set_data(pucNetxImage, sizNetxImage);

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
			printf("Waiting for response\n");
			fOk = m_ptDevice->GetLine(&uResponse.puc, "Upload OK. Restarting the system to run the image.\r\n", 2000);
			if( fOk==true )
			{
				printf("Response: '%s'\n", uResponse.pc);
				free(uResponse.puc);
				/* wait for USIP to execute.
				   Note: Parameter to SLEEP_MS must be <1000, 
				   if SLEEP_MS is implemented using usleep */
				SLEEP_MS(500);
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





bool romloader_uart_read_functinoid_hboot3::read_binary_file(const char* pucFilename, const uint8_t **ppucData, size_t *p_sizData)
{
	bool fRet = false;
	int iRet;
	FILE *fd;
	struct stat tStat;
	size_t sizFile;
	size_t sizBytesRead;
	const uint8_t * memblock;



	iRet = stat(pucFilename, &tStat);
	if (iRet != 0) 
	{
		fprintf(stderr, "Could not stat file %s\n", pucFilename);
	}
	else
	{
		sizFile = tStat.st_size;

		fd = fopen(pucFilename, "rb");
		if (fd == NULL) 
		{
			fprintf(stderr, "Could not open file %s\n", pucFilename);
		}
		else
		{
			memblock = new uint8_t[sizFile];
			
			if (memblock == NULL)
			{
				fprintf(stderr, "Could not allocate %zu bytes\n", sizFile);
			}
			else
			{
				sizBytesRead = fread((void*)memblock, 1, sizFile, fd);

				if (sizBytesRead != sizFile) 
				{
					fprintf(stderr, "Did not read the complete file (%zu/%zu bytes)\n", 
						sizBytesRead, sizFile);
				}
				else
				{
					printf("%zu bytes read.\n", sizBytesRead);
					*ppucData = memblock;
					*p_sizData = sizFile;
					fRet = true;
				}
			}
			fclose(fd);
		}
	}
	return fRet;
}


bool romloader_uart_read_functinoid_hboot3::get_netx90_mi_image(const uint8_t **ppucData, size_t *p_sizData)
{
	bool fRet = false;
	romloader_uart_options *ptOptions;
	const char *pcImageData;
	size_t sizImageLen;
	
	

	ptOptions = m_ptDevice->GetOptions();
	if (ptOptions != NULL)
	{
		ptOptions->getOption_netx90MiImage(&pcImageData, &sizImageLen);
		if ((pcImageData != NULL) && (sizImageLen > 0))
		{
			printf("Got M2M boot image for netx 90, %zu bytes\n", sizImageLen);
			*ppucData = (uint8_t*) pcImageData;
			*p_sizData = sizImageLen;
			fRet = true;
		}
		else 
		{
			*ppucData = NULL;
			*p_sizData = 0;
			fprintf(stderr, "M2M boot image for netx 90 not available (option not set).\n");
		}
	}
	else
	{
		*ppucData = NULL;
		*p_sizData = 0;
		fprintf(stderr, "M2M boot image for netx 90 not available (no options).\n");
	}
	
	return fRet;
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
		fprintf(stderr, "Attempting to enter the machine interface..\n");

		fOk = get_netx90_mi_image(&pucData, &sizData);
		//fOk = read_binary_file("disable_secboot_netx90_com_intram_signed.bin", &pucData, &sizData);

		if( fOk==true )
		{
			fOk = netx90_load_usip(pucData, sizData);
			delete pucData;

			if( fOk==true )
			{
				/* The ROM code is now HBOOT_3.0 machine interface */
				iResult = 0;
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



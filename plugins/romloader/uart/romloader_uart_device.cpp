/***************************************************************************
 *   Copyright (C) 2008 by Christoph Thelen and M. Trensch                 *
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


#include "romloader_uart_device.h"

#include "../uuencoder.h"

/* Load- and entry points for the bootstrap firmware. */
#include "netx/targets/uartmon_netx10_bootstrap_run.h"
#include "netx/targets/uartmon_netx50_bootstrap_run.h"
#include "netx/targets/uartmon_netx500_bootstrap_run.h"

/* Data of the bootstrap firmware. */
#include "netx/targets/uartmon_netx10_bootstrap.h"
#include "netx/targets/uartmon_netx50_bootstrap.h"
#include "netx/targets/uartmon_netx500_bootstrap.h"

/* Data of the monitor firmware. */
#include "netx/targets/uartmon_netx10_monitor.h"
#include "netx/targets/uartmon_netx50_monitor.h"
#include "netx/targets/uartmon_netx500_monitor.h"

#include <stdio.h>
#include <string.h>


#if defined(_MSC_VER)
#       define snprintf _snprintf
#       define SLEEP_MS(ms) Sleep(ms)
#elif defined(__GNUC__) || defined(__MINGW32__) || defined(__MINGW64__)
#       include <unistd.h>
#       define SLEEP_MS(ms) usleep(ms*1000)
#endif


#if MUHKUH_USE_THREADING==MUHKUH_USE_THREADING_MSVC
#       define CRITICAL_SECTION_ENTER(cs) EnterCriticalSection(&cs)
#       define CRITICAL_SECTION_LEAVE(cs) LeaveCriticalSection(&cs)
#elif MUHKUH_USE_THREADING==MUHKUH_USE_THREADING_PTHREAD
#       define CRITICAL_SECTION_ENTER(cs) pthread_mutex_lock(&cs)
#       define CRITICAL_SECTION_LEAVE(cs) pthread_mutex_unlock(&cs)

	static const pthread_mutex_t s_mutex_init = PTHREAD_MUTEX_INITIALIZER;
#else
#       error "Unknown threading selected!"
#endif



romloader_uart_device::romloader_uart_device(const char *pcPortName)
 : m_pcPortName(NULL)
 , m_ptFirstCard(NULL)
 , m_ptLastCard(NULL)
{
	m_pcPortName = strdup(pcPortName);

#if MUHKUH_USE_THREADING==MUHKUH_USE_THREADING_MSVC
	InitializeCriticalSection(&m_csCardLock);
#elif MUHKUH_USE_THREADING==MUHKUH_USE_THREADING_PTHREAD
	memcpy(&m_csCardLock, &s_mutex_init, sizeof(pthread_mutex_t));
#else
#       error "Unknown threading selected!"
#endif
}


romloader_uart_device::~romloader_uart_device(void)
{
	deleteCards();

#if MUHKUH_USE_THREADING==MUHKUH_USE_THREADING_MSVC
	DeleteCriticalSection(&m_csCardLock);
#elif MUHKUH_USE_THREADING==MUHKUH_USE_THREADING_PTHREAD
	/* Nothing to do here. */
#else
#       error "Unknown threading selected!"
#endif
}


void romloader_uart_device::initCards(void)
{
	tBufferCard *ptCard;


	if( m_ptFirstCard!=NULL )
	{
		deleteCards();
	}

	ptCard = new tBufferCard;
	ptCard->pucEnd = ptCard->aucData + mc_sizCardSize;
	ptCard->pucRead = ptCard->aucData;
	ptCard->pucWrite = ptCard->aucData;
	ptCard->ptNext = NULL;

	m_ptFirstCard = ptCard;
	m_ptLastCard = ptCard;
}


void romloader_uart_device::deleteCards(void)
{
	tBufferCard *ptCard;
	tBufferCard *ptNextCard;


	CRITICAL_SECTION_ENTER(m_csCardLock);

	ptCard = m_ptFirstCard;
	while( ptCard!=NULL )
	{
		ptNextCard = ptCard->ptNext;
		delete ptCard;
		ptCard = ptNextCard;
	}
	m_ptFirstCard = NULL;
	m_ptLastCard = NULL;

	CRITICAL_SECTION_LEAVE(m_csCardLock);
}


void romloader_uart_device::writeCards(const unsigned char *pucBuffer, size_t sizBufferSize)
{
	size_t sizLeft;
	size_t sizChunk;
	tBufferCard *ptCard;


	CRITICAL_SECTION_ENTER(m_csCardLock);

	sizLeft = sizBufferSize;
	while( sizLeft>0 )
	{
		// get free space in the current card
		sizChunk = m_ptLastCard->pucEnd - m_ptLastCard->pucWrite;
		// no more space -> create a new card
		if( sizChunk==0 )
		{
			ptCard = new tBufferCard;
			ptCard->pucEnd = ptCard->aucData + mc_sizCardSize;
			ptCard->pucRead = ptCard->aucData;
			ptCard->pucWrite = ptCard->aucData;
			ptCard->ptNext = NULL;
			// append new card
			m_ptLastCard->ptNext = ptCard;
			// close old card
			m_ptLastCard->pucWrite = NULL;
			// set the new last pointer
			m_ptLastCard = ptCard;
			// the new card is empty
			sizChunk = mc_sizCardSize;
		}
		// limit chunk to request size
		if( sizChunk>sizLeft )
		{
			sizChunk = sizLeft;
		}
		// copy data
		memcpy(m_ptLastCard->pucWrite, pucBuffer, sizChunk);
		// advance pointer
		m_ptLastCard->pucWrite += sizChunk;
		pucBuffer += sizChunk;
		sizLeft -= sizChunk;
	}

	CRITICAL_SECTION_LEAVE(m_csCardLock);

	/* Set the signal for received data. */
#if MUHKUH_USE_THREADING==MUHKUH_USE_THREADING_MSVC
	/* Nothing to do here. */
#elif MUHKUH_USE_THREADING==MUHKUH_USE_THREADING_PTHREAD
	pthread_mutex_lock(&m_tRxDataAvail_Mutex);
	pthread_cond_signal(&m_tRxDataAvail_Condition);
	pthread_mutex_unlock(&m_tRxDataAvail_Mutex);
#else
#       error "Unknown threading selected!"
#endif
}


size_t romloader_uart_device::readCards(unsigned char *pucBuffer, size_t sizBuffer)
{
	size_t sizLeft;
	size_t sizRead;
	tBufferCard *ptOldCard;
	tBufferCard *ptNewCard;


	sizLeft = sizBuffer;
	do
	{
		if( m_ptFirstCard==NULL )
		{
			sizRead = 0;
			break;
		}
		else if( m_ptFirstCard->pucWrite!=NULL )
		{
			/* The first card is used by the write part -> lock the cards. */
			CRITICAL_SECTION_ENTER(m_csCardLock);

			/* Get the number of bytes left in this card. */
			sizRead = m_ptFirstCard->pucWrite - m_ptFirstCard->pucRead;
			if( sizRead>sizLeft )
			{
				sizRead = sizLeft;
			}
			/* Card can be empty. */
			if( sizRead>0 )
			{
				/* Copy the data. */
				memcpy(pucBuffer, m_ptFirstCard->pucRead, sizRead);
				/* Advance the read pointer. */
				m_ptFirstCard->pucRead += sizRead;
			}

			CRITICAL_SECTION_LEAVE(m_csCardLock);

			if( sizRead==0 )
			{
				/* No more data available. */
				break;
			}
		}
		else
		{
			// the first card is not used by the write part

			// get the number of bytes left in this card
			sizRead = m_ptFirstCard->pucEnd - m_ptFirstCard->pucRead;
			if( sizRead>sizLeft )
			{
				sizRead = sizLeft;
			}
			// card can be empty for overlapping buffer grow
			if( sizRead>0 )
			{
				// copy the data
				memcpy(pucBuffer, m_ptFirstCard->pucRead, sizRead);
				// advance the read pointer
				m_ptFirstCard->pucRead += sizRead;
			}
			else
			{
				fprintf(stderr, "WARNING: no remaining data: %d\n", sizRead);
			}
			// reached the end of the buffer?
			if( m_ptFirstCard->pucRead>=m_ptFirstCard->pucEnd )
			{
				// card is empty, move on to next card
				ptNewCard = m_ptFirstCard->ptNext;
				if( ptNewCard!=NULL )
				{
					// remember the empty card
					ptOldCard = m_ptFirstCard;
					// move to the new first card
					m_ptFirstCard = ptNewCard;
					// delete the empty card
					delete ptOldCard;
				}
			}
		}

		sizLeft -= sizRead;
		pucBuffer += sizRead;
	} while( sizLeft>0 );

	return sizBuffer-sizLeft;
}


size_t romloader_uart_device::getCardSize(void) const
{
	size_t sizData;
	tBufferCard *ptCard;


	sizData = 0;
	ptCard = m_ptFirstCard;
	while( ptCard!=NULL )
	{
		if( ptCard->pucWrite==NULL )
		{
			sizData += m_ptFirstCard->pucEnd - m_ptFirstCard->pucRead;
		}
		else
		{
			sizData += m_ptFirstCard->pucWrite - m_ptFirstCard->pucRead;
		}
		ptCard = ptCard->ptNext;
	}

	return sizData;
}


unsigned int romloader_uart_device::crc16(unsigned short usCrc, unsigned char ucData)
{
	usCrc  = (usCrc >> 8) | ((usCrc & 0xff) << 8);
	usCrc ^= ucData;
	usCrc ^= (usCrc & 0xff) >> 4;
	usCrc ^= (usCrc & 0x0f) << 12;
	usCrc ^= ((usCrc & 0xff) << 4) << 1;

	return usCrc;
}


bool romloader_uart_device::wait_for_prompt(unsigned long ulTimeout)
{
	bool fFoundPrompt;
	const size_t sizMaxSearch = 32;
	size_t sizCnt;
	size_t sizReceived;
	unsigned char ucData;


	fFoundPrompt = false;

	sizCnt = 0;
	do
	{
		sizReceived = RecvRaw(&ucData, 1, ulTimeout);
		if( sizReceived!=1 )
		{
			/* Failed to receive the next char. */
			fprintf(stderr, "Failed to receive the knock response.\n");
			break;
		}
		else if( ucData=='>' )
		{
			fFoundPrompt = true;
			break;
		}
		else
		{
			++sizCnt;
		}
	} while( sizCnt<sizMaxSearch );

	return fFoundPrompt;
}


bool romloader_uart_device::GetLine(unsigned char **ppucLine, const char *pcEol, unsigned long ulTimeout)
{
	bool fOk;
	unsigned char *pucBuffer;
	unsigned char *pucBufferNew;
	size_t sizBufferCnt;
	size_t sizBufferMax;
	size_t sizEolSeq;
	size_t sizReceived;


	/* Receive char by char until the EOL sequence was received. */

	/* Expect success. */
	fOk = true;

	sizEolSeq = strlen(pcEol);

	/* Init References array. */
	sizBufferCnt = 0;
	sizBufferMax = 80;
	pucBuffer = (unsigned char*)malloc(sizBufferMax);
	if( pucBuffer==NULL )
	{
		fprintf(stderr, "out of memory!\n");
		fOk = false;
	}
	else
	{
		do
		{
			sizReceived = RecvRaw(pucBuffer+sizBufferCnt, 1, ulTimeout);
			if( sizReceived!=1 )
			{
				/* Timeout, that's it... */
				fprintf(stderr, "Timeout!\n");

				fOk = false;
				break;
			}

			/* Check for EOL. */
			sizBufferCnt++;
			if( sizBufferCnt>=sizEolSeq && memcmp(pcEol, pucBuffer+sizBufferCnt-sizEolSeq, sizEolSeq)==0 )
			{
				break;
			}

			/* Is enough space in the array for one more entry? */
			/* NOTE: Do this at the end of the loop because the line is
			terminated with a '\0'. */
			if( sizBufferCnt>=sizBufferMax )
			{
				/* No -> expand the array. */
				sizBufferMax *= 2;
				/* Detect overflow or limitation. */
				if( sizBufferMax<=sizBufferCnt )
				{
					fOk = false;
					break;
				}
				/* Reallocate the array. */
				pucBufferNew = (unsigned char*)realloc(pucBuffer, sizBufferMax);
				if( pucBufferNew==NULL )
				{
					fOk = false;
					break;
				}
				pucBuffer = pucBufferNew;
			}
		} while( true );
	}

	if( fOk==true )
	{
		/* Terminate the line buffer. */
		pucBuffer[sizBufferCnt] = 0;
	}
	else if( pucBuffer!=NULL )
	{
		free(pucBuffer);
		pucBuffer = NULL;
	}

	*ppucLine = pucBuffer;

	return fOk;
}


bool romloader_uart_device::legacy_read_v1(unsigned long ulAddress, unsigned long *pulValue)
{
	union
	{
		unsigned char auc[32];
		char ac[32];
	} uCmd;
	union
	{
		unsigned char *puc;
		char *pc;
	} uResponse;
	size_t sizCmd;
	bool fOk;
	int iResult;
	unsigned long ulReadbackAddress;
	unsigned long ulValue;


	sizCmd = snprintf(uCmd.ac, 32, "DUMP %lx\n", ulAddress);
	/* Send knock sequence with 500ms second timeout. */
	if( SendRaw(uCmd.auc, sizCmd, 500)!=sizCmd )
	{
		/* Failed to send the command to the device. */
		fprintf(stderr, "Failed to send the command to the device.\n");
		fOk = false;
	}
	else
	{
		/* Receive one line. This is the command echo. */
		fOk = GetLine(&uResponse.puc, "\r\n", 2000);
		if( fOk==false )
		{
			fprintf(stderr, "failed to get command response!\n");
		}
		else
		{
			sizCmd = strlen(uResponse.pc);
			hexdump(uResponse.puc, sizCmd);
			free(uResponse.puc);

			/* Receive one line. This is the command result. */
			fOk = GetLine(&uResponse.puc, "\r\n>", 2000);
			if( fOk==false )
			{
				fprintf(stderr, "failed to get command response!\n");
			}
			else
			{
				iResult = sscanf(uResponse.pc, "%08lx: %08lx", &ulReadbackAddress, &ulValue);
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


bool romloader_uart_device::legacy_read_v2(unsigned long ulAddress, unsigned long *pulValue)
{
	union
	{
		unsigned char auc[32];
		char ac[32];
	} uCmd;
	union
	{
		unsigned char *puc;
		char *pc;
	} uResponse;
	size_t sizCmd;
	bool fOk;
	int iResult;
	unsigned long ulReadbackAddress;
	unsigned long ulValue;


	sizCmd = snprintf(uCmd.ac, 32, "D %lx ++3\n", ulAddress);
	/* Send the command with 500ms second timeout. */
	if( SendRaw(uCmd.auc, sizCmd, 500)!=sizCmd )
	{
		/* Failed to send the command to the device. */
		fprintf(stderr, "Failed to send the command to the device.\n");
		fOk = false;
	}
	else
	{
		/* Receive one line. This is the command echo. */
		fOk = GetLine(&uResponse.puc, "\r\n", 2000);
		if( fOk==false )
		{
			fprintf(stderr, "failed to get command response!\n");
		}
		else
		{
			sizCmd = strlen(uResponse.pc);
			hexdump(uResponse.puc, sizCmd);
			free(uResponse.puc);

			/* Receive the rest of the output until the command prompt. This is the command result. */
			fOk = GetLine(&uResponse.puc, "\r\n>", 2000);
			if( fOk==false )
			{
				fprintf(stderr, "failed to get command response!\n");
			}
			else
			{
				iResult = sscanf(uResponse.pc, "%08lx: %08lx", &ulReadbackAddress, &ulValue);
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


bool romloader_uart_device::netx10_load_code(const unsigned char *pucNetxCode, size_t sizNetxCode)
{
	size_t sizLine;
	unsigned long ulLoadAddress;
	union
	{
		unsigned char auc[64];
		char ac[64];
	} uBuffer;
	union
	{
		unsigned char *puc;
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
	if( SendRaw(uBuffer.auc, sizLine, 500)!=sizLine )
	{
		fprintf(stderr, "%s(%p): Failed to send command!\n", m_pcPortName, this);
		fOk = false;
	}
	else if( GetLine(&uResponse.puc, "\r\n", 500)!=true )
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
				printf("%05d/%05d (%d%%)\n", tProgressInfo.sizProcessed, tProgressInfo.sizTotal, tProgressInfo.uiPercent);
				printf("UUE line: '%s'\n", uBuffer.ac);
				if( SendRaw(uBuffer.auc, sizLine, 500)!=sizLine )
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
			fOk = GetLine(&uResponse.puc, "\r\n>", 2000);
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


bool romloader_uart_device::netx50_load_code(const unsigned char *pucNetxCode, size_t sizNetxCode)
{
	size_t sizLine;
	unsigned long ulLoadAddress;
	union
	{
		unsigned char auc[64];
		char ac[64];
	} uBuffer;
	union
	{
		unsigned char *puc;
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
	sizLine = snprintf(uBuffer.ac, sizeof(uBuffer), "luue %lx\n", BOOTSTRAP_DATA_START_NETX50);
	if( SendRaw(uBuffer.auc, sizLine, 500)!=sizLine )
	{
		fprintf(stderr, "%s(%p): Failed to send command!\n", m_pcPortName, this);
		fOk = false;
	}
	else if( GetLine(&uResponse.puc, "\r\n", 500)!=true )
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
				printf("%05d/%05d (%d%%)\n", tProgressInfo.sizProcessed, tProgressInfo.sizTotal, tProgressInfo.uiPercent);
				if( SendRaw(uBuffer.auc, sizLine, 500)!=sizLine )
				{
					fprintf(stderr, "%s(%p): Failed to send uue data!\n", m_pcPortName, this);
					fOk = false;
					break;
				}
				else if( GetLine(&uResponse.puc, "\r\n", 500)!=true )
				{
					fprintf(stderr, "%s(%p): Failed to get response line!\n", m_pcPortName, this);
					fOk = false;
					break;
				}
				free(uResponse.puc);

				// FIXME: The delay is not necessary for a USB connection. Detect USB/UART and enable the delay for UART.
				//SLEEP_MS(10);
			}
		} while( tUuencoder.isFinished()==false );

		if( fOk==true )
		{
			fOk = GetLine(&uResponse.puc, "\r\n>", 2000);
			if( fOk==true )
			{
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


bool romloader_uart_device::netx500_load_code(const unsigned char *pucNetxCode, size_t sizNetxCode)
{
	unsigned short usCrc;
	size_t sizCnt;
	size_t sizLine;
	union
	{
		unsigned char auc[64];
		char ac[64];
	} uBuffer;
	union
	{
		unsigned char *puc;
		char *pc;
	} uResponse;
	unsigned int uiTimeoutMs;
	bool fOk;


	/* Be optimistic. */
	fOk = true;

	uiTimeoutMs = 100;

	/* Get the CRC16 checksum for the bootstrap code. */
	usCrc = 0xffff;
	for(sizCnt=0; sizCnt<sizNetxCode; sizCnt++)
	{
		usCrc = crc16(usCrc, pucNetxCode[sizCnt]);
	}

	/* Construct the command. */
	sizLine = snprintf(uBuffer.ac, sizeof(uBuffer), "LOAD %lx %x %x 10000\n", BOOTSTRAP_DATA_START_NETX500, sizNetxCode, usCrc);
	printf("Load command:\n");
	hexdump(uBuffer.auc, sizLine);
	if( SendRaw(uBuffer.auc, sizLine, 500)!=sizLine )
	{
		fprintf(stderr, "%s(%p): Failed to send command!\n", m_pcPortName, this);
		fOk = false;
	}
	else if( GetLine(&uResponse.puc, "\r\n", 500)!=true )
	{
		fprintf(stderr, "%s(%p): Failed to get command echo!\n", m_pcPortName, this);
		fOk = false;
	}
	else
	{
		printf("response: '%s'\n", uResponse.pc);
		free(uResponse.puc);

		printf("Uploading firmware...\n");
		if( SendRaw(pucNetxCode, sizNetxCode, 500)!=sizNetxCode )
		{
			fprintf(stderr, "%s(%p): Failed to upload the firmware!\n", m_pcPortName, this);
			fOk = false;
		}
		else
		{
			fOk = GetLine(&uResponse.puc, "\r\n>", 2000);
			if( fOk==true )
			{
				printf("response: '%s'\n", uResponse.pc);
				free(uResponse.puc);
			}
			else
			{
				fprintf(stderr, "Failed to get response.\n");
			}
		}
	}

	return fOk;
}



bool romloader_uart_device::netx10_start_code(void)
{
	union
	{
		unsigned char auc[64];
		char ac[64];
	} uBuffer;
	union
	{
		unsigned char *puc;
		char *pc;
	} uResponse;
	size_t sizLine;
	bool fOk;
	unsigned long ulExecAddress;


	/* Construct the command. */
	sizLine = sprintf(uBuffer.ac, "g %lx 0\n", BOOTSTRAP_EXEC_NETX10);
	printf("Start command: '%s'\n", uBuffer.ac);
	if( SendRaw(uBuffer.auc, sizLine, 500)!=sizLine )
	{
		fprintf(stderr, "%s(%p): Failed to send command!\n", m_pcPortName, this);
		fOk = false;
	}
	else if( GetLine(&uResponse.puc, "\r\n", 2000)!=true )
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


bool romloader_uart_device::netx50_start_code(void)
{
	union
	{
		unsigned char auc[64];
		char ac[64];
	} uBuffer;
	union
	{
		unsigned char *puc;
		char *pc;
	} uResponse;
	size_t sizLine;
	bool fOk;
	unsigned long ulExecAddress;


	/* Construct the command. */
	sizLine = sprintf(uBuffer.ac, "call %lx\n", BOOTSTRAP_EXEC_NETX50);

	if( SendRaw(uBuffer.auc, sizLine, 500)!=sizLine )
	{
		fprintf(stderr, "%s(%p): Failed to send command!\n", m_pcPortName, this);
		fOk = false;
	}
	else if( GetLine(&uResponse.puc, "\r\n", 2000)!=true )
	{
		fprintf(stderr, "%s(%p): Failed to get command echo!\n", m_pcPortName, this);
		fOk = false;
	}
	else
	{
		free(uResponse.puc);
		fOk = true;
	}

	return fOk;
}


bool romloader_uart_device::netx500_start_code(void)
{
	union
	{
		unsigned char auc[64];
		char ac[64];
	} uBuffer;
	union
	{
		unsigned char *puc;
		char *pc;
	} uResponse;
	size_t sizLine;
	bool fOk;
	unsigned long ulExecAddress;


	/* Construct the command. */
	sizLine = sprintf(uBuffer.ac, "CALL %lx\n", BOOTSTRAP_EXEC_NETX500);
	printf("Load command: '%s'\n", uBuffer.ac);

	if( SendRaw(uBuffer.auc, sizLine, 500)!=sizLine )
	{
		fprintf(stderr, "%s(%p): Failed to send command!\n", m_pcPortName, this);
		fOk = false;
	}
	else if( GetLine(&uResponse.puc, "\r\n", 2000)!=true )
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


ROMLOADER_ROMCODE romloader_uart_device::update_device(ROMLOADER_CHIPTYP tChiptyp, ROMLOADER_ROMCODE tRomcode)
{
	bool fOk;
	ROMLOADER_ROMCODE tNewRomcode;


	fprintf(stderr, "update device.\n");

	/* Expect failure. */
	tNewRomcode = ROMLOADER_ROMCODE_UNKNOWN;

	if( tChiptyp==ROMLOADER_CHIPTYP_NETX50 )
	{
		fprintf(stderr, "update netx50.\n");

		fOk = netx50_load_code(auc_uartmon_netx50_bootstrap, sizeof(auc_uartmon_netx50_bootstrap));
		if( fOk==true )
		{
			fOk = netx50_start_code();
			if( fOk==true )
			{
				if( SendRaw(auc_uartmon_netx50_monitor, sizeof(auc_uartmon_netx50_monitor), 500)!=sizeof(auc_uartmon_netx50_monitor) )
				{
					fprintf(stderr, "%s(%p): Failed to send command!\n", m_pcPortName, this);
				}
				else
				{
					/* The ROM code is now HBOOT2_SOFT */
					tNewRomcode = ROMLOADER_ROMCODE_HBOOT2_SOFT;
				}
			}
		}
	}
	else if( tChiptyp==ROMLOADER_CHIPTYP_NETX500 || tChiptyp==ROMLOADER_CHIPTYP_NETX100 )
	{
		fprintf(stderr, "update netx500.\n");

		fOk = netx500_load_code(auc_uartmon_netx500_bootstrap, sizeof(auc_uartmon_netx500_bootstrap));
		{
			fOk = netx500_start_code();
			if( fOk==true )
			{
				if( SendRaw(auc_uartmon_netx500_monitor, sizeof(auc_uartmon_netx500_monitor), 500)!=sizeof(auc_uartmon_netx500_monitor) )
				{
					fprintf(stderr, "%s(%p): Failed to send command!\n", m_pcPortName, this);
				}
				else
				{
					/* The ROM code is now HBOOT2_SOFT */
					tNewRomcode = ROMLOADER_ROMCODE_HBOOT2_SOFT;
				}
			}
		}
	}
	else if( tChiptyp==ROMLOADER_CHIPTYP_NETX10 )
	{
		fprintf(stderr, "update netx10.\n");

		fOk = netx10_load_code(auc_uartmon_netx10_bootstrap, sizeof(auc_uartmon_netx10_bootstrap));
		{
			fOk = netx10_start_code();
			printf("start: %d\n", fOk);
			if( fOk==true )
			{
				if( SendRaw(auc_uartmon_netx10_monitor, sizeof(auc_uartmon_netx10_monitor), 500)!=sizeof(auc_uartmon_netx10_monitor) )
				{
					fprintf(stderr, "%s(%p): Failed to send command!\n", m_pcPortName, this);
				}
				else
				{
					/* The ROM code is now HBOOT2_SOFT */
					tNewRomcode = ROMLOADER_ROMCODE_HBOOT2_SOFT;
				}
			}
		}
	}
	
	return tNewRomcode;
}


bool romloader_uart_device::SendBlankLineAndDiscardResponse(void)
{
	const unsigned char aucBlankLine[1] = { '\n' };
	size_t sizTransfered;
	bool fOk;

	
	/* Send enter. */
	sizTransfered = SendRaw(aucBlankLine, sizeof(aucBlankLine), 200);
	if( sizTransfered!=1 )
	{
		fprintf(stderr, "Failed to send enter to device!\n");
		fOk = false;
	}
	else
	{
		/* Receive the rest of the line until '>'. Discard the data. */
		printf("receive the rest of the knock response\n");
		fOk = wait_for_prompt(200);
		if( fOk!=true )
		{
			fprintf(stderr, "received strange response after romloader message!\n");
		}
	}

	return fOk;
}


void romloader_uart_device::hexdump(const unsigned char *pucData, unsigned long ulSize)
{
	const unsigned char *pucDumpCnt, *pucDumpEnd;
	unsigned long ulAddressCnt;
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
		printf("%08lX: ", ulAddressCnt);
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

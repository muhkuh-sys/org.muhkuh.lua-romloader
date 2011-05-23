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

#include <stdio.h>
#include <string.h>

#ifdef _WINDOWS
#       define CRITICAL_SECTION_ENTER(cs) EnterCriticalSection(&cs)
#       define CRITICAL_SECTION_LEAVE(cs) LeaveCriticalSection(&cs)
#else
#       define CRITICAL_SECTION_ENTER(cs) pthread_mutex_lock(&cs)
#       define CRITICAL_SECTION_LEAVE(cs) pthread_mutex_unlock(&cs)

	static const pthread_mutex_t s_mutex_init = PTHREAD_MUTEX_INITIALIZER;
#endif


romloader_uart_device::romloader_uart_device(const char *pcPortName)
 : m_pcPortName(NULL)
 , m_ptFirstCard(NULL)
 , m_ptLastCard(NULL)
{
	m_pcPortName = strdup(pcPortName);

#ifdef _WINDOWS
	InitializeCriticalSection(&m_csCardLock);
#else
	memcpy(&m_csCardLock, &s_mutex_init, sizeof(pthread_mutex_t));
#endif
}


romloader_uart_device::~romloader_uart_device(void)
{
	deleteCards();

#ifdef _WINDOWS
	DeleteCriticalSection(&m_csCardLock);
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
			fprintf(stderr, "New Card\n");

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
#ifndef _WINDOWS
	pthread_mutex_lock(&m_tRxDataAvail_Mutex);
	pthread_cond_signal(&m_tRxDataAvail_Condition);
	pthread_mutex_unlock(&m_tRxDataAvail_Mutex);
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


bool romloader_uart_device::GetLine(unsigned char **ppucLine, const char *pcEol, unsigned long ulTimeout)
{
	bool fOk;
	unsigned char *pucBuffer;
	unsigned char *pucBufferNew;
	size_t sizBufferCnt;
	size_t sizBufferMax;
	size_t sizEolSeqCnt;
	size_t sizEolSeqMax;
	size_t sizReceived;


	/* Receive char by char until the EOL sequence was received. */

	/* Expect success. */
	fOk = true;

	sizEolSeqCnt = 0;
	sizEolSeqMax = strlen(pcEol);

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
				fOk = false;
				break;
			}
			else
			{
				/* Check for EOL. */
				if( pcEol[sizEolSeqCnt]==pucBuffer[sizBufferCnt] )
				{
					++sizEolSeqCnt;
				}
				else
				{
					sizEolSeqCnt = 0;
				}
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
		} while( sizEolSeqCnt<sizEolSeqMax );
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


const char *romloader_uart_device::apcRomcodeWelcomeStrings[3] =
{
	"netX500 Step A\r\n",
	"netX50 Step B\r\n",
	"netX10 Step B\r\n"
};

bool romloader_uart_device::IdentifyLoader(void)
{
	bool fResult = false;
	const unsigned char aucKnock[5] = { '*', 0x00, 0x00, '*', '#' };
	const unsigned char aucKnockResponseMi[7] = { 0x09, 0x00, 0x00, 0x4d, 0x4f, 0x4f, 0x48 };
	unsigned char aucData[13];
	unsigned char *pucResponse;
	size_t sizCnt;
	unsigned long ulMiVersionMaj;
	unsigned long ulMiVersionMin;
	bool fDeviceNeedsUpdate;
	unsigned short usCrc;
	const unsigned char aucBlankLine[1] = { '\n' };


	/* No update by default. */
	fDeviceNeedsUpdate = false;

	/* Send knock sequence with 500ms second timeout. */
	if( SendRaw(aucKnock, sizeof(aucKnock), 500)!=sizeof(aucKnock) )
	{
		/* Failed to send knock sequence to device. */
		fprintf(stderr, "Failed to send knock sequence to device.\n");
	}
	else if( Flush()!=true )
	{
		/* Failed to flush the knock sequence. */
		fprintf(stderr, "Failed to flush the knock sequence.\n");
	}
	else
	{
		sizCnt = RecvRaw(aucData, 1, 1000);
		if( sizCnt!=1 )
		{
			/* Failed to receive first char of knock response. */
			fprintf(stderr, "Failed to receive first char of knock response.\n");
		}
		else
		{
			printf("received knock response: 0x%02x\n", aucData[0]);
			/* This should be '\f', but the first bits might be trashed. */
			if( aucData[0]<0x20 )
			{
				/* This seems to be the welcome message. */

				/* Receive the rest of the line. */
				printf("receive the rest of the knock response\n");
				fResult = GetLine(&pucResponse, "\r\n", 200);
				if( fResult!=true )
				{
					fprintf(stderr, "Failed to receive the rest of the knock response!\n");
				}
				else
				{
					fResult = false;
					for(sizCnt=0; sizCnt<(sizeof(apcRomcodeWelcomeStrings)/sizeof(const char**)); ++sizCnt)
					{
						if( strcmp((const char*)pucResponse, apcRomcodeWelcomeStrings[sizCnt])==0 )
						{
							fResult = true;
							break;
						}
					}
					/* Check for initial bootloader message. */
					if( fResult==false )
					{
						/* Seems to be no netX bootloader. */
						fprintf(stderr, "Received invalid response from the device: '%s'\n", pucResponse);
						free(pucResponse);
					}
					else
					{
						free(pucResponse);

						/* Get prompt, the real console eats the first char, the usb console will echo it. */
						fResult = false;
						sizCnt = RecvRaw(aucData, 3, 400);
						if( sizCnt==0 )
						{
							printf("received no further data after romloader message.\n");
						}
						else if( aucData[0]!='>' )
						{
							fprintf(stderr, "received strange response after romloader message!\n");
							fprintf(stderr, "len: %d, data = 0x%02x, 0x%02x, 0x%02x\n", sizCnt, aucData[0], aucData[1], aucData[2]);
						}
						else if( sizCnt==1 )
						{
							printf("ok, received prompt!\n");
							fDeviceNeedsUpdate = true;
							fResult = true;
						}
						else if( sizCnt==2 && aucData[1]=='#' )
						{
							printf("ok, received hash!\n");
							fDeviceNeedsUpdate = true;
							fResult = true;
						}
						else if( sizCnt==3 && aucData[1]=='*' && aucData[2]=='#' )
						{
							printf("ok, received star and hash!");
							fDeviceNeedsUpdate = true;
							fResult = true;
						}
						else
						{
							fprintf(stderr, "received strange response after romloader message!\n");
							printf("len: %d, data = 0x%02x, 0x%02x, 0x%02x", sizCnt, aucData[0], aucData[1], aucData[2]);
						}
					}
				}
			}
			/* Knock echoed -> this is the prompt or the machine interface. */
			else if( aucData[0]=='*' )
			{
				printf("ok, received star!\n");

				/* Get rest of the response. */
				sizCnt = RecvRaw(aucData, 13, 500);
				hexdump(aucData, sizCnt);
				printf("%d\n", sizCnt);
				hexdump(aucKnockResponseMi, 7);
				if( sizCnt==0 )
				{
					fprintf(stderr, "Failed to receive response after the star!\n");
				}
				else if( sizCnt==13 && memcmp(aucData, aucKnockResponseMi, 7)==0 )
				{
					/* Build the crc for the packet. */
					usCrc = 0;
					for(sizCnt==0; sizCnt<13; ++sizCnt)
					{
						usCrc = crc16(usCrc, aucData[sizCnt]);
					}
					if( usCrc!=0 )
					{
						fprintf(stderr, "Crc of version packet is invalid!\n");
					}
					else
					{
						ulMiVersionMin = aucData[7] | (aucData[8]<<8);;
						ulMiVersionMaj = aucData[9] | (aucData[10]<<8);
						printf("Found new machine interface V%ld.%ld.\n", ulMiVersionMaj, ulMiVersionMin);
						fResult = true;
					}
				}
				else if( aucData[0]!='#' )
				{
					printf("received strange response after the star: 0x%02x\n", aucData[0]);
				}
				else
				{
					printf("ok, received hash!\n");
					fDeviceNeedsUpdate = true;
					fResult = true;
				}
			}
			else if( aucData[0]=='#' )
			{
				printf("ok, received hash!\n");

				fDeviceNeedsUpdate = true;
				fResult = true;
			}
			else
			{
				printf("strange or no response from device, seems to be no netx\n");
			}
		}
	}

	if( fResult==true )
	{
		if( fDeviceNeedsUpdate==true )
		{
			/* Send enter. */
			printf("sending enter to netx.\n");
			sizCnt = SendRaw(aucBlankLine, sizeof(aucBlankLine), 200);
			if( sizCnt!=1 )
			{
				fprintf(stderr, "Failed to send enter to device!\n");
			}
			else
			{
//				fResult = WaitForResponse(&pucResponse, 65536, 1024);
/* TODO:
 1) grab data until next newline + '>' Combo.
 2) download new code
 3) start
*/

				fResult = false;


			}
		}
	}


	return fResult;
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

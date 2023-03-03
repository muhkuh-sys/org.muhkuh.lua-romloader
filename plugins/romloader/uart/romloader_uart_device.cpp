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
#include "romloader_uart_main.h"

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
 , m_ptPluginOptions(NULL)
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
	//printf("~romloader_uart_device %p\n", this);
	deleteCards();

#if MUHKUH_USE_THREADING==MUHKUH_USE_THREADING_MSVC
	DeleteCriticalSection(&m_csCardLock);
#elif MUHKUH_USE_THREADING==MUHKUH_USE_THREADING_PTHREAD
	/* Nothing to do here. */
#else
#       error "Unknown threading selected!"
#endif

	if( m_pcPortName!=NULL )
	{
		free(m_pcPortName);
	}

	//printf("~romloader_uart_device delete m_ptPluginOptions\n");
	delete m_ptPluginOptions;
}


romloader_uart_options *romloader_uart_device::GetOptions()
{
	return m_ptPluginOptions;
}


void romloader_uart_device::SetOptions(const romloader_uart_options *ptOptions)
{
	if (m_ptPluginOptions != NULL) 
	{
		delete m_ptPluginOptions;
		m_ptPluginOptions = NULL;
	}
	
	if (ptOptions != NULL) 
	{
		m_ptPluginOptions = new romloader_uart_options(ptOptions);
	}
}



void romloader_uart_device::initCards(void)
{
	BUFFER_CARD_T *ptCard;


	if( m_ptFirstCard!=NULL )
	{
		deleteCards();
	}

	ptCard = new BUFFER_CARD_T;
	ptCard->pucEnd = ptCard->aucData + mc_sizCardSize;
	ptCard->pucRead = ptCard->aucData;
	ptCard->pucWrite = ptCard->aucData;
	ptCard->ptNext = NULL;

	m_ptFirstCard = ptCard;
	m_ptLastCard = ptCard;
}


void romloader_uart_device::deleteCards(void)
{
	BUFFER_CARD_T *ptCard;
	BUFFER_CARD_T *ptNextCard;


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
	BUFFER_CARD_T *ptCard;


	CRITICAL_SECTION_ENTER(m_csCardLock);

	sizLeft = sizBufferSize;
	while( sizLeft>0 )
	{
		// get free space in the current card
		sizChunk = m_ptLastCard->pucEnd - m_ptLastCard->pucWrite;
		// no more space -> create a new card
		if( sizChunk==0 )
		{
			ptCard = new BUFFER_CARD_T;
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
	BUFFER_CARD_T *ptOldCard;
	BUFFER_CARD_T *ptNewCard;


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
				fprintf(stderr, "WARNING: no remaining data: %ld\n", sizRead);
			}
			// reached the end of the buffer?
			if( m_ptFirstCard->pucRead>=m_ptFirstCard->pucEnd )
			{
				// card is empty, move on to next card

				CRITICAL_SECTION_ENTER(m_csCardLock);

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

				CRITICAL_SECTION_LEAVE(m_csCardLock);
			}
		}

		sizLeft -= sizRead;
		pucBuffer += sizRead;
	} while( sizLeft>0 );

	return sizBuffer-sizLeft;
}



void romloader_uart_device::discardCards(void)
{
	BUFFER_CARD_T *ptCard;
	BUFFER_CARD_T *ptNextCard;


	/* All cards are modified -> lock the cards. */
	CRITICAL_SECTION_ENTER(m_csCardLock);

	ptCard = m_ptFirstCard;
	if( ptCard!=NULL )
	{
		/* Delete all cards except the last one. */
		while( ptCard->ptNext!=NULL )
		{
			ptNextCard = ptCard->ptNext;
			delete ptCard;
			ptCard = ptNextCard;
		}

		/* Clear all data in the last card. */
		m_ptFirstCard = ptCard;
		ptCard->pucEnd = ptCard->aucData + mc_sizCardSize;
		ptCard->pucRead = ptCard->aucData;
		ptCard->pucWrite = ptCard->aucData;
		ptCard->ptNext = NULL;
	}

	CRITICAL_SECTION_LEAVE(m_csCardLock);
}



size_t romloader_uart_device::getCardSize(void) const
{
	size_t sizData;
	BUFFER_CARD_T *ptCard;


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


/* 
 * Receive char by char until the one of the EOL sequences is received
 * or the time runs out.
 * 
 * ppucLine [out] pointer to received data.
 *                Only valid when a match has been found.
 *                Caller must free the buffer.
 * pcEOL1    [in] Pointer to string 1 to be matched (0-terminated)
 * pcEOL2    [in] Pointer to string 2 to be matched (0-terminated)
 * ulTimeout [in] Timeout in ms
 * Returns 0 on timeout (neither EOL1 nor EOL2 were matched)
 * Returns 0 on out-of-memory
 * Returns 1 if EOL1 was matched
 * Returns 2 if EOL2 was matched
 */
int romloader_uart_device::GetLine2(unsigned char **ppucLine, const char *pcEol1, const char *pcEol2, unsigned long ulTimeout)
{
	unsigned char *pucBuffer;
	unsigned char *pucBufferNew;
	size_t sizBufferCnt;
	size_t sizBufferMax;
	size_t sizEolSeq1;
	size_t sizEolSeq2;
	size_t sizReceived;
	int iMatchingEol;

	/* Expect failure. */
	iMatchingEol = 0;

	sizEolSeq1 = strlen(pcEol1);
	sizEolSeq2 = strlen(pcEol2);

	/* Init References array. */
	sizBufferCnt = 0;
	sizBufferMax = 80;
	pucBuffer = (unsigned char*)malloc(sizBufferMax);
	if( pucBuffer==NULL )
	{
		fprintf(stderr, "out of memory!\n");
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

				break;
			}

			/* Check for EOL. */
			sizBufferCnt++;
			if( sizBufferCnt>=sizEolSeq1 && memcmp(pcEol1, pucBuffer+sizBufferCnt-sizEolSeq1, sizEolSeq1)==0 )
			{
				iMatchingEol = 1;
				break;
			}
			
			if( sizBufferCnt>=sizEolSeq2 && memcmp(pcEol2, pucBuffer+sizBufferCnt-sizEolSeq2, sizEolSeq2)==0 )
			{
				iMatchingEol = 2;
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
					break;
				}
				/* Reallocate the array. */
				pucBufferNew = (unsigned char*)realloc(pucBuffer, sizBufferMax);
				if( pucBufferNew==NULL )
				{
					break;
				}
				pucBuffer = pucBufferNew;
			}
		} while( true );
	}

	if( iMatchingEol>0 )
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

	return iMatchingEol;
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

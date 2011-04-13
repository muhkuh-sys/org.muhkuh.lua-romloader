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
	pthread_mutex_lock(&m_tRxDataAvail_Mutex);
	pthread_cond_signal(&m_tRxDataAvail_Condition);
	pthread_mutex_unlock(&m_tRxDataAvail_Mutex);
}


size_t romloader_uart_device::readCards(unsigned char *pucBuffer, size_t sizBufferSize)
{
	size_t sizRead;
	size_t sizLeft;


	sizLeft = sizBufferSize;
	do
	{
		sizRead = readCardData(pucBuffer, sizLeft);
		pucBuffer += sizRead;
		sizLeft -= sizRead;
	} while( sizRead!=0 && sizLeft>0 );

	return sizBufferSize-sizLeft;
}


size_t romloader_uart_device::readCardData(unsigned char *pucBuffer, size_t sizBufferSize)
{
	size_t sizRead;
	tBufferCard *ptOldCard;
	tBufferCard *ptNewCard;


	if( m_ptFirstCard==NULL )
	{
		sizRead = 0;
	}
	else if( m_ptFirstCard->pucWrite!=NULL )
	{
		// the first card is used by the write part -> lock the cards
		CRITICAL_SECTION_ENTER(m_csCardLock);

		// get the number of bytes left in this card
		sizRead = m_ptFirstCard->pucWrite - m_ptFirstCard->pucRead;
		if( sizRead>sizBufferSize )
		{
			sizRead = sizBufferSize;
		}
		// card can be empty
		if( sizRead>0 )
		{
			// copy the data
			memcpy(pucBuffer, m_ptFirstCard->pucRead, sizRead);
			// advance the read pointer
			m_ptFirstCard->pucRead += sizRead;
		}

		CRITICAL_SECTION_LEAVE(m_csCardLock);
	}
	else
	{
		// the first card is not used by the write part

		// get the number of bytes left in this card
		sizRead = m_ptFirstCard->pucEnd - m_ptFirstCard->pucRead;
		if( sizRead>sizBufferSize )
		{
			sizRead = sizBufferSize;
		}
		// card can be empty for overlapping buffer grow
		if( sizRead>0 )
		{
			// copy the data
			memcpy(pucBuffer, m_ptFirstCard->pucRead, sizRead);
			// advance the read pointer
			m_ptFirstCard->pucRead += sizRead;
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

	return sizRead;
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


bool romloader_uart_device::IdentifyLoader(void)
{
	bool fResult = false;
	//wxRegEx reFirstResponse(wxT("netX.* Step [A-Za-z0-9]"));
	//wxRegEx reInvalidCmd(wxT(".*\\?[0-9]{8}[\r\n]*>"));
	const unsigned char aucKnock[2] = { '*', '#' };
	unsigned long ulLength;
	unsigned char aucData[1];
	unsigned char *pucResponse;


	/* Send knock sequence with 1 second timeout. */
	if( SendRaw(aucKnock, sizeof(aucKnock), 1000)!=sizeof(aucKnock) )
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
		ulLength = RecvRaw(aucData, 1, 5000);
	 	if( ulLength!=1 )
	 	{
			/* Failed to receive first char of knock response. */
			fprintf(stderr, "Failed to receive first char of knock response.\n");
		}
		else
		{
			// wxLogMessage(wxT("received knock response: 0x%02x"), ucData);
			printf("received knock response: 0x%02x", aucData);
			/* This should be '\f', but the first bits might be trashed. */
			if( aucData[0]<0x20 )
			{
				/* This seems to be the welcome message. */
#if 0
				// receive the rest of the line
				//wxLogMessage(wxT("receive the rest of the knock response"));
				fResult = GetLine(&pucResponse, "\r\n", 1000);
				if( fResult!=true )
				{
					// failed to receive the rest of the knock response
				}
				else
				{
					// check for initial bootloader message
					fResult = reFirstResponse.Matches(pucResponse);
					if( fResult!=true )
					{
						// seems to be no netX bootloader
						//wxLogMessage(wxT("the knock response does not match the romloader message"));
						//wxLogMessage(wxT("received this response: '") + strResponse + wxT("'"));
					}
					else
					{
						// get prompt, the real console eats the first space, the usb console will echo it
						fResult = false;
						ulLength = RecvRaw(abData, 3, 1000);
						if( ulLength==0 )
						{
							//wxLogMessage(wxT("received no further data after romloader message"));
						}
						else if( abData[0]!='>' )
						{
							//wxLogMessage(wxT("received strange response after romloader message"));
							//wxLogMessage(wxT("len: %d, data = 0x%02x, 0x%02x, 0x%02x"), ulLength, abData[0], abData[1], abData[2]);
						}
						else if( ulLength==1 )
						{
							//wxLogMessage(wxT("ok, received prompt!"));
							fResult = true;
						}
						else if( ulLength==2 && abData[1]=='#' )
						{
							//wxLogMessage(wxT("ok, received hash!"));
							fResult = true;
						}
						else if( ulLength==3 && abData[1]=='*' && abData[2]=='#' )
						{
							//wxLogMessage(wxT("ok, received star and hash!"));
							fResult = true;
						}
						else
						{
							//wxLogMessage(wxT("received strange response after romloader message"));
							//wxLogMessage(wxT("len: %d, data = 0x%02x, 0x%02x, 0x%02x"), ulLength, abData[0], abData[1], abData[2]);
						}
					}
				}
#endif
			}
			/* Knock echoed -> this is the prompt. */
			else if( aucData[0]=='*' )
			{
				printf("ok, received star!\n");

				// get rest of knock sequence
				ulLength = RecvRaw(aucData, 1, 1000);
				if( ulLength==0 )
				{
					printf("failed to receive response after the star\n");
				}
				else if( aucData[0]!='#' )
				{
					printf("received strange response after the star: 0x%02x\n", aucData[0]);
				}
				else
				{
					printf("ok, received hash!\n");
					fResult = true;
				}
			}
			else if( aucData[0]=='#' )
			{
				printf("ok, received hash!\n");

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
#if 0
		/* Send enter. */
		printf("sending enter to netx");
		fResult = SendCommand("", 1000);
		if( fResult!=true )
		{
			//wxLogError(wxT("failed to send enter to device"));
		}
		else
		{
			fResult = WaitForResponse(&pucResponse, 65536, 1024);
			if( fResult!=true )
			{
				//wxLogMessage(wxT("failed to receive response after enter"));
			}
			else
			{
				//wxLogMessage(wxT("knock response:") + strResponse);
			}
		}
#endif
	}


	return fResult;
}


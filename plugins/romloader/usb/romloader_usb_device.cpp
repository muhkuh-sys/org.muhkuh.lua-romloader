/***************************************************************************
 *   Copyright (C) 2007 by Christoph Thelen                                *
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


#include "romloader_usb_device.h"

#include <errno.h>


#if defined(WIN32)
#	define strncasecmp _strnicmp
#endif

const romloader_usb_device::NETX_USB_DEVICE_T romloader_usb_device::atNetxUsbDevices[2] =
{
	{
		"netX500",
		0x0cc4,
		0x0815,
		ROMLOADER_CHIPTYP_NETX500,
		ROMLOADER_ROMCODE_ABOOT,
		0x81,
		0x01
	},
	{
		"netX10",
		0x1939,
		0x000c,
		ROMLOADER_CHIPTYP_NETX10,
		ROMLOADER_ROMCODE_HBOOT,
		0x83,
		0x04
	}
};



romloader_usb_device::romloader_usb_device(const char *pcPluginId)
 : m_pcPluginId(NULL)
 , m_ptFirstCard(NULL)
 , m_ptLastCard(NULL)
{
	m_pcPluginId = strdup(pcPluginId);

	initCards();
}


romloader_usb_device::~romloader_usb_device(void)
{
	deleteCards();

	if( m_pcPluginId!=NULL )
	{
		free(m_pcPluginId);
	}
}


void romloader_usb_device::initCards(void)
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


void romloader_usb_device::deleteCards(void)
{
	tBufferCard *ptCard;
	tBufferCard *ptNextCard;


	ptCard = m_ptFirstCard;
	while( ptCard!=NULL )
	{
		ptNextCard = ptCard->ptNext;
		delete ptCard;
		ptCard = ptNextCard;
	}
	m_ptFirstCard = NULL;
	m_ptLastCard = NULL;
}


void romloader_usb_device::writeCards(const unsigned char *pucBuffer, size_t sizBufferSize)
{
	size_t sizLeft;
	size_t sizChunk;
	tBufferCard *ptCard;


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
}


size_t romloader_usb_device::readCards(unsigned char *pucBuffer, size_t sizBufferSize)
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


size_t romloader_usb_device::readCardData(unsigned char *pucBuffer, size_t sizBufferSize)
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


size_t romloader_usb_device::getCardSize(void) const
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


void romloader_usb_device::flushCards(void)
{
	tBufferCard *ptOldCard;
	tBufferCard *ptNextCard;


	printf("flushing cards:\n");

	while( m_ptFirstCard!=NULL )
	{
		if( m_ptFirstCard->pucWrite!=NULL )
		{
			hexdump(m_ptFirstCard->pucRead, m_ptFirstCard->pucWrite-m_ptFirstCard->pucRead);

			/* Skip all data in this card. */
			m_ptFirstCard->pucRead = m_ptFirstCard->pucWrite;
			/* This must be the last card. */
			break;
		}
		else
		{
			hexdump(m_ptFirstCard->pucRead, m_ptFirstCard->pucEnd-m_ptFirstCard->pucRead);

			/* The card is not used by the write part. */
			ptNextCard = m_ptFirstCard->ptNext;
			if( ptNextCard!=NULL )
			{
				/* Remember the empty card. */
				ptOldCard = m_ptFirstCard;
				/* Move to the new first card. */
				m_ptFirstCard = ptNextCard;
				/* Delete the empty card. */
				delete ptOldCard;
			}
		}
	}
}


bool romloader_usb_device::expect_string(const char *pcString)
{
	unsigned char *pucBuffer;
	size_t sizString;
	size_t sizReceived;
	const unsigned int uiTimeoutMs = 200;
	bool fFound;


	/* Allocate a buffer for the string. */
	sizString = strlen(pcString);
	pucBuffer = new unsigned char[sizString];

	/* Receive data. */
	sizReceived = usb_receive(pucBuffer, sizString, uiTimeoutMs);
	fFound  = true;
	fFound &= (sizReceived==sizString);
	fFound &= ( memcmp(pucBuffer, pcString, sizString)==0 );

	/* FIXME: debug routines, remove */
	if( fFound!=true )
	{
		printf("want:\n");
		hexdump((const unsigned char*)pcString, sizString);
		printf("got:\n");
		hexdump(pucBuffer, sizReceived);
	}

	delete pucBuffer;

	return fFound;
}


bool romloader_usb_device::parse_hex_digit(size_t sizDigits, unsigned long *pulResult)
{
	unsigned char *pucBuffer;
	unsigned long ulResult;
	bool fOk;
	unsigned char uc;
	unsigned int uiDigit;
	size_t sizCnt;
	size_t sizReceived;
	const unsigned int uiTimeoutMs = 200;


	/* Allocate a buffer for the digits. */
	pucBuffer = new unsigned char[sizDigits];

	/* Receive data. */
	sizReceived = usb_receive(pucBuffer, sizDigits, uiTimeoutMs);
	if( sizReceived!=sizDigits )
	{
		/* Not enough chars received. */
		fOk = false;
	}
	else
	{
		ulResult = 0;
		fOk = true;
		sizCnt = 0;
		while( sizCnt<sizDigits )
		{
			uc = pucBuffer[sizCnt];
			if( uc>='0' && uc<='9' )
			{
				uiDigit = uc - '0';
			}
			else if( uc>='a' && uc<='f' )
			{
				uiDigit = uc - 'a' + 10;
			}
			else if( uc>='A' && uc<='F' )
			{
				uiDigit = uc - 'A' + 10;
			}
			else
			{
				fOk = false;
				break;
			}
			ulResult <<= 4;
			ulResult |= uiDigit;

			++sizCnt;
		}

		if( fOk==true )
		{
			if( pulResult!=NULL )
			{
				*pulResult = ulResult;
			}
		}
	}

	return fOk;
}


int romloader_usb_device::parse_uue(size_t sizLength, unsigned char *pucData, unsigned long ulStart, unsigned long ulEnd)
{
	int iResult;
	char acLine[82];
	int iMatch;
	size_t sizLine;
	size_t sizUueBytesInLine;
	size_t sizCharCnt;
	size_t sizByteCnt;
	const char *pcCnt;
	unsigned long ulResult;
	unsigned long ulBeginLineStart;
	unsigned long ulBeginLineEnd;


	/* Wait for 'begin'. */
	do
	{
		/* Receive line. */
		iResult = usb_receive_line(acLine, sizeof(acLine), 100, &sizLine);
		if( iResult==0 && sizLine>6 && strncasecmp(acLine, "begin ", 6)==0 )
		{
			/* Ok, iResult is 0. */
			break;
		}
	} while( iResult==0 );

	if( iResult==0 )
	{
		iMatch = sscanf(acLine, "begin 666 %lx_%lx.bin", &ulBeginLineStart, &ulBeginLineEnd);
		if( iMatch!=2 )
		{
			fprintf(stderr, "first line does not match expected pattern.\n");
			fprintf(stderr, "got:  '%s'\n", acLine);
			fprintf(stderr, "want: 'begin 666 %08x_%08x.bin'", ulStart, ulEnd);
		}
		else if( ulStart!=ulBeginLineStart || ulEnd!=ulBeginLineEnd )
		{
			fprintf(stderr, "received invalid address range!\n");
			fprintf(stderr, "got:  '%s'\n", acLine);
			fprintf(stderr, "want: 'begin 666 %08x_%08x.bin'", ulStart, ulEnd);
		}
		else
		{
			/* found begin, now parse all lines until 'end' is reached */
			do
			{
				/* receive line */
				iResult = usb_receive_line(acLine, sizeof(acLine), 100, &sizLine);
				if( iResult!=0 )
				{
					break;
				}
				else if( strncasecmp(acLine, "end", 3)==0 )
				{
					/* found 'end' */
					break;
				}
				else if( sizLine>0 )
				{
					/* no end or empty line -> must be uencoded line */

					/*
					 * get the linelength character
					 * NOTE: this will warp for numbers smaller than 0x20, and that's what we want
					 */
					sizUueBytesInLine = acLine[0] - 0x20U;
					/*
					 * the length must be between 0x40 and 0x40
					 * NOTE: 0x40 is special as it's used for a 'end of line' marker. it equals 0
					 */
					if( sizUueBytesInLine>0x40 )
					{
						/* illegal line number, break */
						iResult = -1;
						break;
					}
					else
					{
						/* 0x40 must equal 0x00 */
						sizUueBytesInLine &= 0x3f;
						if( sizUueBytesInLine>0 )
						{
							/* check if the real line length matches the uee length */
							sizCharCnt = 2;
							sizByteCnt = 0;
							/* NOTE: this could be done with div and modulo, but that would pull libm in :( */
							while( sizByteCnt<sizUueBytesInLine )
							{
								/* each block needs 4 chars */
								sizCharCnt += 4;
								/* ...and encodes 3 bytes */
								sizByteCnt += 3;
							}
							if( sizCharCnt>sizLine )
							{
								/* line is not long enough for the expected data, seems to be cut off */
								iResult = -1;
								break;
							}
							else
							{
								/* ok, line matches, decode all data */
								pcCnt = acLine + 1;
								do
								{
									/* grab the next 4 chars */
									sizCharCnt = 4;
									ulResult = 0;
									do
									{
										ulResult <<= 6;
										ulResult |= (*(pcCnt++)-0x20) & 0x3f;
									} while( --sizCharCnt!=0 );

									/* write 3 decoded chars */
									sizByteCnt = 3;
									if( sizByteCnt>sizUueBytesInLine )
									{
										sizByteCnt = sizUueBytesInLine;
									}
									do
									{
										/* NOTE: this must be before the data write line, the bits must be in 31..8 */
										ulResult <<= 8;
										*(pucData++) = (unsigned char)(ulResult>>24U);
										--sizUueBytesInLine;
									} while( --sizByteCnt!=0 );
								} while( sizUueBytesInLine!=0 );
							}
						}
					}
				}
			} while( iResult==0 );
		}
	}

	return iResult;
}


int romloader_usb_device::uue_generate(const unsigned char *pucData, size_t sizData, char **ppcUueData, size_t *psizUueData)
{
	size_t sizUueData;
	size_t sizMaxUueData;
	char *pcUueData;
	char *pcUueDataCnt;
	const unsigned char *pucDataCnt;
	const unsigned char *pucDataEnd;
	int iResult;
	size_t sizChunk;
	unsigned long ulUueBuf;
	int iCnt;


	/* Expect error. */
	iResult = -1;

	/* Allocate the output buffer. */
	sizUueData = 0;
	sizMaxUueData = 80 + ((sizData+44)/45)*64;
	pcUueData = (char*)malloc(sizMaxUueData);
	if( pcUueData!=NULL )
	{
		pcUueDataCnt = pcUueData;

		/* Generate the header. */
		iCnt = sprintf(pcUueDataCnt, "begin 666 data.bin\r\n");
		pcUueDataCnt += iCnt;

		/* Dump all memory. */
		pucDataCnt = pucData;
		pucDataEnd = pucData + sizData;
		while( pucDataCnt<pucDataEnd )
		{
			/* get the rest of the bytes to dump */
			sizChunk = pucDataEnd - pucDataCnt;
			/* limit to max uuencode line size */
			if( sizChunk>45 )
			{
				sizChunk = 45;
			}

			/* print the length character for the line */
			*(pcUueDataCnt++) = (unsigned char)(0x20 + sizChunk);

			/* print one line */
			do
			{
				/* clear uuencode buffer */
				ulUueBuf = 0;

				/* get max 3 chars into the buffer */
				iCnt = 3;
				do
				{
					/* still bytes left? */
					if( sizChunk>0 )
					{
						ulUueBuf |= *(pucDataCnt++);
						--sizChunk;
					}
					/* NOTE: the shift operation must be executed after the new data is masked in, the result must be in 8..31 */
					ulUueBuf <<= 8;
				} while( --iCnt>0 );

				/* encode the buffer */
				iCnt = 4;
				do
				{
					*(pcUueDataCnt++) = (unsigned char)(0x20 + (ulUueBuf>>26));
					ulUueBuf <<= 6;
				} while( --iCnt!=0 );
			} while( sizChunk!=0 );

			/* end the line */
			*(pcUueDataCnt++) = '`';
			*(pcUueDataCnt++) = '\r';
			*(pcUueDataCnt++) = '\n';
		}
	
		/* print last line */
		memcpy(pcUueDataCnt, "`\r\nend\r\n", 8);
		pcUueDataCnt += 8;

		/* Get the size of the UUencoded data. */
		sizUueData = pcUueDataCnt - pcUueData;

		iResult = 0;
	}

	*ppcUueData = pcUueData;
	*psizUueData = sizUueData;

	return iResult;
}


void romloader_usb_device::dump_all_cards(void)
{
	tBufferCard *ptCard;


	ptCard = m_ptFirstCard;
	while( ptCard!=NULL )
	{
		printf("Card: %p\n", ptCard);
		printf("  Size: %d\n", ptCard->pucEnd - ptCard->aucData);
		printf("  Start: %p\n", ptCard->aucData);
		printf("  Read:  %p\n", ptCard->pucRead);
		printf("  Write: %p\n", ptCard->pucWrite);
		printf("  Next:  %p\n", ptCard->ptNext);
		if( ptCard->pucWrite!=NULL )
		{
			hexdump(ptCard->pucRead, ptCard->pucWrite-ptCard->pucRead);
		}
		else
		{
			hexdump(ptCard->pucRead, ptCard->pucEnd-ptCard->pucRead);
		}
		
		ptCard = ptCard->ptNext;
	}
}


void romloader_usb_device::hexdump(const unsigned char *pucData, unsigned long ulSize)
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


/***************************************************************************
 *   Copyright (C) 2010 by Christoph Thelen                                *
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


#include "uuencoder.h"

#include <stdio.h>


uuencoder::uuencoder(const unsigned char *pucData, size_t sizData)
{
	m_pucStart = pucData;
	m_pucCnt   = pucData;
	m_pucEnd   = pucData + sizData;
	m_tState   = UUENCODE_STATE_Begin;
}


size_t uuencoder::process(char *pcLine, size_t sizMaxLine)
{
	size_t sizLine;
	size_t sizChunk;
	const unsigned char *pucInputCnt;
	char *pcOutputCnt;
	unsigned long ulUueBuf;
	int iCnt;


	/* Default is no output. */
	sizLine = 0;

	switch( m_tState )
	{
	case UUENCODE_STATE_Begin:
		sizLine = snprintf(pcLine, sizMaxLine, "begin 666 -\n");
		m_tState = UUENCODE_STATE_Data;
		break;

	case UUENCODE_STATE_Data:
		/* Get the size of the next chunk in raw bytes. */
		sizChunk = m_pucEnd - m_pucCnt;
		/* Limit the chunk size to the maximum line size for uucode. */
		if( sizChunk>45 )
		{
			sizChunk = 45;
		}

		/* Get a pointer to the output buffer. */
		pcOutputCnt = pcLine;

		/* Print the length character for the line. */
		*(pcOutputCnt++) = (unsigned char)(0x20 + sizChunk);

		/* Convert the raw bytes to uucode. */
		pucInputCnt = m_pucCnt;
		do
		{
			/* Clear uuencode buffer. */
			ulUueBuf = 0;

			/* Get max 3 chars into the buffer. */
			iCnt = 3;
			do
			{
				/* Still bytes left? */
				if( sizChunk>0 )
				{
					ulUueBuf |= *(pucInputCnt++);
					--sizChunk;
				}
				/* NOTE: The shift operation must be executed after the new data is masked in, the result must be in 8..31 . */
				ulUueBuf <<= 8;
			} while( --iCnt>0 );

			/* Encode the buffer. */
			iCnt = 4;
			do
			{
				*(pcOutputCnt++) = (unsigned char)(0x20 + (ulUueBuf>>26));
				ulUueBuf <<= 6;
			} while( --iCnt!=0 );
		} while( sizChunk!=0 );

		/* End the line. */
		*(pcOutputCnt++) = '`';
		*(pcOutputCnt++) = '\n';

		/* Get the line size. */
		sizLine = pcOutputCnt - pcLine;

		/* Check if all data is processed. */
		m_pucCnt = pucInputCnt;
		if( pucInputCnt>=m_pucEnd )
		{
			m_tState = UUENCODE_STATE_Last1;
		}
		break;

	case UUENCODE_STATE_Last1:
		sizLine = snprintf(pcLine, sizMaxLine, "`\n");
		m_tState = UUENCODE_STATE_Last2;
		break;

	case UUENCODE_STATE_Last2:
		sizLine = snprintf(pcLine, sizMaxLine, "end\n");
		m_tState = UUENCODE_STATE_Finished;
		break;
	}

	return sizLine;
}


bool uuencoder::isFinished(void) const
{
	int iResult;


	iResult = (m_tState==UUENCODE_STATE_Finished);
	printf("is finished: %d\n", iResult);

	return iResult;
}


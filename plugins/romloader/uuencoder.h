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


#include <stddef.h>


#ifndef __UUENCODER_H__
#define __UUENCODER_H__


typedef struct
{
	size_t sizTotal;
	size_t sizProcessed;
	unsigned int uiPercent;
} UUENCODER_PROGRESS_INFO_T;


class uuencoder
{
public:
	uuencoder(void);

	void set_data(const unsigned char *pucData, size_t sizData);
	size_t process(char *pcLine, size_t sizMaxLine);
	void get_progress_info(UUENCODER_PROGRESS_INFO_T *ptProgressInfo);
	bool isFinished(void) const;


private:
	typedef enum
	{
		UUENCODE_STATE_Begin     = 0,
		UUENCODE_STATE_Data      = 1,
		UUENCODE_STATE_Last1     = 2,
		UUENCODE_STATE_Last2     = 3,
		UUENCODE_STATE_Finished  = 4
	} UUENCODE_STATE_T;

	UUENCODE_STATE_T m_tState;
	const unsigned char *m_pucStart;
	const unsigned char *m_pucEnd;
	const unsigned char *m_pucCnt;
};


#endif  /* __UUENCODER_H__ */

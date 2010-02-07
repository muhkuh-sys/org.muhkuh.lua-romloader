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

#include <stdlib.h>
#include <string.h>

#include "growbuffer.h"

growbuffer::growbuffer(unsigned int uInitialSize)
 : sBinCons(0), sBinAlloc(uInitialSize), pcSBin(NULL)
{
	if( uInitialSize!=0 ) {
		pcSBin = (unsigned char*)malloc(uInitialSize);
	}
}


growbuffer::~growbuffer(void)
{
	clear();
}


bool growbuffer::add(const unsigned char *pcData, size_t sLength)
{
	size_t sBinWant;
	unsigned char *pcNew;


	// test for sSeg+1 bytes space in array
	if( (sBinCons+sLength)>=sBinAlloc ) {
		// realloc array to twice the size or sBinCons
		sBinWant = 2*sBinAlloc;
		if( sBinWant<sBinAlloc ) {
			// overflow
			return false;
		}
		if( sBinWant<(sBinCons+sSeg+1) ) {
			sBinWant = sBinCons+sLength;
		}
		pcNew = (unsigned char*)realloc(pcSBin, sBinWant);
		if( pcNew==NULL ) {
			// out of memory
			return false;
		}
		pcSBin = pcNew;
		sBinAlloc = sBinWant;
	}

	// copy the data
	memcpy(pcSBin+sBinCons, pcData, sLength);

	// mark the new buffer segment as consumed
	sBinCons += sLength;

	return true;
}


unsigned char *growbuffer::getData(void) const
{
	return pcSBin;
}


unsigned char *growbuffer::adoptData(void)
{
	unsigned char *pcData;


	pcData = pcSBin;

	// forget the data
	pcSBin = NULL;
	sBinCons = 0;
	sBinAlloc = 0;

	return pcData;
}


size_t growbuffer::getSize(void) const
{
	return sBinCons;
}


void growbuffer::clear(void)
{
	if( pcSBin!=NULL ) {
		free(pcSBin);
	}
	pcSBin = NULL;
	sBinCons = 0;
	sBinAlloc = 0;
}


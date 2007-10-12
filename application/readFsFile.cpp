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


#include "readFsFile.h"

bool readFsFile(growbuffer *ptGrowBuffer, wxString &sBinFile)
{
	const unsigned int cuReadBufferSize = 512;
	unsigned char aucReadBuffer[cuReadBufferSize];
	wxFileSystem fileSystem;
	wxFSFile *ptFsFile;
	wxInputStream *ptInputStream;
	size_t sLastRead;


	// test if file exists
	ptFsFile = fileSystem.OpenFile(sBinFile);
	if( ptFsFile==NULL ) {
		return false;
	}

	// ok, file exists. read all data into a string
	ptInputStream = ptFsFile->GetStream();
	while( !ptInputStream->Eof() ) {
		ptInputStream->Read(aucReadBuffer, cuReadBufferSize);
		sLastRead = ptInputStream->LastRead();
		if( sLastRead==0 ) {
			break;
		}
		ptGrowBuffer->add(aucReadBuffer, sLastRead);
	}

	delete ptFsFile;
	return true;
}


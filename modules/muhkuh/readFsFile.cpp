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

bool readFsFile(growbuffer *ptGrowBuffer, wxString strBinFile)
{
	const unsigned int uiReadBufferSize = 512;
	unsigned char aucReadBuffer[uiReadBufferSize];
	wxFileSystem fileSystem;
	wxFSFile *ptFsFile;
	wxInputStream *ptInputStream;
	size_t sizLastRead;
	bool fResult;


	// test if file exists
	ptFsFile = fileSystem.OpenFile(strBinFile);
	if( ptFsFile==NULL )
	{
		wxLogMessage(wxT("failed to open the file!\n"));
		fResult = false;
	}
	else
	{
		// ok, file exists. read all data into a string
		ptInputStream = ptFsFile->GetStream();
		while( ptInputStream->Eof()==false )
		{
			ptInputStream->Read(aucReadBuffer, uiReadBufferSize);
			sizLastRead = ptInputStream->LastRead();
			if( sizLastRead==0 )
			{
				break;
			}
			ptGrowBuffer->add(aucReadBuffer, sizLastRead);
		}

		delete ptFsFile;

		fResult = true;
	}

	return fResult;
}


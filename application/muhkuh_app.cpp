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


#include "muhkuh_version.h"
#include "muhkuh_app.h"
#include "muhkuh_mainFrame.h"

#include <wx/filesys.h>
#include <wx/fs_inet.h>
#include <wx/fs_zip.h>
#include <wx/socket.h>
#include <wx/stdpaths.h>

//-------------------------------------
// Muhkuh needs several wx features

#if !wxUSE_THREADS
	#error "This tool requires thread support!"
#endif

#if !wxUSE_STREAMS
	#error "This tool requires stream support!"
#endif

#if !wxUSE_ZIPSTREAM
	#error "This tool requires zipstream support!"
#endif

#if !wxUSE_ZLIB
	#error "This tool requires zlib support!"
#endif

//-------------------------------------

IMPLEMENT_APP(muhkuh_app)

bool muhkuh_app::OnInit()
{
	muhkuh_mainFrame *mainframe;


	wxSocketBase::Initialize();

	wxInitAllImageHandlers();
	// add virtual filesystem for zip archives
	wxFileSystem::AddHandler(new wxZipFSHandler);
	// add virtual filesystem for http and ftp
	wxFileSystem::AddHandler(new wxInternetFSHandler);

	// ... from wxLua demo application
#ifdef __WXGTK__
	// this call is very important since otherwise scripts using the decimal
	// point '.' could not work with those locales which use a different symbol
	// (e.g. the comma) for the decimal point...
	// It doesn't work to put os.setlocale('c', 'numeric') in the Lua file that
	// you want to use decimal points in. That's because the file has been lexed
	// and compiler before the locale has changed, so the lexer - the part that
	// recognises numbers - will use the old locale.
	setlocale(LC_NUMERIC, "C");
#endif

	// set application and vendor name, this is needed
	// for the config file (unix) or registry keys (win)
	SetVendorName(wxT(MUHKUH_APPLICATION_NAME " team"));
	SetAppName(wxT(MUHKUH_APPLICATION_NAME));

	// create the muhkuh main frame
	mainframe = new muhkuh_mainFrame();

	// show the frame
	mainframe->Show(true);
	SetTopWindow(mainframe);

	return true;
}


int muhkuh_app::OnExit()
{
	wxConfigBase *pConfig;


	pConfig = wxConfigBase::Set((wxConfigBase *) NULL);
	if( pConfig!=NULL ) {
		delete pConfig;
	}

	return 0;
}

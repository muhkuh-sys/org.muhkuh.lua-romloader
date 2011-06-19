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


#include "license.h"

#include "muhkuh_version.h"
#include "muhkuh_aboutDialog.h"

#if USE_LUA!=0
#include "lua.hpp"
#endif

muhkuh_aboutDialog::muhkuh_aboutDialog(wxWindow* parent, const wxString &strVersion, wxIconBundle &frameIcons)
 : wxDialog(parent, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxCAPTION|wxRESIZE_BORDER|wxSYSTEM_MENU|wxCLOSE_BOX|wxMAXIMIZE_BOX|wxMINIMIZE_BOX)
{
	wxString strMsg;
	wxIcon icon;


	// construct the title string
	strMsg  = _("About ");
	strMsg += wxT(MUHKUH_APPLICATION_NAME);
	SetTitle(strMsg);

	// create the controls

	// the main sizer
	m_mainBoxSizer = new wxBoxSizer(wxVERTICAL);
	SetSizer(m_mainBoxSizer);

	// the logo and app name sizer
	m_logoSizer = new wxBoxSizer(wxHORIZONTAL);
	m_mainBoxSizer->Add(m_logoSizer);

	// create the icon
	icon = frameIcons.GetIcon(32);
	if( icon.IsOk()==true )
	{
		// create new bitmap
		m_logoBitmap = new wxStaticBitmap(this, wxID_ANY, icon);
		m_logoSizer->Add(m_logoBitmap, 0, wxALL|wxALIGN_CENTER_VERTICAL|wxALIGN_CENTER_HORIZONTAL, 8);
	}
	// create the application name
	m_appText = new wxStaticText(this, wxID_ANY, strVersion);
	m_logoSizer->Add(m_appText, 0, wxALL|wxALIGN_CENTER_VERTICAL, 8);

	// add the notebook control
	m_tabs = new wxNotebook(this, wxID_ANY);
	m_mainBoxSizer->Add(m_tabs, 1, wxEXPAND);

	// add the notebook pages
	m_aboutHtml = new wxHtmlWindow(m_tabs, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER);
	setAbout(m_aboutHtml);
	m_tabs->AddPage(m_aboutHtml, _("About"), true);

	m_authorsHtml = new wxHtmlWindow(m_tabs, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER);
	setAuthors(m_authorsHtml);
	m_tabs->AddPage(m_authorsHtml, _("Authors"), false);

	m_licenseHtml = new wxHtmlWindow(m_tabs, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER);
	setLicense(m_licenseHtml);
	m_tabs->AddPage(m_licenseHtml, _("License"), false);
}


void muhkuh_aboutDialog::setAbout(wxHtmlWindow *ptHtmlWin)
{
	wxString strMsg;
	wxColor colBg;
	wxString strOS;
	wxString strCompiler;


	// get the background color
	colBg = GetBackgroundColour();

	// start html document
	strMsg  = wxT("<html><body bgcolor=\"") + colBg.GetAsString(wxC2S_HTML_SYNTAX) + wxT("\">");

	// describe the application here
	strMsg += wxT("<center>");
	strMsg += _("Your friendly testtool with super cow powers!");
	strMsg += wxT("<p>(c) ") wxT(MUHKUH_VERSION_YEAR) wxT(", the Muhkuh team<br>");
	strMsg += wxT("<a href=\"http://muhkuh.sf.net\">http://muhkuh.sf.net</a><br>");
	strMsg += wxT("<a href=\"http://www.sf.net/projects/muhkuh\">http://www.sf.net/projects/muhkuh</a><p><br>");
	strMsg += wxT("</center>");

	strMsg += _("This version uses");
	strMsg += wxT(" <ul><li>");
	strMsg += wxVERSION_STRING;
	strMsg += wxT(" (");
	// NOTE: these strings will be translated, don't optimize to '*abled'
#ifdef UNICODE
	strMsg += wxT("Unicode");
#else
	strMsg += wxT("Ansi");
#endif
	strMsg += wxT(", ");
#ifdef __WXDEBUG__
	strMsg += wxT("Debug");
#else
	strMsg += wxT("Release");
#endif
	strMsg += wxT(")<br><a href=\"http://www.wxwidgets.org\">http://www.wxwidgets.org</a></li>");

#if USE_LUA!=0
	strMsg += wxT("<li>") wxT(LUA_RELEASE) wxT(", ") wxT(LUA_COPYRIGHT) wxT(" by ") wxT(LUA_AUTHORS);
	strMsg += wxT(")<br><a href=\"http://www.lua.org\">http://www.lua.org</a></li>");
#endif

	strMsg += wxT("<li>Silk icon set 1.3 by Mark James<br><a href=\"http://www.famfamfam.com/lab/icons/silk/\">http://www.famfamfam.com/lab/icons/silk/</a></li>");

	strMsg += wxT("</ul><p>");

	strMsg += _("It was compiled on ");

// NOTE: append this to strOS to see multiple hits (like __LINUX__ and __UNIX__)

// just a combination of other defines?
//#ifdef __APPLE__
//	strOS.Append(wxT("any Mac OS version"));
//#endif

#ifdef __AIX__
	strOS.Append(wxT("AIX"));
#endif

#ifdef __BSD__
	strOS.Append(wxT("A *BSD system"));
#endif

#ifdef __CYGWIN__
	strOS.Append(wxT("Cygwin: Unix on Win32"));
#endif

#ifdef __DARWIN__
	strOS.Append(wxT("Mac OS X using the BSD Unix C library (as opposed to using the Metrowerks MSL C/C++ library)"));
#endif

#ifdef __DATA_GENERAL__
	strOS.Append(wxT("DG-UX"));
#endif

#ifdef __DOS_GENERAL__
	strOS.Append(wxT("DOS"));
#endif

#ifdef __FREEBSD__
	strOS.Append(wxT("FreeBSD"));
#endif

#ifdef __HPUX__
	strOS.Append(wxT("HP-UX (Unix)"));
#endif

#ifdef __GNU__
	strOS.Append(wxT("GNU Hurd"));
#endif

#ifdef __LINUX__
	strOS.Append(wxT("Linux"));
#endif

#ifdef __MACH__
	strOS.Append(wxT("Mach-O Architecture (Mac OS X only builds)"));
#endif

#ifdef __OSF__
	strOS.Append(wxT("OSF/1"));
#endif

#ifdef __PALMOS__
	strOS.Append(wxT("PalmOS"));
#endif

#ifdef __SGI__
	strOS.Append(wxT("IRIX"));
#endif

#ifdef __SOLARIS__
	strOS.Append(wxT("Solaris"));
#endif

// just a combination of other defines?
//#ifdef __SUN__
//	strOS.Append(wxT("Any Sun"));
//#endif

#ifdef __SUNOS__
	strOS.Append(wxT("Sun OS"));
#endif

#ifdef __SVR4__
	strOS.Append(wxT("SystemV R4"));
#endif

#ifdef __SYSV__
	strOS.Append(wxT("SystemV generic"));
#endif

#ifdef __ULTRIX__
	strOS.Append(wxT("Ultrix"));
#endif

// this seems to be a combination of linux and unix_like
//#ifdef __UNIX__
//	strOS.Append(wxT("any Unix"));
//#endif

// only take this general entry for other OSes than linux
#if defined(__UNIX_LIKE__) && !defined(__LINUX__)
	strOS.Append(wxT("Unix, BeOS or VMS"));
#endif

#ifdef __VMS__
	strOS.Append(wxT("VMS"));
#endif

#ifdef __WINDOWS__
	strOS.Append(wxT("Windows"));
#endif

	if( strOS.IsEmpty()==true )
	{
		strOS = _("unknown OS");
	}

	strMsg += strOS;

	strMsg += _(" with ");

	// show the compiler
#ifdef __BORLANDC__
	// The value of the macro corresponds to the compiler version: 500 is 5.0.
	strCompiler.Printf(wxT("Borland C++ v%d.%02d"), __BORLANDC__/100, __BORLANDC__%100);
#endif

#ifdef __DJGPP__
	strCompiler.Printf(wxT("DJGPP"));
#endif

#ifdef __DIGITALMARS__
	strCompiler.Printf(wxT("Digital Mars"));
#endif

#ifdef __GNUG__
	strCompiler.Printf(wxT("Gnu C++ v%d.%d"), __GNUC__, __GNUC_MINOR__);
#endif

#ifdef __GNUWIN32__
	strCompiler.Printf(wxT("Gnu-Win32 v%d.%d"), __GNUC__, __GNUC_MINOR__);
#endif

#ifdef __MINGW32__
	strCompiler.Printf(wxT("MinGW v%d.%d"), __GNUC__, __GNUC_MINOR__);
#endif

#ifdef __MWERKS__
	strCompiler.Printf(wxT("CodeWarrior MetroWerks"));
#endif

#ifdef __SUNCC__
	strCompiler.Printf(wxT("Sun CC"));
#endif

#ifdef __SYMANTECC__
	strCompiler.Printf(wxT("Symantec C++"));
#endif

#ifdef __VISAGECPP__
	strCompiler.Printf(wxT("IBM Visual Age (OS/2)"));
#endif

#ifdef __VISUALC__
	// The value of this macro corresponds to the compiler version: 1020 for 4.2 (the first supported version), 1100 for 5.0, 1200 for 6.0 and so on 
	strCompiler.Printf(wxT("Microsoft Visual C++ v%d.%02d"), __VISUALC__/100 - 6, __VISUALC__%100);
#endif

#ifdef __XLC__
	strCompiler.Printf(wxT("AIX compiler"));
#endif

#ifdef __WATCOMC__
	// The value of this macro corresponds to the compiler version, 1100 is 11.0 and 1200 is OpenWatcom.
	strCompiler.Printf(wxT("Watcom C++ v%d.%02d"), __WATCOMC__/100, __WATCOMC__%100);
#endif

#ifdef _WIN32_WCE
	strCompiler.Printf(wxT("Windows CE"));
#endif

	if( strCompiler.IsEmpty()==true )
	{
		strCompiler = _("an unknown compiler");
	}
	strMsg += strCompiler + wxT(" .<br>");

	// close the html document
	strMsg += wxT("</body></html>");

	// assign the page to the document
	ptHtmlWin->SetPage(strMsg);
}


void muhkuh_aboutDialog::setAuthors(wxHtmlWindow *ptHtmlWin)
{
	wxString strMsg;
	wxColor colBg;


	// get the background color
	colBg = GetBackgroundColour();

	// start html document
	strMsg  = wxT("<html><body bgcolor=\"") + colBg.GetAsString(wxC2S_HTML_SYNTAX) + wxT("\">");

	// start the list of all authors
	strMsg += wxT("<dl>");

	// add one author
	strMsg += wxT("<dt>Chris</dt>");
	strMsg += wxT("<dd><a href=\"mailto:doc_bacardi@users.sourceforge.net\">doc_bacardi@users.sourceforge.net</a><p></dd>");

	// close the list of authors
	strMsg += wxT("</dl>");

	// close the html document
	strMsg += wxT("</body></html>");

	// assign the page to the document
	ptHtmlWin->SetPage(strMsg);
}


void muhkuh_aboutDialog::setLicense(wxHtmlWindow *ptHtmlWin)
{
	wxString strMsg;


	strMsg = wxString::FromAscii(license);
	// assign the page to the document
	ptHtmlWin->SetPage(strMsg);
}


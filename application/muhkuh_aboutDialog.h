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


#include <wx/wx.h>
#include <wx/html/htmlwin.h>
#include <wx/notebook.h>


#ifndef __MUHKUH_ABOUTDIALOG_H__
#define __MUHKUH_ABOUTDIALOG_H__


// the 'about' dialog
class muhkuh_aboutDialog : public wxDialog
{
public:
	muhkuh_aboutDialog(wxWindow* parent, const wxString &strVersion, wxIconBundle &frameIcons);

private:
	void setAbout(wxHtmlWindow *ptHtmlWin);
	void setAuthors(wxHtmlWindow *ptHtmlWin);
	void setLicense(wxHtmlWindow *ptHtmlWin);

	wxBoxSizer *m_mainBoxSizer;
	wxBoxSizer *m_logoSizer;
	wxStaticBitmap *m_logoBitmap;
	wxStaticText *m_appText;
	wxNotebook *m_tabs;
	wxHtmlWindow *m_aboutHtml;
	wxHtmlWindow *m_authorsHtml;
	wxHtmlWindow *m_licenseHtml;
};


#endif	/* __MUHKUH_ABOUTDIALOG_H__ */


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

#include "muhkuh_dirlistbox.h"

#include <wx/filename.h>

#include "muhkuh_id.h"

BEGIN_EVENT_TABLE(muhkuh_dirlistbox, wxVListBox)
	EVT_TEXT_ENTER(muhkuh_dirlistbox_TextCtrl,		muhkuh_dirlistbox::OnTextEnter)
	EVT_BUTTON(muhkuh_dirlistbox_BrowseButton,		muhkuh_dirlistbox::OnBrowseButton)
END_EVENT_TABLE()

muhkuh_dirlistbox::muhkuh_dirlistbox(wxWindow *parent, wxWindowID id, const wxPoint& pos, const wxSize& size, const wxArrayString& astrPaths, const wxString &strAppPath, long style)
 : wxVListBox(parent, id, pos, size, style)
 , m_astrPaths(astrPaths)
 , m_strApplicationPath(strAppPath)
{
	wxSize tSize;
	size_t sizCnt;
	size_t sizMax;
	wxMemoryDC memoryDC;


	// create a paintDC to get the pixel length of the path strings
	// use this control's font
	memoryDC.SetFont( GetFont() );

	// loop over all strings and get the pixel width
	sizCnt = 0;
	sizMax = m_astrPaths.GetCount();
	while( sizCnt<sizMax )
	{
		tSize = memoryDC.GetTextExtent( m_astrPaths.Item(sizCnt) );
		m_aiPathPixelHeight.Add( tSize.GetHeight() );
		m_aiPathPixelWidth.Add( tSize.GetWidth() );
		++sizCnt;
	}
	SetLineCount(sizMax);

	// get the size of "..."
	tSize = memoryDC.GetTextExtent( wxT("...") );
	m_iDotDotDotWidth = tSize.GetWidth();

	// no active item
	sizActiveItem = -1;

	m_ptTextCtrl = new wxTextCtrl(this, muhkuh_dirlistbox_TextCtrl, wxT(""), wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER);
	m_ptBrowseButton = new wxButton(this, muhkuh_dirlistbox_BrowseButton, wxT("..."), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT);
	m_ptTextCtrl->Show(false);
	m_ptBrowseButton->Show(false);

	tSize = m_ptTextCtrl->GetEffectiveMinSize();
	tDefaultHeight = tSize.GetHeight();

	tSize.SetWidth(-1);
	m_ptBrowseButton->SetMinSize(tSize);
	tSize = m_ptBrowseButton->GetEffectiveMinSize();
	m_iButtonWidth = tSize.GetWidth();

	m_ptTextCtrl->Connect(muhkuh_dirlistbox_TextCtrl, wxEVT_KEY_DOWN, wxKeyEventHandler(muhkuh_dirlistbox::OnKeyDown));
}


muhkuh_dirlistbox::~muhkuh_dirlistbox(void)
{
}


void muhkuh_dirlistbox::OnDrawItem(wxDC& dc, const wxRect& rect, size_t n) const
{
	wxString strPath;
	wxSize tStringSize;
	int iYOffset;
	const int iXOffset = 4;


	// is this the active item?
	if( n==sizActiveItem )
	{
		// draw active item
		m_ptTextCtrl->SetSize(rect.x, rect.y, rect.width-m_iButtonWidth, rect.height);
		m_ptBrowseButton->SetSize(rect.x+rect.width-m_iButtonWidth, rect.y, m_iButtonWidth, rect.height);
	}
	else
	{
		// draw simple string

		// is the index in range?
		if( n<m_astrPaths.GetCount() )
		{
			strPath = m_astrPaths.Item(n);
			tStringSize.Set( m_aiPathPixelWidth.Item(n), m_aiPathPixelHeight.Item(n) );
		}
		else
		{
			tStringSize.Set( 0, 0 );
		}

		iYOffset = (tDefaultHeight - tStringSize.GetHeight())/2;
		// does the text plus offset fit into the window?
		if( (tStringSize.GetWidth()+iXOffset)>rect.width )
		{
			{
				wxDCClipper clip(dc, rect.x+iXOffset, rect.y, rect.width-m_iDotDotDotWidth, rect.height);
				dc.DrawText(strPath, rect.x+iXOffset, rect.y+iYOffset);
			}
			dc.DrawText(wxT("..."), rect.x+rect.width-m_iDotDotDotWidth, rect.y+iYOffset);
		}
		else
		{
			dc.DrawText(strPath, rect.x+iXOffset, rect.y+iYOffset);
		}
	}
}


wxCoord muhkuh_dirlistbox::OnMeasureItem(size_t n) const
{
	// all lines have the same height, ignore 'n'
	return tDefaultHeight;
}



size_t muhkuh_dirlistbox::Append(const wxString&  item)
{
	wxSize tSize;
	size_t sizLineCount;
	wxMemoryDC memoryDC;


	// create a paintDC to get the pixel length of the path strings
	// use this control's font
	memoryDC.SetFont( GetFont() );

	// get the string's pixel size
	tSize = memoryDC.GetTextExtent(item);
	// append the new item to the path array
	m_astrPaths.Add(item);
	m_aiPathPixelHeight.Add( tSize.GetHeight() );
	m_aiPathPixelWidth.Add( tSize.GetWidth() );
	// set the new linecount
	sizLineCount = m_astrPaths.GetCount();
	SetLineCount(sizLineCount);

	return sizLineCount-1;
}


void muhkuh_dirlistbox::Delete(unsigned int n)
{
	size_t sizLineCount;


	// get old line count
	sizLineCount = m_astrPaths.GetCount();
	if( n<sizLineCount )
	{
		// remove the item from the path array
		m_astrPaths.RemoveAt(n);
		m_aiPathPixelHeight.RemoveAt(n);
		m_aiPathPixelWidth.RemoveAt(n);
		// set new linecount
		--sizLineCount;
		SetLineCount(sizLineCount);
		// Redraw all modified lines
		RefreshAll();
	}
}


unsigned int muhkuh_dirlistbox::GetCount() const
{
	return m_astrPaths.GetCount();
}


wxString  muhkuh_dirlistbox::GetString(unsigned int n) const
{
	return m_astrPaths.Item(n);
}


void muhkuh_dirlistbox::SetString(unsigned int n, const wxString&  string)
{
	wxSize tSize;
	size_t sizLineCount;
	wxMemoryDC memoryDC;


	// create a paintDC to get the pixel length of the path strings
	// use this control's font
	memoryDC.SetFont( GetFont() );

	// get old line count
	sizLineCount = m_astrPaths.GetCount();
	if( n<sizLineCount )
	{
		// get the string's pixel size
		tSize = memoryDC.GetTextExtent(string);

		// remove the old item from the path array
		m_astrPaths.RemoveAt(n);
		m_aiPathPixelHeight.RemoveAt(n);
		m_aiPathPixelWidth.RemoveAt(n);

		// insert the new item
		m_astrPaths.Insert(string, n);
		m_aiPathPixelHeight.Insert(tSize.GetHeight(), n);
		m_aiPathPixelWidth.Insert(tSize.GetWidth(), n);

		// Redraw the modified line
		RefreshLine(n);
	}
}


void muhkuh_dirlistbox::StartEdit(unsigned int n)
{
	size_t sizLineCount;


	// get old line count
	sizLineCount = m_astrPaths.GetCount();
	if( n<sizLineCount )
	{
		// set the text ctrl contents
		m_ptTextCtrl->SetValue(m_astrPaths.Item(n));

		m_ptTextCtrl->Show(true);
		m_ptBrowseButton->Show(true);

		sizActiveItem = n;
		// Redraw the modified line
		RefreshLine(n);

		// set the focus to the text ctrl
		m_ptTextCtrl->SetFocus();
	}
}


void muhkuh_dirlistbox::CancelEdit(void)
{
	muhkuh_dirlistbox::stopEditing(this);
}


void muhkuh_dirlistbox::OnBrowseButton(wxCommandEvent &event)
{
	wxDirDialog *testPathDialog;
	wxString strDialogInitPath;
	wxString strWild;
	wxString strPath;
	int iFirstQm;
	wxFileName fileName;


	if( sizActiveItem>=0 )
	{
		strDialogInitPath = m_ptTextCtrl->GetValue();
		// find first '?'
		iFirstQm = strDialogInitPath.Find(wxT('?'));
		if( iFirstQm!=wxNOT_FOUND )
		{
			// cut off wildcart
			strWild = strDialogInitPath.Mid(iFirstQm);
			if( iFirstQm==0 )
			{
				// only wildcart
				strPath = wxEmptyString;
			}
			else
			{
				strPath = strDialogInitPath.Left(iFirstQm);
			}
		}
		else
		{
			// no wildcart -> use the whole string
			strWild = wxEmptyString;
			strPath = strDialogInitPath;
		}

		fileName.Assign(strPath);
		if(fileName.Normalize(wxPATH_NORM_ALL, m_strApplicationPath ,wxPATH_NATIVE))
		{
			strPath = fileName.GetFullPath();
		}

		testPathDialog = new wxDirDialog(this, _("Choose the lua include path"));
		testPathDialog->SetPath(strPath);

		if( testPathDialog->ShowModal()==wxID_OK )
		{
			strPath = testPathDialog->GetPath() + wxFileName::GetPathSeparator() + strWild;
			m_ptTextCtrl->SetValue(strPath);
			// set focus back to textctrl
			m_ptTextCtrl->SetFocus();
		}
		testPathDialog->Destroy();
	}
}


void muhkuh_dirlistbox::OnTextEnter(wxCommandEvent &event)
{
	wxString strPath;
	size_t sizItem;


	if( sizActiveItem>=0 )
	{
		sizItem = sizActiveItem;
		strPath = m_ptTextCtrl->GetValue();
		stopEditing(this);
		SetString(sizItem, strPath);
	}
}


// NOTE: this function gets the textCtrl pointer as this. This means this is not good...
void muhkuh_dirlistbox::OnKeyDown(wxKeyEvent& event)
{
	wxTextCtrl *ptTextCtrl;
	muhkuh_dirlistbox *ptSelf;


	ptTextCtrl = (wxTextCtrl*)event.GetEventObject();
	if( ptTextCtrl!=NULL )
	{
		ptSelf = (muhkuh_dirlistbox*)ptTextCtrl->GetParent();
		if( ptSelf!=NULL )
		{
			switch ( event.GetKeyCode() )
			{
			case WXK_ESCAPE:
				muhkuh_dirlistbox::stopEditing(ptSelf);
				ptSelf->RefreshAll();
				break;
			default:
				event.Skip();
				break;
			}
		}
	}
}


void muhkuh_dirlistbox::stopEditing(muhkuh_dirlistbox *ptSelf)
{
	if( ptSelf->sizActiveItem>=0 )
	{
		ptSelf->m_ptTextCtrl->Show(false);
		ptSelf->m_ptBrowseButton->Show(false);
		ptSelf->sizActiveItem = -1;
	}
}


wxString muhkuh_dirlistbox::GetPaths(wxChar cSeparator) const
{
	wxString strPaths;
	size_t sizCnt;
	size_t sizMax;


	sizCnt = 0;
	sizMax = m_astrPaths.GetCount();
	if( sizMax>0 )
	{
		while( (sizCnt+1)<sizMax )
		{
			strPaths += m_astrPaths.Item(sizCnt) + cSeparator;
			++sizCnt;
		}
		strPaths += m_astrPaths.Item(sizCnt);
	}

	wxLogMessage(strPaths);
	return strPaths;
}



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
#include <wx/dynarray.h>
#include <wx/vlbox.h>
#include <wx/tooltip.h>


#ifndef __MUHKUH_DIRLISTBOX_H__
#define __MUHKUH_DIRLISTBOX_H__


WX_DEFINE_ARRAY_INT(int, taiPathPixelSize);

class muhkuh_dirlistbox : public wxVListBox
{
public:
	muhkuh_dirlistbox(wxWindow *parent, wxWindowID id, const wxPoint& pos, const wxSize& size, const wxArrayString& astrPaths, const wxString &strAppPath, long style);
	~muhkuh_dirlistbox(void);

	size_t Append(const wxString&  item);
	void Delete(unsigned int n);
	unsigned int GetCount() const;
	wxString  GetString(unsigned int n) const;
	void SetString(unsigned int n, const wxString&  string);
	void StartEdit(unsigned int n);
	void CancelEdit(void);

	void OnBrowseButton(wxCommandEvent &event);
	void OnTextEnter(wxCommandEvent &event);
	void OnKeyDown(wxKeyEvent& event);

	wxString GetPaths(wxChar cSeparator) const;

protected:
	virtual void OnDrawItem(wxDC& dc, const wxRect& rect, size_t n) const;
	virtual wxCoord OnMeasureItem(size_t n) const;

private:
	static void stopEditing(muhkuh_dirlistbox *ptSelf);

	// the array for all paths
	wxArrayString m_astrPaths;
	taiPathPixelSize m_aiPathPixelHeight;
	taiPathPixelSize m_aiPathPixelWidth;

	// the application path
	wxString m_strApplicationPath;

	// the default height for all lines
	wxCoord tDefaultHeight;

	// the active item is shown with an edit field and a browse button
	// it's only valid if it's >=0
	size_t sizActiveItem;

	// size of the browse button
	int m_iButtonWidth;
	int m_iDotDotDotWidth;

	wxTextCtrl *m_ptTextCtrl;
	wxButton *m_ptBrowseButton;
	int m_iTextXOffset;
	wxColor m_colTextNormal;
	wxColor m_colTextSelected;
	wxFont m_fontDirlist;

    DECLARE_EVENT_TABLE()
};

#endif	// __MUHKUH_DIRLISTBOX_H__


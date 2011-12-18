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


#include <stddef.h>

#include <wx/wx.h>
#include <wx/dynarray.h>
#include <wx/scrolwin.h>


#ifndef __MUHKUH_DIRLISTBOX_H__
#define __MUHKUH_DIRLISTBOX_H__


class muhkuh_dirlistbox_element
{
public:
	muhkuh_dirlistbox_element(void)
	{
		m_strPath.Empty();
		m_ulFixedExtensions = 0;
		m_strCustomExtension.Empty();
	}

	wxString m_strPath;
	unsigned long m_ulFixedExtensions;
	wxString m_strCustomExtension;
};

WX_DECLARE_OBJARRAY(muhkuh_dirlistbox_element, MuhkuhDirlistboxElementArray);


class muhkuh_dirlistbox : public wxScrolledWindow
{
public:
	muhkuh_dirlistbox(wxWindow *parent, wxWindowID id, wxString& strPathList, const wxArrayString& astrFixedExtensions, bool fAllowCustomExtension);
	~muhkuh_dirlistbox(void);

	wxString GetPaths(char cSeparator);
	void OnButtonBrowse(wxCommandEvent &event);
	void OnButtonAdd(wxCommandEvent &event);
	void OnButtonRemove(wxCommandEvent &event);
	void OnButtonEdit(wxCommandEvent &event);
	void OnButtonUp(wxCommandEvent &event);
	void OnButtonDown(wxCommandEvent &event);

	void OnFocusChild(wxChildFocusEvent &event);
private:
	wxTextCtrl *get_textctrl(size_t sizIndex);
	wxCheckBox *get_checkbox(size_t sizIndex);
	void read_row(size_t sizRow, muhkuh_dirlistbox_element &tElement);
	int find_window_in_sizer(wxWindow *ptWindow);
	void split_path_list(wxString& strPathList, char cSeparator);
	void highlight_row(size_t sizRow, bool fHighlight);
	void focus_index(size_t sizIndex);
	void exchange_text_ctrl(size_t sizIndex0, size_t sizIndex1);
	void exchange_checkbox(size_t sizIndex0, size_t sizIndex1);
	void exchange_rows(size_t sizRow0, size_t sizRow1);
	bool select_path(wxString &strPath);
	void append_new_list_item(muhkuh_dirlistbox_element &tElement);
	void updateButtons(int iSelection);

	/* These items are not owned by this class. */
	wxFlexGridSizer *m_ptSizer;
	wxToolBar *m_ptToolBar;

	wxBitmap *m_ptBitmap;

	/* The number of fixed extensions which can be selected with a checkbox. */
	size_t m_sizFixedExtensions;
	wxArrayString m_astrFixedExtensions;
	bool m_fAllowCustomExtension;
	int m_iTotalColumns;

	unsigned long m_ulDefaultFixedExtensions;

	MuhkuhDirlistboxElementArray m_atPaths;

	wxString strLastUsedPath;

	int m_iSelectedRow;
	int m_iLastSelectedRow;

    DECLARE_EVENT_TABLE()
};

#endif	// __MUHKUH_DIRLISTBOX_H__


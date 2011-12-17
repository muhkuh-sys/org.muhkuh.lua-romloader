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


#include <wx/filename.h>
#include <wx/stdpaths.h>
#include <wx/settings.h>
#include <wx/tglbtn.h>

#include "muhkuh_dirlistbox.h"
#include "muhkuh_icons.h"


#include <wx/arrimpl.cpp>
WX_DEFINE_OBJARRAY(MuhkuhDirlistboxElementArray);


BEGIN_EVENT_TABLE(muhkuh_dirlistbox, wxScrolledWindow)
	EVT_BUTTON(wxID_ANY,                                            muhkuh_dirlistbox::OnButtonBrowse)

	EVT_TOOL(wxID_ADD,     muhkuh_dirlistbox::OnButtonAdd)
	EVT_TOOL(wxID_DELETE,  muhkuh_dirlistbox::OnButtonRemove)
	EVT_TOOL(wxID_EDIT,    muhkuh_dirlistbox::OnButtonEdit)
	EVT_TOOL(wxID_UP,      muhkuh_dirlistbox::OnButtonUp)
	EVT_TOOL(wxID_DOWN,    muhkuh_dirlistbox::OnButtonDown)

	EVT_CHILD_FOCUS(muhkuh_dirlistbox::OnFocusChild)
END_EVENT_TABLE()


muhkuh_dirlistbox::muhkuh_dirlistbox(wxWindow *parent, wxWindowID id, wxString& strPathList, const wxArrayString& astrFixedExtensions, bool fAllowCustomExtension)
 : wxScrolledWindow(parent, id)
 , m_ptBitmap(NULL)
 , m_iSelectedRow(wxNOT_FOUND)
 , m_iLastSelectedRow(wxNOT_FOUND)
{
	size_t sizNewSize;
	size_t sizCnt;
	size_t sizMax;
	wxBoxSizer *ptMainSizer;
	int iCols;


	m_sizFixedExtensions = astrFixedExtensions.GetCount();
	m_astrFixedExtensions = astrFixedExtensions;
	m_fAllowCustomExtension = fAllowCustomExtension;

	/* Enable all fixed extensions by default for new entries. */
	/* TODO: make this a parameter, maybe... */
	m_ulDefaultFixedExtensions = (1<<(m_sizFixedExtensions+1))-1;

	/* FIXME: pass the separator as a class argument. */
	split_path_list(strPathList, ';');

	/* Create the main sizer. */
	ptMainSizer = new wxBoxSizer(wxVERTICAL);
	SetSizer(ptMainSizer);

	/* Get the number of cols. */
	/* The minimum number of columns is 2: one for the path and one for the browse button. */
	iCols  = 2;
	/* Append the number of standard extensions. */
	iCols += m_sizFixedExtensions;
	/* Add one more column if a custom extension is allowed. */
	if( m_fAllowCustomExtension==true )
	{
		++iCols;
	}
	m_iTotalColumns = iCols;

	/* Create the flex grid sizer for the elements. */
	m_ptSizer = new wxFlexGridSizer(iCols, 2, 0);
	/* The first column holds the text. This one should grow on a resize. */
	m_ptSizer->AddGrowableCol(0, 1);
	/* The text column should only grow in horizontal size. It has only one line. */
	m_ptSizer->SetFlexibleDirection(wxHORIZONTAL);
	/* The 2nd column holds a button with a fixed size. It should not grow at all. */
	m_ptSizer->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_NONE);
	ptMainSizer->Add(m_ptSizer, 1, wxEXPAND);


	/* Assign the standard folder bitmap. */
	m_ptBitmap = new wxBitmap(icon_famfamfam_silk_folder);


	/* Create the path toolbar. */
	m_ptToolBar = new wxToolBar(this, wxID_ANY, wxDefaultPosition, wxDefaultSize,  wxTB_HORIZONTAL|wxNO_BORDER|wxTB_TEXT);
	m_ptToolBar->AddTool(wxID_ADD, _("Add"), icon_famfamfam_silk_folder_add, wxNullBitmap, wxITEM_NORMAL, _("Add Path"), _("Add a new include path to the list"));
	m_ptToolBar->AddTool(wxID_DELETE, _("Remove"), icon_famfamfam_silk_folder_delete, wxNullBitmap, wxITEM_NORMAL, _("Remove Path"), _("Remove the selected include path from the list"));
	m_ptToolBar->AddTool(wxID_EDIT, _("Edit"), icon_famfamfam_silk_folder_edit, wxNullBitmap, wxITEM_NORMAL, _("Edit Path"), _("Edit the selected include path"));
	m_ptToolBar->AddTool(wxID_UP, _("Up"), icon_famfamfam_silk_arrow_up, wxNullBitmap, wxITEM_NORMAL, _("Move Path Up"), _("Move the selected include path up"));
	m_ptToolBar->AddTool(wxID_DOWN, _("Down"), icon_famfamfam_silk_arrow_down, wxNullBitmap, wxITEM_NORMAL, _("Move Path Down"), _("Move the selected include path down"));
	m_ptToolBar->EnableTool(wxID_DELETE, false);
	m_ptToolBar->EnableTool(wxID_EDIT, false);
	m_ptToolBar->EnableTool(wxID_UP, false);
	m_ptToolBar->EnableTool(wxID_DOWN, false);
	m_ptToolBar->Realize();
	ptMainSizer->Add(m_ptToolBar, 0, wxEXPAND);

	/* Add all elements from the paths array. */
	sizCnt = 0;
	sizMax = m_atPaths.GetCount();
	while(sizCnt<sizMax)
	{
		append_new_list_item(m_atPaths.Item(sizCnt));
		++sizCnt;
	}
}


muhkuh_dirlistbox::~muhkuh_dirlistbox(void)
{
	if( m_ptBitmap!=NULL )
	{
		free(m_ptBitmap);
		m_ptBitmap = NULL;
	}
}


void muhkuh_dirlistbox::split_path_list(wxString& strPathList, char cSeparator)
{
	int iSeparatorPos;
	wxArrayString astrPaths;
	size_t sizPathCnt;
	size_t sizPathEnd;
	wxFileName tIncludePath;
	wxArrayString astrDirs;
	size_t sizDirCnt;
	size_t sizDirEnd;
	wxString strDir;
	wxString strSuffix;
	int iIndex;
	muhkuh_dirlistbox_element tElement;


	/* Remove any old path lists. */
	m_atPaths.Clear();

	/* Split the include paths. */
	while( strPathList.IsEmpty()==false )
	{
		iSeparatorPos = strPathList.Find(cSeparator);
		if( iSeparatorPos==wxNOT_FOUND )
		{
			astrPaths.Add(strPathList);
			break;
		}
		else
		{
			if( iSeparatorPos>0 )
			{
				astrPaths.Add(strPathList.Left(iSeparatorPos) );
			}
			strPathList = strPathList.Mid(iSeparatorPos+1);
		}
	}


	/* Sort the include paths */
	sizPathCnt = 0;
	sizPathEnd = astrPaths.GetCount();
	while(sizPathCnt<sizPathEnd)
	{
		/* Search for forbidden chars. */
		tIncludePath.AssignDir(astrPaths.Item(sizPathCnt));
		astrDirs = tIncludePath.GetDirs();
		sizDirCnt = 0;
		sizDirEnd = astrDirs.GetCount();
		while(sizDirCnt<sizDirEnd)
		{
			strDir = astrDirs.Item(sizDirCnt);
			if( strDir.find_first_of(wxFileName::GetForbiddenChars(wxPATH_NATIVE), 0)<strDir.Len() )
			{
				break;
			}
			++sizDirCnt;
		}
		if(sizDirCnt<sizDirEnd)
		{
			/* Remove all valid dir components from the dirs array. */
			if( sizDirCnt>0 )
			{
				astrDirs.RemoveAt(0, sizDirCnt);
			}
			/* Remove all invalid dir components from the path. */
			while(sizDirEnd>sizDirCnt)
			{
				tIncludePath.RemoveLastDir();
				--sizDirEnd;
			}
		}

		/* Get the path and the suffix. */
		tElement.m_strPath = tIncludePath.GetFullPath(wxPATH_NATIVE);

		strSuffix.Clear();
		sizDirCnt = 0;
		sizDirEnd = astrDirs.GetCount();
		while(sizDirCnt<sizDirEnd)
		{
			strSuffix += astrDirs.Item(sizDirCnt);
			++sizDirCnt;
			if(sizDirCnt<sizDirEnd)
			{
				strSuffix += wxFileName::GetPathSeparator();
			}
		}
		
		iIndex = m_astrFixedExtensions.Index(strSuffix);
		if( iIndex==wxNOT_FOUND )
		{
			/* Add this as a custom extension. */
			tElement.m_ulFixedExtensions = 0;
			tElement.m_strCustomExtension = strSuffix;
		}
		else
		{
			/* This is one of the fixed extensions. */
			tElement.m_ulFixedExtensions = 1 << iIndex;
			tElement.m_strCustomExtension.Clear();
		}

		/* Search elements with the same path. */
		sizDirCnt = 0;
		sizDirEnd = m_atPaths.GetCount();
		while(sizDirCnt<sizDirEnd)
		{
			muhkuh_dirlistbox_element &tCnt = m_atPaths.Item(sizDirCnt);

			/* The elements can be combined if the paths match and one of the following conditions is true:
			 *  1) The new elements custom extension is empty.
			 *  2) The existing elemets custom extension is empty.
			 *  3) Both custom extensions match.
			 */
			if( tElement.m_strPath==tCnt.m_strPath &&
			    ( (tElement.m_strCustomExtension.IsEmpty()==true) ||
			      (tCnt.m_strCustomExtension.IsEmpty()==true) ||
			      (tElement.m_strCustomExtension==tCnt.m_strCustomExtension)
			    )
			)
			{
				/* Combine the fixed extension flags. */
				tCnt.m_ulFixedExtensions |= tElement.m_ulFixedExtensions;
				
				/* Keep the new extension if there was none. */
				if( tCnt.m_strCustomExtension.IsEmpty()==true && tElement.m_strCustomExtension.IsEmpty()==false )
				{
					tCnt.m_strCustomExtension = tElement.m_strCustomExtension;
				}
				
				/* Entry processed! */
				break;
			}

			++sizDirCnt;
		}
		/* Add a new list entry? */
		if( (sizDirEnd==0) || (sizDirCnt==sizDirEnd) )
		{
			m_atPaths.Add(tElement);
		}
		
		++sizPathCnt;
	}
}


int muhkuh_dirlistbox::find_window_in_sizer(wxWindow *ptWindow)
{
	int iSelectedRow;
	const wxSizerItemList &tSizerItems = m_ptSizer->GetChildren();
	wxSizerItemList::const_iterator tIter;
	wxSizerItem *ptSizerItem;
	size_t sizCnt;


	iSelectedRow = wxNOT_FOUND;

	sizCnt = 0;
	tIter = tSizerItems.begin();
	while(tIter!=tSizerItems.end())
	{
		ptSizerItem = *tIter;
		if( ptSizerItem!=NULL && ptSizerItem->GetWindow()==ptWindow )
		{
			/* Get the row from the index. */
			iSelectedRow = sizCnt / m_iTotalColumns;
			break;
		}
		else
		{
			++sizCnt;
			++tIter;
		}
	}

	return iSelectedRow;
}


wxTextCtrl *muhkuh_dirlistbox::get_textctrl(size_t sizIndex)
{
	wxTextCtrl *ptTextCtrl;
	wxSizerItem *ptSizerItem;
	wxWindow *ptWindow;


	ptTextCtrl = NULL;

	ptSizerItem = m_ptSizer->GetItem(sizIndex);
	if( ptSizerItem!=NULL && ptSizerItem->IsWindow()==true )
	{
		ptWindow = ptSizerItem->GetWindow();
		if( ptWindow->IsKindOf(wxCLASSINFO(wxTextCtrl))==true )
		{
			ptTextCtrl = (wxTextCtrl*)ptWindow;
		}
	}

	return ptTextCtrl;
}


wxCheckBox *muhkuh_dirlistbox::get_checkbox(size_t sizIndex)
{
	wxCheckBox *ptCheckBox;
	wxSizerItem *ptSizerItem;
	wxWindow *ptWindow;


	ptCheckBox = NULL;

	ptSizerItem = m_ptSizer->GetItem(sizIndex);
	if( ptSizerItem!=NULL && ptSizerItem->IsWindow()==true )
	{
		ptWindow = ptSizerItem->GetWindow();
		if( ptWindow->IsKindOf(wxCLASSINFO(wxCheckBox))==true )
		{
			ptCheckBox = (wxCheckBox*)ptWindow;
		}
	}

	return ptCheckBox;
}


void muhkuh_dirlistbox::read_row(size_t sizRow, muhkuh_dirlistbox_element &tElement)
{
	size_t sizCnt;
	size_t sizEnd;
	wxTextCtrl *ptTextCtrl;
	wxCheckBox *ptCheckBox;
	unsigned long ulBits;
	unsigned long ulMaskCnt;


	/* Get the index of the first element in the row. */
	sizCnt = sizRow * m_iTotalColumns;

	ptTextCtrl = get_textctrl(sizCnt);
	if( ptTextCtrl!=NULL )
	{
		tElement.m_strPath = ptTextCtrl->GetValue();
	}
	else
	{
		tElement.m_strPath.Clear();
	}

	/* Loop over all checkboxes. */
	++sizCnt;
	sizEnd = sizRow * m_iTotalColumns + 1 + m_iTotalColumns;
	ulBits = 0;
	ulMaskCnt = 1;
	while(sizCnt<sizEnd)
	{
		ptCheckBox = get_checkbox(sizCnt);
		if( ptCheckBox!=NULL && ptCheckBox->GetValue()==true )
		{
			ulBits |= ulMaskCnt;
		}
		ulMaskCnt <<= 1;
		++sizCnt;
	}
	tElement.m_ulFixedExtensions = ulBits;

	/* Get the custom extension. */
	ptTextCtrl = get_textctrl(sizCnt);
	if( ptTextCtrl!=NULL )
	{
		tElement.m_strCustomExtension = ptTextCtrl->GetValue();
	}
	else
	{
		tElement.m_strCustomExtension.Clear();
	}
}


wxString muhkuh_dirlistbox::GetPaths(char cSeparator)
{
	wxString strPath;
	bool fIsFirst;
	size_t sizRowCnt;
	size_t sizRowEnd;
	muhkuh_dirlistbox_element tElement;
	size_t sizExtCnt;
	size_t sizExtEnd;


	fIsFirst = true;

	/* Loop over all rows. */
	sizRowCnt = 0;
	sizRowEnd = m_ptSizer->GetEffectiveRowsCount();
	while(sizRowCnt<sizRowEnd)
	{
		read_row(sizRowCnt, tElement);
		if( tElement.m_strPath.IsEmpty()==false )
		{
			if( tElement.m_ulFixedExtensions==0 && tElement.m_strCustomExtension.IsEmpty()==true )
			{
				if( fIsFirst==false )
				{
					strPath.Append(cSeparator);
				}
				else
				{
					fIsFirst = false;
				}
				strPath.Append(tElement.m_strPath);
			}
			else
			{
				/* Add all fixed extensions. */
				sizExtCnt = 0;
				sizExtEnd = m_sizFixedExtensions;
				while(sizExtCnt<sizExtEnd)
				{
					if( (tElement.m_ulFixedExtensions&(1<<sizExtCnt))!=0 )
					{
						if( fIsFirst==false )
						{
							strPath.Append(cSeparator);
						}
						else
						{
							fIsFirst = false;
						}
						strPath.Append(tElement.m_strPath + m_astrFixedExtensions.Item(sizExtCnt));
					}
				}
				
				/* Add the custom extension. */
				if( tElement.m_strCustomExtension.IsEmpty()==false )
				{
					if( fIsFirst==false )
					{
						strPath.Append(cSeparator);
					}
					else
					{
						fIsFirst = false;
					}
					strPath.Append(tElement.m_strPath + tElement.m_strCustomExtension);
				}
			}
		}
	}

	return strPath;
}


void muhkuh_dirlistbox::OnButtonBrowse(wxCommandEvent &event)
{
	wxObject *ptObj;
	wxBitmapButton *ptBitmapButton;
	int iRow;
	size_t sizIdx;
	wxTextCtrl *ptTextCtrl;
	wxString strPath;
	bool fIsUpdated;


	/* Get the object associated with the event. */
	ptObj = event.GetEventObject();
	if( ptObj!=NULL && ptObj->IsKindOf(wxCLASSINFO(wxBitmapButton))==true )
	{
		ptBitmapButton = (wxBitmapButton*)ptObj;

		/* Look up the button in the sizer. */
		iRow = find_window_in_sizer(ptBitmapButton);
		if( iRow!=wxNOT_FOUND && iRow>=0 )
		{
			/* Get the index of the text control. */
			sizIdx = (size_t)(iRow * m_iTotalColumns);
			ptTextCtrl = get_textctrl(sizIdx);
			if( ptTextCtrl!=NULL )
			{
				/* Get the text. */
				strPath = ptTextCtrl->GetValue();
				fIsUpdated = select_path(strPath);
				if( fIsUpdated==true )
				{
					ptTextCtrl->SetValue(strPath);
				}
			}
		}
	}
}


void muhkuh_dirlistbox::OnButtonAdd(wxCommandEvent &event)
{
	wxString strPath;
	bool fIsConfirmed;
	size_t sizIndex;
	muhkuh_dirlistbox_element tElement;


	strPath = strLastUsedPath;
	fIsConfirmed = select_path(strPath);
	if( fIsConfirmed==true )
	{
		tElement.m_strPath = strPath;
		tElement.m_ulFixedExtensions = m_ulDefaultFixedExtensions;
		append_new_list_item(tElement);
		m_ptSizer->Layout();

		/* Set the focus to the new element. The new element goes always to the end of the list. */
		sizIndex = (m_ptSizer->GetEffectiveRowsCount() - 1) * m_iTotalColumns;
		focus_index(sizIndex);
	}
}


void muhkuh_dirlistbox::OnButtonRemove(wxCommandEvent &event)
{
	size_t sizRow;
	size_t sizCnt;
	size_t sizEnd;
	wxSizerItem *ptSizerItem;
	wxWindow *ptWindow;


	/* Is the current selection a valid list index? */
	if( m_iSelectedRow!=wxNOT_FOUND && m_iSelectedRow>=0 )
	{
		sizRow = (size_t)m_iSelectedRow;
		if( sizRow<m_ptSizer->GetEffectiveRowsCount() )
		{
			sizCnt = sizRow * m_iTotalColumns;
			sizEnd = sizRow * m_iTotalColumns + m_iTotalColumns;
			while(sizEnd>sizCnt)
			{
				--sizEnd;

				ptSizerItem = m_ptSizer->GetItem(sizEnd);
				if( ptSizerItem!=NULL && ptSizerItem->IsWindow()==true )
				{
					ptWindow = ptSizerItem->GetWindow();
					if( ptWindow!=NULL )
					{
						m_ptSizer->Detach(ptWindow);
						RemoveChild(ptWindow);
						ptWindow->Destroy();
					}
				}
			}

			m_ptSizer->Layout();

			/* Try to find a new focus element. */

			/* Are still elements in the list? */
			if( m_ptSizer->GetEffectiveRowsCount()==0 )
			{
				/* No entry. Select the panel. */
				SetFocus();
			}
			else
			{
				/* Yes -> there will be a new focus. */

				/* Is the old index still valid? */
				if( sizRow+1>=m_ptSizer->GetEffectiveRowsCount() )
				{
					/* No -> use the last index instead. */
					sizRow = m_ptSizer->GetEffectiveRowsCount() - 1;
				}

				focus_index(sizRow*m_iTotalColumns);
			}
		}
	}
}


void muhkuh_dirlistbox::OnButtonEdit(wxCommandEvent &event)
{
	size_t sizIdx;
	wxTextCtrl *ptTextCtrl;
	wxString strPath;
	bool fIsUpdated;


	if( m_iSelectedRow!=wxNOT_FOUND )
	{
		/* Get the index of the text control. */
		sizIdx = (size_t)(m_iSelectedRow * m_iTotalColumns);
		ptTextCtrl = get_textctrl(sizIdx);
		if( ptTextCtrl!=NULL )
		{
			/* Get the text. */
			strPath = ptTextCtrl->GetValue();
			fIsUpdated = select_path(strPath);
			if( fIsUpdated==true )
			{
				ptTextCtrl->SetValue(strPath);
			}
		}
	}
}


void muhkuh_dirlistbox::OnButtonUp(wxCommandEvent &event)
{
	size_t sizRow;


	/* Is the current selection a valid list index? */
	if( m_iSelectedRow!=wxNOT_FOUND && m_iSelectedRow>=0 )
	{
		sizRow = (size_t)m_iSelectedRow;
		if( sizRow!=0 && sizRow<m_ptSizer->GetEffectiveRowsCount() )
		{
			exchange_rows(sizRow-1, sizRow);

			/* Move the focus one position up. */
			focus_index((sizRow-1)*m_iTotalColumns);
		}
	}
}


void muhkuh_dirlistbox::OnButtonDown(wxCommandEvent &event)
{
	size_t sizRow;


	/* Is the current selection a valid list index? */
	if( m_iSelectedRow!=wxNOT_FOUND && m_iSelectedRow>=0 )
	{
		sizRow = (size_t)m_iSelectedRow;
		if( sizRow+1<m_ptSizer->GetEffectiveRowsCount() )
		{
			exchange_rows(sizRow, sizRow+1);

			/* Move the focus one position down. */
			focus_index((sizRow+1)*m_iTotalColumns);
		}
	}
}


void muhkuh_dirlistbox::OnFocusChild(wxChildFocusEvent &event)
{
	wxWindow *ptWindow;
	int iSelectedRow;
	size_t sizRow;
	wxColor tColor;


	iSelectedRow = find_window_in_sizer(event.GetWindow());

	updateButtons(iSelectedRow);
	m_iSelectedRow = iSelectedRow;

	/* Did the slection change? */
	if( iSelectedRow!=m_iLastSelectedRow )
	{
		/* Was an element selected before? */
		if( m_iLastSelectedRow!=wxNOT_FOUND && m_iLastSelectedRow>=0 )
		{
			/* Deselect the item. */
			sizRow = (size_t)m_iLastSelectedRow;
			highlight_row(sizRow, false);
		}
		if( iSelectedRow!=wxNOT_FOUND && iSelectedRow>=0 )
		{
			/* Select the item. */
			sizRow = (size_t)iSelectedRow;
			highlight_row(sizRow, true);
		}
		m_iLastSelectedRow = iSelectedRow;
	}
}


void muhkuh_dirlistbox::highlight_row(size_t sizRow, bool fHighlight)
{
	wxSystemColour tColorIndex;
	wxColor tColor;
	size_t sizCnt;
	size_t sizEnd;
	wxSizerItem *ptSizerItem;
	wxWindow *ptWindow;


	if( sizRow<m_ptSizer->GetEffectiveRowsCount() )
	{
		if( fHighlight==true )
		{
			tColorIndex = wxSYS_COLOUR_HIGHLIGHT;
		}
		else
		{
			tColorIndex = wxSYS_COLOUR_LISTBOX;
		}
		tColor = wxSystemSettings::GetColour(tColorIndex);

		/* Loop over all elements in the list except the button. */
		sizCnt = sizRow * m_iTotalColumns;
		sizEnd = sizRow * m_iTotalColumns + m_iTotalColumns - 1;
		while(sizCnt<sizEnd)
		{
			ptSizerItem = m_ptSizer->GetItem(sizCnt);
			if( ptSizerItem!=NULL && ptSizerItem->IsWindow()==true )
			{
				ptWindow = ptSizerItem->GetWindow();
				if( ptWindow!=NULL )
				{
					ptWindow->SetBackgroundColour(tColor);
				}
			}
			++sizCnt;
		}
	}
}


void muhkuh_dirlistbox::focus_index(size_t sizIndex)
{
	wxSizerItem *ptSizerItem;
	wxWindow *ptWindow;


	ptSizerItem = m_ptSizer->GetItem(sizIndex);
	if( ptSizerItem!=NULL && ptSizerItem->IsWindow()==true )
	{
		ptWindow = ptSizerItem->GetWindow();
		if( ptWindow!=NULL )
		{
			ptWindow->SetFocus();
		}
	}
}


void muhkuh_dirlistbox::exchange_text_ctrl(size_t sizIndex0, size_t sizIndex1)
{
	wxTextCtrl *ptTextCtrl0;
	wxTextCtrl *ptTextCtrl1;
	wxString strData0;
	wxString strData1;


	ptTextCtrl0 = get_textctrl(sizIndex0);
	ptTextCtrl1 = get_textctrl(sizIndex1);
	if( ptTextCtrl0!=NULL && ptTextCtrl1!=NULL )
	{
		strData0 = ptTextCtrl0->GetValue();
		strData1 = ptTextCtrl1->GetValue();
		
		ptTextCtrl0->SetValue(strData1);
		ptTextCtrl1->SetValue(strData0);
	}
}


void muhkuh_dirlistbox::exchange_checkbox(size_t sizIndex0, size_t sizIndex1)
{
	wxCheckBox *ptCheckBox0;
	wxCheckBox *ptCheckBox1;
	bool fData0;
	bool fData1;


	ptCheckBox0 = get_checkbox(sizIndex0);
	ptCheckBox1 = get_checkbox(sizIndex1);
	if( ptCheckBox0!=NULL && ptCheckBox1!=NULL )
	{
		fData0 = ptCheckBox0->GetValue();
		fData1 = ptCheckBox1->GetValue();
		
		ptCheckBox0->SetValue(fData1);
		ptCheckBox1->SetValue(fData0);
	}
}


void muhkuh_dirlistbox::exchange_rows(size_t sizRow0, size_t sizRow1)
{
	size_t sizIndex0;
	size_t sizIndex1;
	size_t sizCnt;
	size_t sizEnd;


	/* Swap the paths. */
	sizIndex0 = sizRow0 * m_iTotalColumns;
	sizIndex1 = sizRow1 * m_iTotalColumns;
	exchange_text_ctrl(sizIndex0, sizIndex1);
	
	/* Loop over all fixed extensions. */
	sizCnt = 1;
	sizEnd = 1 + m_sizFixedExtensions;
	while(sizCnt<sizEnd)
	{
		/* Swap the checkboxes. */
		sizIndex0 = sizRow0 * m_iTotalColumns + sizCnt;
		sizIndex1 = sizRow1 * m_iTotalColumns + sizCnt;
		exchange_checkbox(sizIndex0, sizIndex1);
		
		++sizCnt;
	}

	/* Swap the custom extension. */
	if( m_fAllowCustomExtension==true )
	{
		sizIndex0 = sizRow0 * m_iTotalColumns + 1 + m_sizFixedExtensions;
		sizIndex1 = sizRow1 * m_iTotalColumns + 1 + m_sizFixedExtensions;
		exchange_text_ctrl(sizIndex0, sizIndex1);
	}
}


bool muhkuh_dirlistbox::select_path(wxString &strPath)
{
	wxDirDialog *ptPathDialog;
	wxFileName tFileName;
	wxArrayString tPathComponents;
	size_t sizCnt;
	size_t sizMax;
	bool fFoundPlaceHolder;
	int iIndex;
	wxString strDialogInitPath;
	bool fPathUpdated;


	/* Try to get an existing folder from the initial path. */
	tFileName.AssignDir(strPath);

	/* The lua include path may contain '?' as a placeholder for the module name.
	 * Find the last element in the path which contains a '?'.
	 */
	tPathComponents = tFileName.GetDirs();
	sizMax = tPathComponents.GetCount();
	sizCnt = sizMax;
	fFoundPlaceHolder = false;
	while(sizCnt!=0)
	{
		--sizCnt;
		iIndex = tPathComponents.Item(sizCnt).Find('?');
		if( iIndex!=wxNOT_FOUND )
		{
			fFoundPlaceHolder = true;
			break;
		}
	}
	/* Found a placeholder? */
	if( fFoundPlaceHolder==true )
	{
		/* Remove it and all following path components. */
		while( sizMax>sizCnt )
		{
			--sizMax;
			tFileName.RemoveLastDir();
		}
	}

	/* Check if the path exists. If not, remove the last directory and try again. */
	while( tFileName.DirExists()!=true && tFileName.GetDirCount()>0 )
	{
		tFileName.RemoveLastDir();
	}

	/* Is anything left? */
	if( tFileName.GetDirCount()>0 )
	{
		/* Yes -> use it as the initial path. */
		strDialogInitPath = tFileName.GetFullPath();
	}
	else
	{
		/* No -> take the plugins dir from the standard paths. */
		strDialogInitPath = wxStandardPaths::Get().GetPluginsDir();
	}

	ptPathDialog = new wxDirDialog(this, _("Choose the lua include path"));
	ptPathDialog->SetPath(strDialogInitPath);

	fPathUpdated = false;
	if( ptPathDialog->ShowModal()==wxID_OK )
	{
		strPath = ptPathDialog->GetPath() + wxFileName::GetPathSeparator();
		strLastUsedPath = strPath;
		fPathUpdated = true;
	}
	ptPathDialog->Destroy();

	return fPathUpdated;
}


void muhkuh_dirlistbox::append_new_list_item(muhkuh_dirlistbox_element &tElement)
{
	size_t sizCnt;
	wxTextCtrl *ptTextCtrl;
	wxCheckBox *ptCheckBox;
	bool fIsChecked;
	wxBitmapButton *ptBitmapButton;


	/* Create the text control. */
	ptTextCtrl = new wxTextCtrl(this, wxID_ANY, tElement.m_strPath, wxDefaultPosition, wxDefaultSize, wxBORDER_NONE|wxTE_LEFT);
	/* Set the text control's background color. */
	ptTextCtrl->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_LISTBOX));
	/* Add the text to the sizer. */
	m_ptSizer->Add(ptTextCtrl, 0, wxEXPAND);
	
	/* Add the checkboxes. */
	for(sizCnt=0; sizCnt<m_sizFixedExtensions; ++sizCnt)
	{
		fIsChecked = (tElement.m_ulFixedExtensions & (1<<sizCnt)) != 0;
		ptCheckBox = new wxCheckBox(this, wxID_ANY, m_astrFixedExtensions.Item(sizCnt));
		ptCheckBox->SetValue(fIsChecked);
		m_ptSizer->Add(ptCheckBox);
	}
	if( m_fAllowCustomExtension==true )
	{
		ptTextCtrl = new wxTextCtrl(this, wxID_ANY, "", wxDefaultPosition, wxDefaultSize, wxBORDER_NONE|wxTE_LEFT);
		m_ptSizer->Add(ptTextCtrl);
	}

	ptBitmapButton = new wxBitmapButton(this, wxID_ANY, *m_ptBitmap);
	m_ptSizer->Add(ptBitmapButton);
}


void muhkuh_dirlistbox::updateButtons(int iSelection)
{
	size_t sizItemIdx;
	bool fPathSelected;
	bool fCanMoveUp;
	bool fCanMoveDown;


	if( iSelection!=wxNOT_FOUND )
	{
		sizItemIdx = (size_t)iSelection;
		fPathSelected = true;
		fCanMoveUp = (sizItemIdx>0);
		fCanMoveDown = (sizItemIdx>=0) && ((sizItemIdx+1)<m_ptSizer->GetEffectiveRowsCount());
	}
	else
	{
		/* The default is that no item is selected. */
		fPathSelected = false;
		fCanMoveUp = false;
		fCanMoveDown = false;
	}

	m_ptToolBar->EnableTool(wxID_DELETE, fPathSelected);
	m_ptToolBar->EnableTool(wxID_EDIT, fPathSelected);
	m_ptToolBar->EnableTool(wxID_UP, fCanMoveUp);
	m_ptToolBar->EnableTool(wxID_DOWN, fCanMoveDown);
}

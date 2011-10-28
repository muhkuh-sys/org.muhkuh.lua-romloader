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

#include "muhkuh_dirlistbox.h"
#include "muhkuh_icons.h"


BEGIN_EVENT_TABLE(muhkuh_dirlistbox, wxScrolledWindow)
	EVT_BUTTON(wxID_ANY,                                            muhkuh_dirlistbox::OnButtonBrowse)

	EVT_TOOL(wxID_ADD,     muhkuh_dirlistbox::OnButtonAdd)
	EVT_TOOL(wxID_DELETE,  muhkuh_dirlistbox::OnButtonRemove)
	EVT_TOOL(wxID_EDIT,    muhkuh_dirlistbox::OnButtonEdit)
	EVT_TOOL(wxID_UP,      muhkuh_dirlistbox::OnButtonUp)
	EVT_TOOL(wxID_DOWN,    muhkuh_dirlistbox::OnButtonDown)

	EVT_CHILD_FOCUS(muhkuh_dirlistbox::OnFocusChild)
END_EVENT_TABLE()


muhkuh_dirlistbox::muhkuh_dirlistbox(wxWindow *parent, wxWindowID id, const wxArrayString& astrPaths)
 : wxScrolledWindow(parent, id)
 , m_sizDirlistEntriesCnt(0)
 , m_sizDirlistEntriesMax(0)
 , m_patDirlistEntries(NULL)
 , m_ptBitmap(NULL)
 , m_iSelectedRow(wxNOT_FOUND)
{
	size_t sizNewSize;
	DIRLIST_ENTRY_T *ptArray;
	size_t sizCnt;
	size_t sizMax;
	wxBoxSizer *ptMainSizer;


	/* Allocate an initial array for the dirlist entries.
	 * The initial size is either 2 times the number of incoming paths or 16.
	 * The higher value is used.
	 */
	sizNewSize = astrPaths.GetCount() * 2;
	if( sizNewSize<16 )
	{
		sizNewSize = 16;
	}
	ptArray = (DIRLIST_ENTRY_T*)malloc(sizeof(DIRLIST_ENTRY_T)*sizNewSize);
	if( ptArray!=NULL )
	{
		m_patDirlistEntries = ptArray;
		m_sizDirlistEntriesMax = sizNewSize;
	}


	/* Create the main sizer. */
	ptMainSizer = new wxBoxSizer(wxVERTICAL);
	SetSizer(ptMainSizer);


	/* Create the flex grid sizer for the elements. */
	m_ptSizer = new wxFlexGridSizer(2, 2, 0);
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
	sizMax = astrPaths.GetCount();
	while(sizCnt<sizMax)
	{
		append_new_list_item(astrPaths.Item(sizCnt));
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

	if( m_patDirlistEntries!=NULL )
	{
		free(m_patDirlistEntries);
		m_patDirlistEntries = NULL;
	}
}


wxString muhkuh_dirlistbox::GetPaths(char cSeparator)
{
	bool fFirstEntry;
	DIRLIST_ENTRY_T *ptCnt;
	DIRLIST_ENTRY_T *ptEnd;
	wxString strPath;
	wxString strPathElement;


	ptCnt = m_patDirlistEntries;
	ptEnd = ptCnt + m_sizDirlistEntriesCnt;
	fFirstEntry = true;
	while(ptCnt<ptEnd)
	{
		strPathElement = ptCnt->ptTextCtrl->GetValue();
		if( strPathElement.IsEmpty()==false )
		{
			if( fFirstEntry==false )
			{
				strPath += cSeparator;
			}
			fFirstEntry = false;
			strPath += strPathElement;
		}
		++ptCnt;
	}

	return strPath;
}


void muhkuh_dirlistbox::OnButtonBrowse(wxCommandEvent &event)
{
	int iId;
	size_t sizIdx;
	wxString strPath;
	bool fIsUpdated;



	/* Is this an event from the text or button controls? */
	iId = event.GetId();
	if( iId>wxID_HIGHEST )
	{
		/* Get the index for the control. */
		iId -= wxID_HIGHEST;
		/* Is this a button control? */
		if( (iId&1)==1 )
		{
			sizIdx = iId / 2;

			/* Is this a valid list index? */
			if( sizIdx<m_sizDirlistEntriesCnt )
			{
				/* Get the text. */
				strPath = m_patDirlistEntries[sizIdx].ptTextCtrl->GetValue();
				fIsUpdated = select_path(strPath);
				if( fIsUpdated==true )
				{
					m_patDirlistEntries[sizIdx].ptTextCtrl->SetValue(strPath);
				}
			}
		}
	}
}


void muhkuh_dirlistbox::OnButtonAdd(wxCommandEvent &event)
{
	wxString strPath;
	bool fIsConfirmed;
	wxTextCtrl *ptTextCtrl;


	strPath = strLastUsedPath;
	fIsConfirmed = select_path(strPath);
	if( fIsConfirmed==true )
	{
		append_new_list_item(strPath);
		m_ptSizer->Layout();

		/* Set the focus to the new element. The new element goes always to the end of the list. */
		if( m_sizDirlistEntriesCnt!=0 )
		{
			ptTextCtrl = m_patDirlistEntries[m_sizDirlistEntriesCnt-1].ptTextCtrl;
			if( ptTextCtrl!=NULL )
			{
				ptTextCtrl->SetFocus();
			}
		}
	}
}


void muhkuh_dirlistbox::OnButtonRemove(wxCommandEvent &event)
{
	size_t sizIdx;
	size_t sizCnt;
	wxTextCtrl *ptTextCtrl;
	wxBitmapButton *ptBitmapButton;
	wxWindow *ptWindow;
	wxWindowID tIdText;
	wxWindowID tIdButton;


	/* Is the current selection a valid list index? */
	if( m_iSelectedRow!=wxNOT_FOUND && m_iSelectedRow>=0 )
	{
		sizIdx = (size_t)m_iSelectedRow;
		if( sizIdx<m_sizDirlistEntriesCnt )
		{
			ptTextCtrl = m_patDirlistEntries[sizIdx].ptTextCtrl;
			ptBitmapButton = m_patDirlistEntries[sizIdx].ptBitmapButton;

			/* Detach the items from the sizer. */
			ptWindow = (wxWindow*)ptTextCtrl;
			m_ptSizer->Detach(ptWindow);
			ptWindow = (wxWindow*)ptBitmapButton;
			m_ptSizer->Detach(ptWindow);

			/* Remove the items from the parent. */
			RemoveChild(ptTextCtrl);
			RemoveChild(ptBitmapButton);

			m_ptSizer->Layout();

			/* Delete the items. */
			ptTextCtrl->Destroy();
			ptBitmapButton->Destroy();

			/* Move the following table entries up and reindex them. */
			sizCnt = sizIdx;
			while(sizCnt+1<m_sizDirlistEntriesCnt)
			{
				/* Copy one element up. */
				ptTextCtrl = m_patDirlistEntries[sizCnt+1].ptTextCtrl;
				ptBitmapButton = m_patDirlistEntries[sizCnt+1].ptBitmapButton;
				m_patDirlistEntries[sizCnt].ptTextCtrl = ptTextCtrl;
				m_patDirlistEntries[sizCnt].ptBitmapButton = ptBitmapButton;

				/* Generate the IDs for the text and the button from the list index. */
				tIdText = wxID_HIGHEST + 2*sizCnt;
				tIdButton = wxID_HIGHEST + 2*sizCnt + 1;

				/* Set the new index. */
				ptTextCtrl->SetId(tIdText);
				ptBitmapButton->SetId(tIdButton);

				++sizCnt;
			}

			/* The list has not one element less. */
			--m_sizDirlistEntriesCnt;


			/* Try to find a new focus element. */

			/* Are still elements in the list? */
			if( m_sizDirlistEntriesCnt==0 )
			{
				/* No entry. Select the panel. */
				SetFocus();
			}
			else
			{
				/* Yes -> there will be a new focus. */

				/* Is the old index still valid? */
				if( sizIdx+1>=m_sizDirlistEntriesCnt )
				{
					/* No -> use the last index instead. */
					sizIdx = m_sizDirlistEntriesCnt - 1;
				}

				/* Get the text control at the new focus position. */
				ptTextCtrl = m_patDirlistEntries[m_sizDirlistEntriesCnt-1].ptTextCtrl;
				if( ptTextCtrl!=NULL )
				{
					/* Focus the text control. */
					ptTextCtrl->SetFocus();
				}
			}
		}
	}
}


void muhkuh_dirlistbox::OnButtonEdit(wxCommandEvent &event)
{
}


void muhkuh_dirlistbox::OnButtonUp(wxCommandEvent &event)
{
	size_t sizIdx;
	wxTextCtrl *ptTextCtrl0;
	wxTextCtrl *ptTextCtrl1;
	wxString strPath0;
	wxString strPath1;


	/* Is the current selection a valid list index? */
	if( m_iSelectedRow!=wxNOT_FOUND && m_iSelectedRow>=0 )
	{
		sizIdx = (size_t)m_iSelectedRow;
		if( sizIdx!=0 && sizIdx<m_sizDirlistEntriesCnt )
		{
			ptTextCtrl0 = m_patDirlistEntries[sizIdx-1].ptTextCtrl;
			ptTextCtrl1 = m_patDirlistEntries[sizIdx].ptTextCtrl;

			strPath0 = ptTextCtrl0->GetValue();
			strPath1 = ptTextCtrl1->GetValue();

			ptTextCtrl0->SetValue(strPath1);
			ptTextCtrl1->SetValue(strPath0);

			/* Move the focus one position up. */
			ptTextCtrl0->SetFocus();
		}
	}
}


void muhkuh_dirlistbox::OnButtonDown(wxCommandEvent &event)
{
	size_t sizIdx;
	wxTextCtrl *ptTextCtrl0;
	wxTextCtrl *ptTextCtrl1;
	wxString strPath0;
	wxString strPath1;


	/* Is the current selection a valid list index? */
	if( m_iSelectedRow!=wxNOT_FOUND && m_iSelectedRow>=0 )
	{
		sizIdx = (size_t)m_iSelectedRow;
		if( sizIdx+1<m_sizDirlistEntriesCnt )
		{
			ptTextCtrl0 = m_patDirlistEntries[sizIdx].ptTextCtrl;
			ptTextCtrl1 = m_patDirlistEntries[sizIdx+1].ptTextCtrl;

			strPath0 = ptTextCtrl0->GetValue();
			strPath1 = ptTextCtrl1->GetValue();

			ptTextCtrl0->SetValue(strPath1);
			ptTextCtrl1->SetValue(strPath0);

			/* Move the focus one position down. */
			ptTextCtrl1->SetFocus();
		}
	}
}


void muhkuh_dirlistbox::OnFocusChild(wxChildFocusEvent &event)
{
	wxWindow *ptWindow;
	int iSelectedRow;
	size_t sizCnt;
	size_t sizEnd;


	ptWindow = event.GetWindow();

	iSelectedRow = wxNOT_FOUND;

	/* Search window pointer in list. */
	sizCnt = 0;
	sizEnd = m_sizDirlistEntriesCnt;
	while(sizCnt<sizEnd)
	{
		if( (m_patDirlistEntries[sizCnt].ptTextCtrl==(wxTextCtrl*)ptWindow) || (m_patDirlistEntries[sizCnt].ptBitmapButton==(wxBitmapButton*)ptWindow) )
		{
			iSelectedRow = (int)sizCnt;
			break;
		}
		++sizCnt;
	}

	updateButtons(iSelectedRow);
	m_iSelectedRow = iSelectedRow;
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
		strPath = ptPathDialog->GetPath() + wxFileName::GetPathSeparator() + wxT("?.lua");
		strLastUsedPath = strPath;
		fPathUpdated = true;
	}
	ptPathDialog->Destroy();

	return fPathUpdated;
}


void muhkuh_dirlistbox::append_new_list_item(wxString strPath)
{
	size_t sizNewSize;
	DIRLIST_ENTRY_T *ptNewArray;
	wxTextCtrl *ptTextCtrl;
	wxBitmapButton *ptBitmapButton;
	wxWindowID tIdText;
	wxWindowID tIdButton;


	/* Is one more element in the array free? */
	if( m_sizDirlistEntriesCnt>=m_sizDirlistEntriesMax )
	{
		/* No -> try to allocate more. */
		sizNewSize = m_sizDirlistEntriesMax * 2;
		/* Detect a wrap around. */
		if( sizNewSize>m_sizDirlistEntriesMax )
		{
			ptNewArray = (DIRLIST_ENTRY_T*)realloc(m_patDirlistEntries, sizeof(DIRLIST_ENTRY_T)*sizNewSize);
			if( ptNewArray!=NULL )
			{
				/* Ok, the array is resized now. */
				m_patDirlistEntries = ptNewArray;
				m_sizDirlistEntriesMax = sizNewSize;
			}
		}
	}

	if( m_sizDirlistEntriesCnt<m_sizDirlistEntriesMax )
	{
		/* Generate the IDs for the text and the button from the list index. */
		tIdText = wxID_HIGHEST + 2*m_sizDirlistEntriesCnt;
		tIdButton = wxID_HIGHEST + 2*m_sizDirlistEntriesCnt + 1;

		/* Create the text and button controls. */
		ptTextCtrl = new wxTextCtrl(this, tIdText, strPath, wxDefaultPosition, wxDefaultSize, wxBORDER_NONE|wxTE_LEFT);
		ptBitmapButton = new wxBitmapButton(this, tIdButton, *m_ptBitmap);

		/* Add the text and button to the list. */
		m_patDirlistEntries[m_sizDirlistEntriesCnt].ptTextCtrl = ptTextCtrl;
		m_patDirlistEntries[m_sizDirlistEntriesCnt].ptBitmapButton = ptBitmapButton;

		/* Add the text and button to the sizer. */
		m_ptSizer->Add(ptTextCtrl, 0, wxEXPAND);
		m_ptSizer->Add(ptBitmapButton);

		++m_sizDirlistEntriesCnt;
	}
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
		fCanMoveDown = (sizItemIdx>=0) && ((sizItemIdx+1)<m_sizDirlistEntriesCnt);
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

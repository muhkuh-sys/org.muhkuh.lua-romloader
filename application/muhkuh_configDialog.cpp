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


#include "muhkuh_id.h"
#include "muhkuh_configDialog.h"
#include "muhkuh_config_reposEntryDialog.h"

#include "icons/famfamfam_silk_icons_v013/accept.xpm"
#include "icons/famfamfam_silk_icons_v013/bullet_green.xpm"
#include "icons/famfamfam_silk_icons_v013/bullet_yellow.xpm"
#include "icons/famfamfam_silk_icons_v013/cancel.xpm"
#include "icons/famfamfam_silk_icons_v013/database_add.xpm"
#include "icons/famfamfam_silk_icons_v013/database_delete.xpm"
#include "icons/famfamfam_silk_icons_v013/database_edit.xpm"
#include "icons/famfamfam_silk_icons_v013/exclamation.xpm"
#include "icons/famfamfam_silk_icons_v013/plugin_add.xpm"
#include "icons/famfamfam_silk_icons_v013/plugin_delete.xpm"
#include "icons/famfamfam_silk_icons_v013/plugin_disabled.xpm"
#include "icons/famfamfam_silk_icons_v013/plugin_go.xpm"


BEGIN_EVENT_TABLE(muhkuh_configDialog, wxDialog)
	EVT_TOOL(muhkuh_configDialog_AddRepository,			muhkuh_configDialog::OnNewRepositoryButton)
	EVT_TOOL(muhkuh_configDialog_EditRepository,			muhkuh_configDialog::OnEditRepositoryButton)
	EVT_TOOL(muhkuh_configDialog_RemoveRepository,			muhkuh_configDialog::OnDeleteRepositoryButton)
	EVT_LIST_ITEM_SELECTED(muhkuh_configDialog_RepositoryList,	muhkuh_configDialog::OnRepositorySelect)
	EVT_LIST_ITEM_DESELECTED(muhkuh_configDialog_RepositoryList,	muhkuh_configDialog::OnRepositoryDeselect)

	EVT_TOOL(muhkuh_configDialog_AddPlugin,				muhkuh_configDialog::OnAddPluginButton)
	EVT_TOOL(muhkuh_configDialog_RemovePlugin,			muhkuh_configDialog::OnRemovePluginButton)
	EVT_TOOL(muhkuh_configDialog_EnablePlugin,			muhkuh_configDialog::OnEnablePluginButton)
	EVT_TOOL(muhkuh_configDialog_DisablePlugin,			muhkuh_configDialog::OnDisablePluginButton)
	EVT_LIST_ITEM_SELECTED(muhkuh_configDialog_PluginList,		muhkuh_configDialog::OnPluginSelect)
	EVT_LIST_ITEM_DESELECTED(muhkuh_configDialog_PluginList,	muhkuh_configDialog::OnPluginDeselect)

	EVT_SIZE(muhkuh_configDialog::OnSize)
END_EVENT_TABLE()


muhkuh_configDialog::muhkuh_configDialog(wxWindow *parent, const wxString strApplicationPath, muhkuh_plugin_manager *ptPluginManager, muhkuh_repository_manager *ptRepositoryManager)
 : wxDialog(parent, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER)
 , m_ptRepositoryManager(ptRepositoryManager)
 , m_ptPluginManager(ptPluginManager)
{
	size_t sizCnt, sizIdx;


	m_strApplicationPath = strApplicationPath;

	// set the title
	SetTitle(_("Muhkuh Settings"));

	// create the controls
	createControls();

	// create the imagelist
	ptRepoImageList = muhkuh_repository::CreateNewImageList();
	// assign image list, NOTE: this makes the ListCtrl the owner, i.e. the image list will be deleted when the ListCtrl is deleted
	m_repositoryList->AssignImageList(ptRepoImageList, wxIMAGE_LIST_SMALL);

	// create imagelist for 2 images with 16x16 pixels
	ptPluginImageList = new wxImageList(16, 16, true, 2);
	ptPluginImageList->Add( wxIcon(icon_famfamfam_silk_bullet_green) );
	ptPluginImageList->Add( wxIcon(icon_famfamfam_silk_bullet_yellow) );
	ptPluginImageList->Add( wxIcon(icon_famfamfam_silk_exclamation) );
	m_pluginList->AssignImageList(ptPluginImageList, wxIMAGE_LIST_SMALL);

	// loop over all repositories and add them to the list
	sizIdx = 0;
	sizCnt = m_ptRepositoryManager->GetRepositoryCount();
	while(sizIdx<sizCnt)
	{
		ShowNewRepository(sizIdx);
		// next entry
		++sizIdx;
	}

	// loop over all plugins and add them to the list
	sizIdx = 0;
	sizCnt = m_ptPluginManager->getPluginCount();
	while(sizIdx<sizCnt)
	{
		ShowNewPlugin(sizIdx);
		++sizIdx;
	}
}


void muhkuh_configDialog::createControls(void)
{
	// init the controls

	// create the main sizer
	m_mainSizer = new wxBoxSizer(wxVERTICAL);
	SetSizer(m_mainSizer);

	// create the result box sizer
	m_repositoryListSizer = new wxStaticBoxSizer(wxVERTICAL, this, _("Repositories"));
	m_mainSizer->Add(m_repositoryListSizer, 1, wxEXPAND);

	m_pluginListSizer = new wxStaticBoxSizer(wxVERTICAL, this, _("Plugins"));
	m_mainSizer->Add(m_pluginListSizer, 1, wxEXPAND);

	// create the repository list
	m_repositoryList = new wxListCtrl(this, muhkuh_configDialog_RepositoryList, wxDefaultPosition, wxDefaultSize, wxLC_REPORT);
	m_repositoryList->InsertColumn(0, _("Repository"));
	m_repositoryListSizer->Add(m_repositoryList, 1, wxEXPAND);

	m_pluginList = new wxListCtrl(this, muhkuh_configDialog_PluginList, wxDefaultPosition, wxDefaultSize, wxLC_REPORT);
	m_pluginList->InsertColumn(0, _("Plugin"));
	m_pluginList->InsertColumn(1, _("Id"));
	m_pluginList->InsertColumn(2, _("Version"));
	m_pluginListSizer->Add(m_pluginList, 1, wxEXPAND);

	m_repositoryToolBar = new wxToolBar(this, wxID_ANY, wxDefaultPosition, wxDefaultSize,  wxTB_HORIZONTAL|wxNO_BORDER|wxTB_TEXT);
	m_repositoryToolBar->AddTool(muhkuh_configDialog_AddRepository, _("Add"), icon_famfamfam_silk_database_add, wxNullBitmap, wxITEM_NORMAL, _("Add Repository"), _("Add a new repository to the list"));
	m_repositoryToolBar->AddTool(muhkuh_configDialog_EditRepository, _("Edit"), icon_famfamfam_silk_database_edit, wxNullBitmap, wxITEM_NORMAL, _("Edit Repository"), _("Edit the settings of the selected repository"));
	m_repositoryToolBar->EnableTool(muhkuh_configDialog_EditRepository, false);
	m_repositoryToolBar->AddTool(muhkuh_configDialog_RemoveRepository, _("Remove"), icon_famfamfam_silk_database_delete, wxNullBitmap, wxITEM_NORMAL, _("Remove Repository"), _("Remove the selected repository from the list"));
	m_repositoryToolBar->EnableTool(muhkuh_configDialog_RemoveRepository, false);
	m_repositoryListSizer->Add(m_repositoryToolBar, 0, wxEXPAND);

	m_pluginToolBar = new wxToolBar(this, wxID_ANY, wxDefaultPosition, wxDefaultSize,  wxTB_HORIZONTAL|wxNO_BORDER|wxTB_TEXT);
	m_pluginToolBar->AddTool(muhkuh_configDialog_AddPlugin, _("Add"), icon_famfamfam_silk_plugin_add, wxNullBitmap, wxITEM_NORMAL, _("Add Plugin"), _("Add a new plugin to the list"));
	m_pluginToolBar->AddTool(muhkuh_configDialog_RemovePlugin, _("Remove"), icon_famfamfam_silk_plugin_delete, wxNullBitmap, wxITEM_NORMAL, _("Remove Plugin"), _("Remove the selected plugin from the list"));
	m_pluginToolBar->EnableTool(muhkuh_configDialog_RemovePlugin, false);
	m_pluginToolBar->AddTool(muhkuh_configDialog_EnablePlugin, _("Enable"), icon_famfamfam_silk_plugin_go, wxNullBitmap, wxITEM_NORMAL, _("Enable Plugin"), _("Enable the selected plugin"));
	m_pluginToolBar->EnableTool(muhkuh_configDialog_EnablePlugin, false);
	m_pluginToolBar->AddTool(muhkuh_configDialog_DisablePlugin, _("Disable"), icon_famfamfam_silk_plugin_disabled, wxNullBitmap, wxITEM_NORMAL, _("Disable Plugin"), _("Disable the selected plugin"));
	m_pluginToolBar->EnableTool(muhkuh_configDialog_DisablePlugin, false);
	m_pluginListSizer->Add(m_pluginToolBar, 0, wxEXPAND);

	m_buttonSizer = new wxBoxSizer(wxHORIZONTAL);
	m_mainSizer->Add(m_buttonSizer, 0, wxEXPAND);
	m_buttonOk = new wxButton(this, wxID_OK);
	m_buttonCancel = new wxButton(this, wxID_CANCEL);
	m_buttonSizer->AddStretchSpacer(1);
	m_buttonSizer->Add(m_buttonOk);
	m_buttonSizer->AddStretchSpacer(1);
	m_buttonSizer->Add(m_buttonCancel);
	m_buttonSizer->AddStretchSpacer(1);

	m_mainSizer->SetSizeHints(this);
}


void muhkuh_configDialog::OnNewRepositoryButton(wxCommandEvent &WXUNUSED(event))
{
	muhkuh_config_reposEntryDialog *ptEntryDialog;
	muhkuh_repository *ptRepos;
	long lIdx;


	ptRepos = new muhkuh_repository(_("new repository"));
	ptEntryDialog = new muhkuh_config_reposEntryDialog(this, m_strApplicationPath, ptRepos);
	if( ptEntryDialog->ShowModal()==wxID_OK )
	{
		// add to list
		lIdx = m_ptRepositoryManager->addRepository(ptRepos);
		// show
		ShowNewRepository(lIdx);
	}
	else
	{
		delete ptRepos;
	}
	ptEntryDialog->Destroy();
}


void muhkuh_configDialog::OnEditRepositoryButton(wxCommandEvent &WXUNUSED(event))
{
	long lIdx;
	muhkuh_config_reposEntryDialog *ptEntryDialog;
	muhkuh_repository *ptRepos;


	// get the focussed item in the repository list
	lIdx = m_repositoryList->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_FOCUSED);
	if( lIdx>=0 )
	{
		ptRepos = m_ptRepositoryManager->GetRepository(lIdx);

		// clone the entry
		ptEntryDialog = new muhkuh_config_reposEntryDialog(this, m_strApplicationPath, ptRepos);
		if( ptEntryDialog->ShowModal()==wxID_OK )
		{
			// update the listctrl
			ShowUpdatedRepositoryEntry(lIdx, ptRepos);
		}
		ptEntryDialog->Destroy();
	}
}


void muhkuh_configDialog::OnDeleteRepositoryButton(wxCommandEvent &WXUNUSED(event))
{
	int iSelectedEntries;
	long lIdx;
	long *plIdx;
	int iCnt;


	// get the number of selected files
	iSelectedEntries = m_repositoryList->GetSelectedItemCount();
	// it at lease one item selected
	if( iSelectedEntries>0 )
	{
		plIdx = new long[iSelectedEntries];
		iCnt = 0;
		// loop over all selected repositories and add them to the requester
		lIdx = -1;
		do
		{
			lIdx = m_repositoryList->GetNextItem(lIdx, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
			if( lIdx>=0 )
			{
				// add index and name to the lists
				plIdx[iCnt] = lIdx;
				++iCnt;
			}
		} while( lIdx>=0 && iCnt<iSelectedEntries );

		// TODO: show a requester with all selected repositories and ask the user for confirmation

		// delete all the files
		while(iCnt>0)
		{
			lIdx = plIdx[--iCnt];
			// erase from the listctrl
			m_repositoryList->DeleteItem(lIdx);
			// erase from the vector
			m_ptRepositoryManager->removeRepository(lIdx);
		}
		// free the index list
		delete[] plIdx;
	}
}


void muhkuh_configDialog::OnRepositorySelect(wxListEvent &event)
{
	SetRepositoryButtons(event.GetIndex());
}


void muhkuh_configDialog::OnRepositoryDeselect(wxListEvent &event)
{
	SetRepositoryButtons(-1);
}


void muhkuh_configDialog::SetRepositoryButtons(long lIdx)
{
	bool fPluginSelected;


	fPluginSelected = ( lIdx>=0 );
	m_repositoryToolBar->EnableTool(muhkuh_configDialog_EditRepository,	fPluginSelected);
	m_repositoryToolBar->EnableTool(muhkuh_configDialog_RemoveRepository,	fPluginSelected);
}


void muhkuh_configDialog::OnAddPluginButton(wxCommandEvent &WXUNUSED(event))
{
	wxFileDialog *pluginDialog;
	wxFileName fileName;
	wxString strPluginName;
	wxString strDialogInitPath;
	long lIdx;


	strDialogInitPath = wxEmptyString;
/*
	if( m_fUseRelativePaths==true )
	{
		fileName.Assign(strDialogInitPath);
		if(fileName.Normalize(wxPATH_NORM_ALL, m_strApplicationPath ,wxPATH_NATIVE))
		{
			strDialogInitPath = fileName.GetFullPath();
		}
	}
*/
	pluginDialog = new wxFileDialog(this, _("Select the new plugin"), strDialogInitPath, wxEmptyString, wxT("*.xml"), wxFD_OPEN|wxFD_FILE_MUST_EXIST);

	if( pluginDialog->ShowModal()==wxID_OK )
	{
		strPluginName = pluginDialog->GetPath();
		wxLogMessage(_("open plugin '%s'"), strPluginName.fn_str());
		lIdx = m_ptPluginManager->addPlugin(strPluginName);
		if( lIdx>=0 )
		{
			ShowNewPlugin(lIdx);
		}
	}
	pluginDialog->Destroy();
}


void muhkuh_configDialog::OnRemovePluginButton(wxCommandEvent &WXUNUSED(event))
{
	int iSelectedEntries;
	long lIdx;
	long *plIdx;
	int iCnt;


	// get the number of selected files
	iSelectedEntries = m_pluginList->GetSelectedItemCount();
	// it at lease one item selected
	if( iSelectedEntries>0 )
	{
		plIdx = new long[iSelectedEntries];
		iCnt = 0;
		// loop over all selected plugins and add them to the requester
		lIdx = -1;
		do
		{
			lIdx = m_pluginList->GetNextItem(lIdx, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
			if( lIdx>=0 )
			{
				// add index and name to the lists
				plIdx[iCnt] = lIdx;
				++iCnt;
			}
		} while( lIdx>=0 && iCnt<iSelectedEntries );

		// TODO: show a requester with all selected plugins and ask the user for confirmation

		// delete all the files
		while(iCnt>0)
		{
			lIdx = plIdx[--iCnt];
			// erase from the listctrl
			m_pluginList->DeleteItem(lIdx);
			// erase from the manager
			m_ptPluginManager->removePlugin(lIdx);
		}
		// free the index list
		delete[] plIdx;
	}
}


void muhkuh_configDialog::OnEnablePluginButton(wxCommandEvent &WXUNUSED(event))
{
	long lIdx;


	// get the focussed item in the plugin list
	lIdx = m_pluginList->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_FOCUSED);
	if( lIdx>=0 )
	{
		m_ptPluginManager->SetEnable(lIdx, true);
		ShowPluginImage(lIdx);
	}
}


void muhkuh_configDialog::OnDisablePluginButton(wxCommandEvent &WXUNUSED(event))
{
	long lIdx;


	// get the focussed item in the plugin list
	lIdx = m_pluginList->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_FOCUSED);
	if( lIdx>=0 )
	{
		m_ptPluginManager->SetEnable(lIdx, false);
		ShowPluginImage(lIdx);
	}
}


void muhkuh_configDialog::OnPluginSelect(wxListEvent &event)
{
	SetPluginButtons(event.GetIndex());
}


void muhkuh_configDialog::OnPluginDeselect(wxListEvent &event)
{
	SetPluginButtons(-1);
}


void muhkuh_configDialog::SetPluginButtons(long lIdx)
{
	bool fPluginSelected;


	fPluginSelected = ( lIdx>=0 );
	m_pluginToolBar->EnableTool(muhkuh_configDialog_RemovePlugin,	fPluginSelected);
	m_pluginToolBar->EnableTool(muhkuh_configDialog_EnablePlugin,	fPluginSelected);
	m_pluginToolBar->EnableTool(muhkuh_configDialog_DisablePlugin,	fPluginSelected);
}


void muhkuh_configDialog::OnSize(wxSizeEvent &event)
{
	int iWidth;


	iWidth = m_repositoryList->GetClientSize().GetWidth();
	m_repositoryList->SetColumnWidth(0, iWidth);

	// continue processing the event
	event.Skip(true);
}


void muhkuh_configDialog::ShowNewRepository(long lIdx)
{
	wxListItem listItem;


	listItem.Clear();
	listItem.SetId(lIdx);
	listItem.SetImage( m_ptRepositoryManager->GetImageListIndex(lIdx) );
	listItem.SetText( m_ptRepositoryManager->GetStringRepresentation(lIdx) );
	// add the entry to the list
	m_repositoryList->InsertItem(listItem);
}


void muhkuh_configDialog::ShowNewPlugin(long lIdx)
{
	wxListItem listItem;
	const muhkuh_plugin_desc *ptPluginDesc;
	wxString strName;
	long lItemIdx;


	/* get the plugin description */
	ptPluginDesc = m_ptPluginManager->getPluginDescription(lIdx);
	if( ptPluginDesc==NULL )
	{
		wxLogError(_("failed to get plugin description!"));
	}
	else
	{
		strName = m_ptPluginManager->GetConfigName(lIdx);
		listItem.Clear();
		listItem.SetId(lIdx);
		listItem.SetText( strName );
		// add the entry to the list
		lItemIdx = m_pluginList->InsertItem(listItem);

		ShowPluginImage(lIdx);

		strName = ptPluginDesc->strPluginId;
		m_pluginList->SetItem(lItemIdx, 1, strName);

		strName.Printf(wxT("%d.%d.%d"), ptPluginDesc->tVersion.uiVersionMajor, ptPluginDesc->tVersion.uiVersionMinor, ptPluginDesc->tVersion.uiVersionSub);
		m_pluginList->SetItem(lItemIdx, 2, strName);
	}
}


void muhkuh_configDialog::ShowPluginImage(long lIdx)
{
	int iImageIdx;


	if( m_ptPluginManager->GetEnable(lIdx)==false )
	{
		iImageIdx = 0;
	}
	else
	{
		iImageIdx = 1;
	}
	m_pluginList->SetItemImage(lIdx, iImageIdx);
}


void muhkuh_configDialog::ShowUpdatedRepositoryEntry(long lIdx, muhkuh_repository *ptRepos)
{
	wxListItem listItem;


	listItem.Clear();
	listItem.SetId(lIdx);
	listItem.SetImage( ptRepos->GetImageListIndex() );
	listItem.SetText( ptRepos->GetStringRepresentation() );
	// replace the old listentry
	m_repositoryList->SetItem(listItem);
}


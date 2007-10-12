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
#include <wx/artprov.h>
#include <wx/filename.h>
#include <wx/imaglist.h>
#include <wx/listctrl.h>
#include <wx/textctrl.h>

#ifndef __MUHKUH_CONFIGDIALOG_H__
#define __MUHKUH_CONFIGDIALOG_H__

#include "muhkuh_plugin_manager.h"
#include "muhkuh_repository_manager.h"


class muhkuh_configDialog : public wxDialog
{
public:
	muhkuh_configDialog(wxWindow *parent, const wxString strApplicationPath, muhkuh_plugin_manager *ptPluginManager, muhkuh_repository_manager *ptRepositoryManager);

	void OnNewRepositoryButton(wxCommandEvent &event);
	void OnEditRepositoryButton(wxCommandEvent &event);
	void OnDeleteRepositoryButton(wxCommandEvent &event);

	void OnAddPluginButton(wxCommandEvent &event);
	void OnRemovePluginButton(wxCommandEvent &event);
	void OnEnablePluginButton(wxCommandEvent &event);
	void OnDisablePluginButton(wxCommandEvent &event);

	void OnSize(wxSizeEvent &event);

private:
	void createControls(void);

	void ShowNewRepository(long lIdx);
	void ShowNewPlugin(long lIdx);

	void ShowPluginImage(long lIdx);
	void ShowUpdatedRepositoryEntry(long lIdx, muhkuh_repository *ptRepos);


	muhkuh_repository_manager *m_ptRepositoryManager;
	muhkuh_plugin_manager *m_ptPluginManager;

	wxString m_strApplicationPath;

	// the controls
	wxBoxSizer *m_mainSizer;
	wxStaticBoxSizer *m_repositoryListSizer;
	wxStaticBoxSizer *m_pluginListSizer;
	wxListCtrl *m_repositoryList;
	wxListCtrl *m_pluginList;
	wxBoxSizer *m_repositoryButtonSizer;
	wxBoxSizer *m_pluginButtonSizer;
	wxBoxSizer *m_buttonSizer;
	wxButton *m_buttonNewRepository;
	wxButton *m_buttonEditRepository;
	wxButton *m_buttonDeleteRepository;
	wxButton *m_buttonAddPlugin;
	wxButton *m_buttonDeletePlugin;
	wxButton *m_buttonEnablePlugin;
	wxButton *m_buttonDisablePlugin;
	wxButton *m_buttonOk;
	wxButton *m_buttonCancel;

	// image list for the repository list
	wxImageList *ptRepoImageList;
	wxImageList *ptPluginImageList;

    DECLARE_EVENT_TABLE()
};




#endif	// __MUHKUH_CONFIGDIALOG_H__

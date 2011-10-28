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
#include <wx/filename.h>
#include <wx/imaglist.h>
#include <wx/textctrl.h>
#include <wx/treebook.h>
#include <wx/treectrl.h>


#ifndef __MUHKUH_CONFIGDIALOG_H__
#define __MUHKUH_CONFIGDIALOG_H__

#include "muhkuh_config_data.h"
#include "muhkuh_dirlistbox.h"


class treeItemIdData : public wxTreeItemData
{
public:
	treeItemIdData(long lId)
	 : m_lId(lId)
	 { };

	long m_lId;
};


class muhkuh_configDialog : public wxDialog
{
public:
	muhkuh_configDialog(wxWindow *parent, const wxString strApplicationPath, muhkuh_config_data *ptConfigData);
	~muhkuh_configDialog(void);

	void OnRadioWelcomeBuiltIn(wxCommandEvent &event);
	void OnRadioWelcomeFile(wxCommandEvent &event);
	void OnRadioDetailsBuiltIn(wxCommandEvent &event);
	void OnRadioDetailsFile(wxCommandEvent &event);

	void OnBrowseStartPageButton(wxCommandEvent &event);
	void OnBrowseDetailPageButton(wxCommandEvent &event);

	void OnNewRepositoryButton(wxCommandEvent &event);
	void OnEditRepositoryButton(wxCommandEvent &event);
	void OnDeleteRepositoryButton(wxCommandEvent &event);
	void OnRepositorySelect(wxTreeEvent &event);
	void OnRepositoryKey(wxTreeEvent &event);

	void OnAddPluginButton(wxCommandEvent &event);
	void OnRemovePluginButton(wxCommandEvent &event);
	void OnEnablePluginButton(wxCommandEvent &event);
	void OnDisablePluginButton(wxCommandEvent &event);
	void OnPluginSelectionChanged(wxTreeEvent &event);
	void OnPluginSelectionChanging(wxTreeEvent &event);
	void OnPluginKey(wxTreeEvent &event);

	void OnButtonOk(wxCommandEvent &event);
private:
	void createControls(wxArrayString &astrLuaIncludePath);
	wxPanel *createControls_application(wxWindow *ptParent);
	wxPanel *createControls_repository(wxWindow *ptParent);
	wxPanel *createControls_plugin(wxWindow *ptParent);
	wxPanel *createControls_lua(wxWindow *ptParent, wxArrayString &astrLuaPaths);

	void ShowNewRepository(long lIdx);
	void ShowNewPlugin(long lIdx);

	void ShowPluginImage(wxTreeItemId tPluginItem);

	void SetPluginButtons(wxTreeItemId tItem);

	void repository_add(void);
	void repository_delete(void);

	void plugin_add(void);
	void plugin_delete(void);
	void plugin_enable(bool fEnablePlugin);

	int get_imagelist_index(muhkuh_repository::REPOSITORY_TYP_E eTyp);


	muhkuh_config_data *m_ptConfigData;
	wxString m_strApplicationPath;

	// the controls
	wxTreebook *m_treeBook;
	wxRadioButton *m_ptRadioBuiltinWelcome;
	wxRadioButton *m_ptRadioFileWelcome;
	wxRadioButton *m_ptRadioBuiltinDetails;
	wxRadioButton *m_ptRadioFileDetails;
	wxTextCtrl *m_ptTextStartPage;
	wxTextCtrl *m_ptTextDetailsPage;
	wxBitmapButton *m_ptButtonStartPage;
	wxBitmapButton *m_ptButtonDetailsPage;
	wxTreeCtrl *m_repositoryTree;
	wxTreeCtrl *m_pluginTree;
	wxBoxSizer *m_buttonSizer;
	wxToolBar *m_repositoryToolBar;
	wxToolBar *m_pluginToolBar;
	wxTextCtrl *m_ptStartupCodeText;
	muhkuh_dirlistbox *m_ptPathListBox;
    DECLARE_EVENT_TABLE()
};


#endif  /* __MUHKUH_CONFIGDIALOG_H__ */

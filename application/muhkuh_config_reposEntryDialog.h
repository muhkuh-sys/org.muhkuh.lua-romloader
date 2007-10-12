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
#include <wx/radiobut.h>
#include <wx/textctrl.h>

#ifndef __MUHKUH_CONFIG_REPOSENTRYDIALOG_H__
#define __MUHKUH_CONFIG_REPOSENTRYDIALOG_H__

#include "muhkuh_repository.h"


class muhkuh_config_reposEntryDialog : public wxDialog
{
public:
	muhkuh_config_reposEntryDialog(wxWindow *parent, const wxString sApplicationPath, muhkuh_repository *ptRepos);

	void OnOkButton(wxCommandEvent &event);
	void OnBrowseDirscanLocationButton(wxCommandEvent &event);
	void OnUseRelPaths(wxCommandEvent &event);
	void OnRadioDirscan(wxCommandEvent &event);
	void OnRadioFilelist(wxCommandEvent &event);

private:
	void createControls(void);
	bool adaptDirscanLocation(wxString sFileName);
	void SwitchInputs(void);

	muhkuh_repository *m_ptRepos;
	muhkuh_repository::REPOSITORY_TYP_E m_eTyp;
	wxString m_strApplicationPath;

	bool m_fUseRelativePaths;

	// the controls
	wxBoxSizer *m_mainSizer;
	wxRadioButton *m_radioDirscan;
	wxRadioButton *m_radioFilelist;

	wxFlexGridSizer *m_repoNameGrid;
	wxStaticText *m_labelRepoName;
	wxTextCtrl *m_textRepoName;

	wxFlexGridSizer *m_dirscanGrid;
	wxStaticText *m_labelDirscanLocation;
	wxTextCtrl *m_textDirscanLocation;
	wxBitmapButton *m_buttonDirscanLocation;
	wxStaticText *m_labelTestExtension;
	wxTextCtrl *m_textTestExtension;
	wxStaticText *m_labelAppPath;
	wxTextCtrl *m_textAppPath;
	wxCheckBox *m_checkUseRelativePaths;

	wxFlexGridSizer *m_filelistGrid;
	wxStaticText *m_labelFilelistLocation;
	wxTextCtrl *m_textFilelistLocation;

	wxBoxSizer *m_buttonSizer;
	wxButton *m_buttonOk;
	wxButton *m_buttonCancel;

    DECLARE_EVENT_TABLE()
};


#endif	/* __MUHKUH_CONFIG_REPOSENTRYDIALOG_H__ */


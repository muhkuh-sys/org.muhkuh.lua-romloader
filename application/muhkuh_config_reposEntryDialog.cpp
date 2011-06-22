/***************************************************************************
 *   Copyright (C) 2007-2008 by Christoph Thelen                           *
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
#include "muhkuh_config_reposEntryDialog.h"

#include "muhkuh_icons.h"


BEGIN_EVENT_TABLE(muhkuh_config_reposEntryDialog, wxDialog)
	EVT_BUTTON(wxID_OK,                                                     muhkuh_config_reposEntryDialog::OnOkButton)

	EVT_BUTTON(muhkuh_configReposEntryDialog_ButtonDirscanLocationBrowse,   muhkuh_config_reposEntryDialog::OnBrowseDirscanLocationButton)
	EVT_BUTTON(muhkuh_configReposEntryDialog_ButtonSingleXmlLocationBrowse, muhkuh_config_reposEntryDialog::OnBrowseSingleXmlLocationButton)
	EVT_BUTTON(muhkuh_configReposEntryDialog_ButtonAllLocalLocationBrowse,  muhkuh_config_reposEntryDialog::OnBrowseAllLocalLocationButton)

	EVT_CHECKBOX(muhkuh_configReposEntryDialog_CheckboxDirscanRelPaths,     muhkuh_config_reposEntryDialog::OnDirscanUseRelPaths)
	EVT_CHECKBOX(muhkuh_configReposEntryDialog_CheckboxSingleXmlRelPaths,   muhkuh_config_reposEntryDialog::OnSingleXmlUseRelPaths)
	EVT_CHECKBOX(muhkuh_configReposEntryDialog_CheckboxAllLocalRelPaths,    muhkuh_config_reposEntryDialog::OnAllLocalUseRelPaths)

	EVT_RADIOBUTTON(muhkuh_configReposEntryDialog_radioDirscan,             muhkuh_config_reposEntryDialog::OnRadioDirscan)
	EVT_RADIOBUTTON(muhkuh_configReposEntryDialog_radioFilelist,            muhkuh_config_reposEntryDialog::OnRadioFilelist)
	EVT_RADIOBUTTON(muhkuh_configReposEntryDialog_radioSingleXml,           muhkuh_config_reposEntryDialog::OnRadioSingleXml)
	EVT_RADIOBUTTON(muhkuh_configReposEntryDialog_radioAllLocal,            muhkuh_config_reposEntryDialog::OnRadioAllLocal)
END_EVENT_TABLE()


muhkuh_config_reposEntryDialog::muhkuh_config_reposEntryDialog(wxWindow *parent, const wxString sApplicationPath, muhkuh_repository *ptRepos)
 : wxDialog(parent, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER)
 , m_ptRepos(ptRepos)
{
	wxFileName tFileName;
	wxString strLocation;


	m_strApplicationPath = sApplicationPath;

	// create the controls
	createControls();

	strLocation = m_ptRepos->GetLocation();
	tFileName.Assign(strLocation);

	m_textRepoName->SetValue( m_ptRepos->GetName() );


	m_textDirscanLocation->SetValue( strLocation );
	m_textFilelistLocation->SetValue( strLocation );
	m_textSingleXmlLocation->SetValue( strLocation );
	m_textAllLocalLocation->SetValue( strLocation );

	m_textTestExtension->SetValue( m_ptRepos->GetExtension() );
	m_textAllLocalTestExtension->SetValue( m_ptRepos->GetExtension() );

	m_textAllLocalXmlPattern->SetValue( m_ptRepos->GetXmlPattern() );

	m_eTyp = m_ptRepos->GetTyp();
	switch( m_eTyp )
	{
	case muhkuh_repository::REPOSITORY_TYP_DIRSCAN:
		m_radioDirscan->SetValue(true);
		m_radioFilelist->SetValue(false);
		m_radioSingleXml->SetValue(false);
		m_radioAllLocal->SetValue(false);
		m_fDirscanUseRelativePaths = tFileName.IsRelative(wxPATH_NATIVE);
		m_fSingleXmlUseRelativePaths = false;
		m_fAllLocalUseRelativePaths = false;
		break;

	case muhkuh_repository::REPOSITORY_TYP_FILELIST:
		m_radioDirscan->SetValue(false);
		m_radioFilelist->SetValue(true);
		m_radioSingleXml->SetValue(false);
		m_radioAllLocal->SetValue(false);
		m_fDirscanUseRelativePaths = false;
		m_fSingleXmlUseRelativePaths = false;
		m_fAllLocalUseRelativePaths = false;
		break;

	case muhkuh_repository::REPOSITORY_TYP_SINGLEXML:
		m_radioDirscan->SetValue(false);
		m_radioFilelist->SetValue(false);
		m_radioSingleXml->SetValue(true);
		m_radioAllLocal->SetValue(false);
		m_fDirscanUseRelativePaths = false;
		m_fSingleXmlUseRelativePaths = tFileName.IsRelative(wxPATH_NATIVE);
		m_fAllLocalUseRelativePaths = false;
		break;

	case muhkuh_repository::REPOSITORY_TYP_ALLLOCAL:
		m_radioDirscan->SetValue(false);
		m_radioFilelist->SetValue(false);
		m_radioSingleXml->SetValue(false);
		m_radioAllLocal->SetValue(true);
		m_fDirscanUseRelativePaths = false;
		m_fSingleXmlUseRelativePaths = false;
		m_fAllLocalUseRelativePaths = tFileName.IsRelative(wxPATH_NATIVE);
		break;

	default:
		m_eTyp = muhkuh_repository::REPOSITORY_TYP_DIRSCAN;
		m_radioDirscan->SetValue(true);
		m_radioFilelist->SetValue(false);
		m_radioSingleXml->SetValue(false);
		m_radioAllLocal->SetValue(false);
		m_textTestExtension->SetValue(wxT("*.mtd"));
		m_fDirscanUseRelativePaths = false;
		m_fSingleXmlUseRelativePaths = false;
		m_fAllLocalUseRelativePaths = false;
	};
	SwitchInputs();

	// set the checkbox, this must be after the filenames
	m_checkDirscanUseRelativePaths->SetValue(m_fDirscanUseRelativePaths);
	m_checkSingleXmlUseRelativePaths->SetValue(m_fSingleXmlUseRelativePaths);
	m_checkAllLocalUseRelativePaths->SetValue(m_fAllLocalUseRelativePaths);
}


void muhkuh_config_reposEntryDialog::createControls(void)
{
	wxSize minSize, maxSize;


	// init the controls

	// create the main sizer
	m_mainSizer = new wxBoxSizer(wxVERTICAL);
	SetSizer(m_mainSizer);

	m_repoNameGrid = new wxFlexGridSizer(1, 3, 0, 0);
	m_repoNameGrid->AddGrowableCol(2, 1);
	m_mainSizer->Add(m_repoNameGrid, 0, wxEXPAND);
	m_labelRepoName = new wxStaticText(this, wxID_ANY, _("Entry name :"));
	m_textRepoName = new wxTextCtrl(this, wxID_ANY);
	m_repoNameGrid->Add(m_labelRepoName, 0, wxALIGN_CENTER_VERTICAL);
	m_repoNameGrid->AddSpacer(4);
	m_repoNameGrid->Add(m_textRepoName, 0, wxEXPAND);

	// create the dirscan radio button
	m_radioDirscan = new wxRadioButton(this, muhkuh_configReposEntryDialog_radioDirscan, _("Directory Scan"), wxDefaultPosition, wxDefaultSize, wxRB_GROUP);
	m_mainSizer->Add(m_radioDirscan, 0, 0);

	// create the input grid
	m_dirscanGrid = new wxFlexGridSizer(4, 4, 0, 0);
	m_dirscanGrid->AddGrowableCol(2, 1);
	m_mainSizer->Add(m_dirscanGrid, 0, wxEXPAND|wxLEFT, 32);

	// fill the input grid
	m_labelDirscanLocation = new wxStaticText(this, wxID_ANY, _("Path to test descriptions :"));
	m_textDirscanLocation = new wxTextCtrl(this, wxID_ANY);
	m_buttonDirscanLocation = new wxBitmapButton(this, muhkuh_configReposEntryDialog_ButtonDirscanLocationBrowse, icon_famfamfam_silk_folder);
	m_dirscanGrid->Add(m_labelDirscanLocation, 0, wxALIGN_CENTER_VERTICAL);
	m_dirscanGrid->AddSpacer(4);
	m_dirscanGrid->Add(m_textDirscanLocation, 0, wxEXPAND);
	m_dirscanGrid->Add(m_buttonDirscanLocation);

	m_labelAppPath = new wxStaticText(this, wxID_ANY, _("Application Path :"));
	m_textAppPath = new wxTextCtrl(this, wxID_ANY, m_strApplicationPath, wxDefaultPosition, wxDefaultSize, wxTE_READONLY);
	m_textAppPath->SetBackgroundColour(GetBackgroundColour());
	m_dirscanGrid->Add(m_labelAppPath, 0, wxALIGN_CENTER_VERTICAL);
	m_dirscanGrid->AddSpacer(4);
	m_dirscanGrid->Add(m_textAppPath, 0, wxEXPAND);
	m_dirscanGrid->AddSpacer(4);

	m_labelTestExtension = new wxStaticText(this, wxID_ANY, _("Test description extension :"));
	m_textTestExtension = new wxTextCtrl(this, wxID_ANY);
	m_dirscanGrid->Add(m_labelTestExtension, 0, wxALIGN_CENTER_VERTICAL);
	m_dirscanGrid->AddSpacer(4);
	m_dirscanGrid->Add(m_textTestExtension, 0, wxEXPAND);
	m_dirscanGrid->AddSpacer(4);

	m_labelDirscanUseRelativePaths = new wxStaticText(this, wxID_ANY, _("Use relative paths :"));
	m_checkDirscanUseRelativePaths = new wxCheckBox(this, muhkuh_configReposEntryDialog_CheckboxDirscanRelPaths, wxT(""), wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT);
	m_dirscanGrid->Add(m_labelDirscanUseRelativePaths);
	m_dirscanGrid->AddSpacer(4);
	m_dirscanGrid->Add(m_checkDirscanUseRelativePaths, 0, wxEXPAND);
	m_dirscanGrid->AddSpacer(4);


	m_radioFilelist = new wxRadioButton(this, muhkuh_configReposEntryDialog_radioFilelist, _("Filelist"), wxDefaultPosition, wxDefaultSize, 0);
	m_mainSizer->Add(m_radioFilelist, 0, 0);

	m_filelistGrid = new wxFlexGridSizer(1, 3, 0, 0);
	m_filelistGrid->AddGrowableCol(2, 1);
	m_mainSizer->Add(m_filelistGrid, 0, wxEXPAND|wxLEFT, 32);

	// fill the input grid
	m_labelFilelistLocation = new wxStaticText(this, wxID_ANY, _("URL of the filelist :"));
	m_textFilelistLocation = new wxTextCtrl(this, wxID_ANY);
	m_filelistGrid->Add(m_labelFilelistLocation, 0, wxALIGN_CENTER_VERTICAL);
	m_filelistGrid->AddSpacer(4);
	m_filelistGrid->Add(m_textFilelistLocation, 0, wxEXPAND);


	m_radioSingleXml = new wxRadioButton(this, muhkuh_configReposEntryDialog_radioSingleXml, _("Single Xml"), wxDefaultPosition, wxDefaultSize, 0);
	m_mainSizer->Add(m_radioSingleXml, 0, 0);

	m_singleXmlGrid = new wxFlexGridSizer(3, 4, 0, 0);
	m_singleXmlGrid->AddGrowableCol(2, 1);
	m_mainSizer->Add(m_singleXmlGrid, 0, wxEXPAND|wxLEFT, 32);
	m_buttonSingleXmlLocation = new wxBitmapButton(this, muhkuh_configReposEntryDialog_ButtonSingleXmlLocationBrowse, icon_famfamfam_silk_folder);

	// fill the input grid
	m_labelSingleXmlLocation = new wxStaticText(this, wxID_ANY, _("Path to the xml test description :"));
	m_textSingleXmlLocation = new wxTextCtrl(this, wxID_ANY);
	m_singleXmlGrid->Add(m_labelSingleXmlLocation, 0, wxALIGN_CENTER_VERTICAL);
	m_singleXmlGrid->AddSpacer(4);
	m_singleXmlGrid->Add(m_textSingleXmlLocation, 0, wxEXPAND);
	m_singleXmlGrid->Add(m_buttonSingleXmlLocation);

	m_labelSingleXmlAppPath = new wxStaticText(this, wxID_ANY, _("Application Path :"));
	m_textSingleXmlAppPath = new wxTextCtrl(this, wxID_ANY, m_strApplicationPath, wxDefaultPosition, wxDefaultSize, wxTE_READONLY);
	m_textSingleXmlAppPath->SetBackgroundColour(GetBackgroundColour());
	m_singleXmlGrid->Add(m_labelSingleXmlAppPath, 0, wxALIGN_CENTER_VERTICAL);
	m_singleXmlGrid->AddSpacer(4);
	m_singleXmlGrid->Add(m_textSingleXmlAppPath, 0, wxEXPAND);
	m_singleXmlGrid->AddSpacer(4);

	m_labelSingleXmlUseRelativePaths = new wxStaticText(this, wxID_ANY, _("Use relative paths :"));
	m_checkSingleXmlUseRelativePaths = new wxCheckBox(this, muhkuh_configReposEntryDialog_CheckboxSingleXmlRelPaths, wxT(""), wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT);
	m_singleXmlGrid->Add(m_labelSingleXmlUseRelativePaths);
	m_singleXmlGrid->AddSpacer(4);
	m_singleXmlGrid->Add(m_checkSingleXmlUseRelativePaths, 0, wxEXPAND);
	m_singleXmlGrid->AddSpacer(4);


	m_radioAllLocal = new wxRadioButton(this, muhkuh_configReposEntryDialog_radioAllLocal, _("All Local Files"), wxDefaultPosition, wxDefaultSize, 0);
	m_mainSizer->Add(m_radioAllLocal, 0, 0);

	m_allLocalGrid = new wxFlexGridSizer(5, 4, 0, 0);
	m_allLocalGrid->AddGrowableCol(2, 1);
	m_mainSizer->Add(m_allLocalGrid, 0, wxEXPAND|wxLEFT, 32);

	// fill the input grid
	m_labelAllLocalLocation = new wxStaticText(this, wxID_ANY, _("Start Path :"));
	m_textAllLocalLocation = new wxTextCtrl(this, wxID_ANY);
	m_buttonAllLocalLocation = new wxBitmapButton(this, muhkuh_configReposEntryDialog_ButtonAllLocalLocationBrowse, icon_famfamfam_silk_folder);
	m_allLocalGrid->Add(m_labelAllLocalLocation, 0, wxALIGN_CENTER_VERTICAL);
	m_allLocalGrid->AddSpacer(4);
	m_allLocalGrid->Add(m_textAllLocalLocation, 0, wxEXPAND);
	m_allLocalGrid->Add(m_buttonAllLocalLocation);

	m_labelAllLocalTestExtension = new wxStaticText(this, wxID_ANY, _("Test description extension :"));
	m_textAllLocalTestExtension = new wxTextCtrl(this, wxID_ANY);
	m_allLocalGrid->Add(m_labelAllLocalTestExtension, 0, wxALIGN_CENTER_VERTICAL);
	m_allLocalGrid->AddSpacer(4);
	m_allLocalGrid->Add(m_textAllLocalTestExtension, 0, wxEXPAND);
	m_allLocalGrid->AddSpacer(4);

	m_labelAllLocalAppPath = new wxStaticText(this, wxID_ANY, _("Application Path :"));
	m_textAllLocalAppPath = new wxTextCtrl(this, wxID_ANY, m_strApplicationPath, wxDefaultPosition, wxDefaultSize, wxTE_READONLY);
	m_textAllLocalAppPath->SetBackgroundColour(GetBackgroundColour());
	m_allLocalGrid->Add(m_labelAllLocalAppPath, 0, wxALIGN_CENTER_VERTICAL);
	m_allLocalGrid->AddSpacer(4);
	m_allLocalGrid->Add(m_textAllLocalAppPath, 0, wxEXPAND);
	m_allLocalGrid->AddSpacer(4);

	m_labelAllLocalXmlPattern = new wxStaticText(this, wxID_ANY, _("Xml Pattern :"));
	m_textAllLocalXmlPattern = new wxTextCtrl(this, wxID_ANY);
	m_allLocalGrid->Add(m_labelAllLocalXmlPattern, 0, wxALIGN_CENTER_VERTICAL);
	m_allLocalGrid->AddSpacer(4);
	m_allLocalGrid->Add(m_textAllLocalXmlPattern, 0, wxEXPAND);
	m_allLocalGrid->AddSpacer(4);

	m_labelAllLocalUseRelativePaths = new wxStaticText(this, wxID_ANY, _("Use relative paths :"));
	m_checkAllLocalUseRelativePaths = new wxCheckBox(this, muhkuh_configReposEntryDialog_CheckboxAllLocalRelPaths, wxT(""), wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT);
	m_allLocalGrid->Add(m_labelAllLocalUseRelativePaths);
	m_allLocalGrid->AddSpacer(4);
	m_allLocalGrid->Add(m_checkAllLocalUseRelativePaths, 0, wxEXPAND);
	m_allLocalGrid->AddSpacer(4);


	// create the button sizer
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
	minSize = GetMinSize();
	maxSize = GetMaxSize();
	maxSize.SetHeight(minSize.GetHeight());
	SetMaxSize(maxSize);

}


void muhkuh_config_reposEntryDialog::SwitchInputs(void)
{
	bool fDirscanEndable;
	bool fFilelistEndable;
	bool fSingleXmlEnable;
	bool fAllLocalEnable;


	switch( m_eTyp )
	{
	case muhkuh_repository::REPOSITORY_TYP_DIRSCAN:
		fDirscanEndable = true;
		fFilelistEndable = false;
		fSingleXmlEnable = false;
		fAllLocalEnable = false;
		break;

	case muhkuh_repository::REPOSITORY_TYP_FILELIST:
		fDirscanEndable = false;
		fFilelistEndable = true;
		fSingleXmlEnable = false;
		fAllLocalEnable = false;
		break;

	case muhkuh_repository::REPOSITORY_TYP_SINGLEXML:
		fDirscanEndable = false;
		fFilelistEndable = false;
		fSingleXmlEnable = true;
		fAllLocalEnable = false;
		break;

	case muhkuh_repository::REPOSITORY_TYP_ALLLOCAL:
		fDirscanEndable = false;
		fFilelistEndable = false;
		fSingleXmlEnable = false;
		fAllLocalEnable = true;
		break;

	default:
		fDirscanEndable = false;
		fFilelistEndable = false;
		fSingleXmlEnable = false;
		fAllLocalEnable = false;
		break;
	};
	
	m_textDirscanLocation->Enable(fDirscanEndable);
	m_buttonDirscanLocation->Enable(fDirscanEndable);
	m_textTestExtension->Enable(fDirscanEndable);
	m_checkDirscanUseRelativePaths->Enable(fDirscanEndable);

	m_textFilelistLocation->Enable(fFilelistEndable);

	m_textSingleXmlLocation->Enable(fSingleXmlEnable);
	m_buttonSingleXmlLocation->Enable(fSingleXmlEnable);
	m_checkSingleXmlUseRelativePaths->Enable(fSingleXmlEnable);

	m_textAllLocalLocation->Enable(fAllLocalEnable);
	m_buttonAllLocalLocation->Enable(fAllLocalEnable);
	m_textAllLocalTestExtension->Enable(fAllLocalEnable);
	m_textAllLocalXmlPattern->Enable(fAllLocalEnable);
}


void muhkuh_config_reposEntryDialog::OnOkButton(wxCommandEvent &WXUNUSED(event))
{
	wxString strName;
	wxString strLocation;
	wxString strExtension;
	wxString strXmlPattern;
	wxFileName fileName;
	bool fAllValuesOk;


	fAllValuesOk = false;

	// the name must not be empty
	strName = m_textRepoName->GetValue();
	if( strName.IsEmpty()==true )
	{
		wxMessageBox(_("Please enter a name for the repository!"), _("Input error"), wxOK|wxICON_ERROR);
	}
	else
	{
		switch( m_eTyp )
		{
		case muhkuh_repository::REPOSITORY_TYP_DIRSCAN:
			adaptDirscanLocation(m_textDirscanLocation->GetValue());

			// get values
			strLocation = m_textDirscanLocation->GetValue();
			strExtension = m_textTestExtension->GetValue();

			// test the values
			if( strLocation.IsEmpty()==true )
			{
				wxMessageBox(_("Please enter the location!"), _("Input error"), wxOK|wxICON_ERROR);
			}
			else
			{
				fileName.SetCwd(m_strApplicationPath);
				fileName.AssignDir(strLocation);
				if( fileName.IsDir()==false )
				{
					wxMessageBox(_("The selected path to the test descriptions is no directory!"), _("Input error"), wxOK|wxICON_ERROR);
				}
				else if( wxFileName::DirExists(fileName.GetPath(wxPATH_GET_VOLUME|wxPATH_GET_SEPARATOR))==false )
				{
					wxMessageBox(_("The selected path to the test descriptions does not exist!"), _("Input error"), wxOK|wxICON_ERROR);
				}
				// the extension must not be empty
				else if( strExtension.IsEmpty()==true )
				{
					wxMessageBox(_("Please select the test descriptions extension file!"), _("Input error"), wxOK|wxICON_ERROR);
				}
				else
				{
					// accept the values
					m_ptRepos->SetDirscan(strLocation, strExtension);
					fAllValuesOk = true;
				}
			}
			break;

		case muhkuh_repository::REPOSITORY_TYP_FILELIST:
			// get values
			strLocation = m_textFilelistLocation->GetValue();

			// the location must not be empty
			if( strLocation.IsEmpty()==true )
			{
				wxMessageBox(_("Please enter the location!"), _("Input error"), wxOK|wxICON_ERROR);
			}
			else
			{
				// accept the values
				m_ptRepos->SetFilelist(strLocation);
				fAllValuesOk = true;
			}
			break;

		case muhkuh_repository::REPOSITORY_TYP_SINGLEXML:
			// get values
			strLocation = m_textSingleXmlLocation->GetValue();

			// the location must not be empty
			if( strLocation.IsEmpty()==true )
			{
				wxMessageBox(_("Please enter the location!"), _("Input error"), wxOK|wxICON_ERROR);
			}
			else
			{
				// accept the values
				m_ptRepos->SetSingleXml(strLocation);
				fAllValuesOk = true;
			}
			break;

		case muhkuh_repository::REPOSITORY_TYP_ALLLOCAL:
			// get values
			strLocation = m_textAllLocalLocation->GetValue();
			strExtension = m_textAllLocalTestExtension->GetValue();
			strXmlPattern = m_textAllLocalXmlPattern->GetValue();

			// test the values
			if( strLocation.IsEmpty()==true )
			{
				wxMessageBox(_("Please enter the location!"), _("Input error"), wxOK|wxICON_ERROR);
			}
			else
			{
				fileName.SetCwd(m_strApplicationPath);
				fileName.AssignDir(strLocation);

				if( fileName.IsDir()==false )
				{
					wxMessageBox(_("The selected path to the test descriptions is no directory!"), _("Input error"), wxOK|wxICON_ERROR);
				}
				else if( wxFileName::DirExists(fileName.GetPath(wxPATH_GET_VOLUME|wxPATH_GET_SEPARATOR))==false )
				{
					wxMessageBox(_("The selected path to the test descriptions does not exist!"), _("Input error"), wxOK|wxICON_ERROR);
				}
				// the extension must not be empty
				else if( strExtension.IsEmpty()==true )
				{
					wxMessageBox(_("Please select the test descriptions extension file!"), _("Input error"), wxOK|wxICON_ERROR);
				}
				else if( strXmlPattern.IsEmpty()==true )
				{
					wxMessageBox(_("Please enter the xml pattern!"), _("Input error"), wxOK|wxICON_ERROR);
				}
				else
				{
					// accept the values
					m_ptRepos->SetAllLocal(strLocation, strExtension, strXmlPattern);
					fAllValuesOk = true;
				}
			}
		}
	}

	if( fAllValuesOk==true )
	{
		m_ptRepos->SetName(strName);
		EndModal(wxID_OK);
	}
}


void muhkuh_config_reposEntryDialog::OnBrowseDirscanLocationButton(wxCommandEvent &WXUNUSED(event))
{
	wxDirDialog *testPathDialog;
	wxFileName fileName;
	wxString strDialogInitPath;


	strDialogInitPath = m_textDirscanLocation->GetValue();
	if( m_fDirscanUseRelativePaths==true )
	{
		fileName.Assign(strDialogInitPath);
		if(fileName.Normalize(wxPATH_NORM_ALL, m_strApplicationPath ,wxPATH_NATIVE))
		{
			strDialogInitPath = fileName.GetFullPath();
		}
	}

	testPathDialog = new wxDirDialog(this, _("Choose the path to the test descriptions"));
	testPathDialog->SetPath(strDialogInitPath);

	if( testPathDialog->ShowModal()==wxID_OK )
	{
		adaptDirscanLocation(testPathDialog->GetPath());
	}
	testPathDialog->Destroy();
}


void muhkuh_config_reposEntryDialog::OnBrowseSingleXmlLocationButton(wxCommandEvent &WXUNUSED(event))
{
	wxFileDialog *testPathDialog;
	wxFileName fileName;
	wxString strDialogInitPath;


	strDialogInitPath = m_textSingleXmlLocation->GetValue();
	if( m_fSingleXmlUseRelativePaths==true )
	{
		fileName.Assign(strDialogInitPath);
		if( fileName.Normalize(wxPATH_NORM_ALL, m_strApplicationPath ,wxPATH_NATIVE)==true )
		{
			strDialogInitPath = fileName.GetFullPath();
		}
	}

	testPathDialog = new wxFileDialog(this, _("Choose the path to the test description"), strDialogInitPath, wxEmptyString, wxT("*.xml"));

	if( testPathDialog->ShowModal()==wxID_OK )
	{
		adaptSingleXmlLocation(testPathDialog->GetPath());
	}
	testPathDialog->Destroy();
}


void muhkuh_config_reposEntryDialog::OnBrowseAllLocalLocationButton(wxCommandEvent &WXUNUSED(event))
{
	wxDirDialog *testPathDialog;
	wxFileName fileName;
	wxString strDialogInitPath;


	strDialogInitPath = m_textAllLocalLocation->GetValue();
	if( m_fAllLocalUseRelativePaths==true )
	{
		fileName.Assign(strDialogInitPath);
		if(fileName.Normalize(wxPATH_NORM_ALL, m_strApplicationPath ,wxPATH_NATIVE))
		{
			strDialogInitPath = fileName.GetFullPath();
		}
	}

	testPathDialog = new wxDirDialog(this, _("Please choose the path which will be scanned."));
	testPathDialog->SetPath(strDialogInitPath);

	if( testPathDialog->ShowModal()==wxID_OK )
	{
		adaptAllLocalLocation(testPathDialog->GetPath());
	}
	testPathDialog->Destroy();
}


void muhkuh_config_reposEntryDialog::OnRadioDirscan(wxCommandEvent &WXUNUSED(event))
{
	m_eTyp = muhkuh_repository::REPOSITORY_TYP_DIRSCAN;
	SwitchInputs();
}


void muhkuh_config_reposEntryDialog::OnRadioFilelist(wxCommandEvent &WXUNUSED(event))
{
	m_eTyp = muhkuh_repository::REPOSITORY_TYP_FILELIST;
	SwitchInputs();
}


void muhkuh_config_reposEntryDialog::OnRadioSingleXml(wxCommandEvent &WXUNUSED(event))
{
	m_eTyp = muhkuh_repository::REPOSITORY_TYP_SINGLEXML;
	SwitchInputs();
}


void muhkuh_config_reposEntryDialog::OnRadioAllLocal(wxCommandEvent &WXUNUSED(event))
{
	m_eTyp = muhkuh_repository::REPOSITORY_TYP_ALLLOCAL;
	SwitchInputs();
}


void muhkuh_config_reposEntryDialog::OnDirscanUseRelPaths(wxCommandEvent &event)
{
	m_fDirscanUseRelativePaths = event.IsChecked();
	adaptDirscanLocation(m_textDirscanLocation->GetValue());
}


void muhkuh_config_reposEntryDialog::OnSingleXmlUseRelPaths(wxCommandEvent &event)
{
	m_fSingleXmlUseRelativePaths = event.IsChecked();
	adaptSingleXmlLocation(m_textSingleXmlLocation->GetValue());
}


void muhkuh_config_reposEntryDialog::OnAllLocalUseRelPaths(wxCommandEvent &event)
{
	m_fSingleXmlUseRelativePaths = event.IsChecked();
	adaptSingleXmlLocation(m_textSingleXmlLocation->GetValue());
}


bool muhkuh_config_reposEntryDialog::adaptDirscanLocation(wxString strFileName)
{
	wxFileName fileName;

	fileName.AssignDir(strFileName);
	if( m_fDirscanUseRelativePaths==true )
	{
		fileName.MakeRelativeTo(m_strApplicationPath);
	}
	else
	{
		fileName.Normalize(wxPATH_NORM_ALL, m_strApplicationPath ,wxPATH_NATIVE);
	}

	m_textDirscanLocation->SetValue( fileName.GetFullPath() );
	return true;
}


bool muhkuh_config_reposEntryDialog::adaptSingleXmlLocation(wxString strFileName)
{
	wxFileName fileName;


	fileName.Assign(strFileName);
	if( m_fSingleXmlUseRelativePaths==true )
	{
		fileName.MakeRelativeTo(m_strApplicationPath);
	}
	else
	{
		fileName.Normalize(wxPATH_NORM_ALL, m_strApplicationPath ,wxPATH_NATIVE);
	}

	m_textSingleXmlLocation->SetValue( fileName.GetFullPath() );
	return true;
}


bool muhkuh_config_reposEntryDialog::adaptAllLocalLocation(wxString strFileName)
{
	wxFileName fileName;


	fileName.AssignDir(strFileName);
	if( m_fAllLocalUseRelativePaths==true )
	{
		fileName.MakeRelativeTo(m_strApplicationPath);
	}
	else
	{
		fileName.Normalize(wxPATH_NORM_ALL, m_strApplicationPath ,wxPATH_NATIVE);
	}

	m_textAllLocalLocation->SetValue( fileName.GetFullPath() );
	return true;
}

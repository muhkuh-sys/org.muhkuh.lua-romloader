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
#include "muhkuh_config_reposEntryDialog.h"


BEGIN_EVENT_TABLE(muhkuh_config_reposEntryDialog, wxDialog)
	EVT_BUTTON(wxID_OK,							muhkuh_config_reposEntryDialog::OnOkButton)
	EVT_BUTTON(muhkuh_configReposEntryDialog_ButtonDirscanLocationBrowse,	muhkuh_config_reposEntryDialog::OnBrowseDirscanLocationButton)
	EVT_BUTTON(muhkuh_configReposEntryDialog_ButtonSingleXmlLocationBrowse,	muhkuh_config_reposEntryDialog::OnBrowseSingleXmlLocationButton)
	EVT_CHECKBOX(muhkuh_configReposEntryDialog_CheckboxDirscanRelPaths,	muhkuh_config_reposEntryDialog::OnDirscanUseRelPaths)
	EVT_CHECKBOX(muhkuh_configReposEntryDialog_CheckboxSingleXmlRelPaths,	muhkuh_config_reposEntryDialog::OnSingleXmlUseRelPaths)
	EVT_RADIOBUTTON(muhkuh_configReposEntryDialog_radioDirscan,		muhkuh_config_reposEntryDialog::OnRadioDirscan)
	EVT_RADIOBUTTON(muhkuh_configReposEntryDialog_radioFilelist,		muhkuh_config_reposEntryDialog::OnRadioFilelist)
	EVT_RADIOBUTTON(muhkuh_configReposEntryDialog_radioSingleXml,		muhkuh_config_reposEntryDialog::OnRadioSingleXml)
END_EVENT_TABLE()


muhkuh_config_reposEntryDialog::muhkuh_config_reposEntryDialog(wxWindow *parent, const wxString sApplicationPath, muhkuh_repository *ptRepos)
 : wxDialog(parent, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER)
 , m_ptRepos(ptRepos)
{
	m_strApplicationPath = sApplicationPath;

	// create the controls
	createControls();

	m_fDirscanUseRelativePaths = false;
	m_fSingleXmlUseRelativePaths = false;

	m_textRepoName->SetValue( m_ptRepos->GetName() );

	m_eTyp = m_ptRepos->GetTyp();
	switch( m_eTyp )
	{
	case muhkuh_repository::REPOSITORY_TYP_DIRSCAN:
		m_textDirscanLocation->SetValue( m_ptRepos->GetLocation() );
		m_textTestExtension->SetValue( m_ptRepos->GetExtension() );
		m_radioDirscan->SetValue(true);
		m_radioFilelist->SetValue(false);
		m_radioSingleXml->SetValue(false);
		break;

	case muhkuh_repository::REPOSITORY_TYP_FILELIST:
		m_textFilelistLocation->SetValue( m_ptRepos->GetLocation() );
		m_radioDirscan->SetValue(false);
		m_radioFilelist->SetValue(true);
		m_radioSingleXml->SetValue(false);
		break;

	case muhkuh_repository::REPOSITORY_TYP_SINGLEXML:
		m_textSingleXmlLocation->SetValue( m_ptRepos->GetLocation() );
		m_radioDirscan->SetValue(false);
		m_radioFilelist->SetValue(false);
		m_radioSingleXml->SetValue(true);
		break;

	default:
		m_eTyp = muhkuh_repository::REPOSITORY_TYP_DIRSCAN;
		m_radioDirscan->SetValue(true);
		m_radioFilelist->SetValue(false);
		m_radioSingleXml->SetValue(false);
		m_textTestExtension->SetValue(wxT("*.mtd"));
	};
	SwitchInputs();

	// set the checkbox, this must be after the filenames
	m_checkDirscanUseRelativePaths->SetValue(m_fDirscanUseRelativePaths);
	m_checkSingleXmlUseRelativePaths->SetValue(m_fSingleXmlUseRelativePaths);
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
	m_buttonDirscanLocation = new wxBitmapButton(this, muhkuh_configReposEntryDialog_ButtonDirscanLocationBrowse, wxArtProvider::GetBitmap(wxART_FILE_OPEN));
	m_dirscanGrid->Add(m_labelDirscanLocation, 0, wxALIGN_CENTER_VERTICAL);
	m_dirscanGrid->AddSpacer(4);
	m_dirscanGrid->Add(m_textDirscanLocation, 0, wxEXPAND);
	m_dirscanGrid->Add(m_buttonDirscanLocation);

	m_labelTestExtension = new wxStaticText(this, wxID_ANY, _("Test description extension :"));
	m_textTestExtension = new wxTextCtrl(this, wxID_ANY);
	m_dirscanGrid->Add(m_labelTestExtension, 0, wxALIGN_CENTER_VERTICAL);
	m_dirscanGrid->AddSpacer(4);
	m_dirscanGrid->Add(m_textTestExtension, 0, wxEXPAND);
	m_dirscanGrid->AddSpacer(4);

	m_labelAppPath = new wxStaticText(this, wxID_ANY, _("Application Path :"));
	m_textAppPath = new wxTextCtrl(this, wxID_ANY, m_strApplicationPath, wxDefaultPosition, wxDefaultSize, wxTE_READONLY);
	m_textAppPath->SetBackgroundColour(GetBackgroundColour());
	m_dirscanGrid->Add(m_labelAppPath, 0, wxALIGN_CENTER_VERTICAL);
	m_dirscanGrid->AddSpacer(4);
	m_dirscanGrid->Add(m_textAppPath, 0, wxEXPAND);
	m_dirscanGrid->AddSpacer(4);

	m_checkDirscanUseRelativePaths = new wxCheckBox(this, muhkuh_configReposEntryDialog_CheckboxDirscanRelPaths, _("Use relative paths :"), wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT);
	m_dirscanGrid->AddSpacer(4);
	m_dirscanGrid->AddSpacer(4);
	m_dirscanGrid->Add(m_checkDirscanUseRelativePaths, 0, wxEXPAND);
	m_dirscanGrid->AddSpacer(4);


	m_radioFilelist = new wxRadioButton(this, muhkuh_configReposEntryDialog_radioFilelist, _("Filelist"), wxDefaultPosition, wxDefaultSize, 0);
	m_mainSizer->Add(m_radioFilelist, 0, 0);

	m_filelistGrid = new wxFlexGridSizer(1, 3, 0, 0);
	m_filelistGrid->AddGrowableCol(2, 1);
	m_mainSizer->Add(m_filelistGrid, 0, wxEXPAND|wxLEFT, 32);

	// fill the input grid
	m_labelFilelistLocation = new wxStaticText(this, wxID_ANY, _("Path to test descriptions :"));
	m_textFilelistLocation = new wxTextCtrl(this, wxID_ANY);
	m_filelistGrid->Add(m_labelFilelistLocation, 0, wxALIGN_CENTER_VERTICAL);
	m_filelistGrid->AddSpacer(4);
	m_filelistGrid->Add(m_textFilelistLocation, 0, wxEXPAND);


	m_radioSingleXml = new wxRadioButton(this, muhkuh_configReposEntryDialog_radioSingleXml, _("Single Xml"), wxDefaultPosition, wxDefaultSize, 0);
	m_mainSizer->Add(m_radioSingleXml, 0, 0);

	m_singleXmlGrid = new wxFlexGridSizer(1, 4, 0, 0);
	m_singleXmlGrid->AddGrowableCol(2, 1);
	m_mainSizer->Add(m_singleXmlGrid, 0, wxEXPAND|wxLEFT, 32);
	m_buttonSingleXmlLocation = new wxBitmapButton(this, muhkuh_configReposEntryDialog_ButtonSingleXmlLocationBrowse, wxArtProvider::GetBitmap(wxART_FILE_OPEN));

	// fill the input grid
	m_labelSingleXmlLocation = new wxStaticText(this, wxID_ANY, _("Path to the xml test description :"));
	m_textSingleXmlLocation = new wxTextCtrl(this, wxID_ANY);
	m_singleXmlGrid->Add(m_labelSingleXmlLocation, 0, wxALIGN_CENTER_VERTICAL);
	m_singleXmlGrid->AddSpacer(4);
	m_singleXmlGrid->Add(m_textSingleXmlLocation, 0, wxEXPAND);
	m_singleXmlGrid->Add(m_buttonSingleXmlLocation);

	m_checkSingleXmlUseRelativePaths = new wxCheckBox(this, muhkuh_configReposEntryDialog_CheckboxSingleXmlRelPaths, _("Use relative paths :"), wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT);
	m_singleXmlGrid->AddSpacer(4);
	m_singleXmlGrid->AddSpacer(4);
	m_singleXmlGrid->Add(m_checkSingleXmlUseRelativePaths, 0, wxEXPAND);
	m_singleXmlGrid->AddSpacer(4);


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


	switch( m_eTyp )
	{
	case muhkuh_repository::REPOSITORY_TYP_DIRSCAN:
		fDirscanEndable = true;
		fFilelistEndable = false;
		fSingleXmlEnable = false;
		break;

	case muhkuh_repository::REPOSITORY_TYP_FILELIST:
		fDirscanEndable = false;
		fFilelistEndable = true;
		fSingleXmlEnable = false;
		break;

	case muhkuh_repository::REPOSITORY_TYP_SINGLEXML:
		fDirscanEndable = false;
		fFilelistEndable = false;
		fSingleXmlEnable = true;
		break;

	default:
		fDirscanEndable = false;
		fFilelistEndable = false;
		fSingleXmlEnable = false;
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
}


void muhkuh_config_reposEntryDialog::OnOkButton(wxCommandEvent &WXUNUSED(event))
{
	wxString strName;
	wxString strLocation;
	wxString strExtension;
//	bool fUseRelativePaths;
	wxFileName fileName;
	bool fAllValuesOk;


	fAllValuesOk = false;

	// the name must not be empty
	strName = m_textRepoName->GetValue();
	if( strName.IsEmpty()==true )
	{
		wxMessageBox(wxT("Please enter a name for the repository!"), wxT("Input error"), wxOK|wxICON_ERROR);
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
//			fUseRelativePaths = m_checkUseRelativePaths->GetValue();

			// test the values
			fileName.SetCwd(m_strApplicationPath);
			if( !strLocation.IsEmpty() )
			{
				fileName.AssignDir(strLocation);
			}
			if( !fileName.IsDir() )
			{
				wxMessageBox(wxTRANSLATE("The selected path to the test descriptions is no directory!"), wxTRANSLATE("Input error"), wxOK|wxICON_ERROR);
			}
			else if( !wxFileName::DirExists(fileName.GetPath(wxPATH_GET_VOLUME|wxPATH_GET_SEPARATOR)) )
			{
				wxMessageBox(wxTRANSLATE("The selected path to the test descriptions does not exist!"), wxTRANSLATE("Input error"), wxOK|wxICON_ERROR);
			}
			// the extension must not be empty
			else if( strExtension.IsEmpty() )
			{
				wxMessageBox(wxT("Please select the test descriptions extension file!"), wxT("Input error"), wxOK|wxICON_ERROR);
			}
			else
			{
				// accept the values
				m_ptRepos->SetDirscan(strLocation, strExtension);
				fAllValuesOk = true;
			}
			break;

		case muhkuh_repository::REPOSITORY_TYP_FILELIST:
			// get values
			strLocation = m_textFilelistLocation->GetValue();

			// the location must not be empty
			if( strLocation.IsEmpty() )
			{
				wxMessageBox(wxT("Please enter the location!"), wxT("Input error"), wxOK|wxICON_ERROR);
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
			if( strLocation.IsEmpty() )
			{
				wxMessageBox(wxT("Please enter the location!"), wxT("Input error"), wxOK|wxICON_ERROR);
			}
			else
			{
				// accept the values
				m_ptRepos->SetSingleXml(strLocation);
				fAllValuesOk = true;
			}
			break;

		default:
			break;
		};
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

	testPathDialog = new wxDirDialog(this, wxT("Choose the path to the test descriptions"));
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
		if(fileName.Normalize(wxPATH_NORM_ALL, m_strApplicationPath ,wxPATH_NATIVE))
		{
			strDialogInitPath = fileName.GetFullPath();
		}
	}

	testPathDialog = new wxFileDialog(this, wxT("Choose the path to the test description"), strDialogInitPath, wxEmptyString, wxT("*.xml"));

	if( testPathDialog->ShowModal()==wxID_OK )
	{
		adaptSingleXmlLocation(testPathDialog->GetPath());
	}
	testPathDialog->Destroy();
}


void muhkuh_config_reposEntryDialog::OnDirscanUseRelPaths(wxCommandEvent &event)
{
	m_fDirscanUseRelativePaths = event.IsChecked();
	adaptDirscanLocation(m_textDirscanLocation->GetValue());
}


bool muhkuh_config_reposEntryDialog::adaptDirscanLocation(wxString strFileName)
{
	wxFileName fileName;

	fileName.AssignDir(strFileName);
	fileName.SetCwd(m_strApplicationPath);
	if( m_fDirscanUseRelativePaths==true )
	{
		fileName.MakeRelativeTo(m_strApplicationPath);
	}
	else
	{
		fileName.MakeAbsolute();
	}

	m_textDirscanLocation->SetValue( fileName.GetFullPath() );
	return true;
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


void muhkuh_config_reposEntryDialog::OnSingleXmlUseRelPaths(wxCommandEvent &event)
{
	m_fSingleXmlUseRelativePaths = event.IsChecked();
	adaptDirscanLocation(m_textDirscanLocation->GetValue());
}


void muhkuh_config_reposEntryDialog::OnRadioSingleXml(wxCommandEvent &WXUNUSED(event))
{
	m_eTyp = muhkuh_repository::REPOSITORY_TYP_SINGLEXML;
	SwitchInputs();
}


bool muhkuh_config_reposEntryDialog::adaptSingleXmlLocation(wxString strFileName)
{
	wxFileName fileName;

	fileName.Assign(strFileName);
	fileName.SetCwd(m_strApplicationPath);
	if( m_fSingleXmlUseRelativePaths==true )
	{
		fileName.MakeRelativeTo(m_strApplicationPath);
	}
	else
	{
		fileName.MakeAbsolute();
	}

	m_textSingleXmlLocation->SetValue( fileName.GetFullPath() );
	return true;
}

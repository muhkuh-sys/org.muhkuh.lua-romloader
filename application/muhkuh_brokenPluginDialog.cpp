/***************************************************************************
 *   Copyright (C) 2008 by Christoph Thelen                                *
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


#include "muhkuh_brokenPluginDialog.h"

#include "muhkuh_id.h"


BEGIN_EVENT_TABLE(muhkuh_brokenPluginDialog, wxDialog)
	EVT_BUTTON(muhkuh_brokenPluginDialog_ButtonDisable,		muhkuh_brokenPluginDialog::OnButtonDisable)
	EVT_BUTTON(muhkuh_brokenPluginDialog_ButtonIgnore,		muhkuh_brokenPluginDialog::OnButtonIgnore)
	EVT_BUTTON(muhkuh_brokenPluginDialog_ButtonConfig,		muhkuh_brokenPluginDialog::OnButtonConfig)
END_EVENT_TABLE()


muhkuh_brokenPluginDialog::muhkuh_brokenPluginDialog(wxWindow *parent, std::vector<unsigned long> *pvPlugins, muhkuh_plugin_manager *ptPluginManager)
 : wxDialog(parent, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER)
 , m_pvPlugins(pvPlugins)
 , m_ptPluginManager(ptPluginManager)
{
	wxBoxSizer *ptMainSizer;
	wxStaticText *ptLabelTop;
	wxTextCtrl *ptTextPlugins;
	wxBoxSizer *ptButtonSizer;
	wxButton *ptButtonDisable;
	wxButton *ptButtonIgnore;
	wxButton *ptButtonConfig;
	std::vector<unsigned long>::const_iterator iter;
	unsigned long ulPluginIdx;
	wxString strPlugins;
	wxString strMsg;


	// set the window title
	SetTitle(_("Plugin Error!"));

	// create the main sizer
	ptMainSizer = new wxBoxSizer(wxVERTICAL);
	SetSizer(ptMainSizer);

	strMsg.Printf(_("%d plugin(s) are not working correctly:"), pvPlugins->size());
	ptLabelTop = new wxStaticText(this, wxID_ANY, strMsg);
	ptMainSizer->Add(ptLabelTop, 0, wxEXPAND|wxBOTTOM, 4);

	iter = pvPlugins->begin();
	while( iter!=pvPlugins->end() )
	{
		ulPluginIdx = *iter;
		strPlugins += ptPluginManager->GetConfigName(ulPluginIdx) + wxT(" : ") + ptPluginManager->GetInitError(ulPluginIdx) + wxT("\n");
		++iter;
	}
	ptTextPlugins = new wxTextCtrl(this, wxID_ANY, strPlugins, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE|wxTE_READONLY);
	ptTextPlugins->SetBackgroundColour(GetBackgroundColour());
	ptMainSizer->Add(ptTextPlugins, 1, wxEXPAND);

	// add  the button sizer
	ptButtonSizer = new wxBoxSizer(wxHORIZONTAL);
	ptMainSizer->Add(ptButtonSizer, 0, wxEXPAND);

	ptButtonDisable = new wxButton(this, muhkuh_brokenPluginDialog_ButtonDisable, _("Disable Plugins"));
	ptButtonIgnore = new wxButton(this, muhkuh_brokenPluginDialog_ButtonIgnore, _("Ignore"));
	ptButtonConfig = new wxButton(this, muhkuh_brokenPluginDialog_ButtonConfig, _("Show Config Menu"));

	ptButtonSizer->AddStretchSpacer(1);
	ptButtonSizer->Add(ptButtonDisable);
	ptButtonSizer->AddStretchSpacer(1);
	ptButtonSizer->Add(ptButtonIgnore);
	ptButtonSizer->AddStretchSpacer(1);
	ptButtonSizer->Add(ptButtonConfig);
	ptButtonSizer->AddStretchSpacer(1);

	ptMainSizer->SetSizeHints(this);
}


void muhkuh_brokenPluginDialog::OnButtonDisable(wxCommandEvent &event)
{
	std::vector<unsigned long>::const_iterator iter;
	unsigned long ulPluginIdx;


	// disable all plugins in the list
	iter = m_pvPlugins->begin();
	while( iter!=m_pvPlugins->end() )
	{
		// get the plugin index
		ulPluginIdx = *iter;

		// disable the plugin
		m_ptPluginManager->SetEnable(ulPluginIdx, false);

		// next list element
		++iter;
	}

	// close the dialog
	EndModal(muhkuh_brokenPluginDialog_ButtonDisable);
}


void muhkuh_brokenPluginDialog::OnButtonIgnore(wxCommandEvent &event)
{
	// ignore the broken plugins
	EndModal(muhkuh_brokenPluginDialog_ButtonIgnore);
}


void muhkuh_brokenPluginDialog::OnButtonConfig(wxCommandEvent &event)
{
	// show the config menu and let the user correct the mess
	EndModal(muhkuh_brokenPluginDialog_ButtonConfig);
}


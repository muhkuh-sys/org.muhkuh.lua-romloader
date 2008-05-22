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

#include <wx/wx.h>
#include "muhkuh_plugin_manager.h"


#ifndef __MUHKUH_BROKENPLUGINDIALOG_H__
#define __MUHKUH_BROKENPLUGINDIALOG_H__


class muhkuh_brokenPluginDialog : public wxDialog
{
public:
	muhkuh_brokenPluginDialog(wxWindow *parent, std::vector<unsigned long> *pvPlugins, muhkuh_plugin_manager *ptPluginManager);
	void OnButtonDisable(wxCommandEvent &event);
	void OnButtonIgnore(wxCommandEvent &event);
	void OnButtonConfig(wxCommandEvent &event);

private:
	std::vector<unsigned long> *m_pvPlugins;
	muhkuh_plugin_manager *m_ptPluginManager;
    DECLARE_EVENT_TABLE()
};


#endif	// __MUHKUH_BROKENPLUGINDIALOG_H__

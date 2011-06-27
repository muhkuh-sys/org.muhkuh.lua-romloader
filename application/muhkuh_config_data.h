/***************************************************************************
 *   Copyright (C) 2011 by Christoph Thelen                                *
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

#ifndef __MUHKUH_CONFIG_DATA_H__
#define __MUHKUH_CONFIG_DATA_H__

#include <wx/wx.h>

#include "muhkuh_plugin_manager.h"
#include "muhkuh_repository_manager.h"
#include "muhkuh_so_api.h"


class MUHKUH_SO_API muhkuh_config_data
{
public:
	muhkuh_config_data(void);
	muhkuh_config_data(const muhkuh_config_data *ptClone);
	~muhkuh_config_data(void);

	void read_config(wxConfigBase *pConfig);
	void write_config(wxConfigBase *pConfig);


	wxString m_strWelcomeFile;
	wxString m_strDetailsFile;

	muhkuh_plugin_manager *m_ptPluginManager;
	muhkuh_repository_manager *m_ptRepositoryManager;

	wxString m_strLuaIncludePath;
	wxString m_strLuaSystemModulePath;
	wxString m_strLuaStartupCode;

	wxString m_strApplicationTitle;
	wxString m_strApplicationIcon;
};


#endif  /* __MUHKUH_CONFIG_DATA_H__ */

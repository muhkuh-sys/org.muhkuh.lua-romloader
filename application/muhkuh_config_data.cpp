/***************************************************************************
 *   Copyright (C) 2010 by Christoph Thelen                                *
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


#include "muhkuh_config_data.h"


muhkuh_config_data::muhkuh_config_data(void)
 : m_ptPluginManager(NULL)
 , m_ptRepositoryManager(NULL)
{
	m_ptPluginManager = new muhkuh_plugin_manager();
	m_ptRepositoryManager = new muhkuh_repository_manager();
}


muhkuh_config_data::muhkuh_config_data(const muhkuh_config_data *ptClone)
 : m_ptPluginManager(NULL)
 , m_ptRepositoryManager(NULL)
{
	m_strWelcomeFile = ptClone->m_strWelcomeFile;
	m_strDetailsFile = ptClone->m_strDetailsFile;

	m_ptPluginManager = new muhkuh_plugin_manager(ptClone->m_ptPluginManager);
	m_ptRepositoryManager = new muhkuh_repository_manager(ptClone->m_ptRepositoryManager);

	m_strLuaIncludePath = ptClone->m_strLuaIncludePath;
	m_strLuaSystemModulePath = ptClone->m_strLuaSystemModulePath;
	m_strLuaStartupCode = ptClone->m_strLuaStartupCode;

	m_strApplicationTitle = ptClone->m_strApplicationTitle;
	m_strApplicationIcon = ptClone->m_strApplicationIcon;
}


muhkuh_config_data::~muhkuh_config_data(void)
{
	if( m_ptPluginManager!=NULL )
	{
		delete m_ptPluginManager;
	}

	if( m_ptRepositoryManager!=NULL )
	{
		delete m_ptRepositoryManager;
	}
}


void muhkuh_config_data::read_config(wxConfigBase *pConfig)
{
	pConfig->SetPath("/MainFrame");
	m_strWelcomeFile = pConfig->Read("welcomepage", wxEmptyString);
	m_strDetailsFile = pConfig->Read("detailspage", wxEmptyString);
	m_strApplicationTitle = pConfig->Read("customtitle", wxEmptyString);
	m_strApplicationIcon = pConfig->Read("customicon", wxEmptyString);

	/* Get lua settings. */
	pConfig->SetPath("/Lua");
	m_strLuaIncludePath = pConfig->Read("includepaths", "lua/?.lua");
	m_strLuaSystemModulePath = pConfig->Read("system_modules", "lua_plugins/?.so");
	m_strLuaStartupCode = pConfig->Read("startupcode", "dofile(\"lua/system_gui.lua\")\n");

	/* Get all plugins. */
	m_ptPluginManager->read_config(pConfig);

	/* Get all repositories. */
	m_ptRepositoryManager->read_config(pConfig);
}


void muhkuh_config_data::write_config(wxConfigBase *pConfig)
{
	pConfig->SetPath("/MainFrame");
	pConfig->Write("welcomepage",  m_strWelcomeFile);
	pConfig->Write("detailspage",  m_strDetailsFile);
	pConfig->Write("customtitle",  m_strApplicationTitle);
	pConfig->Write("customicon",   m_strApplicationIcon);

	// get lua settings
	pConfig->SetPath("/Lua");
	pConfig->Write("includepaths", m_strLuaIncludePath);
	pConfig->Write("system_modules", m_strLuaSystemModulePath);
	pConfig->Write("startupcode",  m_strLuaStartupCode);

	/* Save all plugins. */
	m_ptPluginManager->write_config(pConfig);

	/* Save repositories. */
	m_ptRepositoryManager->write_config(pConfig);

	pConfig->Flush();
}


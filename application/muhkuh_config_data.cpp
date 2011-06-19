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


 
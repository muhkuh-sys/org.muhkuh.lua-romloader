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


#include "muhkuh_plugin_manager.h"

#include <wx/log.h>
#include <wx/regex.h>


muhkuh_plugin_manager::muhkuh_plugin_manager(void)
 : m_ptOpenPlugins(NULL)
{
	// set prefix for messages
	setMe();

	// create the repository list
	m_ptOpenPlugins = new std::vector<muhkuh_plugin*>;
}


muhkuh_plugin_manager::muhkuh_plugin_manager(const muhkuh_plugin_manager *ptClone)
 : m_ptOpenPlugins(NULL)
{
	std::vector<muhkuh_plugin*>::const_iterator iter;
	muhkuh_plugin *ptPluginOrg;
	muhkuh_plugin *ptPluginClone;


	// set prefix for messages
	setMe();

	// create the repository list
	m_ptOpenPlugins = new std::vector<muhkuh_plugin*>;

	// loop over all plugins
	iter = ptClone->m_ptOpenPlugins->begin();
	while( iter!=ptClone->m_ptOpenPlugins->end() )
	{
		// clone the plugin
		ptPluginOrg = *iter;
		ptPluginClone = new muhkuh_plugin(ptPluginOrg);
		if( ptPluginClone!=NULL )
		{
			// add the new plugin to the list
			m_ptOpenPlugins->push_back(ptPluginClone);
		}

		// next entry
		++iter;
	}
}


muhkuh_plugin_manager::~muhkuh_plugin_manager(void)
{
	// unload all plugins
	if( m_ptOpenPlugins!=NULL )
	{
		closeAllPlugins();
		delete m_ptOpenPlugins;
	}
}


void muhkuh_plugin_manager::setMe(void)
{
	m_strMe.Printf(wxT("muhkuh_plugin_manager(%p) :"), this);
}


long muhkuh_plugin_manager::addPlugin(wxString strPluginCfgName)
{
	muhkuh_plugin *ptPlugin;
	long lPluginIdx;


	// assume failure
	lPluginIdx = -1;

	// try to create the plugin
	ptPlugin = new muhkuh_plugin(strPluginCfgName);
	// check if the plugin was loaded
	if( ptPlugin==NULL )
	{
		wxLogError(m_strMe + _("failed to create plugin '%s'"), strPluginCfgName.c_str());
	}
	else
	{
		lPluginIdx = m_ptOpenPlugins->size();

		// plugin is ok, add it to the list
		m_ptOpenPlugins->push_back(ptPlugin);
	}

	return lPluginIdx;
}


void muhkuh_plugin_manager::removePlugin(unsigned long ulIdx)
{
	std::vector<muhkuh_plugin*>::iterator iter;
	muhkuh_plugin *ptPluginIf;


	// check input parameter
	if( ulIdx>=m_ptOpenPlugins->size() )
	{
		wxLogError(m_strMe + wxT("removePlugin : ") + _("idx %ld is out of range, ignoring request"), ulIdx);
	}
	else
	{
		iter = m_ptOpenPlugins->begin();
		iter += ulIdx;
		ptPluginIf = *iter;
		// remove from list
		m_ptOpenPlugins->erase(iter);
		// unload and delete plugin
		delete ptPluginIf;
	}
}


size_t muhkuh_plugin_manager::getPluginCount(void) const
{
	return m_ptOpenPlugins->size();
}


const MUHKUH_PLUGIN_DESCRIPTION_T *muhkuh_plugin_manager::getPluginDescription(unsigned long ulIdx) const
{
	std::vector<muhkuh_plugin*>::const_iterator iter;
	muhkuh_plugin *ptPluginIf;
	const MUHKUH_PLUGIN_DESCRIPTION_T *ptDesc;


	// check input parameter
	if( ulIdx>=m_ptOpenPlugins->size() )
	{
		wxLogError(m_strMe + wxT("getPluginDescription : ") + _("idx %ld is out of range, ignoring request"), ulIdx);

		ptDesc = NULL;
	}
	else
	{
		iter = m_ptOpenPlugins->begin();
		iter += ulIdx;
		ptPluginIf = *iter;

//		ptDesc = ptPluginIf->fn_get_desc();
		wxLogError(m_strMe + wxT("getPluginDescription not yet"));
		ptDesc = NULL;
	}

	return ptDesc;
}


wxString muhkuh_plugin_manager::GetConfigName(unsigned long ulIdx) const
{
	std::vector<muhkuh_plugin*>::const_iterator iter;
	muhkuh_plugin *ptPluginIf;
	wxString strResult;


	// check input parameter
	if( ulIdx>=m_ptOpenPlugins->size() )
	{
		wxLogError(m_strMe + wxT("GetConfigName : ") + _("idx %ld is out of range, ignoring request"), ulIdx);
	}
	else
	{
		iter = m_ptOpenPlugins->begin();
		iter += ulIdx;
		ptPluginIf = *iter;

//		strResult = ptPluginIf->GetConfigName();
		strResult = wxEmptyString;
		wxLogError(m_strMe + wxT("GetConfigName not yet"));
	}

	return strResult;
}


void muhkuh_plugin_manager::SetEnable(unsigned long ulIdx, bool fPluginIsEnabled)
{
	std::vector<muhkuh_plugin*>::const_iterator iter;
	muhkuh_plugin *ptPluginIf;


	// check input parameter
	if( ulIdx>=m_ptOpenPlugins->size() )
	{
		wxLogError(m_strMe + wxT("SetEnable : ") + _("idx %ld is out of range, ignoring request"), ulIdx);
	}
	else
	{
		iter = m_ptOpenPlugins->begin();
		iter += ulIdx;
		ptPluginIf = *iter;
		ptPluginIf->SetEnable(fPluginIsEnabled);
	}
}


bool muhkuh_plugin_manager::GetEnable(unsigned long ulIdx) const
{
	std::vector<muhkuh_plugin*>::const_iterator iter;
	muhkuh_plugin *ptPluginIf;
	bool fPluginIsEnabled;


	// check input parameter
	if( ulIdx>=m_ptOpenPlugins->size() )
	{
		wxLogError(m_strMe + wxT("SetEnable : ") + _("idx %ld is out of range, ignoring request"), ulIdx);

		fPluginIsEnabled = false;
	}
	else
	{
		iter = m_ptOpenPlugins->begin();
		iter += ulIdx;
		ptPluginIf = *iter;
		fPluginIsEnabled = ptPluginIf->GetEnable();
	}

	return fPluginIsEnabled;
}


bool muhkuh_plugin_manager::IsOk(unsigned long ulIdx) const
{
	std::vector<muhkuh_plugin*>::const_iterator iter;
	muhkuh_plugin *ptPluginIf;
	bool fPluginIsOk;


	// check input parameter
	if( ulIdx>=m_ptOpenPlugins->size() )
	{
		wxLogError(m_strMe + wxT("IsOk : ") + _("idx %ld is out of range, ignoring request"), ulIdx);
		fPluginIsOk = false;
	}
	else
	{
		iter = m_ptOpenPlugins->begin();
		iter += ulIdx;
		ptPluginIf = *iter;
		fPluginIsOk = ptPluginIf->IsOk();
	}

	return fPluginIsOk;
}


wxString muhkuh_plugin_manager::GetInitError(unsigned long ulIdx) const
{
	std::vector<muhkuh_plugin*>::const_iterator iter;
	muhkuh_plugin *ptPluginIf;
	wxString strInitError;


	// check input parameter
	if( ulIdx>=m_ptOpenPlugins->size() )
	{
		strInitError.Printf(m_strMe + wxT("SetEnable : ") + _("idx %ld is out of range, ignoring request"), ulIdx);
		wxLogError(strInitError);
	}
	else
	{
		iter = m_ptOpenPlugins->begin();
		iter += ulIdx;
		ptPluginIf = *iter;
		strInitError = ptPluginIf->GetInitError();
	}

	return strInitError;
}


void muhkuh_plugin_manager::read_config(wxConfigBase *pConfig)
{
	size_t sizPluginCnt;
	wxString strPluginEntry;
	bool fFoundPath;
	muhkuh_plugin *ptPlugin;


	// close all plugins
	closeAllPlugins();

	// get all plugins
	pConfig->SetPath(wxT("/Plugins"));
	sizPluginCnt = 0;
	do
	{
		strPluginEntry.Printf(wxT("plugin_%d"), sizPluginCnt);
		fFoundPath = pConfig->HasGroup(strPluginEntry);
		if( fFoundPath==true )
		{
			// one more entry found
			++sizPluginCnt;
			// set new path
			pConfig->SetPath(strPluginEntry);
			// try to create a plugin from the settings
			ptPlugin = new muhkuh_plugin(pConfig);
			if( ptPlugin!=NULL )
			{
				// add the plugin to the list
				m_ptOpenPlugins->push_back(ptPlugin);
			}
			// go one path element back
			pConfig->SetPath(wxT(".."));
		}
	} while( fFoundPath==true );
}


void muhkuh_plugin_manager::write_config(wxConfigBase *pConfig)
{
	wxString strPluginEntry;
	std::vector<muhkuh_plugin*>::const_iterator iter;
	size_t sizPluginCnt;
	muhkuh_plugin *ptPlugin;


	// Delete the complete 'Plugins' path to clear all old entries.
	// Without this, old entries would pop up at the end of the list when you delete a repository.
	pConfig->DeleteGroup(wxT("/Plugins"));

	// save all repositories
	if( m_ptOpenPlugins!=NULL )
	{
		pConfig->SetPath(wxT("/Plugins"));
		iter = m_ptOpenPlugins->begin();
		sizPluginCnt = 0;
		while( iter!=m_ptOpenPlugins->end() )
		{
			strPluginEntry.Printf(wxT("plugin_%d"), sizPluginCnt);
			// set new path
			pConfig->SetPath(strPluginEntry);
			// get the plugin object
			ptPlugin = *iter;
			// write all settings
			ptPlugin->write_config(pConfig);
			// go one path element back
			pConfig->SetPath(wxT(".."));
			// next plugin
			++sizPluginCnt;
			++iter;
		}
		pConfig->SetPath(wxT("/"));
	}
}


void muhkuh_plugin_manager::closeAllPlugins(void)
{
	std::vector<muhkuh_plugin*>::const_iterator iter;
	muhkuh_plugin *ptPluginIf;


	if( m_ptOpenPlugins!=NULL )
	{
		// loop over all plugins
		iter = m_ptOpenPlugins->begin();
		while( iter!=m_ptOpenPlugins->end() )
		{
			ptPluginIf = *iter;
			delete ptPluginIf;
			// next entry
			++iter;
		}
		// clear the complete list
		m_ptOpenPlugins->clear();
	}
}


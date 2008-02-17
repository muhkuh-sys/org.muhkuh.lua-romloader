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

#include <wx/regex.h>


muhkuh_plugin_manager::muhkuh_plugin_manager(void)
 : m_ptOpenPlugins(NULL)
 , m_ptMatchingPlugins(NULL)
{
	// create the repository list
	m_ptOpenPlugins = new std::vector<muhkuh_plugin*>;

	// create the match list
	m_ptMatchingPlugins = new std::vector<muhkuh_plugin_instance*>;
}


muhkuh_plugin_manager::muhkuh_plugin_manager(const muhkuh_plugin_manager *ptClone)
 : m_ptOpenPlugins(NULL)
 , m_ptMatchingPlugins(NULL)
{
	std::vector<muhkuh_plugin*>::const_iterator iter;
	muhkuh_plugin *ptPluginOrg;
	muhkuh_plugin *ptPluginClone;


	// create the repository list
	m_ptOpenPlugins = new std::vector<muhkuh_plugin*>;

	// create the match list
	m_ptMatchingPlugins = new std::vector<muhkuh_plugin_instance*>;

	/* loop over all plugins */
	iter = ptClone->m_ptOpenPlugins->begin();
	while( iter!=ptClone->m_ptOpenPlugins->end() )
	{
		/* clone the plugin */
		ptPluginOrg = *iter;
		ptPluginClone = new muhkuh_plugin(ptPluginOrg);
		if( ptPluginClone!=NULL )
		{
			/* add the new plugin to the list */
			m_ptOpenPlugins->push_back(ptPluginClone);
		}

		/* next entry */
		++iter;
	}
}


muhkuh_plugin_manager::~muhkuh_plugin_manager(void)
{
	/* unload all plugins */
	if( m_ptOpenPlugins!=NULL )
	{
		closeAllPlugins();
		delete m_ptOpenPlugins;
	}

	// delete match list
	if( m_ptMatchingPlugins!=NULL )
	{
		ClearAllMatches();
		delete m_ptMatchingPlugins;
	}
}


void muhkuh_plugin_manager::disableBrokenPlugin(muhkuh_plugin *ptPlugin, wxString strPluginName, wxString strError)
{
	wxString strMsg;


	/* disable plugin */
	if( ptPlugin!=NULL )
	{
		ptPlugin->SetEnable(false);
	}
	/* show warning in log and in a messagebox */
	strMsg.Printf(_("Disabled the plugin '%s': %s"), strPluginName.fn_str(), strError.fn_str());
	wxLogError(strMsg);
	wxMessageBox(strMsg, strPluginName, wxICON_ERROR, NULL);
}


long muhkuh_plugin_manager::addPlugin(wxString strPluginCfgName)
{
	muhkuh_plugin *ptPlugin;
	long lPluginIdx;


	/* assume failure */
	lPluginIdx = -1;

	/* try to create the plugin */
	ptPlugin = new muhkuh_plugin(strPluginCfgName);
	/* check if the plugin was loaded */
	if( ptPlugin==NULL )
	{
		wxLogError(_("failed to create plugin '%s'"), strPluginCfgName.fn_str());
	}
	else
	{
		lPluginIdx = m_ptOpenPlugins->size();

		/* plugin is ok, add it to the list */
		m_ptOpenPlugins->push_back(ptPlugin);
	}

	return lPluginIdx;
}


void muhkuh_plugin_manager::removePlugin(unsigned long ulIdx)
{
	std::vector<muhkuh_plugin*>::iterator iter;
	muhkuh_plugin *ptPluginIf;


	/* check input parameter */
	if( ulIdx>=m_ptOpenPlugins->size() )
	{
		wxLogError(_("muhkuh_plugin_manager::removePlugin : idx %ld is out of range, ignoring request"), ulIdx);
	}
	else
	{
		iter = m_ptOpenPlugins->begin();
		iter += ulIdx;
		ptPluginIf = *iter;
		/* remove from list */
		m_ptOpenPlugins->erase(iter);
		/* unload and delete plugin */
		delete ptPluginIf;
	}
}


size_t muhkuh_plugin_manager::getPluginCount(void) const
{
	return m_ptOpenPlugins->size();
}


const muhkuh_plugin_desc *muhkuh_plugin_manager::getPluginDescription(unsigned long ulIdx) const
{
	std::vector<muhkuh_plugin*>::const_iterator iter;
	muhkuh_plugin *ptPluginIf;
	const muhkuh_plugin_desc *ptDesc;


	/* check input parameter */
	if( ulIdx>=m_ptOpenPlugins->size() )
	{
		wxLogError(_("muhkuh_plugin_manager::getPluginDescription : idx %ld is out of range, ignoring request"), ulIdx);

		ptDesc = NULL;
	}
	else
	{
		iter = m_ptOpenPlugins->begin();
		iter += ulIdx;
		ptPluginIf = *iter;
		ptDesc = ptPluginIf->fn_get_desc();
	}

	return ptDesc;
}


wxString muhkuh_plugin_manager::GetConfigName(unsigned long ulIdx) const
{
	std::vector<muhkuh_plugin*>::const_iterator iter;
	muhkuh_plugin *ptPluginIf;
	wxString strResult;


	/* check input parameter */
	if( ulIdx>=m_ptOpenPlugins->size() )
	{
		wxLogError(_("muhkuh_plugin_manager::GetConfigName : idx %ld is out of range, ignoring request"), ulIdx);
	}
	else
	{
		iter = m_ptOpenPlugins->begin();
		iter += ulIdx;
		ptPluginIf = *iter;
		strResult = ptPluginIf->GetConfigName();
	}

	return strResult;
}


void muhkuh_plugin_manager::SetEnable(unsigned long ulIdx, bool fPluginIsEnabled)
{
	std::vector<muhkuh_plugin*>::const_iterator iter;
	muhkuh_plugin *ptPluginIf;


	/* check input parameter */
	if( ulIdx>=m_ptOpenPlugins->size() )
	{
		wxLogError(_("muhkuh_plugin_manager::SetEnable : idx %ld is out of range, ignoring request"), ulIdx);
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


	/* check input parameter */
	if( ulIdx>=m_ptOpenPlugins->size() )
	{
		wxLogError(_("muhkuh_plugin_manager::SetEnable : idx %ld is out of range, ignoring request"), ulIdx);

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


	/* check input parameter */
	if( ulIdx>=m_ptOpenPlugins->size() )
	{
		wxLogError(_("muhkuh_plugin_manager::IsOk : idx %ld is out of range, ignoring request"), ulIdx);
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


	/* check input parameter */
	if( ulIdx>=m_ptOpenPlugins->size() )
	{
		strInitError.Printf(_("muhkuh_plugin_manager::SetEnable : idx %ld is out of range, ignoring request"), ulIdx);
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


int muhkuh_plugin_manager::initLuaBindings(wxLuaState *ptLuaState)
{
	std::vector<muhkuh_plugin*>::iterator iter;
	muhkuh_plugin *ptPlugin;
	bool fResult;
	int iResult;
	const muhkuh_plugin_desc *ptDesc;
	wxString strPluginName;
	wxString strMsg;


	/* loop over all plugins */
	iter = m_ptOpenPlugins->begin();
	while( iter!=m_ptOpenPlugins->end() )
	{
		/* get the plugin */
		ptPlugin = *iter;
		if( ptPlugin!=NULL )
		{
			/* get the plugin name for messages */
			ptDesc = ptPlugin->fn_get_desc();
			strPluginName = ptDesc->strPluginName;

			fResult = ptPlugin->GetEnable();
			if( fResult==true )
			{
				fResult = ptPlugin->IsOk();
				if( fResult!=true )
				{
					/* disable plugin and show error */
					disableBrokenPlugin(ptPlugin, strPluginName, _("state is not ok"));
				}
				else
				{
					/* init lua bindings */
					iResult = ptPlugin->fn_init_lua(ptLuaState);
					if( iResult!=0 )
					{
						/* disable plugin and show error */
						disableBrokenPlugin(ptPlugin, strPluginName, _("lua binding init failed"));
					}
				}
			}
		}

		/* next entry */
		++iter;
	}

	return 0;
}


bool muhkuh_plugin_manager::ScanPlugins(wxString strPattern)
{
	wxRegEx re;
	std::vector<muhkuh_plugin*>::iterator iter;
	muhkuh_plugin *ptPlugin;
	bool fResult;
	int iResult;
	const muhkuh_plugin_desc *ptDesc;
	wxString strPluginName;
	wxString strPluginId;


	// no list -> no matches
	if( m_ptMatchingPlugins==NULL )
	{
		return false;
	}

	// clear all old matches
	ClearAllMatches();

	// invalidate iterator
	m_cMatchingPluginsIter = m_ptMatchingPlugins->begin();

	// setup a regexp for the search pattern
	fResult = re.Compile(strPattern, wxRE_NEWLINE);
	if( fResult==false )
	{
		// failed to compile regex
		return false;
	}

	// loop over all plugins
	iter = m_ptOpenPlugins->begin();
	while( iter!=m_ptOpenPlugins->end() )
	{
		// get the plugin
		ptPlugin = *iter;
		if( ptPlugin!=NULL )
		{
			// get the plugin name for messages
			ptDesc = ptPlugin->fn_get_desc();
			strPluginName = ptDesc->strPluginName;
			// get the plugin id for the match
			strPluginId = ptDesc->strPluginId;

			fResult = ptPlugin->GetEnable();
			if( fResult==true )
			{
				fResult = ptPlugin->IsOk();
				if( fResult!=true )
				{
					/* disable plugin and show error */
					disableBrokenPlugin(ptPlugin, strPluginName, _("state is not ok"));
				}
				else
				{
					// does the plugin name match the pattern?
					fResult = re.Matches(strPluginId);
					if( fResult==true )
					{
						// pattern matches -> scan for available plugin instances
						iResult = ptPlugin->fn_detect_interfaces(m_ptMatchingPlugins);
						if( iResult<0 )
						{
							/* disable plugin and show error */
							disableBrokenPlugin(ptPlugin, strPluginName, _("failed to scan for interfaces"));
						}
					}
				}
			}
		}

		/* next entry */
		++iter;
	}

	// init iterator
	m_cMatchingPluginsIter = m_ptMatchingPlugins->begin();

	return true;
}


muhkuh_plugin_instance *muhkuh_plugin_manager::GetNextPlugin(void)
{
	muhkuh_plugin_instance *ptInstance = NULL;


	// no list -> no matches
	if( m_ptMatchingPlugins!=NULL && m_cMatchingPluginsIter!=m_ptMatchingPlugins->end() )
	{
		ptInstance = *m_cMatchingPluginsIter;
		++m_cMatchingPluginsIter;
	}

	return ptInstance;
}


void muhkuh_plugin_manager::ClearAllMatches(void)
{
	std::vector<muhkuh_plugin_instance*>::const_iterator iter;
	muhkuh_plugin_instance *ptPluginInst;


	if( m_ptMatchingPlugins!=NULL )
	{
		// loop over all plugins
		iter = m_ptMatchingPlugins->begin();
		while( iter!=m_ptMatchingPlugins->end() )
		{
			ptPluginInst = *iter;
			delete ptPluginInst;
			// next entry
			++iter;
		}
		// clear the complete list
		m_ptMatchingPlugins->clear();
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


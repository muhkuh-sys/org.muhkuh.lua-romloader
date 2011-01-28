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


#include <stddef.h>

#include <vector>

#include <wx/defs.h>
#include <wx/filename.h>


#include "muhkuh_plugin.h"


#ifndef __MUHKUH_PLUGIN_MANAGER_H__
#define __MUHKUH_PLUGIN_MANAGER_H__


class muhkuh_plugin_manager
{
public:
	muhkuh_plugin_manager(void);
	muhkuh_plugin_manager(const muhkuh_plugin_manager *ptClone);
	~muhkuh_plugin_manager(void);

	long addPlugin(wxString strPluginCfgName);
	void removePlugin(unsigned long ulIdx);

	size_t getPluginCount(void) const;
	const muhkuh_plugin_desc *getPluginDescription(unsigned long ulIdx) const;

	wxString GetConfigName(unsigned long ulIdx) const;

	bool IsOk(unsigned long ulIdx) const;
	wxString GetInitError(unsigned long ulIdx) const;
	void SetEnable(unsigned long ulIdx, bool fPluginIsEnabled);
	bool GetEnable(unsigned long ulIdx) const;

	void read_config(wxConfigBase *pConfig);
	void write_config(wxConfigBase *pConfig);

	bool initLuaBindings(wxLuaState *ptLuaState);

	void ScanPlugins(wxString strPattern, wxLuaState *ptLuaState);
	muhkuh_plugin_instance *GetNextPlugin(void);
	void ClearAllMatches(void);

	typedef struct
	{
		const wxChar *pcSymbolName;
		size_t sizOffset;
	} muhkuh_plugin_symbol_offset_t;

private:
	// set prefix for messages
	void setMe(void);

	bool open_plugin(wxString strPluginName, muhkuh_plugin_interface *ptPluginIf);
	void closeAllPlugins(void);
	void close_plugin(muhkuh_plugin_interface *ptPluginIf);

	static const muhkuh_plugin_symbol_offset_t atPluginSymbolOffsets[6];

	// all open plugins
	std::vector<muhkuh_plugin*> *m_ptOpenPlugins;

	// all matches for one instance scan
	std::vector<muhkuh_plugin_instance*> *m_ptMatchingPlugins;
	// iterator for instance scan
	std::vector<muhkuh_plugin_instance*>::const_iterator m_cMatchingPluginsIter;

	// prefix for messages
	wxString m_strMe;
};


#endif	/* __MUHKUH_PLUGIN_MANAGER_H__ */


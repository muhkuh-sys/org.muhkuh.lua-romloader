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


#include <vector>

#include <wx/defs.h>
#include <wx/confbase.h>
#include <wx/dynlib.h>
#include <wx/filename.h>


#include "plugins/muhkuh_plugin_interface.h"


#ifndef __MUHKUH_PLUGIN_H__
#define __MUHKUH_PLUGIN_H__


class muhkuh_plugin
{
public:
	muhkuh_plugin(void);
	muhkuh_plugin(wxString strPluginCfgPath);
	muhkuh_plugin(const muhkuh_plugin *ptCloneMe);
	muhkuh_plugin(wxConfigBase *pConfig);
	~muhkuh_plugin(void);

	wxString GetConfigName(void) const;

	bool IsOk(void) const;
	wxString GetInitError(void) const;

	void SetEnable(bool fPluginIsEnabled);
	bool GetEnable(void) const;

	void write_config(wxConfigBase *pConfig);

	bool Load(wxString strPluginCfgPath);

	int fn_init_lua(wxLuaState *ptLuaState);
	const muhkuh_plugin_desc *fn_get_desc(void) const;
	int fn_detect_interfaces(std::vector<muhkuh_plugin_instance*> *pvInterfaceList);

	typedef struct
	{
		const wxChar *pcSymbolName;
		size_t sizOffset;
	} muhkuh_plugin_symbol_offset_t;

private:
	// set prefix for messages
	void setMe(void);

	bool openXml(wxString strXmlPath);
	bool open(wxString strPluginPath);
	void close(void);

	int fn_init(wxLog *ptLogTarget, wxXmlNode *ptCfgNode, wxString &strPluginId);
	int fn_leave(void);

	void setInitError(wxString strMessage, wxString strPath);


	// list of symbols for the plugin interface
	static const muhkuh_plugin_symbol_offset_t atPluginSymbolOffsets[];

	muhkuh_plugin_desc tPluginDesc;

	bool m_fPluginIsEnabled;
	muhkuh_plugin_interface m_tPluginIf;

	wxLuaState *m_ptLuaState;

	// full path to the plugins xml configuration
	wxString m_strPluginCfgPath;
	// the xml configuration
	wxXmlDocument m_xmldoc;
	// plugin name from the xml config (defaults to m_strPluginCfgPath)
	wxString m_strCfgName;
	// config node for the plugin settings (here the plugin reads/writes the settings)
	wxXmlNode *m_ptCfgNode;
	// complete path to the plugin dll
	wxString m_strSoName;
	// plugin id from the config xml
	wxString m_strPluginId;

	// plugin state
	bool m_fPluginIsOk;
	// init error
	wxString m_strInitError;

	// prefix for messages
	wxString m_strMe;
};


#endif	/* __MUHKUH_PLUGIN_H__ */


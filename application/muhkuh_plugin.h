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
#include <wx/filename.h>
#include <wx/xml/xml.h>


#if USE_LUA!=0
extern "C" {
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"
}
#endif


#ifndef __MUHKUH_PLUGIN_H__
#define __MUHKUH_PLUGIN_H__


typedef struct
{
	wxString strPluginName;                                                                                                                                                                                      
	wxString strPluginId;
	unsigned int uiVersionMajor;                                                                                                                                                                                                     
	unsigned int uiVersionMinor;                                                                                                                                                                                                     
	unsigned int uiVersionSub;
} MUHKUH_PLUGIN_DESCRIPTION_T;


class muhkuh_plugin
{
public:
	muhkuh_plugin(void);
	muhkuh_plugin(wxString strPluginCfgPath);
	muhkuh_plugin(const muhkuh_plugin *ptCloneMe);
	muhkuh_plugin(wxConfigBase *pConfig);
	~muhkuh_plugin(void);

	const MUHKUH_PLUGIN_DESCRIPTION_T *GetPluginDescription(void) const;

	bool IsOk(void) const;
	wxString GetInitError(void) const;

	void SetEnable(bool fPluginIsEnabled);
	bool GetEnable(void) const;

	void write_config(wxConfigBase *pConfig);

	bool Load(wxString strPluginCfgPath);
#if USE_LUA!=0
	wxString GetLuaModuleName(void) const;
#endif

private:
	// set prefix for messages
	void setMe(void);

	wxXmlNode *find_child_node(wxXmlNode *ptParentNode, wxString strNodeName);
	bool openXml(wxString strXmlPath);
	bool open(wxString strPluginPath);
//	void close(void);

	void setInitError(wxString strMessage, wxString strPath);


	bool m_fPluginIsEnabled;

	/* Full path and filename of the plugin's xml configuration. */
	wxString m_strPluginCfgFullPath;

	/* Path of the plugin's xml configuration without the filename. */
	wxString m_strPluginCfgPath;

	/* The complete xml document. */
	wxXmlDocument m_xmldoc;

	/* The plugin description from the xml file. */
	MUHKUH_PLUGIN_DESCRIPTION_T m_tPluginDescription;

#if USE_LUA!=0
	/* Lua specific name of the module. */
	wxString m_strLuaModuleName;
#endif

	/* Plugin state. */
	bool m_fPluginIsOk;
	/* Init error message. */
	wxString m_strInitError;

	/* Common prefix for messages. */
	wxString m_strMe;
};


#endif  /* __MUHKUH_PLUGIN_H__ */


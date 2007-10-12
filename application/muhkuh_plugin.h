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

#include <wx/wx.h>
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
	muhkuh_plugin(wxString strPluginPath);
	muhkuh_plugin(const muhkuh_plugin *ptCloneMe);
	muhkuh_plugin(wxConfigBase *pConfig);
	~muhkuh_plugin(void);

	bool Load(wxString strPluginPath);
	bool IsOk(void);

	void SetEnable(bool fPluginIsEnabled);
	bool GetEnable(void);

	void write_config(wxConfigBase *pConfig);

	int fn_init_lua(wxLuaState *ptLuaState);
	const muhkuh_plugin_desc *fn_get_desc(void);
	int fn_detect_interfaces(std::vector<muhkuh_plugin_instance*> *pvInterfaceList);

	typedef struct
	{
		const wxChar *pcSymbolName;
		size_t sizOffset;
	} muhkuh_plugin_symbol_offset_t;

private:
	bool open(wxString strPluginPath);
	void close(void);

	int fn_init(wxLog *ptLogTarget);
	int fn_leave(void);

	static const muhkuh_plugin_symbol_offset_t atPluginSymbolOffsets[];

	wxString m_strPluginPath;
	bool m_fPluginIsEnabled;
	muhkuh_plugin_interface m_tPluginIf;

	wxLuaState *m_ptLuaState;
};


#endif	/* __MUHKUH_PLUGIN_H__ */


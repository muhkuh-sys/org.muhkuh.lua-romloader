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

#include "muhkuh_components.h"

#include "muhkuh_config_data.h"
#include "muhkuh_lua.h"
#include "muhkuh_plugin_manager.h"
#include "muhkuh_repository_manager.h"
#include "muhkuh_version.h"


#define SCRIPT_PUSH_ERROR(L,...) { lua_pushfstring(L,__VA_ARGS__); }
#define SCRIPT_EXIT_ERROR(L) { lua_error(L); }
#define SCRIPT_ERROR(L,...) { lua_pushfstring(L,__VA_ARGS__); lua_error(L); }


static const char *pcMuhkuhVersion =
{
	MUHKUH_APPLICATION_NAME " " MUHKUH_VERSION_STRING
};


const char *get_version(void)
{
	return pcMuhkuhVersion;
}


int plugin_count(lua_State *ptLuaState)
{
	size_t sizResult;
	muhkuh_plugin_manager *ptPluginManager;


	ptPluginManager = lua_muhkuh_get_plugin_manager(ptLuaState);
	if( ptPluginManager!=NULL )
	{
		sizResult = ptPluginManager->getPluginCount();
	}
	else
	{
		SCRIPT_ERROR(ptLuaState, "Failed to get plugin manager!");
	}

	return sizResult;
}


bool plugin_is_ok(lua_State *ptLuaState, unsigned long ulIdx)
{
	bool fResult;
	muhkuh_plugin_manager *ptPluginManager;


	ptPluginManager = lua_muhkuh_get_plugin_manager(ptLuaState);
	if( ptPluginManager!=NULL )
	{
		fResult = ptPluginManager->IsOk(ulIdx);
	}
	else
	{
		SCRIPT_ERROR(ptLuaState, "Failed to get plugin manager!");
	}

	return fResult;
}


void plugin_set_enable(lua_State *ptLuaState, unsigned long ulIdx, bool fPluginIsEnabled)
{
	muhkuh_plugin_manager *ptPluginManager;


	ptPluginManager = lua_muhkuh_get_plugin_manager(ptLuaState);
	if( ptPluginManager!=NULL )
	{
		ptPluginManager->SetEnable(ulIdx, fPluginIsEnabled);
	}
	else
	{
		SCRIPT_ERROR(ptLuaState, "Failed to get plugin manager!");
	}
}


bool plugin_get_enable(lua_State *ptLuaState, unsigned long ulIdx)
{
	bool fResult;
	muhkuh_plugin_manager *ptPluginManager;


	ptPluginManager = lua_muhkuh_get_plugin_manager(ptLuaState);
	if( ptPluginManager!=NULL )
	{
		fResult = ptPluginManager->GetEnable(ulIdx);
	}
	else
	{
		SCRIPT_ERROR(ptLuaState, "Failed to get plugin manager!");
	}

	return fResult;
}


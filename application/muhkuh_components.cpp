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


const char *get_lua_script_path(void)
{
	const char *pcLuaScriptPath;
	muhkuh_config_data *ptCfgData;


	/* Expect failure. */
	pcLuaScriptPath = NULL;

	ptCfgData = lua_muhkuh_get_config_data();
	if( ptCfgData!=NULL )
	{
		pcLuaScriptPath = ptCfgData->m_strLuaIncludePath.fn_str();
	}

	return pcLuaScriptPath;
}


const char *get_lua_module_path(void)
{
	const char *pcLuaModulePath;
	muhkuh_config_data *ptCfgData;


	/* Expect failure. */
	pcLuaModulePath = NULL;

	ptCfgData = lua_muhkuh_get_config_data();
	if( ptCfgData!=NULL )
	{
		pcLuaModulePath = ptCfgData->m_strLuaSystemModulePath.fn_str();
	}

	return pcLuaModulePath;
}


/* Get all plugins as a table with the plugin name as the key and a table with
 * the plugin attributes as the value.
 */
void get_plugins(lua_State *MUHKUH_SWIG_OUTPUT_CUSTOM_OBJECT)
{
	lua_State *ptLuaState = MUHKUH_SWIG_OUTPUT_CUSTOM_OBJECT;
	muhkuh_plugin_manager *ptPluginManager;
	size_t sizPlugins;
	size_t sizPluginCnt;
	const MUHKUH_PLUGIN_DESCRIPTION_T *ptPluginDescription;


	ptPluginManager = lua_muhkuh_get_plugin_manager(ptLuaState);
	if( ptPluginManager==NULL )
	{
		SCRIPT_ERROR(ptLuaState, "Failed to get plugin manager!");
	}
	else
	{
		/* Get the number of plugins. */
		sizPlugins = ptPluginManager->getPluginCount();
		fprintf(stderr, "%d plugins\n", sizPlugins);

		/* Crate a new table. */
		lua_createtable(ptLuaState, sizPlugins, 0);

		/* Loop over all plugins. */
		sizPluginCnt = 0;
		while( sizPluginCnt<sizPlugins )
		{
			ptPluginDescription = ptPluginManager->getPluginDescription(sizPluginCnt);

			/* Push the index for the entry.
			 * NOTE: this starts at 1 and not at 0.
			 */
			lua_pushnumber(ptLuaState, sizPluginCnt+1);
			/* Create the attribute table for the plugin. */
			if( ptPluginDescription==NULL )
			{
				lua_pushnil(ptLuaState);
			}
			else
			{
				lua_createtable(ptLuaState, 0, 0);

				/* Push the plugin name. */
				lua_pushstring(ptLuaState, "name");
				lua_pushstring(ptLuaState, ptPluginDescription->strPluginName.fn_str());
				lua_rawset(ptLuaState, -3);

				/* Push the "is_ok" flag. */
				lua_pushstring(ptLuaState, "is_ok");
				lua_pushboolean(ptLuaState, ptPluginManager->IsOk(sizPluginCnt) ? 1 : 0 );
				lua_rawset(ptLuaState, -3);

				/* Push the "enabled" flag. */
				lua_pushstring(ptLuaState, "enabled");
				lua_pushboolean(ptLuaState, ptPluginManager->GetEnable(sizPluginCnt) ? 1 : 0 );
				lua_rawset(ptLuaState, -3);

				/* Push the plugin id. */
				lua_pushstring(ptLuaState, "id");
				lua_pushstring(ptLuaState, ptPluginDescription->strPluginId.fn_str());
				lua_rawset(ptLuaState, -3);

				/* Push the plugin version major. */
				lua_pushstring(ptLuaState, "ver_maj");
				lua_pushnumber(ptLuaState, ptPluginDescription->uiVersionMajor);
				lua_rawset(ptLuaState, -3);

				/* Push the plugin version minor. */
				lua_pushstring(ptLuaState, "ver_min");
				lua_pushnumber(ptLuaState, ptPluginDescription->uiVersionMinor);
				lua_rawset(ptLuaState, -3);

				/* Push the plugin version sub. */
				lua_pushstring(ptLuaState, "ver_sub");
				lua_pushnumber(ptLuaState, ptPluginDescription->uiVersionSub);
				lua_rawset(ptLuaState, -3);

				/* Push the plugin lua module name. */
				lua_pushstring(ptLuaState, "module");
				lua_pushstring(ptLuaState, ptPluginDescription->strLuaModuleName.fn_str());
				lua_rawset(ptLuaState, -3);

				/* Push the plugin init error. */
				lua_pushstring(ptLuaState, "init_error");
				lua_pushstring(ptLuaState, ptPluginManager->GetInitError(sizPluginCnt).fn_str());
				lua_rawset(ptLuaState, -3);
			}
			lua_rawset(ptLuaState, -3);

			++sizPluginCnt;
		}
	}
}


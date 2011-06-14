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


extern "C" {
#       include "lua.h"
#       include "lauxlib.h"
#       include "lualib.h"
}


#ifndef __MUHKUH_LUA_H__
#define __MUHKUH_LUA_H__


/* This is the default state for the main frame. */
//extern lua_State *ptDefaultState;

class muhkuh_mainFrame;
void lua_muhkuh_register_mainframe(muhkuh_mainFrame *ptFrame);

lua_State *lua_muhkuh_create_state(void);
void lua_muhkuh_create_default_state(void);
void lua_muhkuh_close_default_state(void);

int lua_muhkuh_get_memory_usage(lua_State *ptLuaState);

int lua_muhkuh_generate_text(lua_State *ptLuaState, const char *pcLuaCode, char **ppcResult);
int lua_muhkuh_run_code(lua_State *ptLuaState, const char *pcLuaCode, char **ppcResult);

const char *lua_muhkuh_error_to_string(int iLuaError);
const char *lua_muhkuh_type_to_string(int iLuaType);

const char *get_version(void);

int plugin_count(lua_State *ptLuaState);
const char *plugin_get_config_name(lua_State *ptLuaState, unsigned long ulIdx);
bool plugin_is_ok(lua_State *ptLuaState, unsigned long ulIdx);
const char *plugin_get_init_error(lua_State *ptLuaState, unsigned long ulIdx);
void plugin_set_enable(lua_State *ptLuaState, unsigned long ulIdx, bool fPluginIsEnabled);
bool plugin_get_enable(lua_State *ptLuaState, unsigned long ulIdx);

#endif  /* __MUHKUH_LUA_H__ */

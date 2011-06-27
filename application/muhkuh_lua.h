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


#include "lua.hpp"


#ifndef __MUHKUH_LUA_H__
#define __MUHKUH_LUA_H__

#include "muhkuh_config_data.h"
#include "muhkuh_so_api.h"


void MUHKUH_SO_API lua_muhkuh_register_config_data(muhkuh_config_data *ptConfigData);
muhkuh_config_data *lua_muhkuh_get_config_data(void);
muhkuh_plugin_manager MUHKUH_SO_API *lua_muhkuh_get_plugin_manager(lua_State *ptLuaState);

lua_State MUHKUH_SO_API *lua_muhkuh_create_state(void);
void MUHKUH_SO_API lua_muhkuh_create_default_state(void);
void MUHKUH_SO_API lua_muhkuh_close_default_state(void);

int MUHKUH_SO_API lua_muhkuh_get_memory_usage(lua_State *ptLuaState);

int MUHKUH_SO_API lua_muhkuh_generate_text(lua_State *ptLuaState, const char *pcLuaCode, char **ppcResult);
int MUHKUH_SO_API lua_muhkuh_run_code(lua_State *ptLuaState, const char *pcLuaCode, char **ppcResult);

const char MUHKUH_SO_API *lua_muhkuh_error_to_string(int iLuaError);
const char MUHKUH_SO_API *lua_muhkuh_type_to_string(int iLuaType);


#endif  /* __MUHKUH_LUA_H__ */

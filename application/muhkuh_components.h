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


#include "lua.hpp"


#ifndef __MUHKUH_COMPONENTS_H__
#define __MUHKUH_COMPONENTS_H__


const char *get_version(void);

int plugin_count(lua_State *ptLuaState);
const char *plugin_get_config_name(lua_State *ptLuaState, unsigned long ulIdx);
bool plugin_is_ok(lua_State *ptLuaState, unsigned long ulIdx);
const char *plugin_get_init_error(lua_State *ptLuaState, unsigned long ulIdx);
void plugin_set_enable(lua_State *ptLuaState, unsigned long ulIdx, bool fPluginIsEnabled);
bool plugin_get_enable(lua_State *ptLuaState, unsigned long ulIdx);

#endif  /* __MUHKUH_COMPONENTS_H__ */

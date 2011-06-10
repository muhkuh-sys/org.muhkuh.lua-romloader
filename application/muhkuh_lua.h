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


lua_State *lua_muhkuh_create_state(void);

const char *lua_muhkuh_error_to_string(int iLuaError);
const char *lua_muhkuh_type_to_string(int iLuaType);

const char *get_version(void);


#endif  /* __MUHKUH_LUA_H__ */

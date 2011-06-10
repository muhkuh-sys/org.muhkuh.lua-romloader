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


#include "muhkuh_version.h"

#include <wx/log.h>

#include "muhkuh_lua.h"


typedef struct
{
	int iLuaError;
	const char *pcMessage;
} LUA_ERROR_TO_STR_T;


typedef struct
{
	int iLuaError;
	const char *pcMessage;
} LUA_TYPE_TO_STR_T;


static const char *pcMuhkuhVersion =
{
	MUHKUH_APPLICATION_NAME " " MUHKUH_VERSION_STRING
};



static const LUA_ERROR_TO_STR_T atLuaErrorToString[] =
{
	{ 0,                    "" },
	{ LUA_YIELD,            "Thread is suspended" },
	{ LUA_ERRRUN,           "Error while running chunk" },
	{ LUA_ERRSYNTAX,        "Syntax error during pre-compilation" },
	{ LUA_ERRMEM,           "Memory allocation error" },
	{ LUA_ERRERR,           "Generic error or an error occurred while running the error handler" },
	{ LUA_ERRFILE,          "Error occurred while opening file" }
};


static const LUA_TYPE_TO_STR_T atLuaTypeToString[] =
{
	{ LUA_TNIL,             "nil" },
	{ LUA_TBOOLEAN,         "boolean" },
	{ LUA_TLIGHTUSERDATA,   "light userdata" },
	{ LUA_TNUMBER,          "number" },
	{ LUA_TSTRING,          "string" },
	{ LUA_TTABLE,           "table" },
	{ LUA_TFUNCTION,        "function" },
	{ LUA_TUSERDATA,        "userdata" },
	{ LUA_TTHREAD,          "thread" }
};


static int lua_muhkuh_panic(lua_State *ptLuaState)
{
	wxLogError(wxT("LUA PANIC: unprotected error in call to Lua API (%s)"), lua_tostring(ptLuaState, -1));
	return 0;
}


static int lua_muhkuh_print(lua_State *ptLuaState)
{
	int iArgMax;
	int iArgCnt;
	wxString strElement;
	wxString strMsg;


	iArgMax = lua_gettop(ptLuaState);
	for(iArgCnt=1; iArgCnt<=iArgMax; iArgCnt++)
	{
		/* Separate arguments with tabs. */
		if( iArgCnt>1 )
		{	
			strElement = wxT("\t");
		}

		if( lua_isstring(ptLuaState,iArgCnt) )
		{
			strElement = wxString::FromAscii( lua_tostring(ptLuaState,iArgCnt) );
		}
		else if( lua_isnil(ptLuaState,iArgCnt) )
		{
			strElement = wxT("nil");
		}
		else if( lua_isboolean(ptLuaState,iArgCnt) )
		{
			strElement = ( lua_toboolean(ptLuaState,iArgCnt) ? wxT("true") : wxT("false") );
		}
		else
		{
			strElement.Printf(wxT("%s:%p"),luaL_typename(ptLuaState,iArgCnt),lua_topointer(ptLuaState,iArgCnt));
		}

		strMsg.Append(strElement);
	}

	wxLogMessage(strMsg);

	return 0;
}


lua_State *lua_muhkuh_create_state(void)
{
	lua_State *ptLuaState;


	/* Create the new state with a standard memory allocator. */
	ptLuaState = luaL_newstate();
	if( ptLuaState!=NULL )
	{
		/* Stop GC during initialization */
		lua_gc(ptLuaState, LUA_GCSTOP, 0);

		/* Add the standard libs. */
		luaL_openlibs(ptLuaState);

		/* Replace the print function with lua_muhkuh_print. */
//		lua_register(ptLuaState, "print", lua_muhkuh_print);

		/* TODO: add the muhkuh binding. */

		/* Init done, restart GC. */
		lua_gc(ptLuaState, LUA_GCRESTART, 0);
	}

	return ptLuaState;
}


const char *lua_muhkuh_error_to_string(int iLuaError)
{
	const char *pcMessage;


	if( iLuaError<(sizeof(atLuaErrorToString)/sizeof(atLuaErrorToString[0])) )
	{
		pcMessage = atLuaErrorToString[iLuaError].pcMessage;
	}
	else
	{
		pcMessage = "Unknown lua error code";
	}

	return pcMessage;
}


const char *lua_muhkuh_type_to_string(int iLuaType)
{
	const char *pcType;


	if( iLuaType<(sizeof(atLuaTypeToString)/sizeof(atLuaTypeToString[0])) )
	{
		pcType = atLuaTypeToString[iLuaType].pcMessage;
	}
	else
	{
		pcType = "Unknown lua type";
	}

	return pcType;
}


const char *get_version(void)
{
	return pcMuhkuhVersion;
}


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
extern "C" {
int luaopen_muhkuh_app(lua_State* L);
}

#ifdef WIN32
#define snprintf _snprintf
#endif


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


/* This is the default state for the main frame. */
lua_State *ptDefaultState = NULL;


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


	strMsg = wxT("[LUA]: ");

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
		lua_register(ptLuaState, "print", lua_muhkuh_print);

		/* Add the muhkuh binding. */
		luaopen_muhkuh_app(ptLuaState);

		/* Init done, restart GC. */
		lua_gc(ptLuaState, LUA_GCRESTART, 0);
	}

	return ptLuaState;
}


void lua_muhkuh_create_default_state(void)
{
	/* Close any existing lua state. */
	lua_muhkuh_close_default_state();

	/* Create a new lua state. */
	ptDefaultState = lua_muhkuh_create_state();
}


void lua_muhkuh_close_default_state(void)
{
	if( ptDefaultState!=NULL )
	{
		lua_close(ptDefaultState);
	}
}


int lua_muhkuh_get_memory_usage(lua_State *ptLuaState)
{
	int iResult;


	if( ptLuaState==NULL )
	{
		ptLuaState = ptDefaultState;
	}

	if( ptLuaState==NULL )
	{
		iResult = 0;
	}
	else
	{
		/* Get the lua memory consumption in kilobytes. */
		iResult = lua_getgccount(ptLuaState);
	}

	return iResult;
}


static char *lua_muhkuh_pop_errormessage(lua_State *ptLuaState)
{
	const char *pcLuaErrorMessage;
	int iResult;
	size_t sizResult;
	char *pcResult;
	const char *pcErrorFormat = "failed to load lua script, error %d: %s, %s";


	/* Get the errormessage from the stack. */
	pcLuaErrorMessage = lua_tostring(ptLuaState, -1);

	/* Test how many bytes are needed for the message. */
	sizResult = snprintf(NULL, 0, pcErrorFormat, iResult, lua_muhkuh_error_to_string(iResult), pcLuaErrorMessage);
	/* Allocate space for the message and the terminating 0 byte. */
	pcResult = (char*)malloc(sizResult+1);
	if( pcResult!=NULL )
	{
		snprintf(pcResult, sizResult, pcErrorFormat, iResult, lua_muhkuh_error_to_string(iResult), pcLuaErrorMessage);
	}
	lua_pop(ptLuaState, 1);

	return pcResult;
}


int lua_muhkuh_execute_html_tag(lua_State *ptLuaState, const char *pcLuaCode, char **ppcResult)
{
	int iResult;
	char *pcResult;
	const char *pcLuaResult;
	size_t sizLuaResult;


	pcResult = NULL;
	if( ptLuaState==NULL )
	{
		ptLuaState = ptDefaultState;
	}

	if( ptLuaState==NULL )
	{
		iResult = -1;
	}
	else
	{
		iResult = luaL_loadstring(ptLuaState, pcLuaCode);
		if( iResult!=0 )
		{
			pcResult = lua_muhkuh_pop_errormessage(ptLuaState);
		}
		else
		{
			iResult = lua_pcall(ptLuaState, 0, 1, 0);
			if( iResult!=0 )
			{
				pcResult = lua_muhkuh_pop_errormessage(ptLuaState);
			}
			else
			{
				/* Get the string and its size. */
				pcLuaResult = lua_tolstring(ptLuaState, -1, &sizLuaResult);
				/* The size does not include the trailing 0. Allocate and copy it too. */
				++sizLuaResult;
				/* Allocate new memory for the string. */
				pcResult = (char*)malloc(sizLuaResult);
				if( pcResult!=NULL )
				{
					/* Copy the string. */
					memcpy(pcResult, pcLuaResult, sizLuaResult);
				}
				/* Remove the string from the stack. */
				lua_pop(ptLuaState, 1);
			}
		}
	}

	*ppcResult = pcResult;

	return iResult;
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


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


#include <wx/log.h>
 
#include "muhkuh_lua.h"


#ifdef WIN32
#       define snprintf _snprintf
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
static lua_State *g_ptDefaultState = NULL;


static const LUA_ERROR_TO_STR_T atLuaErrorToString[] =
{
	{ 0,                    "Ok" },
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


static muhkuh_config_data *g_ptConfigData = NULL;


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


void lua_muhkuh_register_config_data(muhkuh_config_data *ptConfigData)
{
	g_ptConfigData = ptConfigData;
}


muhkuh_config_data *lua_muhkuh_get_config_data(void)
{
	return g_ptConfigData;
}


muhkuh_plugin_manager *lua_muhkuh_get_plugin_manager(lua_State *ptLuaState)
{
	muhkuh_plugin_manager *ptPluginManager;


	ptPluginManager = NULL;

	if( g_ptConfigData!=NULL )
	{
		ptPluginManager = g_ptConfigData->m_ptPluginManager;
	}

	return ptPluginManager;
}


static void lua_set_system_include_path(lua_State *ptLuaState)
{
	int iResult;
	size_t sizOldCpath;
	const char *pcOldCpath;
	wxString strLuaSystemModulePath;
	const char *pcLuaSystemModulePath;
	size_t sizSystemPath;
	size_t sizNewCpath;
	char *pcNewCpath;


	/* Get the lua system module path. */
	if( g_ptConfigData==NULL )
	{
		wxLogError("No config data registered!");
	}
	else
	{
		strLuaSystemModulePath = g_ptConfigData->m_strLuaSystemModulePath;
	
		/* Get the global "package". */
		lua_getglobal(ptLuaState, "package");
		/* This must be a table. */
		iResult = lua_istable(ptLuaState, -1);
		if( iResult!=1 )
		{
			wxLogError("package is no table!");
		}
		else
		{
			lua_pushstring(ptLuaState, "cpath");
			lua_rawget(ptLuaState, -2);
			iResult = lua_isstring(ptLuaState, -1);
			if( iResult!=1 )
			{
				/* Remove "cpath" from the stack. */
				lua_pop(ptLuaState, 1);

				wxLogError("package.cpath is no string!");
			}
			else
			{
				/* Get the string. */
				pcOldCpath = lua_tolstring(ptLuaState, -1, &sizOldCpath);

				/* Allocate space for the new cpath string. */
				sizSystemPath = strLuaSystemModulePath.Len();
				/* The new string consists of the old cpath, a semicolon, the system path and the trailing 0 byte. */
				sizNewCpath = sizOldCpath + 1 + sizSystemPath + 1;
				pcNewCpath = (char*)malloc(sizNewCpath);
				if( pcNewCpath==NULL )
				{
					wxLogError("Failed to allocate %d bytes for the cpath buffer", sizNewCpath);

					/* Remove "cpath" from the stack. */
					lua_pop(ptLuaState, 1);
				}
				else
				{
					/* Start with the old cpath. */
					memcpy(pcNewCpath, pcOldCpath, sizOldCpath);
					/* Append the semicolon. */
					pcNewCpath[sizOldCpath] = ';';
					/* Append the system path. */
					pcLuaSystemModulePath = strLuaSystemModulePath.fn_str();
					memcpy(pcNewCpath+sizOldCpath+1, pcLuaSystemModulePath, sizSystemPath+1);
					wxLogMessage("New cpath: '%s'", pcNewCpath);

					/* Remove old cpath from the stack. */
					lua_pop(ptLuaState, 1);

					/* Push the new cpath on the stack. */
					lua_pushstring(ptLuaState, "cpath");
					lua_pushstring(ptLuaState, pcNewCpath);
					lua_rawset(ptLuaState, -3);

					free(pcNewCpath);
				}
			}
		}
		/* Remove "package" from the stack. */
		lua_pop(ptLuaState, 1);
	}
}


lua_State *lua_muhkuh_create_state(void)
{
	lua_State *ptLuaState;
	int iResult;
	char *pcResult;


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

		/* Set the system include path. */
		lua_set_system_include_path(ptLuaState);

		/* Add the muhkuh binding. */
		iResult = lua_muhkuh_run_code(ptLuaState, "require(\"muhkuh_components_lua\")\n", &pcResult);
		if( iResult!=0 )
		{
			wxLogError(_("Failed to init muhkuh components LUA binding!"));
			if( pcResult!=NULL )
			{
				wxLogError(wxString::FromAscii(pcResult));
				free(pcResult);
			}
		}

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
	g_ptDefaultState = lua_muhkuh_create_state();
}


void lua_muhkuh_close_default_state(void)
{
	if( g_ptDefaultState!=NULL )
	{
		lua_close(g_ptDefaultState);
	}
}


int lua_muhkuh_get_memory_usage(lua_State *ptLuaState)
{
	int iResult;


	if( ptLuaState==NULL )
	{
		ptLuaState = g_ptDefaultState;
	}

	if( ptLuaState==NULL )
	{
		iResult = 0;
	}
	else
	{
		/* Get the lua memory consumption in kilobytes. */
		iResult = lua_gc(ptLuaState, LUA_GCCOUNT, 0);
	}

	return iResult;
}


static char *lua_muhkuh_pop_errormessage(lua_State *ptLuaState, int iResult)
{
	const char *pcLuaErrorMessage;
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
		snprintf(pcResult, sizResult+1, pcErrorFormat, iResult, lua_muhkuh_error_to_string(iResult), pcLuaErrorMessage);
	}
	lua_pop(ptLuaState, 1);

	return pcResult;
}


int lua_muhkuh_generate_text(lua_State *ptLuaState, const char *pcLuaCode, char **ppcResult)
{
	int iResult;
	char *pcResult;
	const char *pcLuaResult;
	size_t sizLuaResult;
	int iLuaResult;


	pcResult = NULL;
	if( ptLuaState==NULL )
	{
		ptLuaState = g_ptDefaultState;
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
			pcResult = lua_muhkuh_pop_errormessage(ptLuaState, iResult);
		}
		else
		{
			iResult = lua_pcall(ptLuaState, 0, 1, 0);
			if( iResult!=0 )
			{
				pcResult = lua_muhkuh_pop_errormessage(ptLuaState, iResult);
			}
			/* NOTE: lua_isstring returns 1 for strnigs and numbers. */
			else if( lua_isstring(ptLuaState, -1)==1 )
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
			else if( lua_isnil(ptLuaState, -1)==1 )
			{
				pcResult = strdup("nil");
			}
			else if( lua_isboolean(ptLuaState, -1)==1 )
			{
				iLuaResult = lua_toboolean(ptLuaState, -1);
				if( iLuaResult==0 )
				{
					pcResult = strdup("false");
				}
				else
				{
					pcResult = strdup("true");
				}
				lua_pop(ptLuaState, 1);
			}
			else
			{
				pcResult = strdup("the function returned an invalid type!");
			}
		}
	}

	*ppcResult = pcResult;

	return iResult;
}


int lua_muhkuh_run_code(lua_State *ptLuaState, const char *pcLuaCode, char **ppcResult)
{
	int iResult;
	char *pcResult;
	const char *pcLuaResult;
	size_t sizLuaResult;
	int iLuaResult;


	pcResult = NULL;
	if( ptLuaState==NULL )
	{
		ptLuaState = g_ptDefaultState;
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
			pcResult = lua_muhkuh_pop_errormessage(ptLuaState, iResult);
		}
		else
		{
			iResult = lua_pcall(ptLuaState, 0, 0, 0);
			if( iResult!=0 )
			{
				pcResult = lua_muhkuh_pop_errormessage(ptLuaState, iResult);
			}
		}
	}

	*ppcResult = pcResult;

	return iResult;
}


const char *lua_muhkuh_error_to_string(int iLuaError)
{
	const LUA_ERROR_TO_STR_T *ptCnt;
	const LUA_ERROR_TO_STR_T *ptEnd;
	const char *pcMessage;


	pcMessage = "Unknown lua error code";

	ptCnt = atLuaErrorToString;
	ptEnd = atLuaErrorToString + (sizeof(atLuaErrorToString)/sizeof(atLuaErrorToString[0]));
	while( ptCnt<ptEnd )
	{
		if( iLuaError==ptCnt->iLuaError )
		{
			pcMessage = ptCnt->pcMessage;
		}
		++ptCnt;
	}

	fprintf(stderr, "LUA Error %d: %s\n", iLuaError, pcMessage);
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


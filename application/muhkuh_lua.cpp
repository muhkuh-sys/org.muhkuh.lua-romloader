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
#include "muhkuh_version.h"


#if defined(_MSC_VER)
#       define snprintf _snprintf
#endif


#define SCRIPT_PUSH_ERROR(L,...) { lua_pushfstring(L,__VA_ARGS__); }
#define SCRIPT_EXIT_ERROR(L) { lua_error(L); }
#define SCRIPT_ERROR(L,...) { lua_pushfstring(L,__VA_ARGS__); lua_error(L); }


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


static muhkuh_config_data *lua_muhkuh_get_config_data(void)
{
	return g_ptConfigData;
}


static muhkuh_plugin_manager *lua_muhkuh_get_plugin_manager(lua_State *ptLuaState)
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
					pcLuaSystemModulePath = strLuaSystemModulePath.c_str();
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


static const char *pcMuhkuhVersion =
{
	MUHKUH_APPLICATION_NAME " " MUHKUH_VERSION_STRING
};

static int lua_muhkuh_fn_get_version(lua_State *ptLuaState)
{
	lua_pushstring(ptLuaState, pcMuhkuhVersion);
	return 1;
}


static int lua_muhkuh_fn_get_lua_script_path(lua_State *ptLuaState)
{
	const char *pcLuaScriptPath;
	muhkuh_config_data *ptCfgData;


	ptCfgData = lua_muhkuh_get_config_data();
	if( ptCfgData!=NULL )
	{
		pcLuaScriptPath = ptCfgData->m_strLuaIncludePath.c_str();
		lua_pushstring(ptLuaState, pcLuaScriptPath);
	}
	else
	{
		lua_pushnil(ptLuaState);
	}

	return 1;
}


static int lua_muhkuh_fn_get_lua_module_path(lua_State *ptLuaState)
{
	const char *pcLuaModulePath;
	muhkuh_config_data *ptCfgData;


	ptCfgData = lua_muhkuh_get_config_data();
	if( ptCfgData!=NULL )
	{
		pcLuaModulePath = ptCfgData->m_strLuaSystemModulePath.c_str();
		lua_pushstring(ptLuaState, pcLuaModulePath);
	}
	else
	{
		lua_pushnil(ptLuaState);
	}

	return 1;
}


static int lua_muhkuh_fn_get_plugins(lua_State *ptLuaState)
{
	muhkuh_plugin_manager *ptPluginManager;
	size_t sizPlugins;
	size_t sizPluginCnt;
	const MUHKUH_PLUGIN_DESCRIPTION_T *ptPluginDescription;


	ptPluginManager = lua_muhkuh_get_plugin_manager(ptLuaState);
	if( ptPluginManager==NULL )
	{
		lua_pushnil(ptLuaState);
		//SCRIPT_ERROR(ptLuaState, "Failed to get plugin manager!");
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
				lua_pushstring(ptLuaState, ptPluginDescription->strPluginName.c_str());
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
				lua_pushstring(ptLuaState, ptPluginDescription->strPluginId.c_str());
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
				lua_pushstring(ptLuaState, ptPluginDescription->strLuaModuleName.c_str());
				lua_rawset(ptLuaState, -3);

				/* Push the plugin init error. */
				lua_pushstring(ptLuaState, "init_error");
				lua_pushstring(ptLuaState, ptPluginManager->GetInitError(sizPluginCnt).c_str());
				lua_rawset(ptLuaState, -3);
			}
			lua_rawset(ptLuaState, -3);

			++sizPluginCnt;
		}
	}

	return 1;
}


typedef struct
{
	const char *pcName;
	lua_CFunction pfn;
} LUA_EXTENSION_FUNCTIONS_T;

static LUA_EXTENSION_FUNCTIONS_T atLuaMuhkuhExtensionFunctions[4] =
{
	{ "get_version",                        lua_muhkuh_fn_get_version },
	{ "get_lua_script_path",                lua_muhkuh_fn_get_lua_script_path },
	{ "get_lua_module_path",                lua_muhkuh_fn_get_lua_module_path },
	{ "get_plugins",                        lua_muhkuh_fn_get_plugins }
};


static void lua_muhkuh_register_muhkuh(lua_State *ptLuaState)
{
	size_t sizFns;
	LUA_EXTENSION_FUNCTIONS_T *ptCnt;
	LUA_EXTENSION_FUNCTIONS_T *ptEnd;


	/* Get the number of extension functions. */
	sizFns = sizeof(atLuaMuhkuhExtensionFunctions)/sizeof(atLuaMuhkuhExtensionFunctions[0]);

	/* Create a new table with 0 array elements and 4 non-array elements. */
	lua_createtable(ptLuaState, 0, sizFns);

	/* Loop over all extension functions and add them to the table. */
	ptCnt = atLuaMuhkuhExtensionFunctions;
	ptEnd = ptCnt + sizFns;
	while( ptCnt<ptEnd )
	{
		/* Create a table entry. */
		lua_pushstring(ptLuaState, ptCnt->pcName);
		lua_pushcclosure(ptLuaState, ptCnt->pfn, 0);
		lua_rawset(ptLuaState, -3);

		++ptCnt;
	}

	/* Set the table to the global "muhkuh_application". */
	lua_setglobal(ptLuaState, "muhkuh_application");
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

		/* Add the "muhkuh" table. */
		lua_muhkuh_register_muhkuh(ptLuaState);

		/* Set the system include path. */
		lua_set_system_include_path(ptLuaState);

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


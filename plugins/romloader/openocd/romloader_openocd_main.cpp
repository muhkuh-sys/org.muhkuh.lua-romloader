/***************************************************************************
 *   Copyright (C) 2007 by Christoph Thelen                                *
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


#include "romloader_openocd_main.h"
#include "_luaif/romloader_openocd_wxlua_bindings.h"


#ifdef _WIN32
#define vsnprintf _vsnprintf
#endif

/*-------------------------------------*/
/* openocd includes */

extern "C"
{
	#include "config.h"
	#include "log.h"
	#include "types.h"
	#include "jtag.h"
	#include "configuration.h"
	#include "interpreter.h"
	#include "xsvf.h"
	#include "target.h"
	#include "command.h"
	#include "replacements.h"
}

/*-------------------------------------*/
/* some local prototypes */

bool fn_connect(void *pvHandle);
void fn_disconnect(void *pvHandle);
bool fn_is_connected(void *pvHandle);
int fn_read_data08(void *pvHandle, unsigned long ulNetxAddress, unsigned char *pucData);
int fn_read_data16(void *pvHandle, unsigned long ulNetxAddress, unsigned short *pusData);
int fn_read_data32(void *pvHandle, unsigned long ulNetxAddress, unsigned long *pulData);
int fn_read_image(void *pvHandle, unsigned long ulNetxAddress, char *pcData, unsigned long ulSize, lua_State *L, int iLuaCallbackTag, void *pvCallbackUserData);
int fn_write_data08(void *pvHandle, unsigned long ulNetxAddress, unsigned char ucData);
int fn_write_data16(void *pvHandle, unsigned long ulNetxAddress, unsigned short usData);
int fn_write_data32(void *pvHandle, unsigned long ulNetxAddress, unsigned long ulData);
int fn_write_image(void *pvHandle, unsigned long ulNetxAddress, const char *pcData, unsigned long ulSize, lua_State *L, int iLuaCallbackTag, void *pvCallbackUserData);
int fn_call(void *pvHandle, unsigned long ulNetxAddress, unsigned long ulParameterR0, lua_State *L, int iLuaCallbackTag, void *pvCallbackUserData);

/*-------------------------------------*/

static muhkuh_plugin_desc plugin_desc =
{
	wxT("OpenOCD Bootloader"),
	wxT(""),
	{ 0, 0, 1 }
};

const romloader_functioninterface tFunctionInterface =
{
	fn_connect,
	fn_disconnect,
	fn_is_connected,
	fn_read_data08,
	fn_read_data16,
	fn_read_data32,
	fn_read_image,
	fn_write_data08,
	fn_write_data16,
	fn_write_data32,
	fn_write_image,
	fn_call
};

static wxLuaState *m_ptLuaState;

static wxArrayString astrInitCfg;
static wxArrayString astrRunCfg;

/*-------------------------------------*/

int romloader_openocd_default_output_handler(struct command_context_s *context, char* line)
{
	wxString strMsg;


	strMsg.Printf(wxT("romloader_openocd(%p)") + wxString::FromAscii(line), context->output_handler_priv);
	wxLogMessage(strMsg);

	return ERROR_OK;
}


void romloader_openocd_log_printf(enum log_levels level, const char *format, ...)
{
	wxString strMsg;
	va_list args;
	char buffer[512];


	if (level > debug_level)
		return;

	va_start(args, format);
	vsnprintf(buffer, 512, format, args);

	strMsg = wxString::FromAscii(buffer);
	switch(level)
	{
	case LOG_ERROR:
		wxLogError(strMsg);
		break;
	case LOG_WARNING:
		wxLogWarning(strMsg);
		break;
	case LOG_INFO:
		wxLogMessage(strMsg);
		break;
	case LOG_DEBUG:
		wxLogDebug(strMsg);
		break;
	default:
		wxLogError(wxT("unknown errorlevel, ") + strMsg);
		break;
	}

	va_end(args);
}


void romloader_openocd_short_log_printf(enum log_levels level, const char *format, ...)
{
	wxString strMsg;
	va_list args;
	char buffer[512];


	if (level > debug_level)
		return;

	va_start(args, format);
	vsnprintf(buffer, 512, format, args);

	strMsg = wxString::FromAscii(buffer);
	switch(level)
	{
	case LOG_ERROR:
		wxLogError(strMsg);
		break;
	case LOG_WARNING:
		wxLogWarning(strMsg);
		break;
	case LOG_INFO:
		wxLogMessage(strMsg);
		break;
	case LOG_DEBUG:
		wxLogDebug(strMsg);
		break;
	default:
		wxLogError(wxT("unknown errorlevel, ") + strMsg);
		break;
	}

	va_end(args);
}


void splitStringToArray(wxString &strLines, wxArrayString *ptArray)
{
	wxString strLine;
	size_t sizLineStart;
	size_t sizPos;
	size_t sizLen;
	wxChar c;


	sizPos = sizLineStart = 0;
	sizLen = strLines.Len();
	while( sizPos<sizLen )
	{
		// find next break
		c = strLines.GetChar(sizPos);
		++sizPos;
		if( c==wxT('\n') || c== wxT('\r') )
		{
			// found line end
			strLine = strLines.SubString(sizLineStart, sizPos);
			// trim string from both sides
			strLine.Trim(true);
			strLine.Trim(false);
			// add string to the array
			if( strLine.IsEmpty()==false )
			{
				ptArray->Add(strLine);
				wxLogMessage(wxT("romloader_openocd(") + plugin_desc.strPluginId + wxT(") : ") + strLine);
			}
			// new line starts after eol
			sizLineStart = sizPos;
		}
	}

	// chars left without newline?
	if( sizLineStart<sizPos )
	{
		strLine = strLines.SubString(sizLineStart, sizPos);
		// trim string from both sides
		strLine.Trim(true);
		strLine.Trim(false);
		if( strLine.IsEmpty()==false )
		{
			ptArray->Add(strLine);
			wxLogMessage(wxT("romloader_openocd(") + plugin_desc.strPluginId + wxT(") : ") + strLine);
		}
	}
}


int readXmlTextArray(wxXmlNode *ptCfgNode, wxString strNodeName, wxArrayString *ptArray)
{
	wxXmlNode *ptNode;
	wxString strCfg;
	int iResult;
	wxString strMsg;


	/* expect failure */
	iResult = 1;

	wxLogMessage(wxT("romloader_openocd(") + plugin_desc.strPluginId + wxT(") : reading section '") + strNodeName + wxT("'"));

	/* get the init node */
	if( ptCfgNode==NULL )
	{
		wxLogMessage(wxT("romloader_openocd(") + plugin_desc.strPluginId + wxT(") : missing config node!"));
	}
	else
	{
		ptNode = ptCfgNode->GetChildren();
		while( ptNode!=NULL )
		{
			if( ptNode->GetType()==wxXML_ELEMENT_NODE && ptNode->GetName()==strNodeName )
			{
				break;
			}
			ptNode = ptNode->GetNext();
		}
		// not found (node is NULL) ?
		if( ptNode==NULL )
		{
			wxLogError(wxT("romloader_openocd(") + plugin_desc.strPluginId + wxT(") : failed to find '") + strNodeName + wxT("' section in config!"));
		}
		else
		{
			strCfg = ptNode->GetNodeContent();
			if( strCfg.IsEmpty()!=false )
			{
				wxLogError(wxT("romloader_openocd(") + plugin_desc.strPluginId + wxT(") : empty '") + strNodeName + wxT("' section in config!"));
			}
			else
			{
				/* split text into lines */
				splitStringToArray(strCfg, ptArray);
				iResult = 0;
			}
		}
	}

	return iResult;
}


/*-------------------------------------*/

int fn_init(wxLog *ptLogTarget, wxXmlNode *ptCfgNode, wxString &strPluginId)
{
	wxLog *pOldLogTarget;
	int iResult;


	/* set main app's log target */
	pOldLogTarget = wxLog::GetActiveTarget();
	if( pOldLogTarget!=ptLogTarget )
	{
		wxLog::SetActiveTarget(ptLogTarget);
		if( pOldLogTarget!=NULL )
		{
			delete pOldLogTarget;
		}
	}

	/* say hi */
	wxLogMessage(wxT("bootloader openocd plugin init: ") + strPluginId);

	/* remember id */
	plugin_desc.strPluginId = strPluginId;

	/* init the lua state */
	m_ptLuaState = NULL;

	/* clear the command arrays */
	astrInitCfg.Empty();
	astrRunCfg.Empty();

	/* read the config file */
	iResult = readXmlTextArray(ptCfgNode, wxT("Init"), &astrInitCfg);
	if( iResult==0 )
	{
		iResult = readXmlTextArray(ptCfgNode, wxT("Run"), &astrRunCfg);
	}

	return iResult;
}


/*-------------------------------------*/

int fn_init_lua(wxLuaState *ptLuaState)
{
	bool fSuccess;


	/* remember the lua state for instance creation */
	m_ptLuaState = ptLuaState;

	/* init the lua bindings */
	fSuccess = wxLuaBinding_romloader_openocd_lua_init();
	if( fSuccess!=true )
	{
		wxLogMessage(wxT("failed to init romloader_openocd lua bindings"));
		return -1;
	}

	return 0;
}


/*-------------------------------------*/

int fn_leave(void)
{
	wxLogMessage(wxT("bootloader openocd plugin leave"));

	return 0;
}

/*-------------------------------------*/

const muhkuh_plugin_desc *fn_get_desc(void)
{
	return &plugin_desc;
}

/*-------------------------------------*/


static void romloader_openocd_close_instance(void *pvHandle)
{
	command_context_t *cmd_ctx;
	target_t *target;
	int iResult;
	wxString strMsg;


	/* cast the handle to the command context */
	cmd_ctx = (command_context_t*)pvHandle;

	strMsg.Printf(wxT("closing romloader openocd at %p"), cmd_ctx);
	wxLogMessage(strMsg);

	/* NOTE: this seems to work with ftd2xx, but not with libftdi */
	if( jtag!=NULL && jtag->quit!=NULL )
	{
		jtag->quit();
	}

	/* close all subsystems */
	iResult = jtag_close(cmd_ctx);
	if( iResult!=ERROR_OK )
	{
		strMsg.Printf(wxT("failed to close jtag interface: %d"), iResult);
		wxLogWarning(strMsg);
	}

	iResult = target_close(cmd_ctx);
	if( iResult!=ERROR_OK )
	{
		strMsg.Printf(wxT("failed to close target interface: %d"), iResult);
		wxLogWarning(strMsg);
	}

	// free commandline interface
	command_done(cmd_ctx);
}


/*-----------------------------------*/


static int romloader_openocd_connect(command_context_t **pptCmdCtx)
{
	int iResult;
	command_context_t *cmd_ctx;
	target_t *target;
	wxString strCmd;
	wxString strMsg;
	size_t sizCfgCnt;
	size_t sizCfgMax;
	int iInitCnt;


	cmd_ctx = command_init();

	/* register subsystem commands */
	log_register_commands(cmd_ctx);
	jtag_register_commands(cmd_ctx);
	interpreter_register_commands(cmd_ctx);
	xsvf_register_commands(cmd_ctx);
	target_register_commands(cmd_ctx);

	/* init the log functions */
	iResult = log_init(cmd_ctx);
	if( iResult!=ERROR_OK )
	{
		strMsg.Printf(wxT("failed to init log level: %d"), iResult);
		wxLogError(strMsg);
	}
	else
	{
		command_set_output_handler(cmd_ctx, romloader_openocd_default_output_handler, NULL);
		log_set_output_handler(romloader_openocd_log_printf, romloader_openocd_short_log_printf);
		cmd_ctx->mode = COMMAND_CONFIG;

		// set config
		sizCfgCnt = 0;
		sizCfgMax = astrInitCfg.GetCount();
		while( sizCfgCnt<sizCfgMax )
		{
			strCmd = astrInitCfg.Item(sizCfgCnt);
			wxLogMessage(wxT("command: ") + strCmd);
			iResult = command_run_line(cmd_ctx, strCmd.ToAscii());
			if( iResult!=ERROR_OK )
			{
				strMsg.Printf(wxT("failed to set config: %d"), iResult);
				wxLogError(strMsg);
				strMsg = wxT("error line was: '") + strCmd + wxT("'");
				wxLogError(strMsg);
				break;
			}
			++sizCfgCnt;
		}
		if( iResult==ERROR_OK )
		{
			cmd_ctx->mode = COMMAND_EXEC;

			iResult = jtag_init(cmd_ctx);
			if( iResult!=ERROR_OK )
			{
				strMsg.Printf(wxT("failed to init jtag: %d"), iResult);
				wxLogError(strMsg);
			}
			else
			{
				iResult = target_init(cmd_ctx);
				if( iResult!=ERROR_OK )
				{
					strMsg.Printf(wxT("failed to init jtag: %d"), iResult);
					wxLogError(strMsg);
				}
				else
				{
					wxMilliSleep(500);

					/* wait for target reset */
					iInitCnt = 10;
					do
					{
						target_call_timer_callbacks();
						wxMilliSleep(100);
					} while( --iInitCnt>0 );

					target = get_current_target(cmd_ctx);
					if( target->state!=TARGET_HALTED )
					{
						wxLogError(wxT("failed to halt the target"));
						iResult = ERROR_TARGET_NOT_HALTED;
					}
					else
					{
						// set config
						sizCfgCnt = 0;
						sizCfgMax = astrRunCfg.GetCount();
						while( sizCfgCnt<sizCfgMax )
						{
							strCmd = astrRunCfg.Item(sizCfgCnt);
							wxLogMessage(wxT("command: ") + strCmd);
							iResult = command_run_line(cmd_ctx, strCmd.ToAscii());
							if( iResult!=ERROR_OK )
							{
								strMsg.Printf(wxT("failed to run command: %d"), iResult);
								wxLogError(strMsg);
								strMsg = wxT("error line was: '") + strCmd + wxT("'");
								wxLogError(strMsg);
								break;
							}
							++sizCfgCnt;
						}
						if( iResult!=ERROR_OK )
						{
							wxLogError("config failed!");
						}
					}
				}
			}
		}
	}

	if( iResult!=ERROR_OK )
	{
		// close connection
		romloader_openocd_close_instance(cmd_ctx);
		*pptCmdCtx = NULL;
	}
	else
	{
		// connection open, ok!
		*pptCmdCtx = cmd_ctx;
	}

	return iResult;
}

/*-----------------------------------*/

int fn_detect_interfaces(std::vector<muhkuh_plugin_instance*> *pvInterfaceList)
{
	int iInterfaces;
	int iResult;
	muhkuh_plugin_instance *ptInst;
	command_context_t *cmd_ctx;
	wxString strName;
	wxString strTyp;
	wxString strLuaCreateFn;


	strTyp = plugin_desc.strPluginId;
	strLuaCreateFn = wxT("muhkuh.romloader_openocd_create");

	iInterfaces = 0;

	// detect interface by trying to open it
	iResult = romloader_openocd_connect(&cmd_ctx);
	if( iResult==ERROR_OK )
	{
		// close the instance
		romloader_openocd_close_instance(cmd_ctx);

		// construct the name
		strName.Printf("romloader_openocd");
		ptInst = new muhkuh_plugin_instance(strName, strTyp, false, strLuaCreateFn, NULL);
		++iInterfaces;

		// add the new instance to the list
		pvInterfaceList->push_back(ptInst);
	}

	return iInterfaces;
}


/*-------------------------------------*/


romloader *romloader_openocd_create(void *pvHandle)
{
	int iResult;
	command_context_t *cmd_ctx;
	wxString strTyp;
	wxString strName;
	romloader *ptInstance = NULL;


	iResult = romloader_openocd_connect(&cmd_ctx);
	if( iResult==ERROR_OK )
	{
		// create the new instance
		strTyp = plugin_desc.strPluginId;
		// TODO: add some info
		strName.Printf("romloader_openocd");
		ptInstance = new romloader(strName, strTyp, &tFunctionInterface, cmd_ctx, romloader_openocd_close_instance, m_ptLuaState);
	}

	return ptInstance;
}


/*-------------------------------------*/


static bool callback(lua_State *L, int iLuaCallbackTag, unsigned long ulProgressData, void *pvCallbackUserData)
{
	bool fStillRunning;
	int iOldTopOfStack;
	int iResult;
	int iLuaType;
	wxString strMsg;


	// check lua state and callback tag
	if( L!=NULL && iLuaCallbackTag!=0 )
	{
		// get the current stack position
		iOldTopOfStack = lua_gettop(L);
		// push the function tag on the stack
		lua_rawgeti(L, LUA_REGISTRYINDEX, iLuaCallbackTag);
		// push the arguments on the stack
		lua_pushnumber(L, ulProgressData);
		lua_pushnumber(L, (long)pvCallbackUserData);
		// call the function
		iResult = lua_pcall(L, 2, 1, 0);
		if( iResult!=0 )
		{
			switch( iResult )
			{
			case LUA_ERRRUN:
				strMsg = wxT("runtime error");
				break;
			case LUA_ERRMEM:
				strMsg = wxT("memory allocation error");
				break;
			default:
				strMsg.Printf(wxT("unknown errorcode: %d"), iResult);
				break;
			}
			wxLogError(wxT("callback function failed: ") + strMsg);
			strMsg = wxlua_getstringtype(L, -1);
			wxLogError(strMsg);
			wxLogError(wxT("cancel operation"));
			fStillRunning = false;
		}
		else
		{
			// get the function's return value
			iLuaType = lua_type(L, -1);
			if( wxlua_iswxluatype(iLuaType, WXLUA_TBOOLEAN)==false )
			{
				wxLogError(wxT("callback function returned a non-boolean type!"));
				fStillRunning = false;
			}
			else
			{
				if( iLuaType==LUA_TNUMBER )
				{
					iResult = lua_tonumber(L, -1);
				}
				else
				{
					iResult = lua_toboolean(L, -1);
				}
				fStillRunning = (iResult!=0);
			}
		}
		// return old stack top
		lua_settop(L, iOldTopOfStack);
	}
	else
	{
		// no callback function -> keep running
		fStillRunning = true;
	}

	return fStillRunning;
}


/*-------------------------------------*/


/* open the connection to the device */
bool fn_connect(void *pvHandle)
{
	return true;
}


/* close the connection to the device */
void fn_disconnect(void *pvHandle)
{
}


/* returns the connection state of the device */
bool fn_is_connected(void *pvHandle)
{
	return false;
}


/* read a byte (8bit) from the netx to the pc */
int fn_read_data08(void *pvHandle, unsigned long ulNetxAddress, unsigned char *pucData)
{
	command_context_t *cmd_ctx;
	target_t *target;
	int iResult;


	/* cast the handle to the command context */
	cmd_ctx = (command_context_t*)pvHandle;

	/* get the target from the command context */
	target = get_current_target(cmd_ctx);

	/* read the data from the netX */
	iResult = target_read_u8(target, ulNetxAddress, pucData);
	if( iResult==ERROR_OK )
	{
		iResult = 0;
	}
	else
	{
		iResult = 1;
	}

	wxMilliSleep(1);

	return iResult;
}


/* read a word (16bit) from the netx to the pc */
int fn_read_data16(void *pvHandle, unsigned long ulNetxAddress, unsigned short *pusData)
{
	command_context_t *cmd_ctx;
	target_t *target;
	int iResult;


	/* cast the handle to the command context */
	cmd_ctx = (command_context_t*)pvHandle;

	/* get the target from the command context */
	target = get_current_target(cmd_ctx);

	/* read the data from the netX */
	iResult = target_read_u16(target, ulNetxAddress, pusData);
	if( iResult==ERROR_OK )
	{
		iResult = 0;
	}
	else
	{
		iResult = 1;
	}

	wxMilliSleep(1);

	return iResult;
}


/* read a long (32bit) from the netx to the pc */
int fn_read_data32(void *pvHandle, unsigned long ulNetxAddress, unsigned long *pulData)
{
	command_context_t *cmd_ctx;
	target_t *target;
	int iResult;


	/* cast the handle to the command context */
	cmd_ctx = (command_context_t*)pvHandle;

	/* get the target from the command context */
	target = get_current_target(cmd_ctx);

	/* read the data from the netX */
	iResult = target_read_u32(target, ulNetxAddress, (u32*)pulData);
	if( iResult==ERROR_OK )
	{
		iResult = 0;
	}
	else
	{
		iResult = 1;
	}

	wxMilliSleep(1);

	return iResult;
}


/* read a byte array from the netx to the pc */
int fn_read_image(void *pvHandle, unsigned long ulNetxAddress, char *pcData, unsigned long ulSize, lua_State *L, int iLuaCallbackTag, void *pvCallbackUserData)
{
	command_context_t *cmd_ctx;
	target_t *target;
	int iResult;


	/* cast the handle to the command context */
	cmd_ctx = (command_context_t*)pvHandle;

	/* get the target from the command context */
	target = get_current_target(cmd_ctx);

	/* read the data from the netX */
	iResult = target_read_buffer(target, ulNetxAddress, ulSize, (u8*)pcData);
	if( iResult==ERROR_OK )
	{
		iResult = 0;
	}
	else
	{
		iResult = 1;
	}

	wxMilliSleep(1);

	return iResult;
}


/* write a byte (8bit) from the pc to the netx */
int fn_write_data08(void *pvHandle, unsigned long ulNetxAddress, unsigned char ucData)
{
	command_context_t *cmd_ctx;
	target_t *target;
	int iResult;


	/* cast the handle to the command context */
	cmd_ctx = (command_context_t*)pvHandle;

	/* get the target from the command context */
	target = get_current_target(cmd_ctx);

	/* read the data from the netX */
	iResult = target_write_u8(target, ulNetxAddress, ucData);
	if( iResult==ERROR_OK )
	{
		iResult = 0;
	}
	else
	{
		iResult = 1;
	}

	wxMilliSleep(1);

	return iResult;
}


/* write a word (16bit) from the pc to the netx */
int fn_write_data16(void *pvHandle, unsigned long ulNetxAddress, unsigned short usData)
{
	command_context_t *cmd_ctx;
	target_t *target;
	int iResult;


	/* cast the handle to the command context */
	cmd_ctx = (command_context_t*)pvHandle;

	/* get the target from the command context */
	target = get_current_target(cmd_ctx);

	/* read the data from the netX */
	iResult = target_write_u16(target, ulNetxAddress, usData);
	if( iResult==ERROR_OK )
	{
		iResult = 0;
	}
	else
	{
		iResult = 1;
	}

	wxMilliSleep(1);

	return iResult;
}


/* write a long (32bit) from the pc to the netx */
int fn_write_data32(void *pvHandle, unsigned long ulNetxAddress, unsigned long ulData)
{
	command_context_t *cmd_ctx;
	target_t *target;
	int iResult;


	/* cast the handle to the command context */
	cmd_ctx = (command_context_t*)pvHandle;

	/* get the target from the command context */
	target = get_current_target(cmd_ctx);

	/* read the data from the netX */
	iResult = target_write_u32(target, ulNetxAddress, ulData);
	if( iResult==ERROR_OK )
	{
		iResult = 0;
	}
	else
	{
		iResult = 1;
	}

	wxMilliSleep(1);

	return iResult;
}


/* write a byte array from the pc to the netx */
int fn_write_image(void *pvHandle, unsigned long ulNetxAddress, const char *pcData, unsigned long ulSize, lua_State *L, int iLuaCallbackTag, void *pvCallbackUserData)
{
	command_context_t *cmd_ctx;
	target_t *target;
	int iResult;


	/* cast the handle to the command context */
	cmd_ctx = (command_context_t*)pvHandle;

	/* get the target from the command context */
	target = get_current_target(cmd_ctx);

	/* write the data to the netX */
	iResult = target_write_buffer(target, ulNetxAddress, ulSize, (u8*)pcData);
	if( iResult==ERROR_OK )
	{
		iResult = 0;
	}
	else
	{
		iResult = 1;
	}

	wxMilliSleep(1);

	return iResult;
}


/* call routine */
int fn_call(void *pvHandle, unsigned long ulNetxAddress, unsigned long ulParameterR0, lua_State *L, int iLuaCallbackTag, void *pvCallbackUserData)
{
	command_context_t *cmd_ctx;
	int iOocdResult;
	int iResult;
	wxString strCmd;
	target_t *target;
	bool fIsRunning;
	enum target_state state;


	/* cast the handle to the command context */
	cmd_ctx = (command_context_t*)pvHandle;

	// expect failure
	iResult = 1;

	// set R0 parameter
	strCmd.Printf(wxT("reg r0 0x%08X"), ulParameterR0);
	iOocdResult = command_run_line(cmd_ctx, strCmd.ToAscii());
	if( iOocdResult!=ERROR_OK )
	{
		wxLogError("config failed!");
	}
	else
	{
		// resume <ulNetxAddress>
		strCmd.Printf(wxT("resume 0x%08X"), ulNetxAddress);
		iOocdResult = command_run_line(cmd_ctx, strCmd.ToAscii());
		if( iOocdResult!=ERROR_OK )
		{
			wxLogError("config failed!");
		}
		else
		{
			// grab messages here
			// TODO: redirect outputhandler, then grab messages, restore default output handler on halt

			// wait for halt
			target = get_current_target(cmd_ctx);
			do
			{
				wxMilliSleep(100);

				target->type->poll(target);
				state = target->state;
				if( state==TARGET_HALTED )
				{
					wxLogMessage(wxT("call finished!"));
					iResult = 0;
				}
				else
				{
					// execute callback
					fIsRunning = callback(L, iLuaCallbackTag, 0, pvCallbackUserData);
					if( fIsRunning!=true )
					{
						// operation was canceled, halt the target
						wxLogMessage(wxT("Call canceled, stopping target..."));
						target = get_current_target(cmd_ctx);
						iOocdResult = target->type->halt(target);
						if( iOocdResult!=ERROR_OK && iOocdResult!=ERROR_TARGET_ALREADY_HALTED )
						{
							wxLogError(wxT("Failed to halt target!"));
						}
						break;
					}
					else
					{
						target_call_timer_callbacks();
					}
				}
			} while( state!=TARGET_HALTED );

			// usb cmd delay
			wxMilliSleep(1);
		}
	}

	return iResult;
}


/*-------------------------------------*/


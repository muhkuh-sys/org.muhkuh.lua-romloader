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

const muhkuh_plugin_desc plugin_desc =
{
	"netX500 OpenOCD Bootloader",
	"romloader_openocd_netx500",
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

int fn_init(wxLog *ptLogTarget)
{
	wxLog *pOldLogTarget;


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
	wxLogMessage(wxT("bootloader openocd plugin init"));

	/* TODO: read this from the config file */

	/* netX500 init config */
	astrInitCfg.Add(wxT("interface ft2232"));
#ifdef __WXMSW__
	astrInitCfg.Add(wxT("ft2232_device_desc \"Amontec JTAGkey A\""));
#else
	astrInitCfg.Add(wxT("ft2232_device_desc \"Amontec JTAGkey\""));
#endif
	astrInitCfg.Add(wxT("ft2232_layout \"jtagkey\""));
	astrInitCfg.Add(wxT("ft2232_vid_pid 0x0403 0xcff8"));
	astrInitCfg.Add(wxT("jtag_speed 1"));
	astrInitCfg.Add(wxT("reset_config trst_and_srst"));
	astrInitCfg.Add(wxT("jtag_device 4 0x1 0xf 0xe"));
	astrInitCfg.Add(wxT("daemon_startup reset"));
	astrInitCfg.Add(wxT("target arm9tdmi little reset_halt 0 arm920t"));
	astrInitCfg.Add(wxT("working_area 0 0x00018000 0x8000 backup"));
	astrInitCfg.Add(wxT("run_and_halt_time 0 5000"));
	/* netX500 run config */
	astrRunCfg.Add(wxT("bp 0x200000 4 hw"));
	astrRunCfg.Add(wxT("reg cpsr 0xd3"));
	astrRunCfg.Add(wxT("reg spsr_svc 0xd3"));
	astrRunCfg.Add(wxT("reg r13_svc 0x7ffc"));
	astrRunCfg.Add(wxT("reg lr_svc 0x200000"));
#if 0
	/* netX50 init config */
	astrInitCfg.Add(wxT("interface ft2232"));
	astrInitCfg.Add(wxT("ft2232_device_desc \"NXHX board\""));
	astrInitCfg.Add(wxT("ft2232_layout \"comstick\""));
	astrInitCfg.Add(wxT("ft2232_vid_pid 0x0640 0x0028"));
	astrInitCfg.Add(wxT("jtag_speed 1"));
	astrInitCfg.Add(wxT("reset_config trst_and_srst"));
	astrInitCfg.Add(wxT("jtag_device 4 0x1 0xf 0xe"));
	astrInitCfg.Add(wxT("daemon_startup reset"));
	astrInitCfg.Add(wxT("target arm966e little reset_halt 0"));
	astrInitCfg.Add(wxT("working_area 0 0x00010000 0x8000 backup"));
	astrInitCfg.Add(wxT("run_and_halt_time 0 5000"));
	/* netX50 run config */
	astrRunCfg.Add(wxT("bp 0x200000 4 hw"));
	astrRunCfg.Add(wxT("reg cpsr 0xd3"));
	astrRunCfg.Add(wxT("reg spsr_svc 0xd3"));
	astrRunCfg.Add(wxT("reg r13_svc 0x7ffc"));
	astrRunCfg.Add(wxT("reg lr_svc 0x200000"));
#endif


	/* init the lua state */
	m_ptLuaState = NULL;

	return 0;
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

int fn_detect_interfaces(std::vector<muhkuh_plugin_instance*> *pvInterfaceList)
{
	int iInterfaces;
	muhkuh_plugin_instance *ptInst;
	wxString strName;
	wxString strTyp;
	wxString strLuaCreateFn;


	strTyp = wxString::FromAscii(plugin_desc.pcPluginId);
	strLuaCreateFn = wxT("muhkuh.romloader_openocd_create");

	iInterfaces = 0;

	// construct the name
	strName.Printf("romloader_openocd");
	ptInst = new muhkuh_plugin_instance(strName, strTyp, false, strLuaCreateFn, NULL);
	++iInterfaces;

	pvInterfaceList->push_back(ptInst);

	return iInterfaces;
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

	// free commandline interface
	command_done(cmd_ctx);
}


/*-------------------------------------*/

romloader *romloader_openocd_create(void *pvHandle)
{
	int iResult;
	command_context_t *cmd_ctx;
	target_t *target;
	wxString strMsg;
	size_t sizCfgCnt;
	size_t sizCfgMax;
	romloader *ptInstance = NULL;
	wxString strName;
	wxString strTyp;


	cmd_ctx = command_init();


	/* register subsystem commands */
	log_register_commands(cmd_ctx);
	jtag_register_commands(cmd_ctx);
	interpreter_register_commands(cmd_ctx);
	xsvf_register_commands(cmd_ctx);
	target_register_commands(cmd_ctx);

	iResult = log_init(cmd_ctx);
	if( iResult!=ERROR_OK )
	{
		strMsg.Printf(wxT("failed to init command context: %d"), iResult);
		wxLogError(strMsg);
	}
	else
	{
		command_set_output_handler(cmd_ctx, configuration_output_handler, NULL);
		cmd_ctx->mode = COMMAND_CONFIG;

		// set config
		sizCfgCnt = 0;
		sizCfgMax = astrInitCfg.GetCount();
		while( sizCfgCnt<sizCfgMax )
		{
			iResult = command_run_line(cmd_ctx, astrInitCfg.Item(sizCfgCnt).ToAscii());
			if( iResult!=ERROR_OK )
			{
				strMsg.Printf(wxT("failed to set config: %d"), iResult);
				wxLogError(strMsg);
				strMsg = wxT("error line was: '") + astrInitCfg.Item(sizCfgCnt) + wxT("'");
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
					usleep(500000);

					/* activate super cow powers */
					target = get_current_target(cmd_ctx);

					/* halt the target */
					wxLogMessage(wxT("Halt the target"));
					iResult = target->type->halt(target);
					if( iResult!=ERROR_OK )
					{
						strMsg.Printf(wxT("failed to halt the target: %d"), iResult);
						wxLogError(strMsg);
					}
				}
			}
		}
	}

	if( iResult!=ERROR_OK )
	{
		// free commandline interface
		command_done(cmd_ctx);
	}
	else
	{
		while (target->state != TARGET_HALTED)
		{
			target->type->poll(target);
		}
		usleep(1000);

		// create the new instance
		strTyp = wxString::FromAscii(plugin_desc.pcPluginId);
		// TODO: add some info
		strName.Printf("romloader_openocd");
		ptInstance = new romloader(strName, strTyp, &tFunctionInterface, cmd_ctx, romloader_openocd_close_instance, m_ptLuaState);
	}

	return ptInstance;
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

	usleep(1000);

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

	usleep(1000);

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

	usleep(1000);

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

	usleep(1000);

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

	usleep(1000);

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

	usleep(1000);

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

	usleep(1000);

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

	usleep(1000);

	return iResult;
}


/* call routine */
int fn_call(void *pvHandle, unsigned long ulNetxAddress, unsigned long ulParameterR0, lua_State *L, int iLuaCallbackTag, void *pvCallbackUserData)
{
	command_context_t *cmd_ctx;
	int iOocdResult;
	int iResult;
	size_t sizCfgCnt;
	size_t sizCfgMax;
	wxString strCmd;
	wxString strMsg;


	/* cast the handle to the command context */
	cmd_ctx = (command_context_t*)pvHandle;

	// set config
	sizCfgCnt = 0;
	sizCfgMax = astrRunCfg.GetCount();
	while( sizCfgCnt<sizCfgMax )
	{
		iOocdResult = command_run_line(cmd_ctx, astrRunCfg.Item(sizCfgCnt).ToAscii());
		if( iOocdResult!=ERROR_OK )
		{
			strMsg.Printf(wxT("failed to run command: %d"), iResult);
			wxLogError(strMsg);
			strMsg = wxT("error line was: '") + astrRunCfg.Item(sizCfgCnt) + wxT("'");
			wxLogError(strMsg);
			break;
		}
		++sizCfgCnt;
	}
	if( iOocdResult!=ERROR_OK )
	{
		wxLogError("config failed!");
		iResult = 1;
	}
	else
	{
		// set R0 parameter
		strCmd.Printf(wxT("reg r0 0x%08X"), ulParameterR0);
		iOocdResult = command_run_line(cmd_ctx, strCmd.ToAscii());
		if( iOocdResult!=ERROR_OK )
		{
			wxLogError("config failed!");
			iResult = 1;
		}
		else
		{
			// resume <ulNetxAddress>
			strCmd.Printf(wxT("resume 0x%08X"), ulNetxAddress);
			iOocdResult = command_run_line(cmd_ctx, strCmd.ToAscii());
			if( iOocdResult!=ERROR_OK )
			{
				wxLogError("config failed!");
				iResult = 1;
			}
			else
			{
				// grab messages here
// something like this
//	embeddedice_write_reg(&arm7_9->eice_cache->reg_list[EICE_COMMS_DATA], target_buffer_get_u32(target, buffer));

				// wait_halt 10
				iOocdResult = command_run_line(cmd_ctx, "wait_halt 10");
				if( iOocdResult!=ERROR_OK )
				{
					fprintf(stderr, "config failed!\n");
					iResult = 1;
				}
				else
				{
					// usb cmd delay
					usleep(1000);
					iResult = 0;
				}
			}
		}
	}

	return iResult;
}


/*-------------------------------------*/


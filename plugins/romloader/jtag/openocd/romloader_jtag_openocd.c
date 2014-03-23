
/*-------------------------------------*/
/* openocd includes */

#include "romloader_jtag_openocd.h"

#include "config.h"

#include <jtag/driver.h>
#include <jtag/jtag.h>
#include <transport/transport.h>
#include <helper/ioutil.h>
#include <helper/util.h>
#include <helper/configuration.h>
#include <flash/nor/core.h>
#include <flash/nand/core.h>
#include <pld/pld.h>
#include <flash/mflash.h>

/* TODO: remove this, do not start a server. */
#include <server/server.h>
#include <server/gdb_server.h>


#include <startup_tcl.c>


#define OPENOCD_VERSION	\
	"Open On-Chip Debugger " VERSION

/* Give scripts and TELNET a way to find out what version this is */
static int jim_version_command(Jim_Interp *interp, int argc,
	Jim_Obj * const *argv)
{
	if (argc > 2)
		return JIM_ERR;
	const char *str = "";
	char *version_str;
	version_str = OPENOCD_VERSION;

	if (argc == 2)
		str = Jim_GetString(argv[1], NULL);

	if (strcmp("git", str) == 0)
		version_str = "";

	Jim_SetResult(interp, Jim_NewStringObj(interp, version_str, -1));

	return JIM_OK;
}


static int log_target_callback_event_handler(struct target *target,
	enum target_event event,
	void *priv)
{
	switch (event) {
		case TARGET_EVENT_GDB_START:
			target->display = 0;
			break;
		case TARGET_EVENT_GDB_END:
			target->display = 1;
			break;
		case TARGET_EVENT_HALTED:
			if (target->display) {
				/* do not display information when debugger caused the halt */
				target_arch_state(target);
			}
			break;
		default:
			break;
	}

	return ERROR_OK;
}


static bool init_at_startup = true;


COMMAND_HANDLER(handle_noinit_command)
{
	if (CMD_ARGC != 0)
		return ERROR_COMMAND_SYNTAX_ERROR;
	init_at_startup = false;
	return ERROR_OK;
}


/* OpenOCD can't really handle failure of this command. Patches welcome! :-) */
COMMAND_HANDLER(handle_init_command)
{

	if (CMD_ARGC != 0)
		return ERROR_COMMAND_SYNTAX_ERROR;

	int retval;
	static int initialized;
	if (initialized)
		return ERROR_OK;

	initialized = 1;

	retval = command_run_line(CMD_CTX, "target init");
	if (ERROR_OK != retval)
		return ERROR_FAIL;

	retval = adapter_init(CMD_CTX);
	if (retval != ERROR_OK) {
		/* we must be able to set up the debug adapter */
		return retval;
	}

	LOG_DEBUG("Debug Adapter init complete");

	/* "transport init" verifies the expected devices are present;
	 * for JTAG, it checks the list of configured TAPs against
	 * what's discoverable, possibly with help from the platform's
	 * JTAG event handlers.  (which require COMMAND_EXEC)
	 */
	command_context_mode(CMD_CTX, COMMAND_EXEC);

	retval = command_run_line(CMD_CTX, "transport init");
	if (ERROR_OK != retval)
		return ERROR_FAIL;

	LOG_DEBUG("Examining targets...");
	if (target_examine() != ERROR_OK)
		LOG_DEBUG("target examination failed");

	command_context_mode(CMD_CTX, COMMAND_CONFIG);

	if (command_run_line(CMD_CTX, "flash init") != ERROR_OK)
		return ERROR_FAIL;

	if (command_run_line(CMD_CTX, "mflash init") != ERROR_OK)
		return ERROR_FAIL;

	if (command_run_line(CMD_CTX, "nand init") != ERROR_OK)
		return ERROR_FAIL;

	if (command_run_line(CMD_CTX, "pld init") != ERROR_OK)
		return ERROR_FAIL;
	command_context_mode(CMD_CTX, COMMAND_EXEC);

	/* initialize telnet subsystem */
	gdb_target_add_all(all_targets);

	target_register_event_callback(log_target_callback_event_handler, CMD_CTX);

	return ERROR_OK;
}


COMMAND_HANDLER(handle_add_script_search_dir_command)
{
	if (CMD_ARGC != 1)
		return ERROR_COMMAND_SYNTAX_ERROR;

	add_script_search_dir(CMD_ARGV[0]);

	return ERROR_OK;
}


static const struct command_registration openocd_command_handlers[] = {
	{
		.name = "version",
		.jim_handler = jim_version_command,
		.mode = COMMAND_ANY,
		.help = "show program version",
	},
	{
		.name = "noinit",
		.handler = &handle_noinit_command,
		.mode = COMMAND_CONFIG,
		.help = "Prevent 'init' from being called at startup.",
		.usage = ""
	},
	{
		.name = "init",
		.handler = &handle_init_command,
		.mode = COMMAND_ANY,
		.help = "Initializes configured targets and servers.  "
			"Changes command mode from CONFIG to EXEC.  "
			"Unless 'noinit' is called, this command is "
			"called automatically at the end of startup.",
		.usage = ""
	},
	{
		.name = "add_script_search_dir",
		.handler = &handle_add_script_search_dir_command,
		.mode = COMMAND_ANY,
		.help = "dir to search for config files and scripts",
		.usage = "<directory>"
	},
	COMMAND_REGISTRATION_DONE
};


static int openocd_register_commands(struct command_context *cmd_ctx)
{
	return register_commands(cmd_ctx, NULL, openocd_command_handlers);
}


struct command_context *global_cmd_ctx;

/* NB! this fn can be invoked outside this file for non PC hosted builds
 * NB! do not change to 'static'!!!!
 */
struct command_context *setup_command_handler(Jim_Interp *interp)
{
	log_init();
	LOG_DEBUG("log_init: complete");

	const char *startup = openocd_startup_tcl;
	struct command_context *cmd_ctx = command_init(startup, interp);

	/* register subsystem commands */
	typedef int (*command_registrant_t)(struct command_context *cmd_ctx_value);
	static const command_registrant_t command_registrants[] = {
		&openocd_register_commands,
		&server_register_commands,
		&gdb_register_commands,
		&log_register_commands,
		&transport_register_commands,
		&interface_register_commands,
		&target_register_commands,
		&flash_register_commands,
		&nand_register_commands,
		&pld_register_commands,
		&mflash_register_commands,
		NULL
	};
	for (unsigned i = 0; NULL != command_registrants[i]; i++) {
		int retval = (*command_registrants[i])(cmd_ctx);
		if (ERROR_OK != retval) {
			command_done(cmd_ctx);
			return NULL;
		}
	}
	LOG_DEBUG("command registration: complete");

	LOG_OUTPUT(OPENOCD_VERSION "\n"
		"Licensed under GNU GPL v2\n");

	global_cmd_ctx = cmd_ctx;

	return cmd_ctx;
}


int romloader_jtag_openocd_init(ROMLOADER_JTAG_DEVICE_T *ptJtagDevice)
{
	/* Initialize command line interface. */
	struct command_context *ptCmdCtx;
	int iResult;


	ptJtagDevice->pvOpenocdContext = NULL;

	ptCmdCtx = setup_command_handler(NULL);
	if( ptCmdCtx!=NULL )
	{
		iResult = util_init(ptCmdCtx);
		if( iResult==ERROR_OK )
		{
			iResult = ioutil_init(ptCmdCtx);
			if( iResult==ERROR_OK )
			{
				command_context_mode(ptCmdCtx, COMMAND_CONFIG);
				command_set_output_handler(ptCmdCtx, configuration_output_handler, NULL);

				iResult = server_preinit();
				if( iResult==ERROR_OK )
				{
					iResult = server_init(ptCmdCtx);
					if( iResult==ERROR_OK )
					{
						ptJtagDevice->pvOpenocdContext = ptCmdCtx;
					}
				}
			}
		}
	}

	return iResult;
}


static const char *apcIfCfg[2] =
{
  "# This interface allows only one transport: jtag.\n"
  "interface ftdi\n"
  "ftdi_device_desc \"Amontec JTAGkey\"\n"
  "ftdi_vid_pid 0x0403 0xcff8\n"
  "# Use a ridiculously low speed to ensure that the connection is working in absolutely all circumstances (even with Enricos flying wires ;) ).\n"
  "adapter_khz 100\n"
  "\n"
  "ftdi_layout_init 0x0c08 0x0f1b\n"
  "ftdi_layout_signal nTRST -data 0x0100 -noe 0x0400\n"
  "ftdi_layout_signal nSRST -data 0x0200 -noe 0x0800\n"
  "set IF_CFG_RESULT [ catch {jtag init} ]\n",

  "interface ftdi\n"
  "ftdi_device_desc \"NXHX50-RE\"\n"
  "ftdi_vid_pid 0x0640 0x0028\n"
  "# Use a ridiculously low speed to ensure that the connection is working in absolutely all circumstances (even with Enricos flying wires ;) ).\n"
  "adapter_khz 100\n"
  "\n"
  "ftdi_layout_init 0x0308 0x030b\n"
  "ftdi_layout_signal nTRST -data 0x0100\n"
  "ftdi_layout_signal nSRST -data 0x0200\n"
  "set IF_CFG_RESULT [ catch {jtag init} ]\n"
};

static const char *pcIfCleanup = "interface_clear\ntransport clear\n";


int get_result(struct command_context *ptCmdCtx, char *pcBuffer, size_t sizBufferMax)
{
  Jim_Interp *ptInterp;
  Jim_Obj *ptResultObj;
  const char *pcResult;
  int iResLen;
  int iResult;


  /* Be pessimistic. */
  iResult = -1;

  ptInterp = ptCmdCtx->interp;

  ptResultObj = Jim_GetResult(ptInterp);
  if( ptResultObj!=NULL )
  {
    iResLen = 0;
    pcResult = Jim_GetString(ptResultObj, &iResLen);
    fprintf(stderr, "Jim_GetString: %p %d\n", pcResult, iResLen);

    /* Does the result still exist? */
    if( pcResult!=NULL && iResLen>0 )
    {
      /* The size of the result must be smaller and not equal because it must be terminated with a 0 byte. */
      if( iResLen<sizBufferMax )
      {
	strncpy(pcBuffer, pcResult, iResLen);
	pcBuffer[iResLen] = '\0';
	iResult = 0;
      }
    }
  }

  return iResult;
}



int romloader_jtag_openocd_detect(ROMLOADER_JTAG_DEVICE_T *ptJtagDevice)
{
	struct command_context *ptCmdCtx;
	const char **ppcIfCfgCnt;
	const char **ppcIfCfgEnd;
	int iResult;
	int sizResult;
	char strResult[256];
	int iTclResult;


	ptCmdCtx = (struct command_context *)(ptJtagDevice->pvOpenocdContext);
	if( ptCmdCtx!=NULL )
	{
		/* Try to run all command chunks to see which interfaces are present. */
		ppcIfCfgCnt = apcIfCfg;
		ppcIfCfgEnd = apcIfCfg + (sizeof(apcIfCfg)/sizeof(apcIfCfg[0]));
		while( ppcIfCfgCnt<ppcIfCfgEnd )
		{
			fprintf(stderr, "Running command ***%s***\n", *ppcIfCfgCnt);
			iResult = command_run_line(ptCmdCtx, *ppcIfCfgCnt);
			fprintf(stderr, "If Cfg Result: %d\n", iResult);
			
			iResult = get_result(ptCmdCtx, strResult, sizeof(strResult));
			fprintf(stderr, "IF_CFG_RESULT: '%s'\n", strResult);
			iResult = sscanf(strResult, "%d", &iTclResult);
			if( iResult==1 && iTclResult==0 )
			{
			}
			
			/* Detection finished, clean up. */
			iResult = command_run_line(ptCmdCtx, pcIfCleanup);
			fprintf(stderr, "If Cleanup Result: %d\n", iResult);
			
			/* Move to the next configuration. */
			++ppcIfCfgCnt;
		}
	}
	
	return 0;
}



void romloader_jtag_openocd_uninit(ROMLOADER_JTAG_DEVICE_T *ptJtagDevice)
{
	struct command_context *ptCmdCtx;


	if( ptJtagDevice!=NULL )
	{
		ptCmdCtx = (struct command_context *)(ptJtagDevice->pvOpenocdContext);
		if( ptCmdCtx!=NULL )
		{
			unregister_all_commands(ptCmdCtx, NULL);

			/* free commandline interface */
			command_done(ptCmdCtx);

			adapter_quit();

			ptJtagDevice->pvOpenocdContext = NULL;
		}
	}
}


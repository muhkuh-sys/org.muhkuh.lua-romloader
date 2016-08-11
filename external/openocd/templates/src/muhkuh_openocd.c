#include "muhkuh_openocd.h"

#include <unistd.h>


/*-------------------------------------*/
/* openocd includes */

#include "openocd.c"



void *muhkuh_openocd_init(void)
{
	/* Initialize command line interface. */
	struct command_context *ptCmdCtx;
	void *pvResult;
	int iResult;


	pvResult = NULL;
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
					/* NOTE: do not call server_init. */
					pvResult = ptCmdCtx;
				}
			}
		}
	}

	return pvResult;
}



int muhkuh_openocd_get_result(void *pvContext, char *pcBuffer, size_t sizBufferMax)
{
	struct command_context *ptCmdCtx;
	Jim_Interp *ptInterp;
	Jim_Obj *ptResultObj;
	const char *pcResult;
	int iResLen;
	int iResult;
	
	
	/* Be pessimistic. */
	iResult = ERROR_FAIL;
	
	ptCmdCtx = (struct command_context *)pvContext;
	if( ptCmdCtx!=NULL )
	{
		ptInterp = ptCmdCtx->interp;
		
		ptResultObj = Jim_GetResult(ptInterp);
		if( ptResultObj!=NULL )
		{
			iResLen = 0;
			pcResult = Jim_GetString(ptResultObj, &iResLen);
			
			/* Does the result still exist? */
			if( pcResult!=NULL && iResLen>0 )
			{
				/* The size of the result must be smaller and not equal because it must be terminated with a 0 byte. */
				if( iResLen<sizBufferMax )
				{
					strncpy(pcBuffer, pcResult, iResLen);
					pcBuffer[iResLen] = '\0';
					iResult = ERROR_OK;
				}
			}
		}
	}
	
	return iResult;
}



int muhkuh_openocd_command_run_line(void *pvContext, char *pcLine)
{
	struct command_context *ptCmdCtx;
	int iResult;


	/* Be pessimistic... */
	iResult = ERROR_FAIL;

	if( pvContext!=NULL )
	{
		ptCmdCtx = (struct command_context *)pvContext;

		iResult = command_run_line(ptCmdCtx, pcLine);
	}

	return iResult;
}



void muhkuh_openocd_uninit(void *pvContext)
{
	struct command_context *ptCmdCtx;


	if( pvContext!=NULL )
	{
		ptCmdCtx = (struct command_context *)pvContext;

		unregister_all_commands(ptCmdCtx, NULL);

		/* free commandline interface */
		command_done(ptCmdCtx);

		adapter_quit();
	}
}



/* Read a byte (8bit) from the netX. */
int muhkuh_openocd_read_data08(void *pvContext, uint32_t ulNetxAddress, uint8_t *pucData)
{
	struct command_context *ptCmdCtx;
	struct target *ptTarget;
	int iResult;


	/* Cast the handle to the command context. */
	ptCmdCtx = (struct command_context*)pvContext;

	/* Get the target from the command context. */
	ptTarget = get_current_target(ptCmdCtx);

	/* Read the data from the netX. */
	iResult = target_read_u8(ptTarget, ulNetxAddress, pucData);
	if( iResult==ERROR_OK )
	{
		iResult = 0;
	}
	else
	{
		iResult = 1;
	}

	/* FIXME: is this really necessary? */
	usleep(1000);

	return iResult;
}


/* Read a word (16bit) from the netX. */
int muhkuh_openocd_read_data16(void *pvContext, uint32_t ulNetxAddress, uint16_t *pusData)
{
	struct command_context *ptCmdCtx;
	struct target *ptTarget;
	int iResult;


	/* cast the handle to the command context */
	ptCmdCtx = (struct command_context*)pvContext;

	/* get the target from the command context */
	ptTarget = get_current_target(ptCmdCtx);

	/* read the data from the netX */
	iResult = target_read_u16(ptTarget, ulNetxAddress, pusData);
	if( iResult==ERROR_OK )
	{
		iResult = 0;
	}
	else
	{
		iResult = 1;
	}

	/* FIXME: is this really necessary? */
	usleep(1000);

	return iResult;
}


/* Read a long (32bit) from the netX. */
int muhkuh_openocd_read_data32(void *pvContext, uint32_t ulNetxAddress, uint32_t *pulData)
{
	struct command_context *ptCmdCtx;
	struct target *ptTarget;
	int iResult;


	/* cast the handle to the command context */
	ptCmdCtx = (struct command_context*)pvContext;

	/* get the target from the command context */
	ptTarget = get_current_target(ptCmdCtx);

	/* read the data from the netX */
	iResult = target_read_u32(ptTarget, ulNetxAddress, pulData);
	if( iResult==ERROR_OK )
	{
		iResult = 0;
	}
	else
	{
		iResult = 1;
	}

	/* FIXME: is this really necessary? */
	usleep(1000);

	return iResult;
}


/* Read a byte array from the netX. */
int muhkuh_openocd_read_image(void *pvContext, uint32_t ulNetxAddress, uint8_t *pucData, uint32_t ulSize)
{
	struct command_context *ptCmdCtx;
	struct target *ptTarget;
	int iResult;


	/* cast the handle to the command context */
	ptCmdCtx = (struct command_context*)pvContext;

	/* get the target from the command context */
	ptTarget = get_current_target(ptCmdCtx);

	/* read the data from the netX */
	iResult = target_read_buffer(ptTarget, ulNetxAddress, ulSize, pucData);
	if( iResult==ERROR_OK )
	{
		iResult = 0;
	}
	else
	{
		iResult = 1;
	}

	/* FIXME: is this really necessary? */
	usleep(1000);

	return iResult;
}


/* Write a byte (8bit) to the netX. */
int muhkuh_openocd_write_data08(void *pvContext, uint32_t ulNetxAddress, uint8_t ucData)
{
	struct command_context *ptCmdCtx;
	struct target *ptTarget;
	int iResult;


	/* cast the handle to the command context */
	ptCmdCtx = (struct command_context*)pvContext;

	/* get the target from the command context */
	ptTarget = get_current_target(ptCmdCtx);

	/* read the data from the netX */
	iResult = target_write_u8(ptTarget, ulNetxAddress, ucData);
	if( iResult==ERROR_OK )
	{
		iResult = 0;
	}
	else
	{
		iResult = 1;
	}

	/* FIXME: is this really necessary? */
	usleep(1000);

	return iResult;
}


/* Write a word (16bit) to the netX. */
int muhkuh_openocd_write_data16(void *pvContext, uint32_t ulNetxAddress, uint16_t usData)
{
	struct command_context *ptCmdCtx;
	struct target *ptTarget;
	int iResult;


	/* cast the handle to the command context */
	ptCmdCtx = (struct command_context*)pvContext;

	/* get the target from the command context */
	ptTarget = get_current_target(ptCmdCtx);

	/* read the data from the netX */
	iResult = target_write_u16(ptTarget, ulNetxAddress, usData);
	if( iResult==ERROR_OK )
	{
		iResult = 0;
	}
	else
	{
		iResult = 1;
	}

	/* FIXME: is this really necessary? */
	usleep(1000);

	return iResult;
}


/* Write a long (32bit) to the netX. */
int muhkuh_openocd_write_data32(void *pvContext, uint32_t ulNetxAddress, uint32_t ulData)
{
	struct command_context *ptCmdCtx;
	struct target *ptTarget;
	int iResult;


	/* cast the handle to the command context */
	ptCmdCtx = (struct command_context*)pvContext;

	/* get the target from the command context */
	ptTarget = get_current_target(ptCmdCtx);

	/* read the data from the netX */
	iResult = target_write_u32(ptTarget, ulNetxAddress, ulData);
	if( iResult==ERROR_OK )
	{
		iResult = 0;
	}
	else
	{
		iResult = 1;
	}

	/* FIXME: is this really necessary? */
	usleep(1000);

	return iResult;
}


/* Write a byte array to the netX. */
int muhkuh_openocd_write_image(void *pvContext, uint32_t ulNetxAddress, const uint8_t *pucData, uint32_t ulSize)
{
	struct command_context *ptCmdCtx;
	struct target *ptTarget;
	int iResult;


	/* Cast the handle to the command context. */
	ptCmdCtx = (struct command_context*)pvContext;

	/* Get the target from the command context. */
	ptTarget = get_current_target(ptCmdCtx);

	/* write the data to the netX */
	iResult = target_write_buffer(ptTarget, ulNetxAddress, ulSize, pucData);
	if( iResult==ERROR_OK )
	{
		iResult = 0;
	}
	else
	{
		iResult = 1;
	}

	/* FIXME: is this really necessary? */
	usleep(1000);

	return iResult;
}



int muhkuh_openocd_call(void *pvContext, uint32_t ulNetxAddress, uint32_t ulR0, PFN_MUHKUH_CALL_PRINT_CALLBACK pfnCallback, void *pvCallbackUserData)
{
	struct command_context *ptCmdCtx;
	struct target *ptTarget;
	int iOocdResult;
	int iResult;
	char strCmd[32];
	int fIsRunning;
	enum target_state tState;


	/* Cast the handle to the command context. */
	ptCmdCtx = (struct command_context*)pvContext;

	/* Get the target from the command context. */
	ptTarget = get_current_target(ptCmdCtx);

	/* Expect failure. */
	iResult = -1;

	/* Set the register R0. */
	memset(strCmd, 0, sizeof(strCmd));
	snprintf(strCmd, sizeof(strCmd)-1, "reg r0 0x%08X", ulR0);
	iOocdResult = command_run_line(ptCmdCtx, strCmd);
	if( iOocdResult!=ERROR_OK )
	{
		fprintf(stderr, "muhkuh_openocd_call: set r0 failed!\n");
	}
	else
	{
		memset(strCmd, 0, sizeof(strCmd));
		snprintf(strCmd, sizeof(strCmd)-1, "resume 0x%08X", ulNetxAddress);
		iOocdResult = command_run_line(ptCmdCtx, strCmd);
		if( iOocdResult!=ERROR_OK )
		{
			fprintf(stderr, "muhkuh_openocd_call: resume failed!\n");
		}
		else
		{
#if 0
			// grab messages here
			// TODO: redirect outputhandler, then grab messages, restore default output handler on halt

			/* Wait for halt. */
			do
			{
				usleep(1000*100);

				ptTarget->type->poll(ptTarget);
				tState = ptTarget->state;
				if( tState==TARGET_HALTED )
				{
					fprintf(stderr, "call finished!\n");
					iResult = 0;
				}
				else
				{
					/* Execute the callback. */
					fIsRunning = pfnCallback(pvCallbackUserData, NULL, 0);
					if( fIsRunning==0 )
					{
						/* The operation was canceled, halt the target. */
						fprintf(stderr, "Call canceled by the user, stopping target...\n");
						iOocdResult = ptTarget->type->halt(ptTarget);
						if( iOocdResult!=ERROR_OK )
						{
							fprintf(stderr, "Failed to halt target: %d\n", iOocdResult);
						}
						break;
					}
					else
					{
						target_call_timer_callbacks();
					}
				}
			} while( tState!=TARGET_HALTED );
#endif
			/* FIXME: is this really necessary? */
			usleep(1000);
		}
	}

	return iResult;
}



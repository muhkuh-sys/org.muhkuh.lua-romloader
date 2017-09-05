#include "muhkuh_openocd.h"

#include <unistd.h>

/*-------------------------------------*/
/* openocd includes */

#include "openocd.c"

#include "target/target_type.h"
#include "target/target_request.h"


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

/* Structure to hold the address of a line and its size.
   Used as output_handler_priv.
 */
typedef struct {
	uint8_t* pucDccData;
	unsigned long ulDccDataSize; 	/* data size without the terminating null byte. */
} DCC_LINE_BUFFER_T;

/* Store a 0-terminated line in the buffer. */
void dcc_line_buffer_put(DCC_LINE_BUFFER_T* ptBuffer, const char *line)
{
	unsigned long ulDccDataSize;
	uint8_t* pucDccData;

	if (line == NULL)
	{
		fprintf(stderr, "dcc_line_buffer_put: line == NULL\n");
	}
	else
	{
		ulDccDataSize = strlen(line);
		pucDccData = malloc(ulDccDataSize+1);
		if (pucDccData != NULL)
		{
			strcpy(pucDccData, line);
			ptBuffer->ulDccDataSize = ulDccDataSize;
			ptBuffer->pucDccData = pucDccData;
		}
		else
		{
			printf("!Error: failed to allocate buffer for DCC debug message\n");
		}

	}

}

/* Remove the current line from the buffer, if any.
   fUsed = 1 if any line which may be in the buffer has already been used.
   fUsed = 0 if any line in the buffer has not been used (prints a warning).
 */
void dcc_line_buffer_clear(DCC_LINE_BUFFER_T* ptBuffer, int fUsed)
{
	if (ptBuffer->pucDccData != NULL)
	{
		if (fUsed == 0)
		{
			fprintf(stderr,"dropping debug message (not passed to Lua callback): %s\n", ptBuffer->pucDccData);
		}

		free(ptBuffer->pucDccData);
		ptBuffer->pucDccData = NULL;
		ptBuffer->ulDccDataSize = 0;
	}
}

int romloader_jtag_command_output_handler(struct command_context *context, const char *line)
{
	DCC_LINE_BUFFER_T* ptBuffer = (DCC_LINE_BUFFER_T*) context->output_handler_priv;
	if (line != NULL)
	{
		dcc_line_buffer_clear(ptBuffer, 0);
		dcc_line_buffer_put(ptBuffer, line);
	}

	return ERROR_OK;
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

	DCC_LINE_BUFFER_T tDccLineBuffer = {NULL, 0};

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
			// redirect output handler, then grab messages, restore default output handler on halt
			command_set_output_handler(ptCmdCtx, &romloader_jtag_command_output_handler, &tDccLineBuffer);

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
					/* Execute the Lua callback. */
					fIsRunning = pfnCallback(pvCallbackUserData, tDccLineBuffer.pucDccData, tDccLineBuffer.ulDccDataSize);
					dcc_line_buffer_clear(&tDccLineBuffer, 1);

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
						/* call OpenOCD timer callbacks */
						target_call_timer_callbacks();
					}
				}
			} while( tState!=TARGET_HALTED );

			/* call timer callbacks once more (to get remaining DCC output), then call the callback to consume this output */
			target_call_timer_callbacks();
			pfnCallback(pvCallbackUserData, tDccLineBuffer.pucDccData, tDccLineBuffer.ulDccDataSize);
			dcc_line_buffer_clear(&tDccLineBuffer, 1);

			command_clear_output_handler(ptCmdCtx);

			/* FIXME: is this really necessary? */
			usleep(1000);
		}
	}

	return iResult;
}


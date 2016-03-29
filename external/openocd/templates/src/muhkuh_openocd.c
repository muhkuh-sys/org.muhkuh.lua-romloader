#include "muhkuh_openocd.h"

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
			fprintf(stderr, "Jim_GetString: %p %d\n", pcResult, iResLen);
			
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


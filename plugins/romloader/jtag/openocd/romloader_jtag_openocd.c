#include "romloader_jtag_openocd.h"

#include <string.h>
#include <stdio.h>


/* This is for Linux. */
#include <dlfcn.h>
#define OPENOCD_SHARED_LIBRARY_FILENAME "/tmp/n/libopenocd.so"



static void *dl_resolve_symbol(void *pvSharedLibraryHandle, const char *pcSymbol)
{
	void *pvFn;


	pvFn = dlsym(pvSharedLibraryHandle, pcSymbol);
	if( pvFn==NULL )
	{
		fprintf(stderr, "Failed to resolve function %s in shared library %s.\n", pcSymbol, OPENOCD_SHARED_LIBRARY_FILENAME);
		fprintf(stderr, "Error: %s\n", dlerror());
	}

	return pvFn;
}



static int openocd_open(ROMLOADER_JTAG_DEVICE_T *ptDevice)
{
	int iResult;
	void *pvSharedLibraryHandle;
	void *pvFn;
	void *pvOpenocdContext;


	/* Be pessimistic... */
	iResult = -1;

	/* Try to open the shared library. */
	pvSharedLibraryHandle = dlopen(OPENOCD_SHARED_LIBRARY_FILENAME, RTLD_NOW|RTLD_LOCAL);
	if( pvSharedLibraryHandle==NULL )
	{
		/* Failed to open the shared library. */
		fprintf(stderr, "Failed to open the shared library %s.\n", OPENOCD_SHARED_LIBRARY_FILENAME);
		fprintf(stderr, "Error: %s\n", dlerror());
	}
	else
	{
		ptDevice->pvSharedLibraryHandle = pvSharedLibraryHandle;

		/* Try to resolve all symbols. */
		pvFn = dl_resolve_symbol(pvSharedLibraryHandle, "muhkuh_openocd_init");
		if( pvFn!=NULL )
		{
			ptDevice->pfnInit = pvFn;
			pvFn = dl_resolve_symbol(pvSharedLibraryHandle, "muhkuh_openocd_get_result");
			if( pvFn!=NULL )
			{
				ptDevice->pfnGetResult = pvFn;
				pvFn = dl_resolve_symbol(pvSharedLibraryHandle, "muhkuh_openocd_command_run_line");
				if( pvFn!=NULL )
				{
					ptDevice->pfnCommandRunLine = pvFn;
					pvFn = dl_resolve_symbol(pvSharedLibraryHandle, "muhkuh_openocd_uninit");
					if( pvFn!=NULL )
					{
						ptDevice->pfnUninit = pvFn;

						/* Call the init function. */
						pvOpenocdContext = ptDevice->pfnInit();
						if( pvOpenocdContext==NULL )
						{
							fprintf(stderr, "Failed to initialize the OpenOCD device context.\n");
						}
						else
						{
							ptDevice->pvOpenocdContext = pvOpenocdContext;
							iResult = 0;
						}
					}
				}
			}
		}

		if( iResult!=0 )
		{
			/* Close the shared library. */
			dlclose(pvSharedLibraryHandle);
		}
	}

	return iResult;
}



/* The romloader_jtag_openocd_init function opens the OpenOCD shared library,
   executes the "version" command and closes the library.

   This is a test if the shared library can be used.
*/
int romloader_jtag_openocd_init(void)
{
	int iResult;
	ROMLOADER_JTAG_DEVICE_T tDevice;
	char acResult[256];


	/* Initialize the device handle. */
	memset(&tDevice, 0, sizeof(ROMLOADER_JTAG_DEVICE_T));

	iResult = openocd_open(&tDevice);
	if( iResult==0 )
	{
		fprintf(stderr, "Init OK!\n");

		/* Run the version command. */
		iResult = tDevice.pfnCommandRunLine(tDevice.pvOpenocdContext, "version\n");
		if( iResult!=0 )
		{
			fprintf(stderr, "Failed to run the version command!\n");
		}
		else
		{
			iResult = tDevice.pfnGetResult(tDevice.pvOpenocdContext, acResult, sizeof(acResult));
			if( iResult!=0 )
			{
				fprintf(stderr, "Failed to get the result for the version command.\n");
			}
			else
			{
				fprintf(stderr, "OpenOCD version %s\n", acResult);
			}
		}

		tDevice.pfnUninit(tDevice.pvOpenocdContext);

		/* Close the shared library. */
		dlclose(tDevice.pvSharedLibraryHandle);
	}

	return iResult;
}



typedef struct INIT_CODE_WITH_ID_STRUCT
{
	const char *pcID;
	const char *pcCode;
} INIT_CODE_WITH_ID_T;


/* This array contains the detection command sequence for all interfaces. All
 * commands for one interface are stored in one string. The commands are
 * separated by newlines.
 *
 * Every detection sequence must set the TCL variable 'IF_CFG_RESULT'. It
 * shows the result of the detection. A value of '1'
 *
 * Use a ridiculously low speed to ensure that the connection is working in
 * absolutely all circumstances (even with Enricos flying wires. ;)
 *
 */
static const INIT_CODE_WITH_ID_T atIfCfg[3] =
{
	{
		.pcID = "Amontec JTAGkey",
		.pcCode = "proc test {} {\n"
		          "    set RESULT -1"
		          "\n"
		          "    interface ftdi\n"
                          "    transport select jtag\n"
		          "    ftdi_device_desc \"Amontec JTAGkey\"\n"
		          "    ftdi_vid_pid 0x0403 0xcff8\n"
		          "    adapter_khz 100\n"
		          "\n"
		          "    ftdi_layout_init 0x0c08 0x0f1b\n"
		          "    ftdi_layout_signal nTRST -data 0x0100 -noe 0x0400\n"
		          "    ftdi_layout_signal nSRST -data 0x0200 -noe 0x0800\n"
		          "    if {[ catch {jtag init} ]==0 } {\n"
		          "        set RESULT {OK}\n"
		          "    }\n"
		          "    return $RESULT\n"
		          "}\n"
		          "test\n"
	},

	{
		.pcID = "NXHX50-RE",
		.pcCode = "proc test {} {\n"
		          "    set RESULT -1\n"
		          "\n"
		          "    interface ftdi\n"
                          "    transport select jtag\n"
		          "    ftdi_device_desc \"NXHX50-RE\"\n"
		          "    ftdi_vid_pid 0x0640 0x0028\n"
		          "    adapter_khz 100\n"
		          "\n"
		          "    ftdi_layout_init 0x0308 0x030b\n"
		          "    ftdi_layout_signal nTRST -data 0x0100\n"
		          "    ftdi_layout_signal nSRST -data 0x0200\n"
		          "    if {[ catch {jtag init} ]==0 } {\n"
		          "        set RESULT {OK}\n"
		          "    }\n"
		          "    return $RESULT\n"
		          "}\n"
		          "test\n"
	},

	{
		.pcID = "NXHX 51-ETM",
		.pcCode = "proc test {} {\n"
		          "    set RESULT -1\n"
		          "\n"
		          "    interface ftdi\n"
                          "    transport select jtag\n"
		          "    ftdi_device_desc \"NXHX 51-ETM\"\n"
		          "    ftdi_vid_pid 0x0640 0x0028\n"
		          "    adapter_khz 100\n"
		          "\n"
		          "    ftdi_layout_init 0x0308 0x030b\n"
		          "    ftdi_layout_signal nTRST -data 0x0100\n"
		          "    ftdi_layout_signal nSRST -data 0x0200\n"
		          "    if {[ catch {jtag init} ]==0 } {\n"
		          "        set RESULT {OK}\n"
		          "    }\n"
		          "    return $RESULT\n"
		          "}\n"
		          "test\n"
	}
};



static const INIT_CODE_WITH_ID_T atScCfg[2] =
{
	{
		.pcID = "netX_ARM966",
		.pcCode = "global SC_CFG_RESULT\n"
		          "\n"
		          "echo {Detecting netX_ARM966 scan chain}\n"
		          "\n"
		          "jtag cleartaps\n"
		          "\n"
		          "set SC_CFG_RESULT 0\n"
		          "\n"
		          "jtag newtap netX_ARM966 cpu -irlen 4 -ircapture 1 -irmask 0xf -expected-id 0x25966021\n"
		          "jtag configure netX_ARM966.cpu -event setup { global SC_CFG_RESULT ; echo {setup done!} ; set SC_CFG_RESULT {OK} }\n"
		          "jtag arp_init\n"
		          "\n"
		          "if { $SC_CFG_RESULT=={OK} } {\n"
		          "    echo {init target}\n"
		          "    target create netX_ARM966.cpu arm966e -endian little -chain-position netX_ARM966.cpu\n"
		          "}\n"
		          "\n"
		          "return $SC_CFG_RESULT\n"
	},

	{
		.pcID = "netX_ARM926",
		.pcCode = "global SC_CFG_RESULT\n"
		          "\n"
		          "echo {Detecting netX_ARM926 scan chain}\n"
		          "\n"
		          "jtag cleartaps\n"
		          "\n"
		          "set SC_CFG_RESULT 0\n"
		          "\n"
		          "jtag newtap netX_ARM926 cpu -irlen 4 -ircapture 1 -irmask 0xf -expected-id 0x07926021\n"
		          "jtag configure netX_ARM926.cpu -event setup { global SC_CFG_RESULT ; echo {setup done!} ; set SC_CFG_RESULT {OK} }\n"
		          "jtag arp_init\n"
		          "\n"
		          "if { $SC_CFG_RESULT=={OK} } {\n"
		          "    echo {init target}\n"
		          "    target create netX_ARM926.cpu arm926ejs -endian little -chain-position netX_ARM926.cpu\n"
		          "}\n"
		          "\n"
		          "return $SC_CFG_RESULT\n"
	}
};




static int romloader_jtag_openocd_detect_scan_chain(void)
{
#if 0
	const INIT_CODE_WITH_ID_T *ptCnt;
	const INIT_CODE_WITH_ID_T *ptEnd;
	int iResult;
	int sizResult;
	char strResult[256];


	/* Try to run all command chunks to see which CPU is there. */
	ptCnt = atScCfg;
	ptEnd = atScCfg + (sizeof(atScCfg)/sizeof(atScCfg[0]));
	while( ptCnt<ptEnd )
	{
		fprintf(stderr, "Detecting CPU %s\n", ptCnt->pcID);
		iResult = command_run_line(ptCmdCtx, ptCnt->pcCode);
		fprintf(stderr, "iResult: %d\n", iResult);
		
		iResult = get_result(ptCmdCtx, strResult, sizeof(strResult));
		fprintf(stderr, "iResult: %d\n", iResult);
		fprintf(stderr, "RESULT: '%s'\n", strResult);
		if( strncmp(strResult, "OK", 3)==0 )
		{
			fprintf(stderr, "Found scan chain!\n");
			break;
		}
		
		/* Move to the next configuration. */
		++ptCnt;
	}
#endif
}



int romloader_jtag_openocd_detect(void)
{
	const INIT_CODE_WITH_ID_T *ptCnt;
	const INIT_CODE_WITH_ID_T *ptEnd;
	int iResult;
	int sizResult;
	ROMLOADER_JTAG_DEVICE_T tDevice;
	char strResult[256];


	/* Be pessimistic... */
	iResult = -1;

	/* Try to run all command chunks to see which interfaces are present. */
	ptCnt = atIfCfg;
	ptEnd = atIfCfg + (sizeof(atIfCfg)/sizeof(atIfCfg[0]));
	while( ptCnt<ptEnd )
	{
		fprintf(stderr, "Detecting IF %s\n", ptCnt->pcID);

		/* Open the shared library. */
		iResult = openocd_open(&tDevice);
		if( iResult!=0 )
		{
			/* This is a fatal error. */
			break;
		}
		else
		{
			/* Run the command chunk. */
			iResult = tDevice.pfnCommandRunLine(tDevice.pvOpenocdContext, ptCnt->pcCode);
			if( iResult!=0 )
			{
				/* This is no fatal error. It just means that this device is not present. */
				fprintf(stderr, "Failed to run the command chunk: %d\n", iResult);
			}
			else
			{
				iResult = tDevice.pfnGetResult(tDevice.pvOpenocdContext, strResult, sizeof(strResult));
				if( iResult!=0 )
				{
					/* This is a fatal error. */
					fprintf(stderr, "Failed to get the result for the version command.\n");
					break;
				}
				else
				{
					fprintf(stderr, "Result from detect: %s\n", strResult);
					iResult = strncmp(strResult, "OK", 3);
					if( iResult==0 )
					{
						fprintf(stderr, "Found Interface!\n");

						/* Now detect the CPU. */
//						romloader_jtag_openocd_detect_scan_chain(ptCmdCtx);

						break;
					}
				}
			}

			tDevice.pfnUninit(tDevice.pvOpenocdContext);

			/* Close the shared library. */
			dlclose(tDevice.pvSharedLibraryHandle);
		}
		
		/* Move to the next configuration. */
		++ptCnt;
	}
	
	return iResult;
}



void romloader_jtag_openocd_uninit(ROMLOADER_JTAG_DEVICE_T *ptJtagDevice)
{
#if 0
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
#endif
}


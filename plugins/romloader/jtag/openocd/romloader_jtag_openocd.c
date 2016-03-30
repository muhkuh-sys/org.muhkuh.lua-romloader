#include "romloader_jtag_openocd.h"

#include <string.h>
#include <stdio.h>


#if defined(_MSC_VER) || defined(__MINGW32__) || defined(__MINGW64__)
#       include <windows.h>

/* FIXME: search the shared library in some common places.
 * One solution would be the same folder as this LUA plugin. Here is a way to get the full path of something in Windows: https://msdn.microsoft.com/en-us/library/windows/desktop/ms683197.aspx
 * The same is possible for Linux: http://stackoverflow.com/questions/1642128/linux-how-to-get-full-name-of-shared-object-just-loaded-from-the-constructor
 */
#define OPENOCD_SHARED_LIBRARY_FILENAME "openocd.dll"

#elif defined(__GNUC__)
	/* This is for Linux. */
#       include <dlfcn.h>

/* FIXME: search the shared library in some common places.
 * One solution would be the same folder as this LUA plugin. Here is a way to get the full path of something in Windows: https://msdn.microsoft.com/en-us/library/windows/desktop/ms683197.aspx
 * The same is possible for Linux: http://stackoverflow.com/questions/1642128/linux-how-to-get-full-name-of-shared-object-just-loaded-from-the-constructor
 */
#define OPENOCD_SHARED_LIBRARY_FILENAME "libopenocd.so"


#endif



static void *sharedlib_open(const char *pcPath)
{
	void *pvSharedLibraryHandle;


#if defined(_MSC_VER) || defined(__MINGW32__) || defined(__MINGW64__)
	pvSharedLibraryHandle = LoadLibrary(pcPath);
#elif defined(__GNUC__)
	pvSharedLibraryHandle = dlopen(pcPath, RTLD_NOW|RTLD_LOCAL);
#else
#       error "Add sharedlib_open for your platform."
#endif

	return pvSharedLibraryHandle;
}



static void sharedlib_close(void *pvSharedLibraryHandle)
{
#if defined(_MSC_VER) || defined(__MINGW32__) || defined(__MINGW64__)
	FreeLibrary((HANDLE)pvSharedLibraryHandle);
#elif defined(__GNUC__)
	dlclose(pvSharedLibraryHandle);
#else
#       error "Add sharedlib_close for your platform."
#endif
}



static void sharedlib_get_error(char *pcBuffer, size_t sizBuffer)
{
	/* No buffer -> no errormessage. */
	if( pcBuffer!=NULL )
	{
		if( sizBuffer!=0 )
		{
			/* Clear the complete buffer with 0. */
			memset(pcBuffer, 0, sizBuffer);

			if( sizBuffer>0 )
			{
#if defined(_MSC_VER) || defined(__MINGW32__) || defined(__MINGW64__)
				FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM, NULL, GetLastError(), LANG_NEUTRAL, pcBuffer, sizBuffer-1, NULL);
#elif defined(__GNUC__)
				strncpy(pcBuffer, dlerror(), sizBuffer-1);
#else
#       error "Add sharedlib_get_error for your platform."
#endif
			}
		}
	}
}



static void *sharedlib_resolve_symbol(void *pvSharedLibraryHandle, const char *pcSymbol)
{
	void *pvFn;
	char acError[1024];


#if defined(_MSC_VER) || defined(__MINGW32__) || defined(__MINGW64__)
	pvFn = GetProcAddress((HMODULE)pvSharedLibraryHandle, pcSymbol);
#elif defined(__GNUC__)
	pvFn = dlsym(pvSharedLibraryHandle, pcSymbol);
#else
#       error "Add sharedlib_resolve_symbol for your platform."
#endif

	if( pvFn==NULL )
	{
		sharedlib_get_error(acError, sizeof(acError));
		fprintf(stderr, "Failed to resolve function %s in shared library %s.\n", pcSymbol, OPENOCD_SHARED_LIBRARY_FILENAME);
		fprintf(stderr, "Error: %s\n", acError);
	}

	return pvFn;
}



static int openocd_open(ROMLOADER_JTAG_DEVICE_T *ptDevice)
{
	int iResult;
	void *pvSharedLibraryHandle;
	void *pvFn;
	void *pvOpenocdContext;
	char acError[1024];


	/* Be pessimistic... */
	iResult = -1;

	/* Try to open the shared library. */
	pvSharedLibraryHandle = sharedlib_open(OPENOCD_SHARED_LIBRARY_FILENAME);
	if( pvSharedLibraryHandle==NULL )
	{
		/* Failed to open the shared library. */
		sharedlib_get_error(acError, sizeof(acError));
		fprintf(stderr, "Failed to open the shared library %s.\n", OPENOCD_SHARED_LIBRARY_FILENAME);
		fprintf(stderr, "Error: %s\n", acError);
	}
	else
	{
		ptDevice->pvSharedLibraryHandle = pvSharedLibraryHandle;

		/* Try to resolve all symbols. */
		pvFn = sharedlib_resolve_symbol(pvSharedLibraryHandle, "muhkuh_openocd_init");
		if( pvFn!=NULL )
		{
			ptDevice->pfnInit = pvFn;
			pvFn = sharedlib_resolve_symbol(pvSharedLibraryHandle, "muhkuh_openocd_get_result");
			if( pvFn!=NULL )
			{
				ptDevice->pfnGetResult = pvFn;
				pvFn = sharedlib_resolve_symbol(pvSharedLibraryHandle, "muhkuh_openocd_command_run_line");
				if( pvFn!=NULL )
				{
					ptDevice->pfnCommandRunLine = pvFn;
					pvFn = sharedlib_resolve_symbol(pvSharedLibraryHandle, "muhkuh_openocd_uninit");
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
			sharedlib_close(pvSharedLibraryHandle);
		}
	}

	return iResult;
}



static void openocd_close(ROMLOADER_JTAG_DEVICE_T *ptDevice)
{
	if( ptDevice->pvSharedLibraryHandle!=NULL )
	{
		if( ptDevice->pvOpenocdContext!=NULL )
		{
			ptDevice->pfnUninit(ptDevice->pvOpenocdContext);
		}

		/* Close the shared library. */
		sharedlib_close(ptDevice->pvSharedLibraryHandle);
	}
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

		openocd_close(&tDevice);
	}

	return iResult;
}



typedef struct INTERFACE_SETUP_STRUCT
{
	const char *pcID;
	const char *pcCode_Setup;
	const char *pcCode_Probe;
} INTERFACE_SETUP_STRUCT_T;


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
static const INTERFACE_SETUP_STRUCT_T atInterfaceCfg[4] =
{
	{
		.pcID = "Amontec JTAGkey",
		.pcCode_Setup = "interface ftdi\n"
                                "transport select jtag\n"
		                "ftdi_device_desc \"Amontec JTAGkey\"\n"
		                "ftdi_vid_pid 0x0403 0xcff8\n"
		                "adapter_khz 100\n"
		                "\n"
		                "ftdi_layout_init 0x0c08 0x0f1b\n"
		                "ftdi_layout_signal nTRST -data 0x0100 -noe 0x0400\n"
		                "ftdi_layout_signal nSRST -data 0x0200 -noe 0x0800\n",
		.pcCode_Probe = "proc probe {} {\n"
		                "    set RESULT -1"
		                "\n"
		                "    if {[ catch {jtag init} ]==0 } {\n"
		                "        set RESULT {OK}\n"
		                "    }\n"
		                "    return $RESULT\n"
		                "}\n"
		                "probe\n"
	},

	{
		.pcID = "NXHX50-RE",
		.pcCode_Setup = "interface ftdi\n"
                                "transport select jtag\n"
		                "ftdi_device_desc \"NXHX50-RE\"\n"
		                "ftdi_vid_pid 0x0640 0x0028\n"
		                "adapter_khz 100\n"
		                "\n"
		                "ftdi_layout_init 0x0308 0x030b\n"
		                "ftdi_layout_signal nTRST -data 0x0100\n"
		                "ftdi_layout_signal nSRST -data 0x0200\n",
		.pcCode_Probe = "proc probe {} {\n"
		                "    set RESULT -1\n"
		                "\n"
		                "    if {[ catch {jtag init} ]==0 } {\n"
		                "        set RESULT {OK}\n"
		                "    }\n"
		                "    return $RESULT\n"
		                "}\n"
		                "probe\n"
	},

	{
		.pcID = "NXHX 51-ETM",
		.pcCode_Setup = "interface ftdi\n"
                                "transport select jtag\n"
		                "ftdi_device_desc \"NXHX 51-ETM\"\n"
		                "ftdi_vid_pid 0x0640 0x0028\n"
		                "adapter_khz 100\n"
		                "\n"
		                "ftdi_layout_init 0x0308 0x030b\n"
		                "ftdi_layout_signal nTRST -data 0x0100\n"
		                "ftdi_layout_signal nSRST -data 0x0200\n",
		.pcCode_Probe = "proc probe {} {\n"
			        "    set RESULT -1\n"
			        "\n"
		                "    if {[ catch {jtag init} ]==0 } {\n"
		                "        set RESULT {OK}\n"
		                "    }\n"
		                "    return $RESULT\n"
		                "}\n"
		                "probe\n"
	},

	{
		.pcID = "NXHX 500-ETM",
		.pcCode_Setup = "interface ftdi\n"
                                "transport select jtag\n"
		                "ftdi_device_desc \"NXHX 500-ETM\"\n"
		                "ftdi_vid_pid 0x0640 0x0028\n"
		                "adapter_khz 100\n"
		                "\n"
		                "ftdi_layout_init 0x0308 0x030b\n"
		                "ftdi_layout_signal nTRST -data 0x0100\n"
		                "ftdi_layout_signal nSRST -data 0x0200\n",
		.pcCode_Probe = "proc probe {} {\n"
			        "    set RESULT -1\n"
			        "\n"
		                "    if {[ catch {jtag init} ]==0 } {\n"
		                "        set RESULT {OK}\n"
		                "    }\n"
		                "    return $RESULT\n"
		                "}\n"
		                "probe\n"
	}
};



typedef struct TARGET_SETUP_STRUCT
{
	const char *pcID;
	const char *pcCode;
} TARGET_SETUP_STRUCT_T;

static const TARGET_SETUP_STRUCT_T atTargetCfg[2] =
{
	{
		.pcID = "netX_ARM966",
		.pcCode = "proc test {} {\n"
		          "    global SC_CFG_RESULT\n"
		          "    set SC_CFG_RESULT 0\n"
		          "\n"
		          "    jtag newtap netX_ARM966 cpu -irlen 4 -ircapture 1 -irmask 0xf -expected-id 0x25966021\n"
		          "    jtag configure netX_ARM966.cpu -event setup { global SC_CFG_RESULT ; echo {Yay} ; set SC_CFG_RESULT {OK} }\n"
		          "    jtag init\n"
		          "\n"
		          "    if { $SC_CFG_RESULT=={OK} } {\n"
		          "        target create netX_ARM966.cpu arm966e -endian little -chain-position netX_ARM966.cpu\n"
		          "    }\n"
		          "\n"
		          "    return $SC_CFG_RESULT\n"
		          "}\n"
		          "test\n"
	},

	{
		.pcID = "netX_ARM926",
		.pcCode = "proc test {} {\n"
		          "    global SC_CFG_RESULT\n"
		          "    set SC_CFG_RESULT 0\n"
		          "\n"
		          "    jtag newtap netX_ARM926 cpu -irlen 4 -ircapture 1 -irmask 0xf -expected-id 0x07926021\n"
		          "    jtag configure netX_ARM926.cpu -event setup { global SC_CFG_RESULT ; set SC_CFG_RESULT {OK} }\n"
		          "    jtag init\n"
		          "\n"
		          "    if { $SC_CFG_RESULT=={OK} } {\n"
		          "        target create netX_ARM926.cpu arm926ejs -endian little -chain-position netX_ARM926.cpu\n"
		          "    }\n"
		          "\n"
		          "    return $SC_CFG_RESULT\n"
		          "}\n"
		          "test\n"
	}
};



static int romloader_jtag_openocd_setup_interface(ROMLOADER_JTAG_DEVICE_T *ptDevice, const INTERFACE_SETUP_STRUCT_T *ptIfCfg)
{
	int iResult;


	/* Run the command chunk. */
	fprintf(stderr, "Run setup chunk for interface %s.\n", ptIfCfg->pcID);
	iResult = ptDevice->pfnCommandRunLine(ptDevice->pvOpenocdContext, ptIfCfg->pcCode_Setup);
	if( iResult!=0 )
	{
		fprintf(stderr, "Failed to run the chunk: %d\n", iResult);
	}

	return iResult;
}



static int romloader_jtag_openocd_probe_interface(ROMLOADER_JTAG_DEVICE_T *ptDevice, const INTERFACE_SETUP_STRUCT_T *ptIfCfg)
{
	int iResult;
	int sizResult;
	char strResult[256];


	/* Try to setup the interface. */
	iResult = romloader_jtag_openocd_setup_interface(ptDevice, ptIfCfg);
	if( iResult!=0 )
	{
		/* This is no fatal error. It just means that this interface can not be used. */
		iResult = 1;
	}
	else
	{
		/* Run the probe chunk. */
		fprintf(stderr, "Run probe chunk for interface %s.\n", ptIfCfg->pcID);
		iResult = ptDevice->pfnCommandRunLine(ptDevice->pvOpenocdContext, ptIfCfg->pcCode_Probe);
		if( iResult!=0 )
		{
			/* This is no fatal error. It just means that this interface can not be used. */
			fprintf(stderr, "Failed to run the chunk: %d\n", iResult);
			iResult = 1;
		}
		else
		{
			iResult = ptDevice->pfnGetResult(ptDevice->pvOpenocdContext, strResult, sizeof(strResult));
			if( iResult!=0 )
			{
				/* This is a fatal error. */
				fprintf(stderr, "Failed to get the result.\n");
				iResult = -1;
			}
			else
			{
				fprintf(stderr, "Result from probe: %s\n", strResult);
				iResult = strncmp(strResult, "OK", 3);
				if( iResult!=0 )
				{
					/* This is no fatal error. */
					iResult = 1;
				}
				else
				{
					fprintf(stderr, "Found interface %s!\n", ptIfCfg->pcID);
				}
			}
		}
	}

	return iResult;
}



static int romloader_jtag_openocd_probe_target(ROMLOADER_JTAG_DEVICE_T *ptDevice, const INTERFACE_SETUP_STRUCT_T *ptIfCfg, const TARGET_SETUP_STRUCT_T *ptTargetCfg)
{
	int iResult;
	int sizResult;
	char strResult[256];


	iResult = romloader_jtag_openocd_setup_interface(ptDevice, ptIfCfg);
	if( iResult!=0 )
	{
		/* This is always a fatal error here as the interface has been used before. */
		iResult = -1;
	}
	else
	{
		fprintf(stderr, "Running detect code for target %s!\n", ptTargetCfg->pcID);
		iResult = ptDevice->pfnCommandRunLine(ptDevice->pvOpenocdContext, ptTargetCfg->pcCode);
		if( iResult!=0 )
		{
			/* This is no fatal error. It just means that this CPU is not present. */
			fprintf(stderr, "Failed to run the command chunk: %d\n", iResult);
			iResult = 1;
		}
		else
		{
			iResult = ptDevice->pfnGetResult(ptDevice->pvOpenocdContext, strResult, sizeof(strResult));
			if( iResult!=0 )
			{
				/* This is a fatal error. */
				fprintf(stderr, "Failed to get the result for the code.\n");
				iResult = -1;
			}
			else
			{
				fprintf(stderr, "Result from detect: %s\n", strResult);
				iResult = strncmp(strResult, "OK", 3);
				if( iResult!=0 )
				{
					/* This is no fatal error. */
					iResult = 1;
				}
				else
				{
					fprintf(stderr, "Found target %s!\n", ptTargetCfg->pcID);
				}
			}
		}
	}

	return iResult;
}



static int romloader_jtag_openocd_detect_target(const INTERFACE_SETUP_STRUCT_T *ptIfCfg)
{
	const TARGET_SETUP_STRUCT_T *ptCnt;
	const TARGET_SETUP_STRUCT_T *ptEnd;
	int iResult;
	int sizResult;
	ROMLOADER_JTAG_DEVICE_T tDevice;
	char strResult[256];


	/* Try to run all command chunks to see which CPU is there. */
	ptCnt = atTargetCfg;
	ptEnd = atTargetCfg + (sizeof(atTargetCfg)/sizeof(atTargetCfg[0]));
	while( ptCnt<ptEnd )
	{
		/* Open the shared library. */
		iResult = openocd_open(&tDevice);
		if( iResult!=0 )
		{
			/* This is a fatal error. */
			break;
		}
		else
		{
			fprintf(stderr, "Detecting target %s\n", ptCnt->pcID);
			iResult = romloader_jtag_openocd_probe_target(&tDevice, ptIfCfg, ptCnt);

			openocd_close(&tDevice);
		}

		if( iResult==0 )
		{
			/* Found an entry! */
			break;
		}
		else if( iResult<0 )
		{
			/* Fatal error! */
			break;
		}
		
		/* Move to the next configuration. */
		++ptCnt;
	}

	return iResult;
}



int romloader_jtag_openocd_detect(void)
{
	const INTERFACE_SETUP_STRUCT_T *ptCnt;
	const INTERFACE_SETUP_STRUCT_T *ptEnd;
	int iResult;
	ROMLOADER_JTAG_DEVICE_T tDevice;


	/* Be pessimistic... */
	iResult = -1;

	/* Try to run all command chunks to see which interfaces are present. */
	ptCnt = atInterfaceCfg;
	ptEnd = atInterfaceCfg + (sizeof(atInterfaceCfg)/sizeof(atInterfaceCfg[0]));
	while( ptCnt<ptEnd )
	{
		fprintf(stderr, "Detecting interface %s\n", ptCnt->pcID);

		/* Open the shared library. */
		iResult = openocd_open(&tDevice);
		if( iResult!=0 )
		{
			/* This is a fatal error. */
			break;
		}
		else
		{
			iResult = romloader_jtag_openocd_probe_interface(&tDevice, ptCnt);

			openocd_close(&tDevice);

			if( iResult==0 )
			{
				/* Detect the CPU on this interface. */
				romloader_jtag_openocd_detect_target(ptCnt);
			}
			else if( iResult<0 )
			{
				/* This is a fatal error. */
				break;
			}
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


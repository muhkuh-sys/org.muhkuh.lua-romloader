#include "romloader_jtag_openocd.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "shared_library.h"


#if defined(_MSC_VER) || defined(__MINGW32__) || defined(__MINGW64__)
/* FIXME: search the shared library in some common places.
 * One solution would be the same folder as this LUA plugin. Here is a way to get the full path of something in Windows: https://msdn.microsoft.com/en-us/library/windows/desktop/ms683197.aspx
 * The same is possible for Linux: http://stackoverflow.com/questions/1642128/linux-how-to-get-full-name-of-shared-object-just-loaded-from-the-constructor
 */
#define OPENOCD_SHARED_LIBRARY_FILENAME "openocd.dll"

#elif defined(__GNUC__)
/* FIXME: search the shared library in some common places.
 * One solution would be the same folder as this LUA plugin. Here is a way to get the full path of something in Windows: https://msdn.microsoft.com/en-us/library/windows/desktop/ms683197.aspx
 * The same is possible for Linux: http://stackoverflow.com/questions/1642128/linux-how-to-get-full-name-of-shared-object-just-loaded-from-the-constructor
 */
#define OPENOCD_SHARED_LIBRARY_FILENAME "/tmp/n/libopenocd.so"


#endif



romloader_jtag_openocd::romloader_jtag_openocd(void)
 : fJtagDeviceIsConnected(false)
 , m_ptDetected(NULL)
 , m_sizDetectedCnt(0)
 , m_sizDetectedMax(0)
{
	memset(&m_tJtagDevice, 0, sizeof(ROMLOADER_JTAG_DEVICE_T));
}



romloader_jtag_openocd::~romloader_jtag_openocd(void)
{
	free_detect_entries();
}



const romloader_jtag_openocd::OPENOCD_NAME_RESOLVE_T romloader_jtag_openocd::atOpenOcdResolve[13] =
{
	{
		.pstrSymbolName = "muhkuh_openocd_init",
		.sizPointerOffset = offsetof(romloader_jtag_openocd::MUHKUH_OPENOCD_FUNCTION_POINTERS_T, pfnInit) / sizeof(void*)
	},
	{
		.pstrSymbolName = "muhkuh_openocd_get_result",
		.sizPointerOffset = offsetof(romloader_jtag_openocd::MUHKUH_OPENOCD_FUNCTION_POINTERS_T, pfnGetResult) / sizeof(void*)
	},
	{
		.pstrSymbolName = "muhkuh_openocd_command_run_line",
		.sizPointerOffset = offsetof(romloader_jtag_openocd::MUHKUH_OPENOCD_FUNCTION_POINTERS_T, pfnCommandRunLine) / sizeof(void*)
	},
	{
		.pstrSymbolName = "muhkuh_openocd_uninit",
		.sizPointerOffset = offsetof(romloader_jtag_openocd::MUHKUH_OPENOCD_FUNCTION_POINTERS_T, pfnUninit) / sizeof(void*)
	},
	{
		.pstrSymbolName = "muhkuh_openocd_read_data08",
		.sizPointerOffset = offsetof(romloader_jtag_openocd::MUHKUH_OPENOCD_FUNCTION_POINTERS_T, pfnReadData08) / sizeof(void*)
	},
	{
		.pstrSymbolName = "muhkuh_openocd_read_data16",
		.sizPointerOffset = offsetof(romloader_jtag_openocd::MUHKUH_OPENOCD_FUNCTION_POINTERS_T, pfnReadData16) / sizeof(void*)
	},
	{
		.pstrSymbolName = "muhkuh_openocd_read_data32",
		.sizPointerOffset = offsetof(romloader_jtag_openocd::MUHKUH_OPENOCD_FUNCTION_POINTERS_T, pfnReadData32) / sizeof(void*)
	},
	{
		.pstrSymbolName = "muhkuh_openocd_read_image",
		.sizPointerOffset = offsetof(romloader_jtag_openocd::MUHKUH_OPENOCD_FUNCTION_POINTERS_T, pfnReadImage) / sizeof(void*)
	},
	{
		.pstrSymbolName = "muhkuh_openocd_write_data08",
		.sizPointerOffset = offsetof(romloader_jtag_openocd::MUHKUH_OPENOCD_FUNCTION_POINTERS_T, pfnWriteData08) / sizeof(void*)
	},
	{
		.pstrSymbolName = "muhkuh_openocd_write_data16",
		.sizPointerOffset = offsetof(romloader_jtag_openocd::MUHKUH_OPENOCD_FUNCTION_POINTERS_T, pfnWriteData16) / sizeof(void*)
	},
	{
		.pstrSymbolName = "muhkuh_openocd_write_data32",
		.sizPointerOffset = offsetof(romloader_jtag_openocd::MUHKUH_OPENOCD_FUNCTION_POINTERS_T, pfnWriteData32) / sizeof(void*)
	},
	{
		.pstrSymbolName = "muhkuh_openocd_write_image",
		.sizPointerOffset = offsetof(romloader_jtag_openocd::MUHKUH_OPENOCD_FUNCTION_POINTERS_T, pfnWriteImage) / sizeof(void*)
	},
	{
		.pstrSymbolName = "muhkuh_openocd_call",
		.sizPointerOffset = offsetof(romloader_jtag_openocd::MUHKUH_OPENOCD_FUNCTION_POINTERS_T, pfnCall) / sizeof(void*)
	}
};



void romloader_jtag_openocd::free_detect_entries(void)
{
	ROMLOADER_JTAG_DETECT_ENTRY_T *ptCnt;
	ROMLOADER_JTAG_DETECT_ENTRY_T *ptEnd;


	if( m_ptDetected!=NULL )
	{
		/* Loop over all entries in the list. */
		ptCnt = m_ptDetected;
		ptEnd = m_ptDetected + m_sizDetectedCnt;
		while( ptCnt<ptEnd )
		{
			if( ptCnt->pcInterface!=NULL )
			{
				free(ptCnt->pcInterface);
			}
			if( ptCnt->pcTarget!=NULL )
			{
				free(ptCnt->pcTarget);
			}

			++ptCnt;
		}
		free(m_ptDetected);
		m_ptDetected = NULL;
	}
	m_sizDetectedCnt = 0;
	m_sizDetectedMax = 0;

}



int romloader_jtag_openocd::add_detected_entry(const char *pcInterface, const char *pcTarget)
{
	int iResult;
	ROMLOADER_JTAG_DETECT_ENTRY_T *ptDetectedNew;


	/* Be optimistic. */
	iResult = 0;

	/* Is enough space in the array for one more entry? */
	if( m_sizDetectedCnt>=m_sizDetectedMax )
	{
		/* No -> expand the array. */
		m_sizDetectedMax *= 2;
		/* Detect overflow or limitation. */
		if( m_sizDetectedMax<=m_sizDetectedCnt )
		{
			iResult = -1;
		}
		else
		{
			/* Reallocate the array. */
			ptDetectedNew = (ROMLOADER_JTAG_DETECT_ENTRY_T*)realloc(m_ptDetected, m_sizDetectedMax*sizeof(ROMLOADER_JTAG_DETECT_ENTRY_T));
			if( ptDetectedNew==NULL )
			{
				iResult = -1;
			}
			else
			{
				m_ptDetected = ptDetectedNew;
			}
		}
	}

	if( iResult==0 )
	{
		m_ptDetected[m_sizDetectedCnt].pcInterface = strdup(pcInterface);
		m_ptDetected[m_sizDetectedCnt].pcTarget = strdup(pcTarget);
		++m_sizDetectedCnt;
	}

	return iResult;
}


/*
   Try to open the shared library.
   If successful, resolve method names and initialize the shared libraly.
 */
int romloader_jtag_openocd::openocd_open(ROMLOADER_JTAG_DEVICE_T *ptDevice)
{
	int iResult;
	void *pvSharedLibraryHandle;
	const OPENOCD_NAME_RESOLVE_T *ptCnt;
	const OPENOCD_NAME_RESOLVE_T *ptEnd;
	void *pvFn;
	void *pvOpenocdContext;
	char acError[1024];


	/* Be optimistic. */
	iResult = 0;

	/* Try to open the shared library. */
	pvSharedLibraryHandle = sharedlib_open(OPENOCD_SHARED_LIBRARY_FILENAME);
	if( pvSharedLibraryHandle==NULL )
	{
		/* Failed to open the shared library. */
		sharedlib_get_error(acError, sizeof(acError));
		fprintf(stderr, "Failed to open the shared library %s.\n", OPENOCD_SHARED_LIBRARY_FILENAME);
		fprintf(stderr, "Error: %s\n", acError);
		iResult = -1;
	}
	else
	{
		ptDevice->pvSharedLibraryHandle = pvSharedLibraryHandle;

		/* Try to resolve all symbols. */
		ptCnt = atOpenOcdResolve;
		ptEnd = atOpenOcdResolve + (sizeof(atOpenOcdResolve)/sizeof(atOpenOcdResolve[0]));
		while( ptCnt<ptEnd )
		{
			pvFn = sharedlib_resolve_symbol(pvSharedLibraryHandle, ptCnt->pstrSymbolName);
			if( pvFn==NULL )
			{
				iResult = -1;
				break;
			}
			else
			{
				ptDevice->tFunctions.pv[ptCnt->sizPointerOffset] = pvFn;
				++ptCnt;
			}
		}

		if( iResult==0 )
		{
			/* Call the init function. */
			pvOpenocdContext = ptDevice->tFunctions.tFn.pfnInit();
			if( pvOpenocdContext==NULL )
			{
				fprintf(stderr, "Failed to initialize the OpenOCD device context.\n");
				iResult = -1;
			}
			else
			{
				ptDevice->pvOpenocdContext = pvOpenocdContext;
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


/* Uninitialize and cose the shared library. */
void romloader_jtag_openocd::openocd_close(ROMLOADER_JTAG_DEVICE_T *ptDevice)
{
	if( ptDevice->pvSharedLibraryHandle!=NULL )
	{
		if( ptDevice->pvOpenocdContext!=NULL )
		{
			ptDevice->tFunctions.tFn.pfnUninit(ptDevice->pvOpenocdContext);
		}

		/* Close the shared library. */
		sharedlib_close(ptDevice->pvSharedLibraryHandle);
	}
}



/* The romloader_jtag_openocd_init function opens the OpenOCD shared library,
   executes the "version" command and closes the library.

   This is a test if the shared library can be used.
*/
int romloader_jtag_openocd::initialize(void)
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
		iResult = tDevice.tFunctions.tFn.pfnCommandRunLine(tDevice.pvOpenocdContext, "version\n");
		if( iResult!=0 )
		{
			fprintf(stderr, "Failed to run the version command!\n");
		}
		else
		{
			iResult = tDevice.tFunctions.tFn.pfnGetResult(tDevice.pvOpenocdContext, acResult, sizeof(acResult));
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



/* This array contains the detection command sequence for all interfaces. All
 * commands for one interface are stored in one string. The commands are
 * separated by newlines.
 *
 * Every detection sequence must set the TCL variable 'IF_CFG_RESULT'. It
 * shows the result of the detection. A value of '1'
 *
 * Use a ridiculously low speed to ensure that the connection is working in
 * absolutely all circumstances (even with Enricos flying wires. ;)
 */

/* FIXME: read this from some kind of configuration file. */

const romloader_jtag_openocd::INTERFACE_SETUP_STRUCT_T romloader_jtag_openocd::atInterfaceCfg[5] =
{
	{
		.pcID = "NXJTAG-USB",
		.pcCode_Setup = "interface ftdi\n"
                                "transport select jtag\n"
		                "ftdi_device_desc \"NXJTAG-USB\"\n"
		                "ftdi_vid_pid 0x1939 0x0023\n"
		                "adapter_khz 100\n"
		                "\n"
		                "ftdi_layout_init 0x0308 0x030b\n"
		                "ftdi_layout_signal nTRST -data 0x0100 -oe 0x0100\n"
		                "ftdi_layout_signal nSRST -data 0x0200 -oe 0x0200\n",
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
		.pcID = "Amontec_JTAGkey",
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
				"ftdi_layout_init 0x0108 0x010b\n"
				"ftdi_layout_signal nTRST -data 0x0100\n"
				"ftdi_layout_signal nSRST -data 0x0200 -oe 0x0200\n",
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
		.pcID = "NXHX_51-ETM",
		.pcCode_Setup = "interface ftdi\n"
                                "transport select jtag\n"
		                "ftdi_device_desc \"NXHX 51-ETM\"\n"
		                "ftdi_vid_pid 0x0640 0x0028\n"
		                "adapter_khz 100\n"
		                "\n"
				"ftdi_layout_init 0x0108 0x010b\n"
				"ftdi_layout_signal nTRST -data 0x0100\n"
				"ftdi_layout_signal nSRST -data 0x0200 -oe 0x0200\n",
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
		.pcID = "NXHX_500-ETM",
		.pcCode_Setup = "interface ftdi\n"
                                "transport select jtag\n"
		                "ftdi_device_desc \"NXHX 500-ETM\"\n"
		                "ftdi_vid_pid 0x0640 0x0028\n"
		                "adapter_khz 100\n"
		                "\n"
				"ftdi_layout_init 0x0108 0x010b\n"
				"ftdi_layout_signal nTRST -data 0x0100\n"
				"ftdi_layout_signal nSRST -data 0x0200 -oe 0x0200\n",
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


/* FIXME: read this from some kind of configuration file. */
const romloader_jtag_openocd::TARGET_SETUP_STRUCT_T romloader_jtag_openocd::atTargetCfg[2] =
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
		          "        netX_ARM966.cpu configure -event reset-init { halt }\n"
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
		          "    jtag configure netX_ARM926.cpu -event setup { global SC_CFG_RESULT ; echo {Yay} ; set SC_CFG_RESULT {OK} }\n"
		          "    jtag init\n"
		          "\n"
		          "    if { $SC_CFG_RESULT=={OK} } {\n"
		          "        target create netX_ARM926.cpu arm926ejs -endian little -chain-position netX_ARM926.cpu\n"
		          "        netX_ARM926.cpu configure -event reset-init { halt }\n"
		          "    }\n"
		          "\n"
		          "    return $SC_CFG_RESULT\n"
		          "}\n"
		          "test\n"
	}
};


/* FIXME: read this from some kind of configuration file. */
/* TODO: Is it necessary to have a special reset code for every interface/target pair? */
const char *romloader_jtag_openocd::pcResetCode = "reset_config trst_and_srst\n"
                                 "adapter_nsrst_delay 500\n"
                                 "jtag_ntrst_delay 500\n"
                                 "\n"
                                 "init\n"
                                 "reset init\n";



int romloader_jtag_openocd::setup_interface(ROMLOADER_JTAG_DEVICE_T *ptDevice, const INTERFACE_SETUP_STRUCT_T *ptIfCfg)
{
	int iResult;


	/* Run the command chunk. */
	fprintf(stderr, "Run setup chunk for interface %s.\n", ptIfCfg->pcID);
	iResult = ptDevice->tFunctions.tFn.pfnCommandRunLine(ptDevice->pvOpenocdContext, ptIfCfg->pcCode_Setup);
	if( iResult!=0 )
	{
		fprintf(stderr, "Failed to run the chunk: %d\n", iResult);
	}

	return iResult;
}


/* Probe for a single interface. */
int romloader_jtag_openocd::probe_interface(ROMLOADER_JTAG_DEVICE_T *ptDevice, const INTERFACE_SETUP_STRUCT_T *ptIfCfg)
{
	int iResult;
	int sizResult;
	char strResult[256];


	/* Try to setup the interface. */
	iResult = setup_interface(ptDevice, ptIfCfg);
	if( iResult!=0 )
	{
		/* This is no fatal error. It just means that this interface can not be used. */
		iResult = 1;
	}
	else
	{
		/* Run the probe chunk. */
		fprintf(stderr, "Run probe chunk for interface %s.\n", ptIfCfg->pcID);
		iResult = ptDevice->tFunctions.tFn.pfnCommandRunLine(ptDevice->pvOpenocdContext, ptIfCfg->pcCode_Probe);
		if( iResult!=0 )
		{
			/* This is no fatal error. It just means that this interface can not be used. */
			fprintf(stderr, "Failed to run the chunk: %d\n", iResult);
			iResult = 1;
		}
		else
		{
			iResult = ptDevice->tFunctions.tFn.pfnGetResult(ptDevice->pvOpenocdContext, strResult, sizeof(strResult));
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


/* Find an interface configuration with the given ID. */
const romloader_jtag_openocd::INTERFACE_SETUP_STRUCT_T *romloader_jtag_openocd::find_interface(const char *pcInterfaceName)
{
	const INTERFACE_SETUP_STRUCT_T *ptCnt;
	const INTERFACE_SETUP_STRUCT_T *ptEnd;
	const INTERFACE_SETUP_STRUCT_T *ptHit;
	int iCompareResult;


	/* No match yet. */
	ptHit = NULL;

	ptCnt = atInterfaceCfg;
	ptEnd = atInterfaceCfg + (sizeof(atInterfaceCfg)/sizeof(atInterfaceCfg[0]));
	while( ptCnt<ptEnd )
	{
		iCompareResult = strcmp(ptCnt->pcID, pcInterfaceName);
		if( iCompareResult==0 )
		{
			ptHit = ptCnt;
			break;
		}

		++ptCnt;
	}

	return ptHit;
}


/* Probe for a single interface/target combination. */
int romloader_jtag_openocd::probe_target(ROMLOADER_JTAG_DEVICE_T *ptDevice, const INTERFACE_SETUP_STRUCT_T *ptIfCfg, const TARGET_SETUP_STRUCT_T *ptTargetCfg)
{
	int iResult;
	int sizResult;
	char strResult[256];


	iResult = setup_interface(ptDevice, ptIfCfg);
	if( iResult!=0 )
	{
		/* This is always a fatal error here as the interface has been used before. */
		iResult = -1;
	}
	else
	{
		fprintf(stderr, "Running detect code for target %s!\n", ptTargetCfg->pcID);
		iResult = ptDevice->tFunctions.tFn.pfnCommandRunLine(ptDevice->pvOpenocdContext, ptTargetCfg->pcCode);
		if( iResult!=0 )
		{
			/* This is no fatal error. It just means that this CPU is not present. */
			fprintf(stderr, "Failed to run the command chunk: %d\n", iResult);
			iResult = 1;
		}
		else
		{
			iResult = ptDevice->tFunctions.tFn.pfnGetResult(ptDevice->pvOpenocdContext, strResult, sizeof(strResult));
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


/* Try all available targets with a given interface. */
int romloader_jtag_openocd::detect_target(const INTERFACE_SETUP_STRUCT_T *ptIfCfg)
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
			iResult = probe_target(&tDevice, ptIfCfg, ptCnt);

			openocd_close(&tDevice);
		}

		if( iResult==0 )
		{
			/* Found an entry! */
			iResult = add_detected_entry(ptIfCfg->pcID, ptCnt->pcID);
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


/* Find a target with the given name. */
const romloader_jtag_openocd::TARGET_SETUP_STRUCT_T *romloader_jtag_openocd::find_target(const char *pcTargetName)
{
	const TARGET_SETUP_STRUCT_T *ptCnt;
	const TARGET_SETUP_STRUCT_T *ptEnd;
	const TARGET_SETUP_STRUCT_T *ptHit;
	int iCompareResult;


	/* No match yet. */
	ptHit = NULL;

	ptCnt = atTargetCfg;
	ptEnd = atTargetCfg + (sizeof(atTargetCfg)/sizeof(atTargetCfg[0]));
	while( ptCnt<ptEnd )
	{
		iCompareResult = strcmp(ptCnt->pcID, pcTargetName);
		if( iCompareResult==0 )
		{
			ptHit = ptCnt;
			break;
		}

		++ptCnt;
	}

	return ptHit;
}


/*
   Detect interfaces/targets.
   Probe for all known interfaces. For each interface found, probe for all known targets.
   Returns a list of ROMLOADER_JTAG_DETECT_ENTRY_T in pptEntries/psizEntries.
 */
int romloader_jtag_openocd::detect(ROMLOADER_JTAG_DETECT_ENTRY_T **pptEntries, size_t *psizEntries)
{
	const INTERFACE_SETUP_STRUCT_T *ptCnt;
	const INTERFACE_SETUP_STRUCT_T *ptEnd;
	int iResult;
	ROMLOADER_JTAG_DEVICE_T tDevice;


	/* Be pessimistic... */
	iResult = -1;

	/* Clear any old results. */
	free_detect_entries();

	/* Initialize the result array. */
	m_sizDetectedCnt = 0;
	m_sizDetectedMax = 16;
	m_ptDetected = (ROMLOADER_JTAG_DETECT_ENTRY_T*)malloc(m_sizDetectedMax*sizeof(ROMLOADER_JTAG_DETECT_ENTRY_T));
	if( m_ptDetected==NULL )
	{
		fprintf(stderr, "Failed to allocate %zd bytes of memory for the detection results!\n", m_sizDetectedMax*sizeof(ROMLOADER_JTAG_DETECT_ENTRY_T));
	}
	else
	{
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
				iResult = -1;
				break;
			}
			else
			{
				/* Detect the interface. */
				iResult = probe_interface(&tDevice, ptCnt);

				/* Clean up after the detection. */
				openocd_close(&tDevice);

				if( iResult==0 )
				{
					/* Detect the CPU on this interface. */
					iResult = detect_target(ptCnt);
				}

				/* Ignore non-fatal errors.
				 * They indicate that the current interface could not be detected.
				 */
				if( iResult>0 )
				{
					iResult = 0;
				}
				/* Do not continue with other interfaces if a fatal error occurred. */
				else if( iResult<0 )
				{
					/* This is a fatal error. */
					break;
				}
			}

			/* Move to the next configuration. */
			++ptCnt;
		}
	}

	/* Discard all results if something really bad happened (like a fatal error). */
	if( iResult<0 )
	{
		free_detect_entries();
	}

	/* Return the result list. */
	*pptEntries = m_ptDetected;
	*psizEntries = m_sizDetectedCnt;

	return iResult;
}


/* Open a connection to a given target on a given interface. */
/* Open the connection to the device. */
int romloader_jtag_openocd::connect(const char *pcInterfaceName, const char *pcTargetName)
{
	int iResult;
	const INTERFACE_SETUP_STRUCT_T *ptInterface;
	const TARGET_SETUP_STRUCT_T *ptTarget;


	if( fJtagDeviceIsConnected==true )
	{
		fprintf(stderr, "romloader_jtag_openocd::connect: Already connected.\n");
	}
	else
	{
		ptInterface = find_interface(pcInterfaceName);
		if( ptInterface==NULL )
		{
			fprintf(stderr, "Could not find interface %s!\n", pcInterfaceName);
		}
		else
		{
			ptTarget = find_target(pcTargetName);
			if( ptTarget==NULL )
			{
				fprintf(stderr, "Could not find target %s!\n", pcTargetName);
			}
			else
			{
				/* Open the shared library. */
				iResult = openocd_open(&m_tJtagDevice);
				if( iResult==0 )
				{
					iResult = probe_target(&m_tJtagDevice, ptInterface, ptTarget);
					if( iResult==0 )
					{
						/* Stop the target. */
						fprintf(stderr, "Running reset code.\n");
						iResult = m_tJtagDevice.tFunctions.tFn.pfnCommandRunLine(m_tJtagDevice.pvOpenocdContext, pcResetCode);
						if( iResult!=0 )
						{
							fprintf(stderr, "Failed to run the reset code: %d\n", iResult);
							iResult = -1;
						}
						else
						{
							fJtagDeviceIsConnected = true;
						}
					}
				}
			}
		}
	}

	return iResult;
}



/* Close the connection to the device. */
void romloader_jtag_openocd::disconnect(void)
{
	openocd_close(&m_tJtagDevice);
	fJtagDeviceIsConnected = false;
}



/* Read a byte (8bit) from the netX. */
int romloader_jtag_openocd::read_data08(uint32_t ulNetxAddress, uint8_t *pucData)
{
	int iResult;


	/* Be pessimistic. */
	iResult = -1;

	if( fJtagDeviceIsConnected==true && m_tJtagDevice.pvOpenocdContext!=NULL && m_tJtagDevice.tFunctions.tFn.pfnReadData08!=NULL )
	{
		/* Read memory. */
		iResult = m_tJtagDevice.tFunctions.tFn.pfnReadData08(m_tJtagDevice.pvOpenocdContext, ulNetxAddress, pucData);
		if( iResult!=0 )
		{
			fprintf(stderr, "read_data08: Failed to read address 0x%08x: %d\n", ulNetxAddress, iResult);
			iResult = -1;
		}
	}

	return iResult;
}



/* Read a word (16bit) from the netX. */
int romloader_jtag_openocd::read_data16(uint32_t ulNetxAddress, uint16_t *pusData)
{
	int iResult;


	/* Be pessimistic. */
	iResult = -1;

	if( fJtagDeviceIsConnected==true && m_tJtagDevice.pvOpenocdContext!=NULL && m_tJtagDevice.tFunctions.tFn.pfnReadData16!=NULL )
	{
		/* Read memory. */
		iResult = m_tJtagDevice.tFunctions.tFn.pfnReadData16(m_tJtagDevice.pvOpenocdContext, ulNetxAddress, pusData);
		if( iResult!=0 )
		{
			fprintf(stderr, "read_data16: Failed to read address 0x%08x: %d\n", ulNetxAddress, iResult);
			iResult = -1;
		}
	}

	return iResult;
}



/* Read a long (32bit) from the netX. */
int romloader_jtag_openocd::read_data32(uint32_t ulNetxAddress, uint32_t *pulData)
{
	int iResult;


	/* Be pessimistic. */
	iResult = -1;

	if( fJtagDeviceIsConnected==true && m_tJtagDevice.pvOpenocdContext!=NULL && m_tJtagDevice.tFunctions.tFn.pfnReadData32!=NULL )
	{
		/* Read memory. */
		iResult = m_tJtagDevice.tFunctions.tFn.pfnReadData32(m_tJtagDevice.pvOpenocdContext, ulNetxAddress, pulData);
		if( iResult!=0 )
		{
			fprintf(stderr, "read_data32: Failed to read address 0x%08x: %d\n", ulNetxAddress, iResult);
			iResult = -1;
		}
	}

	return iResult;
}



/* Read a byte array from the netX. */
int romloader_jtag_openocd::read_image(uint32_t ulNetxAddress, uint8_t *pucData, uint32_t sizData)
{
	int iResult;


	/* Be pessimistic. */
	iResult = -1;

	if( fJtagDeviceIsConnected==true && m_tJtagDevice.pvOpenocdContext!=NULL && m_tJtagDevice.tFunctions.tFn.pfnReadImage!=NULL )
	{
		/* Read memory. */
		iResult = m_tJtagDevice.tFunctions.tFn.pfnReadImage(m_tJtagDevice.pvOpenocdContext, ulNetxAddress, pucData, sizData);
		if( iResult!=0 )
		{
			fprintf(stderr, "read_data32: Failed to read address 0x%08x: %d\n", ulNetxAddress, iResult);
			iResult = -1;
		}
	}

	return iResult;
}



/* Write a byte (8bit) to the netX. */
int romloader_jtag_openocd::write_data08(uint32_t ulNetxAddress, uint8_t ucData)
{
	int iResult;


	/* Be pessimistic. */
	iResult = -1;

	if( fJtagDeviceIsConnected==true && m_tJtagDevice.pvOpenocdContext!=NULL && m_tJtagDevice.tFunctions.tFn.pfnWriteData08!=NULL )
	{
		/* Read memory. */
		iResult = m_tJtagDevice.tFunctions.tFn.pfnWriteData08(m_tJtagDevice.pvOpenocdContext, ulNetxAddress, ucData);
		if( iResult!=0 )
		{
			fprintf(stderr, "read_data32: Failed to read address 0x%08x: %d\n", ulNetxAddress, iResult);
			iResult = -1;
		}
	}

	return iResult;
}



/* Write a word (16bit) to the netX. */
int romloader_jtag_openocd::write_data16(uint32_t ulNetxAddress, uint16_t usData)
{
	int iResult;


	/* Be pessimistic. */
	iResult = -1;

	if( fJtagDeviceIsConnected==true && m_tJtagDevice.pvOpenocdContext!=NULL && m_tJtagDevice.tFunctions.tFn.pfnWriteData16!=NULL )
	{
		/* Read memory. */
		iResult = m_tJtagDevice.tFunctions.tFn.pfnWriteData16(m_tJtagDevice.pvOpenocdContext, ulNetxAddress, usData);
		if( iResult!=0 )
		{
			fprintf(stderr, "read_data32: Failed to read address 0x%08x: %d\n", ulNetxAddress, iResult);
			iResult = -1;
		}
	}

	return iResult;
}



/* Write a long (32bit) to the netX. */
int romloader_jtag_openocd::write_data32(uint32_t ulNetxAddress, uint32_t ulData)
{
	int iResult;


	/* Be pessimistic. */
	iResult = -1;

	if( fJtagDeviceIsConnected==true && m_tJtagDevice.pvOpenocdContext!=NULL && m_tJtagDevice.tFunctions.tFn.pfnWriteData32!=NULL )
	{
		/* Read memory. */
		iResult = m_tJtagDevice.tFunctions.tFn.pfnWriteData32(m_tJtagDevice.pvOpenocdContext, ulNetxAddress, ulData);
		if( iResult!=0 )
		{
			fprintf(stderr, "read_data32: Failed to read address 0x%08x: %d\n", ulNetxAddress, iResult);
			iResult = -1;
		}
	}

	return iResult;
}



/* Write a byte array to the netX. */
int romloader_jtag_openocd::write_image(uint32_t ulNetxAddress, const uint8_t *pucData, uint32_t sizData)
{
	int iResult;


	/* Be pessimistic. */
	iResult = -1;

	if( fJtagDeviceIsConnected==true && m_tJtagDevice.pvOpenocdContext!=NULL && m_tJtagDevice.tFunctions.tFn.pfnWriteImage!=NULL )
	{
		/* Read memory. */
		iResult = m_tJtagDevice.tFunctions.tFn.pfnWriteImage(m_tJtagDevice.pvOpenocdContext, ulNetxAddress, pucData, sizData);
		if( iResult!=0 )
		{
			fprintf(stderr, "read_data32: Failed to read address 0x%08x: %d\n", ulNetxAddress, iResult);
			iResult = -1;
		}
	}

	return iResult;
}


#if 0
int romloader_jtag_openocd::call(uint32_t ulNetxAddress, uint32_t ulParameterR0, PFN_MUHKUH_CALL_CALLBACK pfnCallback)
{
	int iResult;


	/* Be pessimistic. */
	iResult = -1;



	return iResult;
}
#endif


uint32_t romloader_jtag_openocd::get_image_chunk_size(void)
{
	/* This is the suggested size of a data chunk before executing the LUA callback. */
	/* TODO: Is a fixed number OK or should this depend on some parameters like the JTAG speed?
	 *       Maybe this could also be one fixed option for every interface/target pair - just like the reset code.
	 */
	return 1024;
}



void romloader_jtag_openocd::uninit(void)
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


#include "romloader_papa_schlumpf.h"

#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "papa_schlumpf_firmware_interface.h"


/*-------------------------------------*/

#if defined(_MSC_VER)
#       define snprintf _snprintf
#       define ssize_t size_t
#endif

/*-------------------------------------*/

#if CFG_DEBUGMSG!=0
	/* show all messages by default */
	static unsigned long s_ulCurSettings = 0xffffffff;

	#define DEBUGZONE(n)  (s_ulCurSettings&(0x00000001<<(n)))

	/* NOTE: These defines must match the ZONE_* defines. */
	#define DBG_ZONE_ERROR      0
	#define DBG_ZONE_WARNING    1
	#define DBG_ZONE_FUNCTION   2
	#define DBG_ZONE_INIT       3
	#define DBG_ZONE_VERBOSE    7

	#define ZONE_ERROR          DEBUGZONE(DBG_ZONE_ERROR)
	#define ZONE_WARNING        DEBUGZONE(DBG_ZONE_WARNING)
	#define ZONE_FUNCTION       DEBUGZONE(DBG_ZONE_FUNCTION)
	#define ZONE_INIT           DEBUGZONE(DBG_ZONE_INIT)
	#define ZONE_VERBOSE        DEBUGZONE(DBG_ZONE_VERBOSE)

	#define DEBUGMSG(cond,printf_exp) ((void)((cond)?(printf printf_exp),1:0))
#else  /* CFG_DEBUGMSG!=0 */
	#define DEBUGMSG(cond,printf_exp) ((void)0)
#endif /* CFG_DEBUGMSG!=0 */




typedef union PAPA_SCHLUMPF_PARAMETER_DATA_UNION
{
	PAPA_SCHLUMPF_PARAMETER_T tPapaSchlumpfParameter;
	uint8_t auc[sizeof(PAPA_SCHLUMPF_PARAMETER_T)];
} PAPA_SCHLUMPF_PARAMETER_DATA_T;



typedef union PAPA_SCHLUMPF_DATA_BUFFER_UNION
{
	uint8_t  aucBuf[64];
	uint16_t ausBuf[64 / sizeof(uint16_t)];
	uint32_t aulBuf[64 / sizeof(uint32_t)];
} PAPA_SCHLUMPF_DATA_BUFFER_T;


/*-------------------------------------*/

const char *romloader_papa_schlumpf_provider::m_pcPluginNamePattern = "romloader_papa_schlumpf_%02x_%02x";

romloader_papa_schlumpf_provider::romloader_papa_schlumpf_provider(swig_type_info *p_romloader_papa_schlumpf, swig_type_info *p_romloader_papa_schlumpf_reference)
 : muhkuh_plugin_provider("romloader_papa_schlumpf")
 , m_ptDevice(NULL)
{
	DEBUGMSG(ZONE_FUNCTION, ("+romloader_papa_schlumpf_provider::romloader_papa_schlumpf_provider(): p_romloader_papa_schlumpf=%p, p_romloader_papa_schlumpf_reference=%p\n", p_romloader_papa_schlumpf, p_romloader_papa_schlumpf_reference));

	/* Get the romloader_papa_schlumpf LUA type. */
	m_ptPluginTypeInfo = p_romloader_papa_schlumpf;
	m_ptReferenceTypeInfo = p_romloader_papa_schlumpf_reference;

	m_ptDevice = new romloader_papa_schlumpf_device(m_pcPluginId);

	DEBUGMSG(ZONE_FUNCTION, ("-romloader_papa_schlumpf_provider::romloader_papa_schlumpf_provider()\n"));
}


romloader_papa_schlumpf_provider::~romloader_papa_schlumpf_provider(void)
{
	DEBUGMSG(ZONE_FUNCTION, ("+romloader_papa_schlumpf_provider::~romloader_papa_schlumpf_provider()\n"));

	if( m_ptDevice!=NULL )
	{
		delete m_ptDevice;
		m_ptDevice = NULL;
	}

	DEBUGMSG(ZONE_FUNCTION, ("-romloader_papa_schlumpf_provider::~romloader_papa_schlumpf_provider()\n"));
}


int romloader_papa_schlumpf_provider::DetectInterfaces(lua_State *ptLuaStateForTableAccess)
{
	int iResult;
	romloader_papa_schlumpf_device::PAPA_SCHLUMPF_ERROR_T tResult;
	romloader_papa_schlumpf_device::INTERFACE_REFERENCE_T *ptIf;
	romloader_papa_schlumpf_device::INTERFACE_REFERENCE_T *ptIfCnt;
	romloader_papa_schlumpf_device::INTERFACE_REFERENCE_T *ptIfEnd;
	size_t sizIf;
	romloader_papa_schlumpf_reference *ptRef;
	int iReferences;
	const size_t sizMaxName = 32;
	char acName[sizMaxName];


	DEBUGMSG(ZONE_FUNCTION, ("+romloader_papa_schlumpf_provider::DetectInterfaces(): ptLuaStateForTableAccess=%p\n", ptLuaStateForTableAccess));

	iReferences = 0;

	if( m_ptDevice!=NULL )
	{
		/* Detect all devices. */
		ptIf = NULL;
		tResult = m_ptDevice->detect_interfaces(&ptIf, &sizIf);
		if( tResult==romloader_papa_schlumpf_device::PAPA_SCHLUMPF_ERROR_Ok && ptIf!=NULL )
		{
			ptIfCnt = ptIf;
			ptIfEnd = ptIf + sizIf;
			while( ptIfCnt<ptIfEnd )
			{
				snprintf(acName, sizMaxName-1, m_pcPluginNamePattern, ptIfCnt->uiBusNr, ptIfCnt->uiDevAdr);
				ptRef = new romloader_papa_schlumpf_reference(acName, m_pcPluginId, ptIfCnt->acPathString, false, this);
				add_reference_to_table(ptLuaStateForTableAccess, ptRef);
				++ptIfCnt;
				++iReferences;
			}

			free(ptIf);
		}
	}

	DEBUGMSG(ZONE_FUNCTION, ("-romloader_papa_schlumpf_provider::DetectInterfaces(): iReferences=%d\n", iReferences));

	return iReferences;
}


romloader_papa_schlumpf *romloader_papa_schlumpf_provider::ClaimInterface(const muhkuh_plugin_reference *ptReference)
{
	romloader_papa_schlumpf *ptPlugin;
	const char *pcName;
	unsigned int uiBusNr;
	unsigned int uiDevAdr;


	DEBUGMSG(ZONE_FUNCTION, ("+romloader_papa_schlumpf_provider::ClaimInterface(): ptReference=%p\n", ptReference));

	/* expect error */
	ptPlugin = NULL;


	if( ptReference==NULL )
	{
		fprintf(stderr, "%s(%p): claim_interface(): missing reference!\n", m_pcPluginId, this);
	}
	else
	{
		pcName = ptReference->GetName();
		if( pcName==NULL )
		{
			fprintf(stderr, "%s(%p): claim_interface(): missing name!\n", m_pcPluginId, this);
		}
		else if( sscanf(pcName, m_pcPluginNamePattern, &uiBusNr, &uiDevAdr)!=2 )
		{
			fprintf(stderr, "%s(%p): claim_interface(): invalid name: %s\n", m_pcPluginId, this, pcName);
		}
		else if( m_ptDevice==NULL )
		{
			fprintf(stderr, "%s(%p): the device was not initialized!\n", m_pcPluginId, this);
		}
		else
		{
			ptPlugin = new romloader_papa_schlumpf(pcName, m_pcPluginId, this, uiBusNr, uiDevAdr);
			printf("%s(%p): claim_interface(): claimed interface %s.\n", m_pcPluginId, this, pcName);
		}
	}

	DEBUGMSG(ZONE_FUNCTION, ("-romloader_papa_schlumpf_provider::ClaimInterface(): ptPlugin=%p\n", ptPlugin));

	return ptPlugin;
}


bool romloader_papa_schlumpf_provider::ReleaseInterface(muhkuh_plugin *ptPlugin)
{
	bool fOk;
	const char *pcName;
	unsigned int uiBusNr;
	unsigned int uiDevAdr;


	DEBUGMSG(ZONE_FUNCTION, ("+romloader_papa_schlumpf_provider::ReleaseInterface(): ptPlugin=%p\n", ptPlugin));

	/* expect error */
	fOk = false;

	if( ptPlugin==NULL )
	{
		fprintf(stderr, "%s(%p): release_interface(): missing plugin!\n", m_pcPluginId, this);
	}
	else
	{
		pcName = ptPlugin->GetName();
		if( pcName==NULL )
		{
			fprintf(stderr, "%s(%p): release_interface(): missing name!\n", m_pcPluginId, this);
		}
		else if( sscanf(pcName, m_pcPluginNamePattern, &uiBusNr, &uiDevAdr)!=2 )
		{
			fprintf(stderr, "%s(%p): release_interface(): invalid name: %s\n", m_pcPluginId, this, pcName);
		}
		else
		{
			printf("%s(%p): released interface %s.\n", m_pcPluginId, this, pcName);
			fOk = true;
		}
	}

	DEBUGMSG(ZONE_FUNCTION, ("-romloader_papa_schlumpf_provider::ReleaseInterface(): fOk=%d\n", fOk));

	return fOk;
}


/*-------------------------------------*/

romloader_papa_schlumpf::romloader_papa_schlumpf(const char *pcName, const char *pcTyp, romloader_papa_schlumpf_provider *ptProvider, unsigned int uiBusNr, unsigned int uiDeviceAdr)
 : romloader(pcName, pcTyp, ptProvider)
 , m_ptDevice(NULL)
 , m_ptProvider(ptProvider)
 , m_uiBusNr(uiBusNr)
 , m_uiDeviceAdr(uiDeviceAdr)
 , m_ptCallBackLuaFn(NULL)
 , m_lCallBackUserData(0)
{
	DEBUGMSG(ZONE_FUNCTION, ("+romloader_papa_schlumpf::romloader_papa_schlumpf(): pcName='%s', pcTyp='%s', ptProvider=%p, uiBusNr=%d, uiDeviceAdr=%d\n", pcName, pcTyp, ptProvider, uiBusNr, uiDeviceAdr));

	m_ptDevice = new romloader_papa_schlumpf_device(m_pcName);

	DEBUGMSG(ZONE_FUNCTION, ("-romloader_papa_schlumpf::romloader_usb()\n"));
}



romloader_papa_schlumpf::romloader_papa_schlumpf(const char *pcName, const char *pcTyp, const char *pcLocation, romloader_papa_schlumpf_provider *ptProvider, unsigned int uiBusNr, unsigned int uiDeviceAdr)
 : romloader(pcName, pcTyp, pcLocation, ptProvider)
 , m_ptDevice(NULL)
 , m_ptProvider(ptProvider)
 , m_uiBusNr(uiBusNr)
 , m_uiDeviceAdr(uiDeviceAdr)
 , m_ptCallBackLuaFn(NULL)
 , m_lCallBackUserData(0)
{
	DEBUGMSG(ZONE_FUNCTION, ("+romloader_papa_schlumpf::romloader_papa_schlumpf(): pcName='%s', pcTyp='%s', ptProvider=%p, uiBusNr=%d, uiDeviceAdr=%d\n", pcName, pcTyp, ptProvider, uiBusNr, uiDeviceAdr));

	m_ptDevice = new romloader_papa_schlumpf_device(m_pcName);

	DEBUGMSG(ZONE_FUNCTION, ("-romloader_papa_schlumpf::romloader_papa_schlumpf()\n"));
}



romloader_papa_schlumpf::~romloader_papa_schlumpf(void)
{
	DEBUGMSG(ZONE_FUNCTION, ("-romloader_papa_schlumpf::~romloader_papa_schlumpf()\n"));

	if( m_ptDevice!=NULL )
	{
		delete m_ptDevice;
		m_ptDevice = NULL;
	}

	DEBUGMSG(ZONE_FUNCTION, ("-romloader_papa_schlumpf::~romloader_papa_schlumpf()\n"));
}



/* open the connection to the device */
void romloader_papa_schlumpf::Connect(lua_State *ptClientData)
{
	int iResult;


	DEBUGMSG(ZONE_FUNCTION, ("+romloader_papa_schlumpf::Connect(): ptClientData=%p\n", ptClientData));

	if( m_ptDevice==NULL )
	{
		fprintf(stderr, "Device not initialized!\n");
	}
	else if( m_fIsConnected!=false )
	{
		printf("%s(%p): already connected, ignoring new connect request\n", m_pcName, this);
	}
	else
	{
		iResult = m_ptDevice->Connect(m_uiBusNr, m_uiDeviceAdr);
		if( iResult!=LIBUSB_SUCCESS )
		{
			MUHKUH_PLUGIN_PUSH_ERROR(ptClientData, "%s(%p): failed to connect to device", m_pcName, this);
		}
		else
		{
			/* NOTE: set m_fIsConnected to true here or detect_chiptyp and chip_init will fail! */
			m_fIsConnected = true;
			/* FIXME: detect the netX type. At the moment we have already netX500 and netX100 and in the future there will be netX4000 cards. */
			m_tChiptyp = ROMLOADER_CHIPTYP_NETX100;
		}

		DEBUGMSG(ZONE_FUNCTION, ("-romloader_papa_schlumpf::Connect(): iResult=%d\n", iResult));

		if( iResult!=LIBUSB_SUCCESS )
		{
			MUHKUH_PLUGIN_EXIT_ERROR(ptClientData);
		}
	}
}



/* close the connection to the device */
void romloader_papa_schlumpf::Disconnect(lua_State *ptClientData)
{
	DEBUGMSG(ZONE_FUNCTION, ("+romloader_papa_schlumpf::Disconnect(): ptClientData=%p\n", ptClientData));

	/* NOTE: allow disconnects even if the plugin was already disconnected. */
	if( m_ptDevice!=NULL )
	{
		m_ptDevice->Disconnect();
	}

	m_fIsConnected = false;

	DEBUGMSG(ZONE_FUNCTION, ("-romloader_papa_schlumpf::Disconnect()\n"));
}



uint8_t romloader_papa_schlumpf::read_data08(lua_State *ptClientData, uint32_t ulNetxAddress)
{
	int iResult;
	uint8_t ucData;
	bool isOk = true;


	/* Be optimistic. */
	isOk = true;

	/* Is the device already initialized? */
	if( m_ptDevice==NULL )
	{
		MUHKUH_PLUGIN_PUSH_ERROR(ptClientData, "%s(%p): the USB level is not initialized!\n", this->m_pcName, this);
		isOk = false;
	}
	else
	{
		iResult = m_ptDevice->read_data08(ulNetxAddress, &ucData);
		if( iResult!=0 )
		{
			MUHKUH_PLUGIN_PUSH_ERROR(ptClientData, "%s(%p): failed to execute command!\n", this->m_pcName, this);
			isOk = false;
		}
	}

	if(!isOk)
	{
		MUHKUH_PLUGIN_EXIT_ERROR(ptClientData);
	}

	return ucData;
}



uint16_t romloader_papa_schlumpf::read_data16(lua_State *ptClientData, uint32_t ulNetxAddress)
{
	int iResult;
	uint16_t usData;
	bool isOk = true;


	/* Be optimistic. */
	isOk = true;

	/* Is the device already initialized? */
	if( m_ptDevice==NULL )
	{
		MUHKUH_PLUGIN_PUSH_ERROR(ptClientData, "%s(%p): the USB level is not initialized!\n", this->m_pcName, this);
		isOk = false;
	}
	else
	{
		iResult = m_ptDevice->read_data16(ulNetxAddress, &usData);
		if( iResult!=0 )
		{
			MUHKUH_PLUGIN_PUSH_ERROR(ptClientData, "%s(%p): failed to execute command!\n", this->m_pcName, this);
			isOk = false;
		}
	}

	if(!isOk)
	{
		MUHKUH_PLUGIN_EXIT_ERROR(ptClientData);
	}

	return usData;
}



uint32_t romloader_papa_schlumpf::read_data32(lua_State *ptClientData, uint32_t ulNetxAddress)
{
	int iResult;
	uint32_t ulData;
	bool isOk = true;


	/* Be optimistic. */
	isOk = true;

	/* Is the device already initialized? */
	if( m_ptDevice==NULL )
	{
		MUHKUH_PLUGIN_PUSH_ERROR(ptClientData, "%s(%p): the USB level is not initialized!\n", this->m_pcName, this);
		isOk = false;
	}
	else
	{
		iResult = m_ptDevice->read_data32(ulNetxAddress, &ulData);
		if( iResult!=0 )
		{
			MUHKUH_PLUGIN_PUSH_ERROR(ptClientData, "%s(%p): failed to execute command!\n", this->m_pcName, this);
			isOk = false;
		}
	}

	if(!isOk)
	{
		MUHKUH_PLUGIN_EXIT_ERROR(ptClientData);
	}

	return ulData;
}



void romloader_papa_schlumpf::read_image(uint32_t ulNetxAddress, uint32_t ulSize, char **ppcBUFFER_OUT, size_t *psizBUFFER_OUT, SWIGLUA_REF tLuaFn, long lCallbackUserData)
{
	int iResult;
	char *pcDataComplete;
	char *pcDataCnt;
	uint32_t sizDataRead;
	uint32_t sizDataCurrent;
	uint32_t sizDataLeft;
	bool isRunning;
	bool isOk;


	isOk = true;
	pcDataComplete = NULL;

	/* Is the device already initialized? */
	if( m_ptDevice==NULL )
	{
		MUHKUH_PLUGIN_PUSH_ERROR(tLuaFn.L, "%s(%p): the USB level is not initialized!\n", this->m_pcName, this);
		isOk = false;
	}
	else
	{
		pcDataComplete = (char *) malloc(ulSize);
		if( pcDataComplete==NULL )
		{
			MUHKUH_PLUGIN_PUSH_ERROR(tLuaFn.L, "%s(%p): memory error!\n", this->m_pcName, this);
			isOk = false;
		}
		else
		{
			sizDataRead    = 0;
			sizDataCurrent = 0;
			sizDataLeft    = ulSize;

			pcDataCnt = pcDataComplete;

			isRunning = true;

			do
			{
				sizDataCurrent = sizDataLeft;
				if(sizDataCurrent > PAPA_SCHLUMPF_COMMAND_DATA_SIZE)
				{
					sizDataCurrent = PAPA_SCHLUMPF_COMMAND_DATA_SIZE;
				}

				iResult = m_ptDevice->read_image(ulNetxAddress+sizDataRead, sizDataCurrent, pcDataCnt);
				if( iResult!=0 )
				{
					MUHKUH_PLUGIN_PUSH_ERROR(tLuaFn.L, "%s(%p): failed to send command!\n", this->m_pcName, this);
					isOk = false;
					break;
				}

				/*Update values*/
				sizDataLeft -= sizDataCurrent;
				sizDataRead += sizDataCurrent;

				isRunning = this->callback_long(&tLuaFn, sizDataRead, lCallbackUserData);
				if(!isRunning)
				{
					MUHKUH_PLUGIN_PUSH_ERROR(tLuaFn.L, "%s(%p): read_image cancelled!", this->m_pcName, this);
					isOk = false;
					break;
				}
			} while(sizDataLeft > 0);
		}
	}

	if(!isOk)
	{
		if( pcDataComplete!=NULL )
		{
			free(pcDataComplete);
			pcDataComplete = NULL;
			sizDataRead = 0;
		}

		printf("Exit Error\n");
		MUHKUH_PLUGIN_EXIT_ERROR(tLuaFn.L);
	}

	*ppcBUFFER_OUT = pcDataComplete;
	*psizBUFFER_OUT = sizDataRead;
}



void romloader_papa_schlumpf::write_data08(lua_State *ptClientData, uint32_t ulNetxAddress, uint8_t ucData)
{
	int iResult;
	bool isOk = true;


	/* Be optimistic. */
	isOk = true;

	/* Is the device already initialized? */
	if( m_ptDevice==NULL )
	{
		MUHKUH_PLUGIN_PUSH_ERROR(ptClientData, "%s(%p): the USB level is not initialized!\n", this->m_pcName, this);
		isOk = false;
	}
	else
	{
		iResult = m_ptDevice->write_data08(ulNetxAddress, ucData);
		if( iResult!=0 )
		{
			MUHKUH_PLUGIN_PUSH_ERROR(ptClientData, "%s(%p): failed to execute command!\n", this->m_pcName, this);
			isOk = false;
		}
	}

	if(!isOk)
	{
		MUHKUH_PLUGIN_EXIT_ERROR(ptClientData);
	}
}



void romloader_papa_schlumpf::write_data16(lua_State *ptClientData, uint32_t ulNetxAddress, uint16_t usData)
{
	int iResult;
	bool isOk = true;


	/* Be optimistic. */
	isOk = true;

	/* Is the device already initialized? */
	if( m_ptDevice==NULL )
	{
		MUHKUH_PLUGIN_PUSH_ERROR(ptClientData, "%s(%p): the USB level is not initialized!\n", this->m_pcName, this);
		isOk = false;
	}
	else
	{
		iResult = m_ptDevice->write_data16(ulNetxAddress, usData);
		if( iResult!=0 )
		{
			MUHKUH_PLUGIN_PUSH_ERROR(ptClientData, "%s(%p): failed to execute command!\n", this->m_pcName, this);
			isOk = false;
		}
	}

	if(!isOk)
	{
		MUHKUH_PLUGIN_EXIT_ERROR(ptClientData);
	}
}



void romloader_papa_schlumpf::write_data32(lua_State *ptClientData, uint32_t ulNetxAddress, uint32_t ulData)
{
	int iResult;
	bool isOk = true;


	/* Be optimistic. */
	isOk = true;

	/* Is the device already initialized? */
	if( m_ptDevice==NULL )
	{
		MUHKUH_PLUGIN_PUSH_ERROR(ptClientData, "%s(%p): the USB level is not initialized!\n", this->m_pcName, this);
		isOk = false;
	}
	else
	{
		iResult = m_ptDevice->write_data32(ulNetxAddress, ulData);
		if( iResult!=0 )
		{
			MUHKUH_PLUGIN_PUSH_ERROR(ptClientData, "%s(%p): failed to execute command!\n", this->m_pcName, this);
			isOk = false;
		}
	}

	if(!isOk)
	{
		MUHKUH_PLUGIN_EXIT_ERROR(ptClientData);
	}
}



void romloader_papa_schlumpf::write_image(uint32_t ulNetxAddress, const char *pcBUFFER_IN, size_t sizBUFFER_IN, SWIGLUA_REF tLuaFn, long lCallbackUserData)
{
	int iResult;
	uint32_t sizInBuf;
	uint32_t sizDataBytesWritten;
	uint32_t sizDataBytesLeft;
	uint32_t sizDataBytesCurrent;
	bool isRunning;
	bool isOk;


	sizInBuf            = 0;
	sizDataBytesWritten = 0;
	sizDataBytesLeft    = sizBUFFER_IN;
	sizDataBytesCurrent = 0;

	isRunning = true;
	isOk = true;

	/* Is the device already initialized? */
	if( m_ptDevice==NULL )
	{
		MUHKUH_PLUGIN_PUSH_ERROR(tLuaFn.L, "%s(%p): the USB level is not initialized!\n", this->m_pcName, this);
		isOk = false;
	}
	else
	{
		/*Send Data in Packets not larger than PAPA_SCHLUMPF_COMMAND_DATA_SIZE*/
		do
		{
			/*Set current data size to send*/
			sizDataBytesCurrent = sizDataBytesLeft;
			/*Check if current data size fits in one USB packet*/
			if(sizDataBytesCurrent > PAPA_SCHLUMPF_COMMAND_DATA_SIZE)
			{
				/*package needs to be splitted. */
				sizDataBytesCurrent = PAPA_SCHLUMPF_COMMAND_DATA_SIZE;
			}

			iResult = m_ptDevice->write_image(ulNetxAddress+sizDataBytesWritten, sizDataBytesCurrent, pcBUFFER_IN+sizDataBytesWritten);
			if( iResult!=0 )
			{
				MUHKUH_PLUGIN_PUSH_ERROR(tLuaFn.L, "%p: failed to send command!\n", this);
				isOk = false;
				break;
			}

			/*Update values*/
			sizDataBytesWritten += sizDataBytesCurrent;
			sizDataBytesLeft    -= sizDataBytesCurrent;

			isRunning = callback_long(&tLuaFn, sizDataBytesWritten, lCallbackUserData);
			if(!isRunning)
			{
				MUHKUH_PLUGIN_PUSH_ERROR(tLuaFn.L, "%p: write_image cancelled!", this);
				isOk = false;
				break;
			}
		} while( sizDataBytesLeft>0 );
	}

	if(!isOk)
	{
		MUHKUH_PLUGIN_EXIT_ERROR(tLuaFn.L);
	}
}



int romloader_papa_schlumpf::call_progress_data_static(void *pvUser, const char *pcProgressData, size_t sizProgressData)
{
	romloader_papa_schlumpf *ptThis;
	int iResult;


	/* This is a static method. Get the class instance from the user data. */
	ptThis = (romloader_papa_schlumpf*)pvUser;
	iResult = ptThis->call_progress_data(pcProgressData, sizProgressData);
	return iResult;
}



int romloader_papa_schlumpf::call_progress_data(const char *pcProgressData, size_t sizProgressData)
{
	int iIsRunning;
	bool fIsRunning;


	iIsRunning = 0;
	fIsRunning = callback_string(m_ptCallBackLuaFn, pcProgressData, sizProgressData, m_lCallBackUserData);
	if( fIsRunning==true )
	{
		iIsRunning = 1;
	}

	return iIsRunning;
}



void romloader_papa_schlumpf::call(uint32_t ulNetxAddress, uint32_t ulParameterR0, SWIGLUA_REF tLuaFn, long lCallbackUserData)
{
	bool fIsOk;
	int iResult;


	/* Be optimistic. */
	fIsOk = true;

	/* Is the device already initialized? */
	if( m_ptDevice==NULL )
	{
		MUHKUH_PLUGIN_PUSH_ERROR(tLuaFn.L, "%s(%p): the USB level is not initialized!\n", this->m_pcName, this);
		fIsOk = false;
	}
	else
	{
		/* Send the call command. */
		m_ptCallBackLuaFn = &tLuaFn;
		m_lCallBackUserData = lCallbackUserData;
		iResult = m_ptDevice->call(ulNetxAddress, ulParameterR0, romloader_papa_schlumpf::call_progress_data_static, this);
		if( iResult!=0 )
		{
			MUHKUH_PLUGIN_PUSH_ERROR(tLuaFn.L, "%s(%p): failed to send command!\n", this->m_pcName, this);
			fIsOk = false;
		}
	}

	if( fIsOk!=true )
	{
		MUHKUH_PLUGIN_EXIT_ERROR(tLuaFn.L);
	}
}






#if 0



/* This is the context of the libusb. */
libusb_context *g_ptLibUsbContext;

/* This is the handle for the papa_schlumpf device. */
libusb_device_handle *g_ptDevHandlePapaSchlumpf;

/* This is the name of the plugin. It is used for error messages on the screen. */
static const char *g_pcPluginId = "papa_schlumpf";




static PAPA_SCHLUMPF_ERROR_T send_start_packet()
{
	PAPA_SCHLUMPF_ERROR_T tResult;
	int iLibUsbResult;
	union
	{
		unsigned char auc[sizeof(PAPA_SCHLUMPF_PARAMETER_T)];
		PAPA_SCHLUMPF_PARAMETER_T tParameter;
	} uData;
	int iLength;
	unsigned int uiTimeoutMs;
	int iTransfered;


	/* Expect success. */
	tResult = PAPA_SCHLUMPF_ERROR_Ok;

	/* Fill the struct. */
	//uData.tParameter.ulDummy = 0x12345678;
	uData.tParameter.ulPapaSchlumpfUsbCommand = USB_COMMAND_Start;

	/* Wait at most 1 second. */
	uiTimeoutMs = 1000;

	/* Send data to endpoint 1. */
	iLength = sizeof(PAPA_SCHLUMPF_PARAMETER_T);
	iLibUsbResult = libusb_bulk_transfer(g_ptDevHandlePapaSchlumpf, 0x01, uData.auc, iLength, &iTransfered, uiTimeoutMs);
	if( iLibUsbResult==LIBUSB_SUCCESS )
	{
		/* Were all bytes transfered? */
		if( iTransfered!=iLength )
		{
			/* Not all bytes were transfered. */
			tResult = PAPA_SCHLUMPF_ERROR_TransferError;
		}
	}
	else
	{
		fprintf(stderr, "%s: failed to write data: %d:%s\n", g_pcPluginId, iLibUsbResult, libusb_strerror(libusb_error(iLibUsbResult)));
		tResult = PAPA_SCHLUMPF_ERROR_TransferError;
	}

	return tResult;
}

/* Receive the print messages and the final result.  */
static PAPA_SCHLUMPF_ERROR_T receive_report(char **ppcResponse, size_t *psizResponse, uint32_t *pulStatus)
{
	PAPA_SCHLUMPF_ERROR_T tResult;
	int iLibUsbResult;
	int iLength;
	unsigned int uiTimeoutMs;
	int iTransfered;
	char *pcResponse;
    size_t sizResponse;
	char *pcResponseNew;
    size_t sizResponseNew;
	union
	{
		unsigned char auc[4096];
		char ac[4096];
		uint32_t aul[4096/sizeof(uint32_t)];
	} uData;
	uint32_t ulStatus;
	int iEndPacketReceived;


	/* Expect success. */
	tResult = PAPA_SCHLUMPF_ERROR_Ok;

    pcResponse = NULL;
    sizResponse = 0;
    iEndPacketReceived = 0;

    while( iEndPacketReceived==0 )
    {
        /* Receive data packets in 4096 byte chunks. */
        iLength = 4096;
        uiTimeoutMs = 10000;
        iLibUsbResult = libusb_bulk_transfer(g_ptDevHandlePapaSchlumpf, 0x81,(unsigned char *) uData.ac, iLength, &iTransfered, uiTimeoutMs);
        if( iLibUsbResult==LIBUSB_SUCCESS )
        {
            /* Is this the special end packet? */
            if( iTransfered==8 && uData.aul[0]==0xffffffff )
            {
                ulStatus = uData.aul[1];
                iEndPacketReceived = 1;
            }
            else if( iTransfered>0 )
            {
                /* Add the new chunk to the buffer. */
                if( pcResponse==NULL )
                {
                    /* Allocate a new buffer. */
                    pcResponseNew = (char*)malloc(iTransfered);
                    if( pcResponseNew==NULL )
                    {
                        tResult = PAPA_SCHLUMPF_ERROR_MemoryError;
                        break;
                    }
                    else
                    {
                        /* Copy the received data into the new buffer. */
                        pcResponse = pcResponseNew;
                        memcpy(pcResponse, uData.ac, iTransfered);
                        sizResponse = iTransfered;
                    }
                }
                else
                {
                    sizResponseNew = sizResponse + iTransfered;
                    /* Allocate a new buffer. */
                    pcResponseNew = (char*)malloc(sizResponseNew);
                    if( pcResponseNew==NULL )
                    {
                        tResult = PAPA_SCHLUMPF_ERROR_MemoryError;
                        break;
                    }
                    else
                    {
                        /* Copy the old data into the new buffer. */
                        memcpy(pcResponseNew, pcResponse, sizResponse);
                        /* Append the received data to the new buffer. */
                        memcpy(pcResponseNew+sizResponse, uData.ac, iTransfered);
                        /* Free the old buffer. */
                        free(pcResponse);
                        /* Use the new buffer from now on. */
                        pcResponse = pcResponseNew;
                        sizResponse = sizResponseNew;
                    }
                }
            }
        }
        else
        {
            tResult = PAPA_SCHLUMPF_ERROR_TransferError;
            break;
        }
	}

    if( tResult!=PAPA_SCHLUMPF_ERROR_Ok )
    {
        if( pcResponse!=NULL )
        {
            free(pcResponse);
        }
        pcResponse = NULL;
        sizResponse = 0;
    }

    *ppcResponse = pcResponse;
    *psizResponse = sizResponse;
    *pulStatus = ulStatus;

	return tResult;
}

/* Start here! */
PapaSchlumpfDevice *run_test(unsigned long *pulSTATUS_OUT, char **ppcSTRING_OUT, size_t *psizSTRING_OUT)
{
	/*romloader*/
	PapaSchlumpfDevice *ptDevice;

	/* Our result for the function. */
	unsigned long ulResult;

	/* Results from libusb. */
	int iResult;

	const struct libusb_version *ptLibUsbVersion;

	char *pcResponse;
	size_t sizResponse;
	uint32_t ulStatus;

	pcResponse = NULL;
	sizResponse = 0;

	/* Initialize global variables. */
	g_ptLibUsbContext = NULL;
	g_ptDevHandlePapaSchlumpf = NULL;

	/* Expect success. */
	ulResult = PAPA_SCHLUMPF_ERROR_Ok;

	/* Say "hi". */
	printf("Papa Schlumpf\n");

	/* Show the libusb version. */
	ptLibUsbVersion = libusb_get_version();
	printf("Using libusb %d.%d.%d.%d%s\n", ptLibUsbVersion->major, ptLibUsbVersion->minor, ptLibUsbVersion->micro, ptLibUsbVersion->nano, ptLibUsbVersion->rc);

	/* Initialize the libusb context. */
	iResult = libusb_init(&g_ptLibUsbContext);
	if( iResult!=0 )
	{
		printf("Error initializing libusb: %d\n", iResult);
		ulResult = PAPA_SCHLUMPF_ERROR_LibUsb_Open;
	}
	else
	{
		/* Set the debug level to a bit more verbose. */
		libusb_set_debug(g_ptLibUsbContext, LIBUSB_LOG_LEVEL_INFO);

		printf("libusb is open!\n");

		/* Search for exactly one papa schlumpf hardware and open it. */
		ulResult = scan_for_papa_schlumpf_hardware();
		if( ulResult==PAPA_SCHLUMPF_ERROR_Ok )
		{
			printf("Found HW!\n");

			/* Initialize the device. */
			ptDevice = new PapaSchlumpfDevice(g_ptDevHandlePapaSchlumpf, g_ptLibUsbContext);

			/* Send a packet. */
			ulResult = send_start_packet();
			if( ulResult==PAPA_SCHLUMPF_ERROR_Ok )
			{
				/* TODO: receive response. */
				ulResult = receive_report(&pcResponse, &sizResponse, &ulStatus);
				if( ulResult==PAPA_SCHLUMPF_ERROR_Ok )
				{
					if( ulStatus!=0 )
					{
						ulResult = PAPA_SCHLUMPF_ERROR_netxError;
					}
				}
			}
		}
	}

	if (ulResult != PAPA_SCHLUMPF_ERROR_Ok) {
		/*If the Papa Schlumpf device was found properly,*/
		/*then this is done via the destructor of the romloader object*/

		/* Release and close the handle. */
		libusb_release_interface(g_ptDevHandlePapaSchlumpf, 0);
		libusb_close(g_ptDevHandlePapaSchlumpf);

		/* Close the libusb context. */
		libusb_exit(g_ptLibUsbContext);
	}

	*pulSTATUS_OUT = ulResult;
	*ppcSTRING_OUT = pcResponse;
	*psizSTRING_OUT = sizResponse;

	return ptDevice;
}
#endif

/*-------------------------------------*/


romloader_papa_schlumpf_reference::romloader_papa_schlumpf_reference(void)
 : muhkuh_plugin_reference()
{
}


romloader_papa_schlumpf_reference::romloader_papa_schlumpf_reference(const char *pcName, const char *pcTyp, bool fIsUsed, romloader_papa_schlumpf_provider *ptProvider)
 : muhkuh_plugin_reference(pcName, pcTyp, fIsUsed, ptProvider)
{
}


romloader_papa_schlumpf_reference::romloader_papa_schlumpf_reference(const char *pcName, const char *pcTyp, const char *pcLocation, bool fIsUsed, romloader_papa_schlumpf_provider *ptProvider)
 : muhkuh_plugin_reference(pcName, pcTyp, pcLocation, fIsUsed, ptProvider)
{
}


romloader_papa_schlumpf_reference::romloader_papa_schlumpf_reference(const romloader_papa_schlumpf_reference *ptCloneMe)
 : muhkuh_plugin_reference(ptCloneMe)
{
}


/*-------------------------------------*/


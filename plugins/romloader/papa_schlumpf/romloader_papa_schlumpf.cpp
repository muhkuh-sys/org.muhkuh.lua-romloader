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

	#define DEBUGMSG(cond,printf_exp) ((void)((cond)?(uprintf printf_exp),1:0))
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
	DEBUGMSG(ZONE_FUNCTION, ("+romloader_papa_schlumpf_provider::romloader_papa_schlumpf_provider(): p_romloader_papa_schlumpf=%p, p_romloader_papa_schlumpf_reference=%p\n", p_romloader_usb, p_romloader_usb_reference));

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

	DEBUGMSG(ZONE_FUNCTION, ("-romloader_papa_schlumpf_provider::DetectInterfaces(): sizReferences=%d\n", sizReferences));

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
#if 0
		else if( m_ptUsbDevice==NULL )
		{
			/* libusb was not initialized */
			fprintf(stderr, "%s(%p): libusb was not initialized!\n", m_pcPluginId, this);
		}
		else
		{
			ptPlugin = new romloader_usb(pcName, m_pcPluginId, this, uiBusNr, uiDevAdr);
			printf("%s(%p): claim_interface(): claimed interface %s.\n", m_pcPluginId, this, pcName);
		}
#endif
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
 , g_pLibusbDeviceHandle(NULL)
 , g_pLibusbContext(NULL)
// , m_ptUsbProvider(ptProvider)
// , m_uiBusNr(uiBusNr)
// , m_uiDeviceAdr(uiDeviceAdr)
// , m_ptUsbDevice(NULL)
{
	DEBUGMSG(ZONE_FUNCTION, ("+romloader_papa_schlumpf::romloader_papa_schlumpf(): pcName='%s', pcTyp='%s', ptProvider=%p, uiBusNr=%d, uiDeviceAdr=%d\n", pcName, pcTyp, ptProvider, uiBusNr, uiDeviceAdr));

	/* create a new libusb context */
//	m_ptUsbDevice = new romloader_usb_device_libusb(m_pcName);

	DEBUGMSG(ZONE_FUNCTION, ("-romloader_papa_schlumpf::romloader_usb()\n"));
}


romloader_papa_schlumpf::romloader_papa_schlumpf(const char *pcName, const char *pcTyp, const char *pcLocation, romloader_papa_schlumpf_provider *ptProvider, unsigned int uiBusNr, unsigned int uiDeviceAdr)
 : romloader(pcName, pcTyp, pcLocation, ptProvider)
 , g_pLibusbDeviceHandle(NULL)
 , g_pLibusbContext(NULL)
// , m_ptUsbProvider(ptProvider)
// , m_uiBusNr(uiBusNr)
// , m_uiDeviceAdr(uiDeviceAdr)
// , m_ptUsbDevice(NULL)
{
	DEBUGMSG(ZONE_FUNCTION, ("+romloader_papa_schlumpf::romloader_papa_schlumpf(): pcName='%s', pcTyp='%s', ptProvider=%p, uiBusNr=%d, uiDeviceAdr=%d\n", pcName, pcTyp, ptProvider, uiBusNr, uiDeviceAdr));

	/* create a new libusb context */
//	m_ptUsbDevice = new romloader_usb_device_libusb(m_pcName);

	DEBUGMSG(ZONE_FUNCTION, ("-romloader_papa_schlumpf::romloader_papa_schlumpf()\n"));
}


romloader_papa_schlumpf::~romloader_papa_schlumpf(void)
{
	DEBUGMSG(ZONE_FUNCTION, ("-romloader_papa_schlumpf::~romloader_papa_schlumpf()\n"));

//	if( m_ptUsbDevice!=NULL )
//	{
//		delete(m_ptUsbDevice);
//	}

	DEBUGMSG(ZONE_FUNCTION, ("-romloader_papa_schlumpf::~romloader_papa_schlumpf()\n"));
}


/* open the connection to the device */
void romloader_papa_schlumpf::Connect(lua_State *ptClientData)
{
	int iResult;


	DEBUGMSG(ZONE_FUNCTION, ("+romloader_papa_schlumpf::Connect(): ptClientData=%p\n", ptClientData));

	if( m_fIsConnected!=false )
	{
		printf("%s(%p): already connected, ignoring new connect request\n", m_pcName, this);
	}
	else
	{
#if 0
		iResult = m_ptDevice->Connect(m_uiBusNr, m_uiDeviceAdr);
		if( iResult!=LIBUSB_SUCCESS )
		{
			MUHKUH_PLUGIN_PUSH_ERROR(ptClientData, "%s(%p): failed to connect to device", m_pcName, this);
		}
		else
		{
			/* NOTE: set m_fIsConnected to true here or detect_chiptyp and chip_init will fail! */
			m_fIsConnected = true;

			/* Synchronize with the client to get these settings:
			 *   chip type
			 *   sequence number
			 *   maximum packet size
			 */
			if( synchronize()!=true )
			{
				fprintf(stderr, "%s(%p): failed to synchronize!", m_pcName, this);
				MUHKUH_PLUGIN_PUSH_ERROR(ptClientData, "%s(%p): failed to synchronize!", m_pcName, this);
				m_fIsConnected = false;
				m_ptUsbDevice->Disconnect();
				iResult = -1;
			}
		}
#endif
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

//	if( m_ptUsbDevice!=NULL )
//	{
//		m_ptUsbDevice->Disconnect();
//	}

	m_fIsConnected = false;

	DEBUGMSG(ZONE_FUNCTION, ("-romloader_papa_schlumpf::Disconnect()\n"));
}


uint8_t romloader_papa_schlumpf::read_data08(lua_State *ptClientData, uint32_t ulDataSourceAddress)
{
	/* Init values*/
	uint32_t    sizInBuf = 0;
	romloader_papa_schlumpf_device::USB_STATUS_T tUsbStatus;
	PAPA_SCHLUMPF_USB_COMMAND_STATUS_T tPapaSchlumpfUsbCommandStatus;
	PAPA_SCHLUMPF_PARAMETER_DATA_T tCmd;
	PAPA_SCHLUMPF_DATA_BUFFER_T tBuffer;

	bool isRunning = true;
	bool isOk = true;

	/*Prepare the Parameter struct*/
	tCmd.tPapaSchlumpfParameter.ulPapaSchlumpfUsbCommand = USB_COMMAND_Read_8;
	tCmd.tPapaSchlumpfParameter.uPapaSchlumpfUsbParameter.tPapaSchlumpfUsbParameterRead.ulAddress   = ulDataSourceAddress;
	tCmd.tPapaSchlumpfParameter.uPapaSchlumpfUsbParameter.tPapaSchlumpfUsbParameterRead.ulElemCount = 1;

	/*Send Command*/
	tUsbStatus = m_ptDevice->sendAndReceivePackets(tCmd.auc, sizeof(tCmd), tBuffer.aucBuf, &sizInBuf);
	if(tUsbStatus != romloader_papa_schlumpf_device::USB_STATUS_OK)
	{
		MUHKUH_PLUGIN_PUSH_ERROR(ptClientData, "%p: failed to send command!\n", this);
		isOk = false;
	}

	/*Check if error in response*/
	tPapaSchlumpfUsbCommandStatus = PAPA_SCHLUMPF_USB_COMMAND_STATUS_T(tBuffer.aulBuf[0]);
	if(tPapaSchlumpfUsbCommandStatus != USB_COMMAND_STATUS_Ok)
	{
		MUHKUH_PLUGIN_PUSH_ERROR(ptClientData,"%s: failed to execute command!\n", this->m_tChiptyp);
		isOk = false;
	}

	if(!isOk)
	{
		printf("Exit Error\n");
		MUHKUH_PLUGIN_EXIT_ERROR(ptClientData);
	}

	return tBuffer.aucBuf[4];
}

uint16_t romloader_papa_schlumpf::read_data16(lua_State *ptClientData, uint32_t ulDataSourceAddress)
{
	/*Init values*/
	uint32_t   sizInBuf = 0;
	romloader_papa_schlumpf_device::USB_STATUS_T tUsbStatus;
	PAPA_SCHLUMPF_USB_COMMAND_STATUS_T tPapaSchlumpfUsbCommandStatus;
	PAPA_SCHLUMPF_PARAMETER_DATA_T tCmd;
	PAPA_SCHLUMPF_DATA_BUFFER_T tBuffer;

	bool isRunning = true;
	bool isOk = true;

	/*Prepare the Parameter struct*/
	tCmd.tPapaSchlumpfParameter.ulPapaSchlumpfUsbCommand = USB_COMMAND_Read_16;
	tCmd.tPapaSchlumpfParameter.uPapaSchlumpfUsbParameter.tPapaSchlumpfUsbParameterRead.ulAddress   = ulDataSourceAddress;
	tCmd.tPapaSchlumpfParameter.uPapaSchlumpfUsbParameter.tPapaSchlumpfUsbParameterRead.ulElemCount = 1;

	/*Send Command*/
	tUsbStatus = m_ptDevice->sendAndReceivePackets(tCmd.auc, sizeof(tCmd), tBuffer.aucBuf, &sizInBuf);
	if(tUsbStatus != USB_STATUS_OK)
	{
		MUHKUH_PLUGIN_PUSH_ERROR(ptClientData, "%p: failed to send command!\n", this);
		isOk = false;
	}

	/*Check if error in response*/
	tPapaSchlumpfUsbCommandStatus = PAPA_SCHLUMPF_USB_COMMAND_STATUS_T(tBuffer.aulBuf[0]);
	if(tPapaSchlumpfUsbCommandStatus != USB_COMMAND_STATUS_Ok)
	{
		MUHKUH_PLUGIN_PUSH_ERROR(ptClientData,"%s: failed to execute command!\n", this->m_tChiptyp);
		isOk = false;
	}

	if(!isOk)
	{
		printf("Exit Error\n");
		MUHKUH_PLUGIN_EXIT_ERROR(ptClientData);
	}

	return tBuffer.ausBuf[2];
}

uint32_t romloader_papa_schlumpf::read_data32(lua_State *ptClientData, uint32_t ulDataSourceAddress)
{
	/*Init values*/
	uint32_t   sizInBuf = 0;
	romloader_papa_schlumpf_device::USB_STATUS_T tUsbStatus;
	PAPA_SCHLUMPF_USB_COMMAND_STATUS_T tPapaSchlumpfUsbCommandStatus;
	PAPA_SCHLUMPF_PARAMETER_DATA_T tCmd;
	PAPA_SCHLUMPF_DATA_BUFFER_T tBuffer;

	bool isRunning = true;
	bool isOk = true;

	/*Prepare the Parameter struct*/
	tCmd.tPapaSchlumpfParameter.ulPapaSchlumpfUsbCommand      = USB_COMMAND_Read_32;
	tCmd.tPapaSchlumpfParameter.uPapaSchlumpfUsbParameter.tPapaSchlumpfUsbParameterRead.ulAddress   = ulDataSourceAddress;
	tCmd.tPapaSchlumpfParameter.uPapaSchlumpfUsbParameter.tPapaSchlumpfUsbParameterRead.ulElemCount = 1;

	/*Send Command*/
	tUsbStatus = m_ptDevice->sendAndReceivePackets(tCmd.auc, sizeof(tCmd), tBuffer.aucBuf, &sizInBuf);
	if(tUsbStatus != romloader_papa_schlumpf_device::USB_STATUS_OK)
	{
		MUHKUH_PLUGIN_PUSH_ERROR(ptClientData, "%p: failed to send command!\n", this);
		isOk = false;
	}

	/*Check if error in response*/
	tPapaSchlumpfUsbCommandStatus = PAPA_SCHLUMPF_USB_COMMAND_STATUS_T(tBuffer.aulBuf[0]);
	if(tPapaSchlumpfUsbCommandStatus != USB_COMMAND_STATUS_Ok)
	{
		MUHKUH_PLUGIN_PUSH_ERROR(ptClientData,"%s: failed to execute command!\n", this->m_tChiptyp);
		isOk = false;
	}

	if(!isOk)
	{
		printf("Exit Error\n");
		MUHKUH_PLUGIN_EXIT_ERROR(ptClientData);
	}

	return tBuffer.aulBuf[1];
}

void romloader_papa_schlumpf::read_image(uint32_t ulNetxAddress, uint32_t ulSize, char **ppcBUFFER_OUT, size_t *psizBUFFER_OUT, SWIGLUA_REF tLuaFn, long lCallbackUserData)
{
	/*Init values*/
	uint32_t sizInBuf = 0;
	romloader_papa_schlumpf_device::USB_STATUS_T tUsbStatus;
	PAPA_SCHLUMPF_USB_COMMAND_STATUS_T tPapaSchlumpfUsbCommandStatus;
	PAPA_SCHLUMPF_PARAMETER_DATA_T tCmd;
	PAPA_SCHLUMPF_DATA_BUFFER_T tBuffer;

	char * cDataComplete = (char *) malloc(ulSize);

	uint32_t sizDataRead    = 0;
	uint32_t sizDataCurrent = 0;
	uint32_t sizDataLeft    = ulSize;

	bool isRunning = true;
	bool isOk = true;

	/*Set common parameters*/
	tCmd.tPapaSchlumpfParameter.ulPapaSchlumpfUsbCommand = USB_COMMAND_Read_8;

	do
	{
		sizDataCurrent = sizDataLeft;

		if(sizDataCurrent > PAPA_SCHLUMPF_COMMAND_DATA_SIZE)
		{
			sizDataCurrent = PAPA_SCHLUMPF_COMMAND_DATA_SIZE;
		}

		/*Set address in parameter structure (account for bytes already read)*/
		tCmd.tPapaSchlumpfParameter.uPapaSchlumpfUsbParameter.tPapaSchlumpfUsbParameterRead.ulAddress   = ulNetxAddress + sizDataRead;
		/*Set size in parameter structure*/
		tCmd.tPapaSchlumpfParameter.uPapaSchlumpfUsbParameter.tPapaSchlumpfUsbParameterRead.ulElemCount = sizDataCurrent;

		tUsbStatus = m_ptDevice->sendAndReceivePackets(tCmd.auc, sizeof(tCmd), tBuffer.aucBuf, &sizInBuf);
		if(tUsbStatus != romloader_papa_schlumpf_device::USB_STATUS_OK)
		{
			MUHKUH_PLUGIN_PUSH_ERROR(tLuaFn.L, "%p: failed to send command!\n", this);
			isOk = false;
			break;
		}

		/*Check if error in response*/
		tPapaSchlumpfUsbCommandStatus = PAPA_SCHLUMPF_USB_COMMAND_STATUS_T(tBuffer.aulBuf[0]);
		if(tPapaSchlumpfUsbCommandStatus != USB_COMMAND_STATUS_Ok)
		{
			MUHKUH_PLUGIN_PUSH_ERROR(tLuaFn.L,"%s: failed to execute command!\n", this->m_tChiptyp);
			isOk = false;
			break;
		}

		/*Copy Data to output buffer*/
		memcpy(tBuffer.aucBuf + 4, cDataComplete + sizDataRead, sizDataCurrent);

		/*Update values*/
		sizDataLeft -= sizDataCurrent;
		sizDataRead += sizDataCurrent;

		isRunning = this->callback_long(&tLuaFn, sizDataRead, lCallbackUserData);
		if(!isRunning)
		{
			MUHKUH_PLUGIN_PUSH_ERROR(tLuaFn.L, "%p: read_image cancelled!", this);
			isOk = false;
			break;
		}
	} while(sizDataLeft > 0);

	if(!isOk)
	{
		printf("Exit Error\n");
		MUHKUH_PLUGIN_EXIT_ERROR(tLuaFn.L);
	}

	*ppcBUFFER_OUT = cDataComplete;
	*psizBUFFER_OUT = sizDataRead;
}

void romloader_papa_schlumpf::write_data08(lua_State *ptClientData, uint32_t ulNetxAddress, uint8_t ucData)
{
	/*Init values*/
	uint32_t   sizInBuf = 0;
	romloader_papa_schlumpf_device::USB_STATUS_T tUsbStatus;
	PAPA_SCHLUMPF_USB_COMMAND_STATUS_T tPapaSchlumpfUsbCommandStatus;
	PAPA_SCHLUMPF_PARAMETER_DATA_T tCmd;
	PAPA_SCHLUMPF_DATA_BUFFER_T tBuffer;

	bool isRunning = true;
	bool isOk = true;

	/*Prepare the Parameter struct*/
	tCmd.tPapaSchlumpfParameter.ulPapaSchlumpfUsbCommand       = USB_COMMAND_Write_8;
	tCmd.tPapaSchlumpfParameter.uPapaSchlumpfUsbParameter.tPapaSchlumpfUsbParameterWrite.ulAddress   = ulNetxAddress;
	tCmd.tPapaSchlumpfParameter.uPapaSchlumpfUsbParameter.tPapaSchlumpfUsbParameterWrite.ulElemCount = 1;
	tCmd.tPapaSchlumpfParameter.uPapaSchlumpfUsbParameter.tPapaSchlumpfUsbParameterWrite.uData.ucData[0] = ucData;

	/*Send Command*/
	tUsbStatus = m_ptDevice->sendAndReceivePackets(tCmd.auc, sizeof(tCmd), tBuffer.aucBuf, &sizInBuf);
	if(tUsbStatus != USB_STATUS_OK)
	{
		MUHKUH_PLUGIN_PUSH_ERROR(ptClientData, "%p: failed to send command!\n", this);
		isOk = false;
	}

	/*Check if error in response*/
	tPapaSchlumpfUsbCommandStatus = PAPA_SCHLUMPF_USB_COMMAND_STATUS_T(tBuffer.aulBuf[0]);
	if(tPapaSchlumpfUsbCommandStatus != USB_COMMAND_STATUS_Ok)
	{
		MUHKUH_PLUGIN_PUSH_ERROR(ptClientData,"%s: failed to execute command!\n", this->m_tChiptyp);
		isOk = false;
	}

	if(!isOk)
	{
		printf("Exit Error\n");
		MUHKUH_PLUGIN_EXIT_ERROR(ptClientData);
	}
}

void romloader_papa_schlumpf::write_data16(lua_State *ptClientData, uint32_t ulNetxAddress, uint16_t usData)
{
	/*Init values*/
	uint32_t   sizInBuf = 0;
	romloader_papa_schlumpf_device::USB_STATUS_T tUsbStatus;
	PAPA_SCHLUMPF_USB_COMMAND_STATUS_T tPapaSchlumpfUsbCommandStatus;
	PAPA_SCHLUMPF_PARAMETER_DATA_T tCmd;
	PAPA_SCHLUMPF_DATA_BUFFER_T tBuffer;

	bool isRunning = true;
	bool isOk = true;

	/*Prepare the Parameter struct*/
	tCmd.tPapaSchlumpfParameter.ulPapaSchlumpfUsbCommand = USB_COMMAND_Write_16;
	tCmd.tPapaSchlumpfParameter.uPapaSchlumpfUsbParameter.tPapaSchlumpfUsbParameterWrite.ulAddress   = ulNetxAddress;
	tCmd.tPapaSchlumpfParameter.uPapaSchlumpfUsbParameter.tPapaSchlumpfUsbParameterWrite.ulElemCount = 1;
	tCmd.tPapaSchlumpfParameter.uPapaSchlumpfUsbParameter.tPapaSchlumpfUsbParameterWrite.uData.usData[0] = usData;

	/*Send Command*/
	tUsbStatus = m_ptDevice->sendAndReceivePackets(tCmd.auc, sizeof(tCmd), tBuffer.aucBuf, &sizInBuf);
	if(tUsbStatus != USB_STATUS_OK)
	{
		MUHKUH_PLUGIN_PUSH_ERROR(ptClientData, "%p: failed to send command!\n", this);
		isOk = false;
	}

	/*Check if error in response*/
	tPapaSchlumpfUsbCommandStatus = PAPA_SCHLUMPF_USB_COMMAND_STATUS_T(tBuffer.aulBuf[0]);
	if(tPapaSchlumpfUsbCommandStatus != USB_COMMAND_STATUS_Ok)
	{
		MUHKUH_PLUGIN_PUSH_ERROR(ptClientData,"%s: failed to execute command!\n", this->m_tChiptyp);
		isOk = false;
	}

	if(!isOk)
	{
		printf("Exit Error\n");
		MUHKUH_PLUGIN_EXIT_ERROR(ptClientData);
	}
}

void romloader_papa_schlumpf::write_data32(lua_State *ptClientData, uint32_t ulNetxAddress, uint32_t ulData)
{
	/*Init values*/
	uint32_t   sizInBuf = 0;
	romloader_papa_schlumpf_device::USB_STATUS_T tUsbStatus;
	PAPA_SCHLUMPF_USB_COMMAND_STATUS_T tPapaSchlumpfUsbCommandStatus;
	PAPA_SCHLUMPF_PARAMETER_DATA_T tCmd;
	PAPA_SCHLUMPF_DATA_BUFFER_T tBuffer;

	bool isRunning = true;
	bool isOk = true;

	/*Prepare the Parameter struct*/
	tCmd.tPapaSchlumpfParameter.ulPapaSchlumpfUsbCommand = USB_COMMAND_Write_32;
	tCmd.tPapaSchlumpfParameter.uPapaSchlumpfUsbParameter.tPapaSchlumpfUsbParameterWrite.ulAddress   = ulNetxAddress;
	tCmd.tPapaSchlumpfParameter.uPapaSchlumpfUsbParameter.tPapaSchlumpfUsbParameterWrite.ulElemCount = 1;
	tCmd.tPapaSchlumpfParameter.uPapaSchlumpfUsbParameter.tPapaSchlumpfUsbParameterWrite.uData.ulData[0] = ulData;

	/*Send Command*/
	tUsbStatus = m_ptDevice->sendAndReceivePackets(tCmd.auc, sizeof(tCmd), tBuffer.aucBuf, &sizInBuf);
	if(tUsbStatus != USB_STATUS_OK)
	{
		MUHKUH_PLUGIN_PUSH_ERROR(ptClientData, "%p: failed to send command!\n", this);
		isOk = false;
	}

	/*Check if error in response*/
	tPapaSchlumpfUsbCommandStatus = PAPA_SCHLUMPF_USB_COMMAND_STATUS_T(tBuffer.aulBuf[0]);
	if(tPapaSchlumpfUsbCommandStatus != USB_COMMAND_STATUS_Ok)
	{
		MUHKUH_PLUGIN_PUSH_ERROR(ptClientData,"%s: failed to execute command!\n", this->m_tChiptyp);
		isOk = false;
	}

	if(!isOk)
	{
		printf("Exit Error\n");
		MUHKUH_PLUGIN_EXIT_ERROR(ptClientData);
	}
}

void romloader_papa_schlumpf::write_image(uint32_t ulNetxAddress, const char *pcBUFFER_IN, size_t sizBUFFER_IN, SWIGLUA_REF tLuaFn, long lCallbackUserData)
{
	/*Init values*/
	PAPA_SCHLUMPF_USB_COMMAND_STATUS_T tPapaSchlumpfUsbCommandStatus;
	romloader_papa_schlumpf_device::USB_STATUS_T tUsbStatus;
	PAPA_SCHLUMPF_PARAMETER_DATA_T tCmd;
	PAPA_SCHLUMPF_DATA_BUFFER_T tBuffer;
	uint32_t sizInBuf;
	uint32_t sizDataBytesWritten;
	uint32_t sizDataBytesLeft;
	uint32_t sizDataBytesCurrent;
	bool isRunning;
	bool isOk;
	uint8_t *aucDataBuf;


	sizInBuf            = 0;
	sizDataBytesWritten = 0;
	sizDataBytesLeft    = sizBUFFER_IN;
	sizDataBytesCurrent = 0;

	isRunning = true;
	isOk = true;

	aucDataBuf = tCmd.tPapaSchlumpfParameter.uPapaSchlumpfUsbParameter.tPapaSchlumpfUsbParameterWrite.uData.ucData;

	/*Set the USB command*/
	tCmd.tPapaSchlumpfParameter.ulPapaSchlumpfUsbCommand = USB_COMMAND_Write_8;

	/*Send Data in Packets not larger than PAPA_SCHLUMPF_COMMAND_DATA_SIZE*/
	do
	{
		/*Set current data size to send*/
		sizDataBytesCurrent = sizDataBytesLeft;

		/*Check if current data size fits in one USB packet*/
		if(sizDataBytesCurrent > PAPA_SCHLUMPF_COMMAND_DATA_SIZE)
		{
			/*package needs to be splitted*/
			sizDataBytesCurrent = PAPA_SCHLUMPF_COMMAND_DATA_SIZE;
		}

		/*Set the size*/
		tCmd.tPapaSchlumpfParameter.uPapaSchlumpfUsbParameter.tPapaSchlumpfUsbParameterWrite.ulElemCount = sizDataBytesCurrent;

		/*Set the destination address (account for already written bytes)*/
		tCmd.tPapaSchlumpfParameter.uPapaSchlumpfUsbParameter.tPapaSchlumpfUsbParameterWrite.ulAddress = ulNetxAddress + sizDataBytesWritten;

		/*Copy data to Parameter Data Buffer*/
		memcpy(aucDataBuf, pcBUFFER_IN + sizDataBytesWritten, sizDataBytesCurrent);

		/*execute command*/
		tUsbStatus = m_ptDevice->sendAndReceivePackets(tCmd.auc, sizeof(tCmd), tBuffer.aucBuf, &sizInBuf);
		if(tUsbStatus != USB_STATUS_OK)
		{
			MUHKUH_PLUGIN_PUSH_ERROR(tLuaFn.L, "%p: failed to send command!\n", this);
			isOk = false;
			break;
		}

		/*Check if error in response*/
		tPapaSchlumpfUsbCommandStatus = PAPA_SCHLUMPF_USB_COMMAND_STATUS_T(tBuffer.aulBuf[0]);
		if(tPapaSchlumpfUsbCommandStatus != USB_COMMAND_STATUS_Ok)
		{
			MUHKUH_PLUGIN_PUSH_ERROR(tLuaFn.L,"%s: failed to execute command!\n", this->m_tChiptyp);
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
	} while(sizDataBytesLeft > 0);

	if(!isOk)
	{
		printf("Exit Error\n");
		MUHKUH_PLUGIN_EXIT_ERROR(tLuaFn.L);
	}
}

void romloader_papa_schlumpf::call(uint32_t ulNetxAddress, uint32_t ulParameterR0, SWIGLUA_REF tLuaFn, long lCallbackUserData)
{
	/*Init values*/
	PAPA_SCHLUMPF_USB_COMMAND_STATUS_T tPapaSchlumpfUsbCommandStatus;
	romloader_papa_schlumpf_device::USB_STATUS_T tUsbStatus;
	PAPA_SCHLUMPF_PARAMETER_DATA_T tCmd;
	PAPA_SCHLUMPF_DATA_BUFFER_T tBuffer;

	uint8_t* pcProgressData;

	uint32_t sizProgressData;
	uint32_t sizInBuf = 0;

	bool isRunning  = true;
	bool isOk       = true;

	/*Set parameters*/
	tCmd.tPapaSchlumpfParameter.ulPapaSchlumpfUsbCommand = USB_COMMAND_Call;
	tCmd.tPapaSchlumpfParameter.uPapaSchlumpfUsbParameter.tPapaSchlumpfUsbParameterCall.ulFunctionAddress   = ulNetxAddress;
	tCmd.tPapaSchlumpfParameter.uPapaSchlumpfUsbParameter.tPapaSchlumpfUsbParameterCall.ulFunctionParameter = ulParameterR0;

	/*Send Command*/
	tUsbStatus = m_ptDevice->sendPacket(tCmd.auc, sizeof(tCmd));
	if(tUsbStatus != USB_STATUS_OK)
	{
		MUHKUH_PLUGIN_PUSH_ERROR(tLuaFn.L, "%p: failed to send command!\n", this);
		isOk = false;
	}
	else
	{
		/*Check if call is OK*/
		tUsbStatus = m_ptDevice->receivePacket(tBuffer.aucBuf, &sizInBuf);
		if(tUsbStatus != USB_STATUS_OK)
		{
			MUHKUH_PLUGIN_PUSH_ERROR(tLuaFn.L, "%p: failed to receive message!\n", this);
			isOk = false;
		}
		else
		{
			if(tBuffer.aulBuf[0] != USB_COMMAND_STATUS_Ok)
			{
				MUHKUH_PLUGIN_PUSH_ERROR(tLuaFn.L,"%s: failed to execute command!\n", this->m_tChiptyp);
				isOk = false;
			}
			else
			{
				/*Status is OK, receive print messages*/
				while(1)
				{
					/*Reset values*/
					pcProgressData  = NULL;
					sizProgressData = 0;

					/*Receive Message*/
					tUsbStatus = m_ptDevice->receivePacket(tBuffer.aucBuf, &sizInBuf);
					if(tUsbStatus != USB_STATUS_OK)
					{
						MUHKUH_PLUGIN_PUSH_ERROR(tLuaFn.L, "%p: failed to read message!\n", this);
						isOk = false;
						break;
					}
					else
					{
						if(tBuffer.aulBuf[0] == USB_COMMAND_STATUS_CALL_DONE)
						{
							/*Call is Done*/
							isOk = true;
							break;
						}
						else if(tBuffer.aulBuf[0] != USB_COMMAND_STATUS_Print)
						{
							/*Unknown answer*/
							MUHKUH_PLUGIN_PUSH_ERROR(tLuaFn.L, "%p: received invalid message!\n", this);
							isOk = false;
							break;
						}
						else
						{
							/*Is a Print Message*/
							pcProgressData = tBuffer.aucBuf + sizeof(uint32_t); //Get a pointer to the Data (minus the status)
							sizProgressData = sizInBuf - sizeof(uint32_t);       //Get the size of the Data  (minus the status)
						}
					}

					if(pcProgressData != NULL)
					{
						isRunning = this->callback_string(&tLuaFn,(char *) pcProgressData, sizProgressData, lCallbackUserData);
						if(!isRunning)
						{
							MUHKUH_PLUGIN_PUSH_ERROR(tLuaFn.L, "%p: the call was cancelled!", this);
							isOk = false;
							break;
						}
					}
				} //while(1) --> receive messages until call done
			}//if(uDataBuf.aulBuf[0] != USB_COMMAND_STATUS_Ok) --> Checked if call was ok
		}//if(tUsbStatus != USB_STATUS_OK) --> Received initial answer for command
	}//if(tUsbStatus != USB_STATUS_OK) --> Initial command sent

	if(!isOk)
	{
		printf("Exit Error\n");
		MUHKUH_PLUGIN_EXIT_ERROR(tLuaFn.L);
	}
}




#if 0
/* Look through all USB devices. If one Papa Schlumpf device was found, open it.
 * 0 or more than one Papa Schlumpf device results in an error.
 */
romloader_papa_schlumpf::PAPA_SCHLUMPF_ERROR_T romloader_papa_schlumpf::scan_for_papa_schlumpf_hardware(void)
{
	PAPA_SCHLUMPF_ERROR_T tResult;
	int iLibUsbResult;
	ssize_t ssizDevList;
	libusb_device **ptDeviceList;
	libusb_device **ptDevCnt, **ptDevEnd;
	libusb_device *ptDevice;
	libusb_device *ptDevPapaSchlumpf;
	libusb_device_handle *ptDevHandle;
	libusb_device_handle *ptDevHandlePapaSchlumpf;
	struct libusb_device_descriptor sDevDesc;


	/* Expect success. */
	tResult = PAPA_SCHLUMPF_ERROR_Ok;

	/* No device open yet. */
	ptDevHandlePapaSchlumpf = NULL;

	/* Get the list of all connected USB devices. */
	ptDeviceList = NULL;
	ssizDevList = libusb_get_device_list(g_ptLibUsbContext, &ptDeviceList);
	if( ssizDevList<0 )
	{
		/* Failed to get the list of all USB devices. */
		fprintf(stderr, "%s: failed to detect usb devices: %ld:%s\n", g_pcPluginId, ssizDevList, libusb_strerror(libusb_error(ssizDevList)));
		tResult = PAPA_SCHLUMPF_ERROR_FailedToListUSBDevices;
	}
	else
	{
		/* No Papa Schlumpf device found yet. */
		ptDevPapaSchlumpf = NULL;

		/* Loop over all devices. */
		ptDevCnt = ptDeviceList;
		ptDevEnd = ptDevCnt + ssizDevList;
		while( ptDevCnt<ptDevEnd )
		{
			ptDevice = *ptDevCnt;

			/* Try to open the device. */
			iLibUsbResult = libusb_open(ptDevice, &ptDevHandle);
			if( iLibUsbResult==LIBUSB_SUCCESS )
			{
				/* Get the device descriptor. */
				iLibUsbResult = libusb_get_descriptor(ptDevHandle, LIBUSB_DT_DEVICE, 0, (unsigned char*)&sDevDesc, sizeof(struct libusb_device_descriptor));
				if( iLibUsbResult==sizeof(struct libusb_device_descriptor) )
				{
					printf("Looking at USB device VID=0x%04x, PID=0x%04x\n", sDevDesc.idVendor, sDevDesc.idProduct);

					/* Is this a "Papa Schlumpf" device? */
					if( sDevDesc.idVendor==PAPA_SCHLUMPF_USB_VENDOR_ID && sDevDesc.idProduct==PAPA_SCHLUMPF_USB_PRODUCT_ID )
					{
						printf("Found a Papa Schlumpf device!\n");

						if( ptDevPapaSchlumpf == NULL)
						{
							/* Check if this device is the first which gets found*/
							ptDevPapaSchlumpf = ptDevice;
						}
						else
						{
							printf("Error! More than one Papa Schlumpf device connected!\n");
							tResult = PAPA_SCHLUMPF_ERROR_FoundMoreThanOneDevice;
							break;
						}
					}
				}

				/* Close the device. */
				libusb_close(ptDevHandle);
			}

			/* Next device in the list... */
			++ptDevCnt;
		}

		/* Did no error occur and found a device? */
		if( tResult==PAPA_SCHLUMPF_ERROR_Ok )
		{
			if( ptDevPapaSchlumpf==NULL )
			{
				fprintf(stderr, "%s: no hardware found!\n", g_pcPluginId);
				tResult = PAPA_SCHLUMPF_ERROR_NoHardwareFound;
			}
			else
			{
				/* Open the device. */
				iLibUsbResult = libusb_open(ptDevPapaSchlumpf, &ptDevHandlePapaSchlumpf);
				if( iLibUsbResult==LIBUSB_SUCCESS )
				{
					/* Claim interface #0. */
					iLibUsbResult = libusb_claim_interface(ptDevHandlePapaSchlumpf, 0);
					if( iLibUsbResult==LIBUSB_SUCCESS )
					{
						/* The handle to the device is now open. */
						g_ptDevHandlePapaSchlumpf = ptDevHandlePapaSchlumpf;
					}
					else
					{
						fprintf(stderr, "%s: failed to claim the interface: %d:%s\n", g_pcPluginId, iLibUsbResult, libusb_strerror(libusb_error(iLibUsbResult)));
						libusb_close(ptDevHandlePapaSchlumpf);
						tResult = PAPA_SCHLUMPF_ERROR_FailedToClaimInterface;
					}
				}
				else
				{
					fprintf(stderr, "%s: failed to open the usb devices: %d:%s\n", g_pcPluginId, iLibUsbResult, libusb_strerror(libusb_error(iLibUsbResult)));
					tResult = PAPA_SCHLUMPF_ERROR_FailedToOpenDevice;
				}
			}
		}


		/* Free the device list. */
		if( ptDeviceList!=NULL )
		{
			libusb_free_device_list(ptDeviceList, 1);
		}
	}

	return tResult;
}
#endif



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


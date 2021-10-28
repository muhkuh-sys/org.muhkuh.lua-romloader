/***************************************************************************
 *   Copyright (C) 2013 by Christoph Thelen                                *
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


#include <ctype.h>
#include <stdio.h>

#include "romloader_jtag_main.h"

/*-------------------------------------*/

#define CFG_DEBUGMSG 0
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


/*-------------------------------------*/

class romloader_jtag_read_functinoid : public romloader_read_functinoid
{
public:
	romloader_jtag_read_functinoid(romloader_jtag *ptDevice, lua_State *ptClientData)
	{
		m_ptJtagDevice = ptDevice;
		m_ptClientData = ptClientData;
	}

	/* always returns true, because m_ptJtagDevice->read_data32 raises an error if the read fails. */
	bool read_data32(uint32_t ulAddress, uint32_t *pulValue)
	{
		uint32_t ulValue;

		ulValue = m_ptJtagDevice->read_data32(m_ptClientData, ulAddress);
		*pulValue = ulValue;
		
		return true;
	}

private:
	romloader_jtag *m_ptJtagDevice;
	lua_State *m_ptClientData;
};



/*-------------------------------------*/


romloader_jtag_options::romloader_jtag_options(muhkuh_log *ptLog)
 : muhkuh_plugin_options(ptLog)
 , m_tOption_jtagReset(JTAG_RESET_HardReset)
 , m_tOption_jtagFrequencyKhz(0)
{
}


romloader_jtag_options::romloader_jtag_options(const romloader_jtag_options *ptCloneMe)
 : muhkuh_plugin_options(ptCloneMe)
 , m_tOption_jtagReset(ptCloneMe->m_tOption_jtagReset)
 , m_tOption_jtagFrequencyKhz(ptCloneMe->m_tOption_jtagFrequencyKhz)
{
}


romloader_jtag_options::~romloader_jtag_options(void)
{
}


void romloader_jtag_options::set_option(const char *pcKey, lua_State *ptLuaState, int iIndex)
{
	int iType;
	const char *pcValue;
	lua_Number dValue;
	unsigned long ulValue;
	const JTAG_RESET_TO_NAME_T *ptCnt;
	const JTAG_RESET_TO_NAME_T *ptEnd;
	const JTAG_RESET_TO_NAME_T *ptHit;


	if( strcmp(pcKey, "jtag_reset")==0 )
	{
		/* The value for the JTAG reset must be a string. */
		iType = lua_type(ptLuaState, iIndex);
		if( iType!=LUA_TSTRING )
		{
			m_ptLog->debug("Ignoring option '%s': the value must be a string, but it is a %s.", pcKey, lua_typename(ptLuaState, iType));
		}
		else
		{
			pcValue = lua_tostring(ptLuaState, iIndex);
			ptCnt = atJtagResetToName;
			ptEnd = atJtagResetToName + (sizeof(atJtagResetToName)/sizeof(atJtagResetToName[0]));
			ptHit = NULL;
			while( ptCnt<ptEnd )
			{
				if( strcmp(pcValue, ptCnt->pcName)==0 )
				{
					ptHit = ptCnt;
					break;
				}
				else
				{
					++ptCnt;
				}
			}
			if( ptHit==NULL )
			{
				m_ptLog->debug("Ignoring option '%s': the value '%s' is invalid.", pcKey, pcValue);
			}
			else
			{
				m_tOption_jtagReset = ptHit->tJtagReset;
				m_ptLog->debug("Setting option '%s' to '%s'.", pcKey, pcValue);
			}
		}
	}
	else if( strcmp(pcKey, "jtag_frequency_khz")==0 )
	{
		/* The value for the JTAG frequency must be a number. */
		iType = lua_type(ptLuaState, iIndex);
		if( iType!=LUA_TNUMBER )
		{
			m_ptLog->debug("Ignoring option '%s': the value must be a number, but it is a %s.", pcKey, lua_typename(ptLuaState, iType));
		}
		else
		{
			dValue = lua_tonumber(ptLuaState, iIndex);
			ulValue = (unsigned long)dValue;
			m_tOption_jtagFrequencyKhz = ulValue;
			m_ptLog->debug("Setting option '%s' to %d.", pcKey, ulValue);
		}
	}
	else
	{
		m_ptLog->debug("Ignoring unknown option '%s'.", pcKey);
	}
}


romloader_jtag_options::JTAG_RESET_T romloader_jtag_options::getOption_jtagReset(void)
{
	return m_tOption_jtagReset;
}


unsigned long romloader_jtag_options::getOption_jtagFrequencyKhz(void)
{
	return m_tOption_jtagFrequencyKhz;
}


/*-------------------------------------*/


const char *romloader_jtag_provider::m_pcPluginNamePattern = "romloader_jtag_%s@%s@%s";

romloader_jtag_provider::romloader_jtag_provider(swig_type_info *p_romloader_jtag, swig_type_info *p_romloader_jtag_reference)
 : muhkuh_plugin_provider("romloader_jtag")
 , m_fIsInitialized(false)
 , m_ptJtagDevice(NULL)
{
	int iResult;


	DEBUGMSG(ZONE_FUNCTION, ("+romloader_jtag_provider::romloader_jtag_provider(): p_romloader_jtag=%p, p_romloader_jtag_reference=%p\n", p_romloader_jtag, p_romloader_jtag_reference));

	/* Get the romloader_jtag LUA type. */
	m_ptPluginTypeInfo = p_romloader_jtag;
	m_ptReferenceTypeInfo = p_romloader_jtag_reference;

	/* Create a new options container. */
	m_ptPluginOptions = new romloader_jtag_options(m_ptLog);

	m_ptJtagDevice = new romloader_jtag_openocd(m_ptLog);
	if( m_ptJtagDevice!=NULL )
	{
		/* Try to initialize the JTAG driver. */
		iResult = m_ptJtagDevice->initialize();
		if( iResult==0 )
		{
			m_fIsInitialized = true;
		}
	}

	DEBUGMSG(ZONE_FUNCTION, ("-romloader_jtag_provider::romloader_jtag_provider()\n"));
}



romloader_jtag_provider::~romloader_jtag_provider(void)
{
	DEBUGMSG(ZONE_FUNCTION, ("+romloader_jtag_provider::~romloader_jtag_provider()\n"));

	if( m_ptJtagDevice!=NULL )
	{
		delete m_ptJtagDevice;
	}

	DEBUGMSG(ZONE_FUNCTION, ("-romloader_jtag_provider::~romloader_jtag_provider()\n"));
}


romloader_jtag_options *romloader_jtag_provider::GetOptions(void)
{
	return (romloader_jtag_options*)m_ptPluginOptions;
}

/*
   Detect interfaces and targets.
   Calls romloader_jtag_openocd::detect.
   For each interface/target combination found, a romloader_jtag_reference is created and added to ptLuaStateForTableAccess.
 */
int romloader_jtag_provider::DetectInterfaces(lua_State *ptLuaStateForTableAccess, lua_State *ptLuaStateForTableAccessOptional)
{
	int iResult;
	romloader_jtag_openocd::ROMLOADER_JTAG_DETECT_ENTRY_T *ptEntries;
	romloader_jtag_openocd::ROMLOADER_JTAG_DETECT_ENTRY_T *ptEntriesCnt;
	romloader_jtag_openocd::ROMLOADER_JTAG_DETECT_ENTRY_T *ptEntriesEnd;
	romloader_jtag_reference *ptRef;
	size_t sizEntries;
	bool fIsBusy;
	char strId[1024];


	DEBUGMSG(ZONE_FUNCTION, ("+romloader_jtag_provider::DetectInterfaces(): ptLuaStateForTableAccess=%p\n", ptLuaStateForTableAccess));

	sizEntries = 0;

	if( m_fIsInitialized==true && m_ptJtagDevice!=NULL )
	{
		/* Process the plugin options. */
		if( ptLuaStateForTableAccessOptional!=NULL )
		{
			processOptions(ptLuaStateForTableAccessOptional, 3);
		}

		/* detect devices */
		ptEntries = NULL;
		iResult = m_ptJtagDevice->detect(&ptEntries, &sizEntries);
		if( iResult==0 && ptEntries!=NULL )
		{
			ptEntriesCnt = ptEntries;
			ptEntriesEnd = ptEntries + sizEntries;
			while( ptEntriesCnt<ptEntriesEnd )
			{
				/* create the new instance */
				memset(strId, 0, sizeof(strId));
				snprintf(strId, sizeof(strId)-1, m_pcPluginNamePattern, ptEntriesCnt->pcTarget, ptEntriesCnt->pcInterface, ptEntriesCnt->pcLocation);
				fIsBusy = false;
				ptRef = new romloader_jtag_reference(strId, m_pcPluginId, fIsBusy, this);
				if( ptRef!=NULL )
				{
					add_reference_to_table(ptLuaStateForTableAccess, ptRef);
				}
				++ptEntriesCnt;
			}
		}
	}

	DEBUGMSG(ZONE_FUNCTION, ("-romloader_jtag_provider::DetectInterfaces(): sizEntries=%zd\n", sizEntries));

	return sizEntries;
}


/*
   Instantiate the plugin.
   Creates a romloader_jtag instance from a romloader_jtag_reference.
 */
romloader_jtag *romloader_jtag_provider::ClaimInterface(const muhkuh_plugin_reference *ptReference)
{
	romloader_jtag *ptPlugin;
	const char *pcName;
	const char *pcTargetName;
	size_t sizTargetName;
	const char *pcInterfaceName;
	size_t sizInterfaceName;
	const char *pcLocation;
	size_t sizLocation;
	const char *pcNamePrefix = "romloader_jtag_";
	const size_t sizNamePrefix = 15; /* size of pcNamePrefix without terminating 0. */
	char acInterface[1024];
	char acTarget[1024];
	char acLocation[1024];


	DEBUGMSG(ZONE_FUNCTION, ("+romloader_jtag_provider::ClaimInterface(): ptReference=%p\n", ptReference));

	/* expect error */
	ptPlugin = NULL;


	if( m_fIsInitialized!=true || m_ptJtagDevice==NULL )
	{
		fprintf(stderr, "%s(%p): claim_interface(): the plugin is not initialized!\n", m_pcPluginId, this);
	}
	else if( ptReference==NULL )
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
		else
		{
			/* Cut off the leading "romloader_jtag_". */
			if( strncmp(pcName, pcNamePrefix, sizNamePrefix)!=0 )
			{
				fprintf(stderr, "%s(%p): claim_interface(): the name does not start with \"%s\": %s\n", m_pcPluginId, this, pcNamePrefix, pcName);
			}
			else
			{
				/* Split the rest by the first "@". */
				pcTargetName = pcName + sizNamePrefix;
				pcInterfaceName = pcTargetName;
				while( *pcInterfaceName!='@' )
				{
					if( *pcInterfaceName=='\0' )
					{
						pcInterfaceName = NULL;
						break;
					}
					else
					{
						++pcInterfaceName;
					}
				}

				if( pcInterfaceName==NULL )
				{
					fprintf(stderr, "%s(%p): claim_interface(): the name contains no '@': %s\n", m_pcPluginId, this, pcName);
				}
				else
				{
					sizTargetName = pcInterfaceName - pcTargetName;
					if( sizTargetName==0 )
					{
						fprintf(stderr, "%s(%p): claim_interface(): no target name before the '@': %s\n", m_pcPluginId, this, pcName);
					}
					else
					{
						/* pcInterfaceName points to the '@' right now. Move one forward to get to the interface name. */
						++pcInterfaceName;

						/* Split the rest by the first "@". */
						pcLocation = pcInterfaceName;
						while( *pcLocation!='@' )
						{
							if( *pcLocation=='\0' )
							{
								pcLocation = NULL;
								break;
							}
							else
							{
								++pcLocation;
							}
						}

						if( pcLocation==NULL )
						{
							fprintf(stderr, "%s(%p): claim_interface(): the name contains no second '@': %s\n", m_pcPluginId, this, pcName);
						}
						else
						{
							/* Get the length of the interface name. */
							sizInterfaceName = pcLocation - pcInterfaceName;
							/* pcLocation points to the '@' right now. Move one forward to get to the location. */
							++pcLocation;
							/* Get the size of the location. */
							sizLocation = strlen(pcLocation);

							if( sizInterfaceName==0 )
							{
								fprintf(stderr, "%s(%p): claim_interface(): no interface name after the '@': %s\n", m_pcPluginId, this, pcName);
							}
							else if( sizLocation==0 )
							{
								fprintf(stderr, "%s(%p): claim_interface(): no location after the second '@': %s\n", m_pcPluginId, this, pcName);
							}
							else if( sizTargetName>(sizeof(acTarget)-1) )
							{
								fprintf(stderr, "%s(%p): claim_interface(): the target name is too long: %s\n", m_pcPluginId, this, pcName);
							}
							else if( sizInterfaceName>(sizeof(acInterface)-1) )
							{
								fprintf(stderr, "%s(%p): claim_interface(): the interface name is too long: %s\n", m_pcPluginId, this, pcName);
							}
							else if( sizLocation>(sizeof(acLocation)-1) )
							{
								fprintf(stderr, "%s(%p): claim_interface(): the location is too long: %s\n", m_pcPluginId, this, pcName);
							}
							else
							{
								memset(acInterface, 0, sizeof(acInterface));
								memset(acTarget, 0, sizeof(acTarget));
								memset(acLocation, 0, sizeof(acLocation));
								memcpy(acInterface, pcInterfaceName, sizInterfaceName);
								memcpy(acTarget, pcTargetName, sizTargetName);
								memcpy(acLocation, pcLocation, sizLocation);

								fprintf(stderr, "Interface name: %s\n", acInterface);
								fprintf(stderr, "Target name: %s\n", acTarget);
								fprintf(stderr, "Location: %s\n", acLocation);

								ptPlugin = new romloader_jtag(pcName, m_pcPluginId, this, acInterface, acTarget, acLocation);
								printf("%s(%p): claim_interface(): claimed interface %s.\n", m_pcPluginId, this, pcName);
							}
						}
					}
				}
			}
		}
	}

	DEBUGMSG(ZONE_FUNCTION, ("-romloader_jtag_provider::ClaimInterface(): ptPlugin=%p\n", ptPlugin));

	return ptPlugin;
}

/*
   Release a muhkuh_plugin instance. Does nothing.
 */
bool romloader_jtag_provider::ReleaseInterface(muhkuh_plugin *ptPlugin)
{
	bool fOk;
	const char *pcName;


	DEBUGMSG(ZONE_FUNCTION, ("+romloader_jtag_provider::ReleaseInterface(): ptPlugin=%p\n", ptPlugin));

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
		else
		{
			printf("%s(%p): released interface %s.\n", m_pcPluginId, this, pcName);
			fOk = true;
		}
	}

	DEBUGMSG(ZONE_FUNCTION, ("-romloader_jtag_provider::ReleaseInterface(): fOk=%d\n", fOk));

	return fOk;
}


/*-------------------------------------*/
/*
   Open the JTAG plugin.
   Creates a romloader_jtag_openocd instance.
   Tries to load the openOCD library.
   Stores the interface/target name strings.
 */
romloader_jtag::romloader_jtag(const char *pcName, const char *pcTyp, romloader_jtag_provider *ptProvider, const char *pcInterfaceName, const char *pcTargetName, const char *pcLocation)
 : romloader(pcName, pcTyp, ptProvider)
 , m_ptJtagProvider(ptProvider)
 , m_fIsInitialized(false)
 , m_ptJtagDevice(NULL)
 , m_pcInterfaceName(NULL)
 , m_pcTargetName(NULL)
 , m_pcLocation(NULL)
{
	int iResult;
	romloader_jtag_options *ptOptions;


	DEBUGMSG(ZONE_FUNCTION, ("+romloader_jtag::romloader_jtag(): pcName='%s', pcTyp='%s', ptProvider=%p, pcInterfaceName=%s, pcTargetName=%s\n", pcName, pcTyp, ptProvider, pcInterfaceName, pcTargetName));

	/* Show the options. */
	ptOptions = m_ptJtagProvider->GetOptions();
	m_ptLog->debug("Options: jtag_reset=%d, jtag_frequency=%d", ptOptions->getOption_jtagReset(), ptOptions->getOption_jtagFrequencyKhz());

	m_ptJtagDevice = new romloader_jtag_openocd(m_ptLog);
	if( m_ptJtagDevice!=NULL )
	{
		/* Try to initialize the JTAG driver. */
		iResult = m_ptJtagDevice->initialize();
		if( iResult==0 )
		{
			/* Create a copy of the interface and target names. */
			m_pcInterfaceName = clone_string(pcInterfaceName, SIZ_MAX_MUHKUH_PLUGIN_STRING);
			m_pcTargetName = clone_string(pcTargetName, SIZ_MAX_MUHKUH_PLUGIN_STRING);
			m_pcLocation = clone_string(pcLocation, SIZ_MAX_MUHKUH_PLUGIN_STRING);

			m_fIsInitialized = true;
		}
	}

	DEBUGMSG(ZONE_FUNCTION, ("-romloader_jtag::romloader_jtag()\n"));
}



romloader_jtag::~romloader_jtag(void)
{
	DEBUGMSG(ZONE_FUNCTION, ("-romloader_jtag::~romloader_jtag()\n"));

	if( m_pcInterfaceName!=NULL )
	{
		delete[] m_pcInterfaceName;
	}
	if( m_pcTargetName!=NULL )
	{
		delete[] m_pcTargetName;
	}
	if( m_pcLocation!=NULL )
	{
		delete[] m_pcLocation;
	}
	if( m_ptJtagDevice!=NULL )
	{
		delete m_ptJtagDevice;
	}

	DEBUGMSG(ZONE_FUNCTION, ("-romloader_jtag::~romloader_jtag()\n"));
}


/* open the connection to the device */
void romloader_jtag::Connect(lua_State *ptClientData)
{
	int iResult;
	bool fOk;
	romloader_jtag_read_functinoid ptReadFn(this, ptClientData);

	DEBUGMSG(ZONE_FUNCTION, ("+romloader_jtag::Connect(): ptClientData=%p\n", ptClientData));

	if( m_fIsConnected!=false )
	{
		printf("%s(%p): already connected, ignoring new connect request\n", m_pcName, this);
	}
	else
	{
		/* TODO: add the connect method. */

		iResult = m_ptJtagDevice->connect(m_pcInterfaceName, m_pcTargetName, m_pcLocation);
		if( iResult!=0 )
		{
			MUHKUH_PLUGIN_PUSH_ERROR(ptClientData, "%s(%p): failed to connect to device", m_pcName, this);
		}
		else
		{
			/* NOTE: set m_fIsConnected to true here or detect_chiptyp and chip_init will fail! */
			m_fIsConnected = true;

			fOk = detect_chiptyp(&ptReadFn);
			if (fOk != true)
			{
				MUHKUH_PLUGIN_PUSH_ERROR(ptClientData, "%s(%p): failed to detect chip type", m_pcName, this);
			}
			else
			{

				iResult = m_ptJtagDevice->init_chip(m_tChiptyp);

				if( iResult!=0 )
				{
					MUHKUH_PLUGIN_PUSH_ERROR(ptClientData, "%s(%p): failed to initialize the chip", m_pcName, this);
				}
				else
				{

				}
			}
		}
		DEBUGMSG(ZONE_FUNCTION, ("-romloader_jtag::Connect(): iResult=%d\n", iResult));

		if( iResult!=0 )
		{
			MUHKUH_PLUGIN_EXIT_ERROR(ptClientData);
		}
	}
}



/* close the connection to the device */
void romloader_jtag::Disconnect(lua_State *ptClientData)
{
	DEBUGMSG(ZONE_FUNCTION, ("+romloader_jtag::Disconnect(): ptClientData=%p\n", ptClientData));

	/* NOTE: allow disconnects even if the plugin was already disconnected. */
	if( m_ptJtagDevice!=NULL )
	{
		m_ptJtagDevice->disconnect();
	}

	m_fIsConnected = false;

	DEBUGMSG(ZONE_FUNCTION, ("-romloader_jtag::Disconnect()\n"));
}



romloader::TRANSPORTSTATUS_T romloader_jtag::send_raw_packet(const void *pvPacket, size_t sizPacket)
{
}



romloader::TRANSPORTSTATUS_T romloader_jtag::receive_packet(void)
{
}



/* Read a byte (8bit) from the netX. */
uint8_t romloader_jtag::read_data08(lua_State *ptClientData, uint32_t ulNetxAddress)
{
	uint8_t ucData;
	bool fOk;
	int iResult;


	DEBUGMSG(ZONE_FUNCTION, ("+romloader_jtag::read_data08(): ptClientData=%p, ulNetxAddress=0x%08x\n", ptClientData, ulNetxAddress));

	if( m_fIsConnected==false )
	{
		MUHKUH_PLUGIN_PUSH_ERROR(ptClientData, "%s(%p): read_data08: not connected!", m_pcName, this);
		fOk = false;
	}
	else
	{
		iResult = m_ptJtagDevice->read_data08(ulNetxAddress, &ucData);
		if( iResult!=0 )
		{
			fOk = false;
		}
		else
		{
			fOk = true;
		}
	}

	DEBUGMSG(ZONE_FUNCTION, ("-romloader_jtag::read_data08(): fOk=%d, ucData=0x%02x\n", fOk, ucData));

	if( fOk!=true )
	{
		printf("Exit Error\n");
		MUHKUH_PLUGIN_EXIT_ERROR(ptClientData);
	}

	return ucData;
}


/* Read a word (16bit) from the netX. */
uint16_t romloader_jtag::read_data16(lua_State *ptClientData, uint32_t ulNetxAddress)
{
	uint16_t usData;
	bool fOk;
	int iResult;


	DEBUGMSG(ZONE_FUNCTION, ("+romloader_jtag::read_data16(): ptClientData=%p, ulNetxAddress=0x%08x\n", ptClientData, ulNetxAddress));

	if( m_fIsConnected==false )
	{
		MUHKUH_PLUGIN_PUSH_ERROR(ptClientData, "%s(%p): read_data16: not connected!", m_pcName, this);
		fOk = false;
	}
	else
	{
		iResult = m_ptJtagDevice->read_data16(ulNetxAddress, &usData);
		if( iResult!=0 )
		{
			fOk = false;
		}
		else
		{
			fOk = true;
		}
	}

	DEBUGMSG(ZONE_FUNCTION, ("-romloader_jtag::read_data16(): fOk=%d, usData=0x%04x\n", fOk, usData));

	if( fOk!=true )
	{
		printf("Exit Error\n");
		MUHKUH_PLUGIN_EXIT_ERROR(ptClientData);
	}

	return usData;
}


/* Read a long (32bit) from the netX. */
uint32_t romloader_jtag::read_data32(lua_State *ptClientData, uint32_t ulNetxAddress)
{
	uint32_t ulData;
	bool fOk;
	int iResult;


	DEBUGMSG(ZONE_FUNCTION, ("+romloader_jtag::read_data32(): ptClientData=%p, ulNetxAddress=0x%08x\n", ptClientData, ulNetxAddress));

	if( m_fIsConnected==false )
	{
		MUHKUH_PLUGIN_PUSH_ERROR(ptClientData, "%s(%p): read_data32: not connected!", m_pcName, this);
		fOk = false;
	}
	else
	{
		iResult = m_ptJtagDevice->read_data32(ulNetxAddress, &ulData);
		if( iResult!=0 )
		{
			fOk = false;
		}
		else
		{
			fOk = true;
		}
	}

	/* Print the function end marker already here as the MUHKUH_PLUGIN_EXIT_ERROR might leave the function. */
	DEBUGMSG(ZONE_FUNCTION, ("-romloader_jtag::read_data32(): fOk=%d, ulData=0x%08x\n", fOk, ulData));

	if( fOk!=true )
	{
		printf("Exit Error\n");
		MUHKUH_PLUGIN_EXIT_ERROR(ptClientData);
	}

	return ulData;
}


/* Read a byte array from the netX.  */
void romloader_jtag::read_image(uint32_t ulNetxAddress, uint32_t ulSize, char **ppcBUFFER_OUT, size_t *psizBUFFER_OUT, SWIGLUA_REF tLuaFn, long lCallbackUserData)
{
	uint8_t *pucBufferStart;
	uint8_t *pucBuffer;
	size_t sizBuffer;
	bool fOk;
	int iResult;
	uint32_t ulChunk;
	bool fIsRunning;
	long lBytesProcessed;
	uint32_t ulMaxChunkSize;



	DEBUGMSG(ZONE_FUNCTION, ("+romloader_jtag::read_image(): ulNetxAddress=0x%08x, ulSize=0x%08x, ppcBUFFER_OUT=%p, psizBUFFER_OUT=%p, tLuaFn.L=%p, lCallbackUserData=0x%08lx\n", ulNetxAddress, ulSize, ppcBUFFER_OUT, psizBUFFER_OUT, tLuaFn.L, lCallbackUserData));

	/* Be optimistic. */
	fOk = true;

	pucBufferStart = NULL;
	sizBuffer = 0;

	if( m_fIsConnected==false )
	{
		MUHKUH_PLUGIN_PUSH_ERROR(tLuaFn.L, "%s(%p): read_image: not connected!", m_pcName, this);
		fOk = false;
	}
	/* if ulSize == 0, we return with fOk == true, pcBufferStart == NULL and sizBuffer == 0 */
	else if ( ulSize>0 )
	{
		/* Get the suggested chunk size. */
		ulMaxChunkSize = m_ptJtagDevice->get_image_chunk_size();

		pucBufferStart = (uint8_t*)malloc(ulSize);
		if( pucBufferStart==NULL )
		{
			MUHKUH_PLUGIN_PUSH_ERROR(tLuaFn.L, "%s(%p): read_image: failed to allocate %d bytes!", m_pcName, this, ulSize);
			fOk = false;
		}
		else
		{
			sizBuffer = ulSize;

			pucBuffer = pucBufferStart;
			lBytesProcessed = 0;
			do
			{
				ulChunk = ulSize;
				if( ulChunk>ulMaxChunkSize )
				{
					ulChunk = ulMaxChunkSize;
				}

				iResult = m_ptJtagDevice->read_image(ulNetxAddress, pucBuffer, ulChunk);
				if( iResult!=0 )
				{
					MUHKUH_PLUGIN_PUSH_ERROR(tLuaFn.L, "%s(%p): read_image failed!", m_pcName, this);
					fOk = false;
					break;
				}

				pucBuffer += ulChunk;
				ulSize -= ulChunk;
				ulNetxAddress += ulChunk;
				lBytesProcessed += ulChunk;

				fIsRunning = callback_long(&tLuaFn, lBytesProcessed, lCallbackUserData);
				if( fIsRunning!=true )
				{
					MUHKUH_PLUGIN_PUSH_ERROR(tLuaFn.L, "%s(%p): read_image cancelled!", m_pcName, this);
					fOk = false;
					break;
				}
			} while( ulSize!=0 );
		}
	}

	DEBUGMSG(ZONE_FUNCTION, ("-romloader_jtag::read_image(): fOk=%d, pcBufferStart=%p, sizBuffer=0x%08zx\n", fOk, pucBufferStart, sizBuffer));

	if( fOk==true )
	{
		*ppcBUFFER_OUT = (char*)pucBufferStart;
		*psizBUFFER_OUT = sizBuffer;
	}
	else
	{
		if ( pucBufferStart!=NULL)
		{
			free(pucBufferStart);
		}
		printf("Exit Error\n");
		MUHKUH_PLUGIN_EXIT_ERROR(tLuaFn.L);
	}
}


/* write a byte (8bit) from the pc to the netx */
void romloader_jtag::write_data08(lua_State *ptClientData, uint32_t ulNetxAddress, uint8_t ucData)
{
	bool fOk;
	int iResult;


	DEBUGMSG(ZONE_FUNCTION, ("+romloader_jtag::write_data08(): ptClientData=%p, ulNetxAddress=0x%08x, ucData=0x%02x\n", ptClientData, ulNetxAddress, ucData));

	if( m_fIsConnected==false )
	{
		MUHKUH_PLUGIN_PUSH_ERROR(ptClientData, "%s(%p): write_data08: not connected!", m_pcName, this);
		fOk = false;
	}
	else
	{
		iResult = m_ptJtagDevice->write_data08(ulNetxAddress, ucData);
		if( iResult!=0 )
		{
			fOk = false;
		}
		else
		{
			fOk = true;
		}
	}

	DEBUGMSG(ZONE_FUNCTION, ("-romloader_jtag::write_data08(): fOk=%d\n", fOk));

	if( fOk!=true )
	{
		printf("Exit Error\n");
		MUHKUH_PLUGIN_EXIT_ERROR(ptClientData);
	}
}


/* write a word (16bit) from the pc to the netx */
void romloader_jtag::write_data16(lua_State *ptClientData, uint32_t ulNetxAddress, uint16_t usData)
{
	bool fOk;
	int iResult;


	DEBUGMSG(ZONE_FUNCTION, ("+romloader_jtag::write_data16(): ptClientData=%p, ulNetxAddress=0x%08x, usData=0x%04x\n", ptClientData, ulNetxAddress, usData));

	if( m_fIsConnected==false )
	{
		MUHKUH_PLUGIN_PUSH_ERROR(ptClientData, "%s(%p): write_data16: not connected!", m_pcName, this);
		fOk = false;
	}
	else
	{
		iResult = m_ptJtagDevice->write_data16(ulNetxAddress, usData);
		if( iResult!=0 )
		{
			fOk = false;
		}
		else
		{
			fOk = true;
		}
	}

	DEBUGMSG(ZONE_FUNCTION, ("-romloader_jtag::write_data16(): fOk=%d\n", fOk));

	if( fOk!=true )
	{
		printf("Exit Error\n");
		MUHKUH_PLUGIN_EXIT_ERROR(ptClientData);
	}
}


/* write a long (32bit) from the pc to the netx */
void romloader_jtag::write_data32(lua_State *ptClientData, uint32_t ulNetxAddress, uint32_t ulData)
{
	bool fOk;
	int iResult;


	DEBUGMSG(ZONE_FUNCTION, ("+romloader_jtag::write_data32(): ptClientData=%p, ulNetxAddress=0x%08x, ulData=0x%08x\n", ptClientData, ulNetxAddress, ulData));

	if( m_fIsConnected==false )
	{
		MUHKUH_PLUGIN_PUSH_ERROR(ptClientData, "%s(%p): write_data32: not connected!", m_pcName, this);
		fOk = false;
	}
	else
	{
		iResult = m_ptJtagDevice->write_data32(ulNetxAddress, ulData);
		if( iResult!=0 )
		{
			fOk = false;
		}
		else
		{
			fOk = true;
		}
	}

	DEBUGMSG(ZONE_FUNCTION, ("-romloader_jtag::write_data32(): fOk=%d\n", fOk));

	if( fOk!=true )
	{
		printf("Exit Error\n");
		MUHKUH_PLUGIN_EXIT_ERROR(ptClientData);
	}
}


/* write a byte array from the pc to the netx */
void romloader_jtag::write_image(uint32_t ulNetxAddress, const char *pcBUFFER_IN, size_t sizBUFFER_IN, SWIGLUA_REF tLuaFn, long lCallbackUserData)
{
	bool fOk;
	uint32_t ulChunk;
	uint32_t ulMaxChunkSize;
	int iResult;
	bool fIsRunning;
	long lBytesProcessed;
	const uint8_t *pucData;
	uint32_t ulSizeData;


	DEBUGMSG(ZONE_FUNCTION, ("+romloader_jtag::write_image(): ulNetxAddress=0x%08x, pcBUFFER_IN=%p, sizBUFFER_IN=0x%08zx, tLuaFn.L=%p, lCallbackUserData=0x%08lx\n", ulNetxAddress, pcBUFFER_IN, sizBUFFER_IN, tLuaFn.L, lCallbackUserData));

	/* Be optimistic. */
	fOk = true;

	if( m_fIsConnected==false )
	{
		MUHKUH_PLUGIN_PUSH_ERROR(tLuaFn.L, "%s(%p): write_image: not connected!", m_pcName, this);
		fOk = false;
	}
	else if( sizBUFFER_IN!=0 )
	{
		/* Get the suggested chunk size. */
		ulMaxChunkSize = m_ptJtagDevice->get_image_chunk_size();

		pucData = (const uint8_t*)pcBUFFER_IN;
		ulSizeData = sizBUFFER_IN;

		lBytesProcessed = 0;
		do
		{
			ulChunk = ulSizeData;
			if( ulChunk>ulMaxChunkSize )
			{
				ulChunk = ulMaxChunkSize;
			}

			iResult = m_ptJtagDevice->write_image(ulNetxAddress, pucData, ulChunk);
			if( iResult!=0 )
			{
				MUHKUH_PLUGIN_PUSH_ERROR(tLuaFn.L, "%s(%p): write_image failed!", m_pcName, this);
				fOk = false;
				break;
			}

			pucData += ulChunk;
			ulSizeData -= ulChunk;
			ulNetxAddress += ulChunk;
			lBytesProcessed += ulChunk;

			fIsRunning = callback_long(&tLuaFn, lBytesProcessed, lCallbackUserData);
			if( fIsRunning!=true )
			{
				MUHKUH_PLUGIN_PUSH_ERROR(tLuaFn.L, "%s(%p): write_image cancelled!", m_pcName, this);
				fOk = false;
				break;
			}
		} while( ulSizeData!=0 );
	}

	DEBUGMSG(ZONE_FUNCTION, ("-romloader_jtag::write_image(): fOk=%d\n", fOk));

	if( fOk!=true )
	{
		printf("Exit Error\n");
		MUHKUH_PLUGIN_EXIT_ERROR(tLuaFn.L);
	}
}


/*
Passing of the callback:

* here:
  void romloader_jtag::call(uint32_t ulNetxAddress, uint32_t ulParameterR0, SWIGLUA_REF tLuaFn, long lCallbackUserData)
	iResult = m_ptJtagDevice->call(ulNetxAddress, ulParameterR0, NULL );

In romloader_jtag_openocd:
  int romloader_jtag_openocd::call(uint32_t ulNetxAddress, uint32_t ulParameterR0, PFN_MUHKUH_CALL_PRINT_CALLBACK pfnCallback)
	iResult = m_tJtagDevice.tFunctions.tFn.pfnCall(m_tJtagDevice.pvOpenocdContext, ulNetxAddress, ulParameterR0, pfnCallback, NULL);

* In muhkuh_openocd:
  int muhkuh_openocd_call(void *pvContext, uint32_t ulNetxAddress, uint32_t ulR0, PFN_MUHKUH_CALL_PRINT_CALLBACK pfnCallback, void *pvCallbackUserData)
	fIsRunning = pfnCallback(pvCallbackUserData, NULL, 0);


* In romloader:
  bool romloader::callback_string(SWIGLUA_REF *ptLuaFn, const char *pcProgressData, size_t sizProgressData, long lCallbackUserData)


romloader_jtag_openocd.h/muhkuh_openocd.h
typedef int (*PFN_MUHKUH_CALL_PRINT_CALLBACK) (void *pvCallbackUserData, uint8_t *pucData, unsigned long ulDataSize);


typedef struct { lua_State* L; int ref;} SWIGLUA_REF;

* lCallbackUserData is a value which is transparently passed through from the call in the Lua script to the Lua callback routine.

*/

typedef struct
{
	romloader_jtag* ptInstance;
	SWIGLUA_REF*    ptLuaFn;
	long            lCallbackUserData;
} T_JTAG_CALLBACK_DATA;

extern "C" {

int romloader_jtag_callback_string_c(void *pvCallbackUserData, uint8_t *pucData, unsigned long ulDataSize)
{
	T_JTAG_CALLBACK_DATA* ptCallbackData = (T_JTAG_CALLBACK_DATA*) pvCallbackUserData;
	SWIGLUA_REF* ptLuaFn = ptCallbackData->ptLuaFn;
	long lCallbackUserData = ptCallbackData->lCallbackUserData;
	romloader_jtag* pthis = ptCallbackData->ptInstance;
	bool fIsRunning;
	int iIsRunning;

	//DEBUGMSG(ZONE_FUNCTION, ("+romloader_jtag_callback_string_c\n"));
	fIsRunning = pthis->callback_string_public(ptLuaFn, (const char*)pucData, ulDataSize, lCallbackUserData);
	iIsRunning = fIsRunning? 1 : 0;
	//DEBUGMSG(ZONE_FUNCTION, ("-romloader_jtag_callback_string_c return value: %d\n", iIsRunning));

	return iIsRunning;
}
}


bool romloader_jtag::callback_string_public(SWIGLUA_REF *ptLuaFn, const char *pcProgressData, size_t sizProgressData, long lCallbackUserData)
{
	return callback_string(ptLuaFn, pcProgressData, sizProgressData, lCallbackUserData);
}

/* call routine */
void romloader_jtag::call(uint32_t ulNetxAddress, uint32_t ulParameterR0, SWIGLUA_REF tLuaFn, long lCallbackUserData)
{
	bool fOk;
	int iResult;
	T_JTAG_CALLBACK_DATA tCallbackData;

	DEBUGMSG(ZONE_FUNCTION, ("+romloader_jtag::call(): ulNetxAddress=0x%08x, ulParameterR0=0x%08x, tLuaFn.L=%p, lCallbackUserData=0x%08lx\n", ulNetxAddress, ulParameterR0, tLuaFn.L, lCallbackUserData));

	if( m_fIsConnected==false )
	{
		MUHKUH_PLUGIN_PUSH_ERROR(tLuaFn.L, "%s(%p): call: not connected!", m_pcName, this);
		fOk = false;
	}
	else
	{
			DEBUGMSG(ZONE_FUNCTION, ("+romloader_jtag::call(): m_ptJtagDevice->call\n" ));

			tCallbackData.ptInstance = this;
			tCallbackData.ptLuaFn = &tLuaFn;
			tCallbackData.lCallbackUserData = lCallbackUserData;

			iResult = m_ptJtagDevice->call(ulNetxAddress, ulParameterR0, (PFN_MUHKUH_CALL_PRINT_CALLBACK) &romloader_jtag_callback_string_c, &tCallbackData);
			if (iResult == 0)
			{
				fOk = true;
			}
			else
			{
				MUHKUH_PLUGIN_PUSH_ERROR(tLuaFn.L, "%s(%p): call failed!", m_pcName, this);
				fOk = false;
			}
	}

	DEBUGMSG(ZONE_FUNCTION, ("-romloader_jtag::call(): fOk=%d\n", fOk));

	if( fOk!=true )
	{
		printf("Exit Error\n");
		MUHKUH_PLUGIN_EXIT_ERROR(tLuaFn.L);
	}
}


void romloader_jtag::hexdump(const unsigned char *pucData, unsigned long ulSize, unsigned long ulNetxAddress)
{
	const unsigned char *pucDumpCnt, *pucDumpEnd;
	unsigned long ulAddressCnt;
	size_t sizBytesLeft;
	size_t sizChunkSize;
	size_t sizChunkCnt;


	// show a hexdump of the data
	pucDumpCnt = pucData;
	pucDumpEnd = pucData + ulSize;
	ulAddressCnt = ulNetxAddress;
	while( pucDumpCnt<pucDumpEnd )
	{
		// get number of bytes for the next line
		sizChunkSize = 16;
		sizBytesLeft = pucDumpEnd - pucDumpCnt;
		if( sizChunkSize>sizBytesLeft )
		{
			sizChunkSize = sizBytesLeft;
		}

		// start a line in the dump with the address
		printf("%08lX: ", ulAddressCnt);
		// append the data bytes
		sizChunkCnt = sizChunkSize;
		while( sizChunkCnt!=0 )
		{
			printf("%02X ", *(pucDumpCnt++));
			--sizChunkCnt;
		}
		// next line
		printf("\n");
		ulAddressCnt += sizChunkSize;
	}
}


/*-------------------------------------*/


romloader_jtag_reference::romloader_jtag_reference(void)
 : muhkuh_plugin_reference()
{
}


romloader_jtag_reference::romloader_jtag_reference(const char *pcName, const char *pcTyp, bool fIsUsed, romloader_jtag_provider *ptProvider)
 : muhkuh_plugin_reference(pcName, pcTyp, fIsUsed, ptProvider)
{
}


romloader_jtag_reference::romloader_jtag_reference(const char *pcName, const char *pcTyp, const char *pcLocation, bool fIsUsed, romloader_jtag_provider *ptProvider)
 : muhkuh_plugin_reference(pcName, pcTyp, pcLocation, fIsUsed, ptProvider)
{
}


romloader_jtag_reference::romloader_jtag_reference(const romloader_jtag_reference *ptCloneMe)
 : muhkuh_plugin_reference(ptCloneMe)
{
}


/*-------------------------------------*/


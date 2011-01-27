/***************************************************************************
 *   Copyright (C) 2008 by Christoph Thelen                                *
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

#include "romloader_uart_main.h"


#ifdef _WIN32
	#define snprintf _snprintf
#endif

/*-------------------------------------*/

const char *romloader_uart_provider::m_pcPluginNamePattern = "romloader_uart_%s";

romloader_uart_provider::romloader_uart_provider(swig_type_info *p_romloader_uart, swig_type_info *p_romloader_uart_reference)
 : muhkuh_plugin_provider("romloader_uart")
{
	int iResult;


	printf("%s(%p): provider create\n", m_pcPluginId, this);

	/* get the romloader_uart lua type */
	m_ptPluginTypeInfo = p_romloader_uart;
	m_ptReferenceTypeInfo = p_romloader_uart_reference;
}


romloader_uart_provider::~romloader_uart_provider(void)
{
	printf("%s(%p): provider delete\n", m_pcPluginId, this);
}


int romloader_uart_provider::DetectInterfaces(lua_State *ptLuaStateForTableAccess)
{
	int iResult;
	size_t sizDeviceNames;
	char **ppcDeviceNames;
	char **ppcDeviceNamesCnt;
	char **ppcDeviceNamesEnd;
	romloader_uart_reference *ptReference;
	bool fDeviceIsBusy;


	/* detect all interfaces */
	sizDeviceNames = romloader_uart_device_platform::ScanForPorts(&ppcDeviceNames);
	printf("found %d devs, %p\n", sizDeviceNames, ppcDeviceNames);

	if( ppcDeviceNames!=NULL )
	{
		/* add all detected devices to the table */
		ppcDeviceNamesCnt = ppcDeviceNames;
		ppcDeviceNamesEnd = ppcDeviceNames + sizDeviceNames;
		while( ppcDeviceNamesCnt<ppcDeviceNamesEnd )
		{
			/* TODO: check if the device is busy, but how */
			fDeviceIsBusy = false;
	
			/* create the new instance */
			printf("create instance '%s'\n", *ppcDeviceNamesCnt);

			ptReference = new romloader_uart_reference(*ppcDeviceNamesCnt, m_pcPluginId, fDeviceIsBusy, this);
			add_reference_to_table(ptLuaStateForTableAccess, ptReference);

			/* free the name */
			free(*ppcDeviceNamesCnt);

			/* next device */
			++ppcDeviceNamesCnt;
		}

		/* free the list */
		free(ppcDeviceNames);
	}

	return sizDeviceNames;
}


romloader_uart *romloader_uart_provider::ClaimInterface(const muhkuh_plugin_reference *ptReference)
{
	romloader_uart *ptPlugin;
	const char *pcName;
	char acDevice[PATH_MAX];


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
		else if( sscanf(pcName, m_pcPluginNamePattern, acDevice)!=1 )
		{
			fprintf(stderr, "%s(%p): claim_interface(): invalid name: %s\n", m_pcPluginId, this, pcName);
		}
		else
		{
			ptPlugin = new romloader_uart(pcName, m_pcPluginId, this, acDevice);
			printf("%s(%p): claim_interface(): claimed interface %s.\n", m_pcPluginId, this, pcName);
		}
	}

	return ptPlugin;
}


bool romloader_uart_provider::ReleaseInterface(muhkuh_plugin *ptPlugin)
{
	bool fOk;
	const char *pcName;
	unsigned int uiBusNr;
	unsigned int uiDevAdr;


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

	return fOk;
}


/*-------------------------------------*/


romloader_uart::romloader_uart(const char *pcName, const char *pcTyp, romloader_uart_provider *ptProvider, const char *pcDeviceName)
 : romloader(pcName, pcTyp, ptProvider)
 , m_fIsConnected(false)
 , m_ptUartDev(NULL)
{
	printf("%s(%p): created in romloader_uart\n", m_pcName, this);

	m_ptUartDev = new romloader_uart_device_platform(pcDeviceName);
}


romloader_uart::~romloader_uart(void)
{
	printf("%s(%p): deleted in romloader_uart\n", m_pcName, this);

	if( m_ptUartDev!=NULL )
	{
		m_ptUartDev->Close();
	}
}


bool romloader_uart::chip_init(lua_State *ptClientData)
{
	bool fResult;


	switch( m_tChiptyp )
	{
	case ROMLOADER_CHIPTYP_NETX500:
	case ROMLOADER_CHIPTYP_NETX100:
		switch( m_tRomcode )
		{
		case ROMLOADER_ROMCODE_ABOOT:
			// aboot does not set the serial vectors
			write_data32(ptClientData, 0x10001ff0, 0);
			write_data32(ptClientData, 0x10001ff4, 0);
			write_data32(ptClientData, 0x10001ff8, 0);
			write_data32(ptClientData, 0x10001ffc, 0);
			fResult = true;
			break;
		case ROMLOADER_ROMCODE_HBOOT:
			// hboot needs no special init
			fResult = true;
			break;
		case ROMLOADER_ROMCODE_UNKNOWN:
			fResult = false;
			break;
		}
		break;

	case ROMLOADER_CHIPTYP_NETX50:
		switch( m_tRomcode )
		{
		case ROMLOADER_ROMCODE_ABOOT:
			// this is an unknown combination
			fResult = false;
			break;
		case ROMLOADER_ROMCODE_HBOOT:
			// hboot needs no special init
			// netx50 needs sdram fix
			printf("%s(%p): running netx50 sdram fix...\n", m_pcName, this);
			write_data32(ptClientData, 0x1c005830, 0x00000001);
			write_data32(ptClientData, 0x1c005104, 0xbffffffc);
			write_data32(ptClientData, 0x1c00510c, 0x00480001);
			write_data32(ptClientData, 0x1c005110, 0x00000001);
			printf("%s(%p): netx50 sdram fix ok!\n", m_pcName, this);
			write_data32(ptClientData, 0x1c00510c, 0);
			write_data32(ptClientData, 0x1c005110, 0);
			write_data32(ptClientData, 0x1c005830, 0);
			fResult = true;
			break;
		case ROMLOADER_ROMCODE_UNKNOWN:
			fResult = false;
			break;
		}
		break;

	case ROMLOADER_CHIPTYP_UNKNOWN:
		fResult = false;
		break;
	}

	return fResult;
}


void romloader_uart::Connect(lua_State *ptClientData)
{
	int iResult;


	/* Expect error. */
	iResult = -1;

	printf("%s(%p): connect\n", m_pcName, this);

	if( m_ptUartDev!=NULL && m_fIsConnected==false )
	{
		if( m_ptUartDev->Open()!=true )
		{
			MUHKUH_PLUGIN_PUSH_ERROR(ptClientData, "%s(%p): failed to open device!", m_pcName, this);
		}
		else if( m_ptUartDev->IdentifyLoader()!=true )
		{
			MUHKUH_PLUGIN_PUSH_ERROR(ptClientData, "%s(%p): failed to identify loader!", m_pcName, this);
		}
		else if( detect_chiptyp(ptClientData)!=true )
		{
			MUHKUH_PLUGIN_PUSH_ERROR(ptClientData, "%s(%p): failed to detect chiptyp!", m_pcName, this);
		}
		else if( chip_init(ptClientData)!=true )
		{
			MUHKUH_PLUGIN_PUSH_ERROR(ptClientData, "%s(%p): failed to init chip!", m_pcName, this);
		}
		else
		{
			m_fIsConnected = true;
			iResult = 0;
		}

		if( iResult!=0 )
		{
			m_ptUartDev->Close();
			MUHKUH_PLUGIN_EXIT_ERROR(ptClientData);
		}
	}
}


void romloader_uart::Disconnect(lua_State *ptClientData)
{
	printf("%s(%p): disconnect\n", m_pcName, this);

	if( m_ptUartDev!=NULL )
	{
		m_ptUartDev->Close();
	}

	m_fIsConnected = false;
}


unsigned char romloader_uart::read_data08(lua_State *ptClientData, unsigned long ulNetxAddress)
{
	fprintf(stderr, "read_data08: 0x%08x\n", ulNetxAddress);
}


unsigned short romloader_uart::read_data16(lua_State *ptClientData, unsigned long ulNetxAddress)
{
	fprintf(stderr, "read_data16: 0x%08x\n", ulNetxAddress);
}


unsigned long romloader_uart::read_data32(lua_State *ptClientData, unsigned long ulNetxAddress)
{
	fprintf(stderr, "read_data32: 0x%08x\n", ulNetxAddress);
}

#if 0
bool romloader_uart::parseDumpLine(const char *pcLine, size_t sizLineLen, unsigned long ulAddress, unsigned long ulElements, unsigned char *pucBuffer, wxString &strErrorMsg)
{
	bool fResult;
	int iMatches;
	unsigned long ulResultAddress;
	unsigned long ulChunkCnt;
	unsigned int uiByte;


	// expect success
	fResult = true;

	// is enough input data left?
	if( sizLineLen<(10+ulElements*3) )
	{
		strErrorMsg = wxT("strange response from netx!");
		fResult = false;
	}
	else
	{
		// get the address
		iMatches = sscanf(pcLine, "%8lX: ", &ulResultAddress);
		if( iMatches!=1 )
		{
			strErrorMsg = wxT("strange response from netx!");
			fResult = false;
		}
		else if( ulResultAddress!=ulAddress )
		{
			strErrorMsg = wxT("response does not match request!");
			fResult = false;
		}
		else
		{
			// advance parse ptr to data part of the line
			pcLine += 10;
			// get all bytes
			ulChunkCnt = ulElements;
			while( ulChunkCnt!=0 )
			{
				// get one hex digit
				iMatches = sscanf(pcLine, "%2X ", &uiByte);
				if( iMatches!=1 )
				{
					strErrorMsg = wxT("strange response from netx!");
					fResult = false;
					break;
				}
				// advance parse ptr to data part of the line
				pcLine += 3;
				*(pucBuffer++) = (char)uiByte;
				// one number processed
				--ulChunkCnt;
			}
		}
	}

	return fResult;
}
#endif

void romloader_uart::read_image(unsigned long ulNetxAddress, unsigned long ulSize, char **ppcBUFFER_OUT, size_t *psizBUFFER_OUT, SWIGLUA_REF tLuaFn, long lCallbackUserData)
{
	fprintf(stderr, "read_image: 0x%08x, 0x%08x\n", ulNetxAddress, ulSize);
}


void romloader_uart::write_data08(lua_State *ptClientData, unsigned long ulNetxAddress, unsigned char ucData)
{
	fprintf(stderr, "write_data08: 0x%08x, 0x%02x\n", ulNetxAddress, ucData);
}


void romloader_uart::write_data16(lua_State *ptClientData, unsigned long ulNetxAddress, unsigned short usData)
{
	fprintf(stderr, "write_data16: 0x%08x, 0x%04x\n", ulNetxAddress, usData);
}


void romloader_uart::write_data32(lua_State *ptClientData, unsigned long ulNetxAddress, unsigned long ulData)
{
	fprintf(stderr, "write_data32: 0x%08x, 0x%08x\n", ulNetxAddress, ulData);
}

#if 0
unsigned int romloader_uart::buildCrc(const char *pcData, size_t sizDataLen)
{
  const unsigned char *pucCnt;
  const unsigned char *pucEnd;
  unsigned int uiCrc;


  pucCnt = (const unsigned char*)pcData;
  pucEnd = pucCnt + sizDataLen;

  uiCrc = 0xffff;
  while(pucCnt<pucEnd)
  {
    uiCrc = crc16(uiCrc, *(pucCnt++));
  }

  return uiCrc;
}
#endif

void romloader_uart::write_image(unsigned long ulNetxAddress, const char *pcBUFFER_IN, size_t sizBUFFER_IN, SWIGLUA_REF tLuaFn, long lCallbackUserData)
{
	fprintf(stderr, "write_image: 0x%08x, 0x%08x\n", ulNetxAddress, sizBUFFER_IN);
}


void romloader_uart::call(unsigned long ulNetxAddress, unsigned long ulParameterR0, SWIGLUA_REF tLuaFn, long lCallbackUserData)
{
	fprintf(stderr, "call: 0x%08x, 0x%08x\n", ulNetxAddress, ulParameterR0);
}


/*-------------------------------------*/


romloader_uart_reference::romloader_uart_reference(void)
 : muhkuh_plugin_reference()
{
}


romloader_uart_reference::romloader_uart_reference(const char *pcName, const char *pcTyp, bool fIsUsed, romloader_uart_provider *ptProvider)
 : muhkuh_plugin_reference(pcName, pcTyp, fIsUsed, ptProvider)
{
}


romloader_uart_reference::romloader_uart_reference(const romloader_uart_reference *ptCloneMe)
 : muhkuh_plugin_reference(ptCloneMe)
{
}


/*-------------------------------------*/

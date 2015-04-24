/***************************************************************************
 *   Copyright (C) 2011 by Christoph Thelen                                *
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


#include "romloader_eth_main.h"

#define ETH_REPLY_TIMEOUT_MS 500

#if defined(_MSC_VER)
#       define snprintf _snprintf
#endif

/*-------------------------------------*/

class romloader_eth_read_functinoid : public romloader_read_functinoid
{
public:
	romloader_eth_read_functinoid(romloader_eth *ptDevice, lua_State *ptClientData)
	{
		m_ptDevice = ptDevice;
		m_ptClientData = ptClientData;
	}

	uint32_t read_data32(uint32_t ulAddress)
	{
		uint32_t ulValue;


		ulValue = m_ptDevice->read_data32(m_ptClientData, ulAddress);
		return ulValue;
	}

private:
	romloader_eth *m_ptDevice;
	lua_State *m_ptClientData;
};


/*-------------------------------------*/

const char *romloader_eth_provider::m_pcPluginNamePattern = "romloader_eth_%s";

romloader_eth_provider::romloader_eth_provider(swig_type_info *p_romloader_eth, swig_type_info *p_romloader_eth_reference)
 : muhkuh_plugin_provider("romloader_eth")
{
	int iResult;


	printf("%s(%p): provider create\n", m_pcPluginId, this);

	/* get the romloader_eth lua type */
	m_ptPluginTypeInfo = p_romloader_eth;
	m_ptReferenceTypeInfo = p_romloader_eth_reference;
}


romloader_eth_provider::~romloader_eth_provider(void)
{
	printf("%s(%p): provider delete\n", m_pcPluginId, this);
}


int romloader_eth_provider::DetectInterfaces(lua_State *ptLuaStateForTableAccess)
{
	int iResult;
	size_t sizDeviceNames;
	char **ppcDeviceNames;
	char **ppcDeviceNamesCnt;
	char **ppcDeviceNamesEnd;
	romloader_eth_reference *ptReference;
	bool fDeviceIsBusy;


	/* detect all interfaces */
	sizDeviceNames = romloader_eth_device_platform::ScanForServers(&ppcDeviceNames);
	printf("found %ld devs, %p\n", sizDeviceNames, ppcDeviceNames);

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

			ptReference = new romloader_eth_reference(*ppcDeviceNamesCnt, m_pcPluginId, fDeviceIsBusy, this);
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


romloader_eth *romloader_eth_provider::ClaimInterface(const muhkuh_plugin_reference *ptReference)
{
	romloader_eth *ptPlugin;
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
			ptPlugin = new romloader_eth(pcName, m_pcPluginId, this, acDevice);
			printf("%s(%p): claim_interface(): claimed interface %s.\n", m_pcPluginId, this, pcName);
		}
	}

	return ptPlugin;
}


bool romloader_eth_provider::ReleaseInterface(muhkuh_plugin *ptPlugin)
{
	bool fOk;
	const char *pcName;
	char acDevice[PATH_MAX];


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
		else if( sscanf(pcName, m_pcPluginNamePattern, acDevice)!=1 )
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


romloader_eth::romloader_eth(const char *pcName, const char *pcTyp, romloader_eth_provider *ptProvider, const char *pcServerName)
 : romloader(pcName, pcTyp, ptProvider)
 , m_fIsConnected(false)
 , m_ptEthDev(NULL)
{
	printf("%s(%p): created in romloader_eth\n", m_pcName, this);

	m_ptEthDev = new romloader_eth_device_platform(pcServerName);
}


romloader_eth::~romloader_eth(void)
{
	printf("%s(%p): deleted in romloader_uart\n", m_pcName, this);

	if( m_ptEthDev!=NULL )
	{
		m_ptEthDev->Close();
		delete m_ptEthDev;
	}
}


void romloader_eth::hexdump(const uint8_t *pucData, uint32_t ulSize)
{
	const uint8_t *pucDumpCnt, *pucDumpEnd;
	uint32_t ulAddressCnt;
	size_t sizBytesLeft;
	size_t sizChunkSize;
	size_t sizChunkCnt;


	// show a hexdump of the data
	pucDumpCnt = pucData;
	pucDumpEnd = pucData + ulSize;
	ulAddressCnt = 0;
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
		printf("%08X: ", ulAddressCnt);
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

/* The ethernet monitor on netX56 sets the serial vectors at the "old" location 0x0809fff0.
   If the chip is a netX56, set the appropriate serial vectors at 0x08000100. */
void romloader_eth::set_serial_vectors(lua_State *ptClientData)
{
	if (m_tChiptyp == ROMLOADER_CHIPTYP_NETX56)
	{
		write_data32(ptClientData, 0x08000100UL, 0x080f4639UL);
		write_data32(ptClientData, 0x08000104UL, 0x080f4805UL);
		write_data32(ptClientData, 0x08000108UL, 0x080f4605UL);
		write_data32(ptClientData, 0x0800010cUL, 0x080f47bdUL);
	}
	else if (m_tChiptyp == ROMLOADER_CHIPTYP_NETX56B)
	{
		write_data32(ptClientData, 0x08000100UL, 0x080f4cf9UL);
		write_data32(ptClientData, 0x08000104UL, 0x080f4ec9UL);
		write_data32(ptClientData, 0x08000108UL, 0x080f4cc5UL);
		write_data32(ptClientData, 0x0800010cUL, 0x080f4e81UL);
	}
}



void romloader_eth::Connect(lua_State *ptClientData)
{
	int iResult;
	romloader_eth_read_functinoid tFn(this, ptClientData);


	/* Expect error. */
	iResult = -1;

	printf("%s(%p): connect\n", m_pcName, this);

	if( m_ptEthDev!=NULL && m_fIsConnected==false )
	{
		if( m_ptEthDev->Open()!=true )
		{
			MUHKUH_PLUGIN_PUSH_ERROR(ptClientData, "%s(%p): failed to open device!", m_pcName, this);
		}
		else if( detect_chiptyp(&tFn)!=true )
		{
			MUHKUH_PLUGIN_PUSH_ERROR(ptClientData, "%s(%p): failed to detect chiptyp!", m_pcName, this);
		}
		else
		{
			m_fIsConnected = true;
			set_serial_vectors(ptClientData);
			iResult = 0;
		}

		if( iResult!=0 )
		{
			m_ptEthDev->Close();
			MUHKUH_PLUGIN_EXIT_ERROR(ptClientData);
		}
	}
}


void romloader_eth::Disconnect(lua_State *ptClientData)
{
	printf("%s(%p): disconnect\n", m_pcName, this);

	if( m_ptEthDev!=NULL )
	{
		m_ptEthDev->Close();
	}

	m_fIsConnected = false;
}


uint8_t romloader_eth::read_data08(lua_State *ptClientData, uint32_t ulNetxAddress)
{
	uint8_t aucCommand[7];
	int iResult;
	uint8_t ucValue;
	bool fOk;
	size_t sizResponse;


	/* Expect error. */
	fOk = false;

	ucValue = 0;

	aucCommand[0] = MI_COMMAND_Read | (MI_ACCESSSIZE_Byte<<6);
	aucCommand[1] = 1;
	aucCommand[2] = 0;
	aucCommand[3] = (uint8_t)( ulNetxAddress      & 0xffU);
	aucCommand[4] = (uint8_t)((ulNetxAddress>>8 ) & 0xffU);
	aucCommand[5] = (uint8_t)((ulNetxAddress>>16) & 0xffU);
	aucCommand[6] = (uint8_t)((ulNetxAddress>>24) & 0xffU);
	iResult = m_ptEthDev->ExecuteCommand(aucCommand, 7, aucRxBuffer, sizeof(aucRxBuffer), &sizResponse);
	if( iResult!=0 )
	{
		MUHKUH_PLUGIN_PUSH_ERROR(ptClientData, "%s(%p): failed to execute command!", m_pcName, this);
	}
	else
	{
		if( sizResponse!=1+1 )
		{
			MUHKUH_PLUGIN_PUSH_ERROR(ptClientData, "%s(%p): answer to read_data08 has wrong packet size of %d!", m_pcName, this, sizResponse);
		}
		else
		{
			ucValue = aucRxBuffer[1];
			fOk = true;
		}
	}

	if( fOk!=true )
	{
		MUHKUH_PLUGIN_EXIT_ERROR(ptClientData);
	}

	return ucValue;
}


uint16_t romloader_eth::read_data16(lua_State *ptClientData, uint32_t ulNetxAddress)
{
	uint8_t aucCommand[7];
	int iResult;
	uint16_t usValue;
	bool fOk;
	size_t sizResponse;


	/* Expect error. */
	fOk = false;

	usValue = 0;

	aucCommand[0] = MI_COMMAND_Read | (MI_ACCESSSIZE_Word<<6);
	aucCommand[1] = 2;
	aucCommand[2] = 0;
	aucCommand[3] = (uint8_t)( ulNetxAddress      & 0xffU);
	aucCommand[4] = (uint8_t)((ulNetxAddress>>8 ) & 0xffU);
	aucCommand[5] = (uint8_t)((ulNetxAddress>>16) & 0xffU);
	aucCommand[6] = (uint8_t)((ulNetxAddress>>24) & 0xffU);
	iResult = m_ptEthDev->ExecuteCommand(aucCommand, 7, aucRxBuffer, sizeof(aucRxBuffer), &sizResponse);
	if( iResult!=0 )
	{
		MUHKUH_PLUGIN_PUSH_ERROR(ptClientData, "%s(%p): failed to execute command!", m_pcName, this);
	}
	else
	{
		if( sizResponse!=1+2 )
		{
			MUHKUH_PLUGIN_PUSH_ERROR(ptClientData, "%s(%p): answer to read_data16 has wrong packet size of %d!", m_pcName, this, sizResponse);
		}
		else
		{
			usValue = aucRxBuffer[1] |
			          aucRxBuffer[2]<<8;
			fOk = true;
		}
	}

	if( fOk!=true )
	{
		MUHKUH_PLUGIN_EXIT_ERROR(ptClientData);
	}

	return usValue;
}


uint32_t romloader_eth::read_data32(lua_State *ptClientData, uint32_t ulNetxAddress)
{
	uint8_t aucCommand[7];
	int iResult;
	uint32_t ulValue;
	bool fOk;
	size_t sizResponse;


	/* Expect error. */
	fOk = false;

	ulValue = 0;

	aucCommand[0] = MI_COMMAND_Read | (MI_ACCESSSIZE_Long<<6);
	aucCommand[1] = 4;
	aucCommand[2] = 0;
	aucCommand[3] = (uint8_t)( ulNetxAddress      & 0xffU);
	aucCommand[4] = (uint8_t)((ulNetxAddress>>8 ) & 0xffU);
	aucCommand[5] = (uint8_t)((ulNetxAddress>>16) & 0xffU);
	aucCommand[6] = (uint8_t)((ulNetxAddress>>24) & 0xffU);
	iResult = m_ptEthDev->ExecuteCommand(aucCommand, 7, aucRxBuffer, sizeof(aucRxBuffer), &sizResponse);
	if( iResult!=0 )
	{
		MUHKUH_PLUGIN_PUSH_ERROR(ptClientData, "%s(%p): failed to execute command!", m_pcName, this);
	}
	else
	{
		if( sizResponse!=1+4 )
		{
			MUHKUH_PLUGIN_PUSH_ERROR(ptClientData, "%s(%p): answer to read_data32 has wrong packet size of %d!", m_pcName, this, sizResponse);
		}
		else
		{
			ulValue = aucRxBuffer[1] |
			          aucRxBuffer[2]<<8 |
			          aucRxBuffer[3]<<16 |
			          aucRxBuffer[4]<<24;
			fOk = true;
		}
	}

	if( fOk!=true )
	{
		MUHKUH_PLUGIN_EXIT_ERROR(ptClientData);
	}

	return ulValue;
}


void romloader_eth::read_image(uint32_t ulNetxAddress, uint32_t ulSize, char **ppcBUFFER_OUT, size_t *psizBUFFER_OUT, SWIGLUA_REF tLuaFn, long lCallbackUserData)
{
	char *pcBuffer;
	size_t sizBuffer;
	bool fOk;
	size_t sizChunk;
	uint8_t aucCommand[7];
	bool fIsRunning;
	long lBytesProcessed;
	size_t sizResponse;
	int iResult;


	/* Be optimistic. */
	fOk = true;

	if( m_fIsConnected==false )
	{
		MUHKUH_PLUGIN_PUSH_ERROR(tLuaFn.L, "%s(%p): not connected!", m_pcName, this);
		fOk = false;
	}
	else if( ulSize==0 )
	{
		pcBuffer = NULL;
		sizBuffer = 0;
	}
	else
	{
		sizBuffer = ulSize;
		pcBuffer = (char*)malloc(sizBuffer);
		if( pcBuffer==NULL )
		{
			MUHKUH_PLUGIN_PUSH_ERROR(tLuaFn.L, "%s(%p): failed to allocate %d bytes!", m_pcName, this, ulSize);
			fOk = false;
		}
		else
		{
			*ppcBUFFER_OUT = pcBuffer;
			*psizBUFFER_OUT = sizBuffer;

			lBytesProcessed = 0;
			do
			{
				sizChunk = ulSize;
				if( sizChunk>(MI_ETH_MAX_PACKET_SIZE-1) )
				{
					sizChunk = (MI_ETH_MAX_PACKET_SIZE-1);
				}

				aucCommand[0] = MI_COMMAND_Read | (MI_ACCESSSIZE_Byte<<6);
				aucCommand[1] = (uint8_t)(sizChunk&0xffU);
				aucCommand[2] = (uint8_t)((sizChunk>>8U)&0xffU);
				aucCommand[3] = (uint8_t)( ulNetxAddress      & 0xffU);
				aucCommand[4] = (uint8_t)((ulNetxAddress>>8 ) & 0xffU);
				aucCommand[5] = (uint8_t)((ulNetxAddress>>16) & 0xffU);
				aucCommand[6] = (uint8_t)((ulNetxAddress>>24) & 0xffU);
				
				iResult = m_ptEthDev->ExecuteCommand(aucCommand, 7, aucRxBuffer, sizeof(aucRxBuffer), &sizResponse);
				if( iResult!=0 )
				{
					MUHKUH_PLUGIN_PUSH_ERROR(tLuaFn.L, "%s(%p): failed to execute command!", m_pcName, this);
					fOk = false;
					break;
				}
				else
				{
					if( sizResponse!=1+sizChunk )
					{
						MUHKUH_PLUGIN_PUSH_ERROR(tLuaFn.L, "%s(%p): answer to read_image has wrong packet size of %d!", m_pcName, this, sizResponse);
						fOk = false;
						break;
					}
					else
					{
						memcpy(pcBuffer, aucRxBuffer+1, sizChunk);
						pcBuffer += sizChunk;
						ulSize -= sizChunk;
						ulNetxAddress += sizChunk;
						lBytesProcessed += sizChunk;

						fIsRunning = callback_long(&tLuaFn, lBytesProcessed, lCallbackUserData);
						if( fIsRunning!=true )
						{
							break;
						}
					}
				}
			} while( ulSize!=0 );
		}
	}

	if( fOk!=true )
	{
		MUHKUH_PLUGIN_EXIT_ERROR(tLuaFn.L);
	}
}


void romloader_eth::write_data08(lua_State *ptClientData, uint32_t ulNetxAddress, uint8_t ucData)
{
	uint8_t aucCommand[8];
	uint32_t ulValue;
	bool fOk;
	size_t sizResponse;
	int iResult;


	/* Expect error. */
	fOk = false;

	ulValue = 0;

	aucCommand[0] = MI_COMMAND_Write | (MI_ACCESSSIZE_Byte<<6);
	aucCommand[1] = 1;
	aucCommand[2] = 0;
	aucCommand[3] = (uint8_t)( ulNetxAddress      & 0xffU);
	aucCommand[4] = (uint8_t)((ulNetxAddress>>8 ) & 0xffU);
	aucCommand[5] = (uint8_t)((ulNetxAddress>>16) & 0xffU);
	aucCommand[6] = (uint8_t)((ulNetxAddress>>24) & 0xffU);
	aucCommand[7] = ucData;
	iResult = m_ptEthDev->ExecuteCommand(aucCommand, 8, aucRxBuffer, sizeof(aucRxBuffer), &sizResponse);
	if( iResult!=0 )
	{
		MUHKUH_PLUGIN_PUSH_ERROR(ptClientData, "%s(%p): failed to execute command!", m_pcName, this);
	}
	else
	{
		if( sizResponse!=1 )
		{
			MUHKUH_PLUGIN_PUSH_ERROR(ptClientData, "%s(%p): answer to write_data08 has wrong packet size of %d!", m_pcName, this, sizResponse);
		}
		else
		{
			fOk = true;
		}
	}

	if( fOk!=true )
	{
		MUHKUH_PLUGIN_EXIT_ERROR(ptClientData);
	}
}


void romloader_eth::write_data16(lua_State *ptClientData, uint32_t ulNetxAddress, uint16_t usData)
{
	uint8_t aucCommand[9];
	bool fOk;
	size_t sizResponse;
	int iResult;


	/* Expect error. */
	fOk = false;

	aucCommand[0] = MI_COMMAND_Write | (MI_ACCESSSIZE_Word<<6);
	aucCommand[1] = 2;
	aucCommand[2] = 0;
	aucCommand[3] = (uint8_t)( ulNetxAddress      & 0xffU);
	aucCommand[4] = (uint8_t)((ulNetxAddress>>8 ) & 0xffU);
	aucCommand[5] = (uint8_t)((ulNetxAddress>>16) & 0xffU);
	aucCommand[6] = (uint8_t)((ulNetxAddress>>24) & 0xffU);
	aucCommand[7] = (uint8_t)( usData    & 0xffU);
	aucCommand[8] = (uint8_t)((usData>>8)& 0xffU);
	iResult = m_ptEthDev->ExecuteCommand(aucCommand, 9, aucRxBuffer, sizeof(aucRxBuffer), &sizResponse);
	if( iResult!=0 )
	{
		MUHKUH_PLUGIN_PUSH_ERROR(ptClientData, "%s(%p): failed to execute command!", m_pcName, this);
	}
	else
	{
		if( sizResponse!=1 )
		{
			MUHKUH_PLUGIN_PUSH_ERROR(ptClientData, "%s(%p): answer to write_data16 has wrong packet size of %d!", m_pcName, this, sizResponse);
		}
		else
		{
			fOk = true;
		}
	}

	if( fOk!=true )
	{
		MUHKUH_PLUGIN_EXIT_ERROR(ptClientData);
	}
}


void romloader_eth::write_data32(lua_State *ptClientData, uint32_t ulNetxAddress, uint32_t ulData)
{
	uint8_t aucCommand[11];
	uint32_t ulValue;
	bool fOk;
	size_t sizResponse;
	int iResult;


	/* Expect error. */
	fOk = false;

	ulValue = 0;

	aucCommand[ 0] = MI_COMMAND_Write | (MI_ACCESSSIZE_Long<<6);
	aucCommand[ 1] = 4;
	aucCommand[ 2] = 0;
	aucCommand[ 3] = (uint8_t)( ulNetxAddress      & 0xffU);
	aucCommand[ 4] = (uint8_t)((ulNetxAddress>>8 ) & 0xffU);
	aucCommand[ 5] = (uint8_t)((ulNetxAddress>>16) & 0xffU);
	aucCommand[ 6] = (uint8_t)((ulNetxAddress>>24) & 0xffU);
	aucCommand[ 7] = (uint8_t)( ulData     & 0xffU);
	aucCommand[ 8] = (uint8_t)((ulData>> 8)& 0xffU);
	aucCommand[ 9] = (uint8_t)((ulData>>16)& 0xffU);
	aucCommand[10] = (uint8_t)((ulData>>24)& 0xffU);
	iResult = m_ptEthDev->ExecuteCommand(aucCommand, 11, aucRxBuffer, sizeof(aucRxBuffer), &sizResponse);
	if( iResult!=0 )
	{
		MUHKUH_PLUGIN_PUSH_ERROR(ptClientData, "%s(%p): failed to execute command!", m_pcName, this);
	}
	else
	{
		if( sizResponse!=1 )
		{
			MUHKUH_PLUGIN_PUSH_ERROR(ptClientData, "%s(%p): answer to write_data32 has wrong packet size of %d!", m_pcName, this, sizResponse);
		}
		else
		{
			fOk = true;
		}
	}

	if( fOk!=true )
	{
		MUHKUH_PLUGIN_EXIT_ERROR(ptClientData);
	}
}


void romloader_eth::write_image(uint32_t ulNetxAddress, const char *pcBUFFER_IN, size_t sizBUFFER_IN, SWIGLUA_REF tLuaFn, long lCallbackUserData)
{
	bool fOk;
	size_t sizChunk;
	uint8_t aucCommand[MI_ETH_MAX_PACKET_SIZE];
	size_t sizOutBuf;
	size_t sizInBuf;
	bool fIsRunning;
	long lBytesProcessed;
	size_t sizResponse;
	int iResult;


	/* Be optimistic. */
	fOk = true;

	if( m_fIsConnected==false )
	{
		MUHKUH_PLUGIN_PUSH_ERROR(tLuaFn.L, "%s(%p): not connected!", m_pcName, this);
		fOk = false;
	}
	else if( sizBUFFER_IN!=0 )
	{
		lBytesProcessed = 0;
		do
		{
			sizChunk = sizBUFFER_IN;
			if( sizChunk>(MI_ETH_MAX_PACKET_SIZE-7) )
			{
				sizChunk = (MI_ETH_MAX_PACKET_SIZE-7);
			}

			aucCommand[0] = MI_COMMAND_Write | (MI_ACCESSSIZE_Byte<<6);
			aucCommand[1] = (uint8_t)(sizChunk&0xffU);
			aucCommand[2] = (uint8_t)((sizChunk>>8U)&0xffU);
			aucCommand[3] = (uint8_t)( ulNetxAddress      & 0xffU);
			aucCommand[4] = (uint8_t)((ulNetxAddress>>8 ) & 0xffU);
			aucCommand[5] = (uint8_t)((ulNetxAddress>>16) & 0xffU);
			aucCommand[6] = (uint8_t)((ulNetxAddress>>24) & 0xffU);
			memcpy(aucCommand+7, pcBUFFER_IN, sizChunk);
			
			iResult = m_ptEthDev->ExecuteCommand(aucCommand, sizChunk+7, aucRxBuffer, sizeof(aucRxBuffer), &sizResponse);
			if( iResult!=0 )
			{
				MUHKUH_PLUGIN_PUSH_ERROR(tLuaFn.L, "%s(%p): failed to execute command!", m_pcName, this);
				fOk = false;
				break;
			}
			else
			{
				if( sizResponse!=1 )
				{
					MUHKUH_PLUGIN_PUSH_ERROR(tLuaFn.L, "%s(%p): answer to write_image has wrong packet size of %d!", m_pcName, this, sizResponse);
					fOk = false;
					break;
				}
				else
				{
					pcBUFFER_IN += sizChunk;
					sizBUFFER_IN -= sizChunk;
					ulNetxAddress += sizChunk;
					lBytesProcessed += sizChunk;

					fIsRunning = callback_long(&tLuaFn, lBytesProcessed, lCallbackUserData);
					if( fIsRunning!=true )
					{
						break;
					}
				}
			}
		} while( sizBUFFER_IN!=0 );
	}

	if( fOk!=true )
	{
		MUHKUH_PLUGIN_EXIT_ERROR(tLuaFn.L);
	}
}


void romloader_eth::call(uint32_t ulNetxAddress, uint32_t ulParameterR0, SWIGLUA_REF tLuaFn, long lCallbackUserData)
{
	bool fOk;
	uint8_t aucCommand[9];
	uint8_t aucMessageBuffer[ETH_MAX_PACKET_SIZE];
	const uint8_t aucCancelBuf[1] = { 0x2b };
	uint8_t ucStatus;
	bool fIsRunning;
	char *pcProgressData;
	size_t sizProgressData;
	size_t sizResponse;
	int iResult;


	if( m_fIsConnected==false )
	{
		MUHKUH_PLUGIN_PUSH_ERROR(tLuaFn.L, "%s(%p): not connected!", m_pcName, this);
		fOk = false;
	}
	else
	{
		/* Construct the command packet. */
		aucCommand[0x00] = MI_COMMAND_Execute;
		aucCommand[0x01] = (uint8_t)( ulNetxAddress      & 0xffU);
		aucCommand[0x02] = (uint8_t)((ulNetxAddress>>8 ) & 0xffU);
		aucCommand[0x03] = (uint8_t)((ulNetxAddress>>16) & 0xffU);
		aucCommand[0x04] = (uint8_t)((ulNetxAddress>>24) & 0xffU);
		aucCommand[0x05] = (uint8_t)( ulParameterR0      & 0xffU);
		aucCommand[0x06] = (uint8_t)((ulParameterR0>>8 ) & 0xffU);
		aucCommand[0x07] = (uint8_t)((ulParameterR0>>16) & 0xffU);
		aucCommand[0x08] = (uint8_t)((ulParameterR0>>24) & 0xffU);

		iResult = m_ptEthDev->ExecuteCommand(aucCommand, 9, aucRxBuffer, sizeof(aucRxBuffer), &sizResponse);
		if( iResult!=0 )
		{
			MUHKUH_PLUGIN_PUSH_ERROR(tLuaFn.L, "%s(%p): failed to execute command!", m_pcName, this);
			fOk = false;
		}
		else
		{
			if( sizResponse!=1 )
			{
				MUHKUH_PLUGIN_PUSH_ERROR(tLuaFn.L, "%s(%p): answer to call command has wrong packet size of %d!", m_pcName, this, sizResponse);
				fOk = false;
			}
			else
			{
				/* Receive message packets. */
				while(1)
				{
					pcProgressData = NULL;
					sizProgressData = 0;

					iResult = m_ptEthDev->RecvPacket(aucMessageBuffer, sizeof(aucMessageBuffer), 1000, &sizResponse);
					if( iResult==0 && sizResponse==0 )
					{
						/* Do nothing in case of timeout. The application is just running quietly. */
					}
					else if( iResult!=0 )
					{
						MUHKUH_PLUGIN_PUSH_ERROR(tLuaFn.L, "%s(%p): Failed to receive a packet: %d", m_pcName, this, iResult);
						fOk = false;
					}
					else
					{
						ucStatus = aucMessageBuffer[0];
						if( ucStatus==MI_STATUS_CallMessage )
						{
							pcProgressData = (char*)(aucMessageBuffer+1);
							sizProgressData = sizResponse - 1;
						}
						else if( ucStatus==MI_STATUS_CallFinished )
						{
							fOk = true;
							break;
						}
					}

					if (pcProgressData != NULL)
					{
						fIsRunning = callback_string(&tLuaFn, pcProgressData, sizProgressData, lCallbackUserData);
						if( fIsRunning!=true )
						{
							/* Send a cancel request to the device. */
							iResult = m_ptEthDev->SendPacket(aucCancelBuf, 1);

							MUHKUH_PLUGIN_PUSH_ERROR(tLuaFn.L, "%s(%p): the call was canceled!", m_pcName, this);
//							fOk = false;
							fOk = true;
							break;
						}
					}

				}
			}
		}
	}

	if( fOk!=true )
	{
		MUHKUH_PLUGIN_EXIT_ERROR(tLuaFn.L);
	}
}


/*-------------------------------------*/


romloader_eth_reference::romloader_eth_reference(void)
 : muhkuh_plugin_reference()
{
}


romloader_eth_reference::romloader_eth_reference(const char *pcName, const char *pcTyp, bool fIsUsed, romloader_eth_provider *ptProvider)
 : muhkuh_plugin_reference(pcName, pcTyp, fIsUsed, ptProvider)
{
}


romloader_eth_reference::romloader_eth_reference(const romloader_eth_reference *ptCloneMe)
 : muhkuh_plugin_reference(ptCloneMe)
{
}


/*-------------------------------------*/

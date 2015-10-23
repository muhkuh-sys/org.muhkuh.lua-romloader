/***************************************************************************
 *   Copyright (C) 2015 by Christoph Thelen                                *
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

#include "romloader_dpm_main.h"
#include "romloader_dpm_device_linux_uio.h"


/*-------------------------------------*/

const char *romloader_dpm_provider::m_pcPluginNamePattern = "romloader_dpm_%s";

romloader_dpm_provider::romloader_dpm_provider(swig_type_info *p_romloader_dpm, swig_type_info *p_romloader_dpm_reference)
 : muhkuh_plugin_provider("romloader_dpm")
{
	printf("%s(%p): provider create\n", m_pcPluginId, this);

	/* get the romloader_dpm lua type */
	m_ptPluginTypeInfo = p_romloader_dpm;
	m_ptReferenceTypeInfo = p_romloader_dpm_reference;
}



romloader_dpm_provider::~romloader_dpm_provider(void)
{
	printf("%s(%p): provider delete\n", m_pcPluginId, this);
}



int romloader_dpm_provider::DetectInterfaces(lua_State *ptLuaStateForTableAccess)
{
	size_t sizDeviceNames;
	char **ppcDeviceNames;
	char **ppcDeviceNamesCnt;
	char **ppcDeviceNamesEnd;
	romloader_dpm_reference *ptReference;
	bool fDeviceIsBusy;


	/* Detect all interfaces. */
	sizDeviceNames = romloader_dpm_device_linux_uio::ScanForDevices(&ppcDeviceNames);

	if( ppcDeviceNames!=NULL )
	{
		/* Add all detected devices to the table. */
		ppcDeviceNamesCnt = ppcDeviceNames;
		ppcDeviceNamesEnd = ppcDeviceNames + sizDeviceNames;
		while( ppcDeviceNamesCnt<ppcDeviceNamesEnd )
		{
			/* DPM devices are never busy. */
			fDeviceIsBusy = false;
	
			/* Create the new instance. */
			ptReference = new romloader_dpm_reference(*ppcDeviceNamesCnt, m_pcPluginId, fDeviceIsBusy, this);
			add_reference_to_table(ptLuaStateForTableAccess, ptReference);

			/* Free the name. */
			free(*ppcDeviceNamesCnt);

			/* Next device. */
			++ppcDeviceNamesCnt;
		}

		/* Free the list. */
		free(ppcDeviceNames);
	}

	return sizDeviceNames;
}



romloader_dpm *romloader_dpm_provider::ClaimInterface(const muhkuh_plugin_reference *ptReference)
{
	romloader_dpm *ptPlugin;
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
			ptPlugin = new romloader_dpm(pcName, m_pcPluginId, this, acDevice);
			printf("%s(%p): claim_interface(): claimed interface %s.\n", m_pcPluginId, this, pcName);
		}
	}

	return ptPlugin;
}



bool romloader_dpm_provider::ReleaseInterface(muhkuh_plugin *ptPlugin)
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


romloader_dpm::romloader_dpm(const char *pcName, const char *pcTyp, romloader_dpm_provider *ptProvider, const char *pcDeviceName)
 : romloader(pcName, pcTyp, ptProvider)
{
	printf("%s(%p): created in romloader_dpm\n", m_pcName, this);
}


romloader_dpm::~romloader_dpm(void)
{
	printf("%s(%p): deleted in romloader_dpm\n", m_pcName, this);
}



void romloader_dpm::Connect(lua_State *ptClientData)
{
	bool fResult;
	int iResult;


	/* Expect error. */
	fResult = false;

	printf("%s(%p): connect\n", m_pcName, this);

	if( m_fIsConnected==false )
	{
		/* TODO: open the UIO driver. */
		/* TODO: download the monitor if not already running. */


//		if( fResult!=true )
//		{
			m_fIsConnected = false;
			MUHKUH_PLUGIN_EXIT_ERROR(ptClientData);
//		}
	}
}


void romloader_dpm::Disconnect(lua_State *ptClientData)
{
	printf("%s(%p): disconnect\n", m_pcName, this);

	m_fIsConnected = false;
}



uint8_t romloader_dpm::read_data08(lua_State *ptClientData, uint32_t ulNetxAddress)
{
	uint8_t aucCommand[7];
	uint8_t ucValue;
	bool fOk;


	/* Expect error. */
	fOk = false;

	ucValue = 0;

	if( m_fIsConnected==false )
	{
		MUHKUH_PLUGIN_PUSH_ERROR(ptClientData, "%s(%p): not connected!", m_pcName, this);
	}
	else
	{
#if 0
		/* Get the next sequence number. */
		m_uiMonitorSequence = (m_uiMonitorSequence + 1) & (MONITOR_SEQUENCE_MSK>>MONITOR_SEQUENCE_SRT);

		aucCommand[0] = MONITOR_COMMAND_Read |
		                (MONITOR_ACCESSSIZE_Byte<<MONITOR_ACCESSSIZE_SRT) |
		                (uint8_t)(m_uiMonitorSequence << MONITOR_SEQUENCE_SRT);
		aucCommand[1] = 1;
		aucCommand[2] = 0;
		aucCommand[3] = (uint8_t)( ulNetxAddress       & 0xffU);
		aucCommand[4] = (uint8_t)((ulNetxAddress>> 8U) & 0xffU);
		aucCommand[5] = (uint8_t)((ulNetxAddress>>16U) & 0xffU);
		aucCommand[6] = (uint8_t)((ulNetxAddress>>24U) & 0xffU);
		tResult = execute_command(aucCommand, 7);
		if( tResult!=UARTSTATUS_OK )
		{
			MUHKUH_PLUGIN_PUSH_ERROR(ptClientData, "%s(%p): failed to execute command!", m_pcName, this);
		}
		else
		{
			if( m_sizPacketInputBuffer!=4+2 )
			{
				MUHKUH_PLUGIN_PUSH_ERROR(ptClientData, "%s(%p): answer to read_data08 has wrong packet size of %d!", m_pcName, this, m_sizPacketInputBuffer);
			}
			else
			{
				ucValue = m_aucPacketInputBuffer[3];
				fOk = true;
			}
		}
#endif
	}

	if( fOk!=true )
	{
		MUHKUH_PLUGIN_EXIT_ERROR(ptClientData);
	}

	return ucValue;
}



uint16_t romloader_dpm::read_data16(lua_State *ptClientData, uint32_t ulNetxAddress)
{
	uint8_t aucCommand[7];
	uint16_t usValue;
	bool fOk;


	/* Expect error. */
	fOk = false;

	usValue = 0;

	if( m_fIsConnected==false )
	{
		MUHKUH_PLUGIN_PUSH_ERROR(ptClientData, "%s(%p): not connected!", m_pcName, this);
	}
	else
	{
#if 0
		/* Get the next sequence number. */
		m_uiMonitorSequence = (m_uiMonitorSequence + 1) & (MONITOR_SEQUENCE_MSK>>MONITOR_SEQUENCE_SRT);

		aucCommand[0] = MONITOR_COMMAND_Read |
		                (MONITOR_ACCESSSIZE_Word<<MONITOR_ACCESSSIZE_SRT) |
		                (uint8_t)(m_uiMonitorSequence << MONITOR_SEQUENCE_SRT);
		aucCommand[1] = 2;
		aucCommand[2] = 0;
		aucCommand[3] = (uint8_t)( ulNetxAddress       & 0xffU);
		aucCommand[4] = (uint8_t)((ulNetxAddress>> 8U) & 0xffU);
		aucCommand[5] = (uint8_t)((ulNetxAddress>>16U) & 0xffU);
		aucCommand[6] = (uint8_t)((ulNetxAddress>>24U) & 0xffU);
		tResult = execute_command(aucCommand, 7);
		if( tResult!=UARTSTATUS_OK )
		{
			MUHKUH_PLUGIN_PUSH_ERROR(ptClientData, "%s(%p): failed to execute command!", m_pcName, this);
		}
		else
		{
			if( m_sizPacketInputBuffer!=4+3 )
			{
				MUHKUH_PLUGIN_PUSH_ERROR(ptClientData, "%s(%p): answer to read_data16 has wrong packet size of %d!", m_pcName, this, m_sizPacketInputBuffer);
			}
			else
			{
				usValue = ((uint16_t)(m_aucPacketInputBuffer[3])) |
				          ((uint16_t)(m_aucPacketInputBuffer[4]))<<8U;
				fOk = true;
			}
		}
#endif
	}

	if( fOk!=true )
	{
		MUHKUH_PLUGIN_EXIT_ERROR(ptClientData);
	}

	return usValue;
}



uint32_t romloader_dpm::read_data32(lua_State *ptClientData, uint32_t ulNetxAddress)
{
	uint8_t aucCommand[7];
	uint32_t ulValue;
	bool fOk;


	/* Expect error. */
	fOk = false;

	ulValue = 0;

	if( m_fIsConnected==false )
	{
		MUHKUH_PLUGIN_PUSH_ERROR(ptClientData, "%s(%p): not connected!", m_pcName, this);
	}
	else
	{
#if 0
		/* Get the next sequence number. */
		m_uiMonitorSequence = (m_uiMonitorSequence + 1) & (MONITOR_SEQUENCE_MSK>>MONITOR_SEQUENCE_SRT);

		aucCommand[0] = MONITOR_COMMAND_Read |
		                (MONITOR_ACCESSSIZE_Long<<MONITOR_ACCESSSIZE_SRT) |
		                (uint8_t)(m_uiMonitorSequence << MONITOR_SEQUENCE_SRT);
		aucCommand[1] = 4;
		aucCommand[2] = 0;
		aucCommand[3] = (uint8_t)( ulNetxAddress       & 0xffU);
		aucCommand[4] = (uint8_t)((ulNetxAddress>> 8U) & 0xffU);
		aucCommand[5] = (uint8_t)((ulNetxAddress>>16U) & 0xffU);
		aucCommand[6] = (uint8_t)((ulNetxAddress>>24U) & 0xffU);
		tResult = execute_command(aucCommand, 7);
		if( tResult!=UARTSTATUS_OK )
		{
			MUHKUH_PLUGIN_PUSH_ERROR(ptClientData, "%s(%p): failed to execute command!", m_pcName, this);
		}
		else
		{
			if( m_sizPacketInputBuffer!=4+5 )
			{
				MUHKUH_PLUGIN_PUSH_ERROR(ptClientData, "%s(%p): answer to read_data32 has wrong packet size of %d!", m_pcName, this, m_sizPacketInputBuffer);
			}
			else
			{
				ulValue = ((uint32_t)(m_aucPacketInputBuffer[3]))      |
				          ((uint32_t)(m_aucPacketInputBuffer[4]))<< 8U |
				          ((uint32_t)(m_aucPacketInputBuffer[5]))<<16U |
				          ((uint32_t)(m_aucPacketInputBuffer[6]))<<24U;
				fOk = true;
			}
		}
#endif
	}

	if( fOk!=true )
	{
		MUHKUH_PLUGIN_EXIT_ERROR(ptClientData);
	}

	return ulValue;
}



void romloader_dpm::read_image(uint32_t ulNetxAddress, uint32_t ulSize, char **ppcBUFFER_OUT, size_t *psizBUFFER_OUT, SWIGLUA_REF tLuaFn, long lCallbackUserData)
{
	char *pcBufferStart;
	char *pcBuffer;
	size_t sizBuffer;
	bool fOk;
	size_t sizChunk;
	uint8_t aucCommand[m_sizMaxPacketSizeHost];
	bool fIsRunning;
	long lBytesProcessed;


	/* Be optimistic. */
	fOk = true;

	pcBufferStart = NULL;
	sizBuffer = 0;

	if( m_fIsConnected==false )
	{
		MUHKUH_PLUGIN_PUSH_ERROR(tLuaFn.L, "%s(%p): not connected!", m_pcName, this);
		fOk = false;
	}
	/* if ulSize == 0, we return with fOk == true, pcBufferStart == NULL and sizBuffer == 0 */
	else if( ulSize > 0 )
	{
#if 0
		pcBufferStart = (char*)malloc(ulSize);
		if( pcBufferStart==NULL )
		{
			MUHKUH_PLUGIN_PUSH_ERROR(tLuaFn.L, "%s(%p): failed to allocate %d bytes!", m_pcName, this, ulSize);
			fOk = false;
		}
		else
		{
			sizBuffer = ulSize;

			pcBuffer = pcBufferStart;
			lBytesProcessed = 0;
			do
			{
				sizChunk = ulSize;
				if( sizChunk>m_sizMaxPacketSizeClient-6)
				{
					sizChunk = m_sizMaxPacketSizeClient-6;
				}

				/* Get the next sequence number. */
				m_uiMonitorSequence = (m_uiMonitorSequence + 1) & (MONITOR_SEQUENCE_MSK>>MONITOR_SEQUENCE_SRT);

				aucCommand[0] = MONITOR_COMMAND_Read |
				                (MONITOR_ACCESSSIZE_Byte<<MONITOR_ACCESSSIZE_SRT) |
				                (uint8_t)(m_uiMonitorSequence << MONITOR_SEQUENCE_SRT);
				aucCommand[1] = (uint8_t)( sizChunk       & 0xffU);
				aucCommand[2] = (uint8_t)((sizChunk>> 8U) & 0xffU);
				aucCommand[3] = (uint8_t)( ulNetxAddress       & 0xffU);
				aucCommand[4] = (uint8_t)((ulNetxAddress>> 8U) & 0xffU);
				aucCommand[5] = (uint8_t)((ulNetxAddress>>16U) & 0xffU);
				aucCommand[6] = (uint8_t)((ulNetxAddress>>24U) & 0xffU);

				tResult = execute_command(aucCommand, 7);
				if( tResult!=UARTSTATUS_OK )
				{
					MUHKUH_PLUGIN_PUSH_ERROR(tLuaFn.L, "%s(%p): failed to execute command!", m_pcName, this);
					fOk = false;
					break;
				}
				else
				{
					if( m_sizPacketInputBuffer!=4+sizChunk+1 )
					{
						MUHKUH_PLUGIN_PUSH_ERROR(tLuaFn.L, "%s(%p): answer to read_image has wrong packet size of %d!", m_pcName, this, m_sizPacketInputBuffer);
						fOk = false;
						break;
					}
					else
					{
						memcpy(pcBuffer, m_aucPacketInputBuffer+3, sizChunk);
						pcBuffer += sizChunk;
						ulSize -= sizChunk;
						ulNetxAddress += sizChunk;
						lBytesProcessed += sizChunk;

						fIsRunning = callback_long(&tLuaFn, lBytesProcessed, lCallbackUserData);
						if( fIsRunning!=true )
						{
							MUHKUH_PLUGIN_PUSH_ERROR(tLuaFn.L, "%s(%p): read_image canceled!", m_pcName, this);
							fOk = false;
							break;
						}
					}
				}
			} while( ulSize!=0 );
		}
#endif
	}

	if( fOk == true )
	{
		*ppcBUFFER_OUT = pcBufferStart;
		*psizBUFFER_OUT = sizBuffer;
	}
	else
	{
		if ( pcBufferStart!=NULL) free(pcBufferStart); 
		MUHKUH_PLUGIN_EXIT_ERROR(tLuaFn.L);
	}
}



void romloader_dpm::write_data08(lua_State *ptClientData, uint32_t ulNetxAddress, uint8_t ucData)
{
	uint8_t aucCommand[8];
	uint32_t ulValue;
	bool fOk;


	/* Expect error. */
	fOk = false;

	ulValue = 0;

	if( m_fIsConnected==false )
	{
		MUHKUH_PLUGIN_PUSH_ERROR(ptClientData, "%s(%p): not connected!", m_pcName, this);
	}
	else
	{
#if 0
		/* Get the next sequence number. */
		m_uiMonitorSequence = (m_uiMonitorSequence + 1) & (MONITOR_SEQUENCE_MSK>>MONITOR_SEQUENCE_SRT);

		aucCommand[0] = MONITOR_COMMAND_Write |
		                (MONITOR_ACCESSSIZE_Byte<<MONITOR_ACCESSSIZE_SRT) |
		                (uint8_t)(m_uiMonitorSequence << MONITOR_SEQUENCE_SRT);
		aucCommand[1] = 1;
		aucCommand[2] = 0;
		aucCommand[3] = (uint8_t)( ulNetxAddress       & 0xffU);
		aucCommand[4] = (uint8_t)((ulNetxAddress>> 8U) & 0xffU);
		aucCommand[5] = (uint8_t)((ulNetxAddress>>16U) & 0xffU);
		aucCommand[6] = (uint8_t)((ulNetxAddress>>24U) & 0xffU);
		aucCommand[7] = ucData;
		tResult = execute_command(aucCommand, 8);
		if( tResult!=UARTSTATUS_OK )
		{
			MUHKUH_PLUGIN_PUSH_ERROR(ptClientData, "%s(%p): failed to execute command!", m_pcName, this);
		}
		else
		{
			if( m_sizPacketInputBuffer!=4+1 )
			{
				MUHKUH_PLUGIN_PUSH_ERROR(ptClientData, "%s(%p): answer to write_data08 has wrong packet size of %d!", m_pcName, this, m_sizPacketInputBuffer);
			}
			else
			{
				fOk = true;
			}
		}
#endif
	}

	if( fOk!=true )
	{
		MUHKUH_PLUGIN_EXIT_ERROR(ptClientData);
	}
}


void romloader_dpm::write_data16(lua_State *ptClientData, uint32_t ulNetxAddress, uint16_t usData)
{
	uint8_t aucCommand[9];
	uint32_t ulValue;
	bool fOk;


	/* Expect error. */
	fOk = false;

	ulValue = 0;

	if( m_fIsConnected==false )
	{
		MUHKUH_PLUGIN_PUSH_ERROR(ptClientData, "%s(%p): not connected!", m_pcName, this);
	}
	else
	{
#if 0
		/* Get the next sequence number. */
		m_uiMonitorSequence = (m_uiMonitorSequence + 1) & (MONITOR_SEQUENCE_MSK>>MONITOR_SEQUENCE_SRT);

		aucCommand[0] = MONITOR_COMMAND_Write |
		                (MONITOR_ACCESSSIZE_Word<<MONITOR_ACCESSSIZE_SRT) |
		                (uint8_t)(m_uiMonitorSequence << MONITOR_SEQUENCE_SRT);
		aucCommand[1] = 2;
		aucCommand[2] = 0;
		aucCommand[3] = (uint8_t)( ulNetxAddress       & 0xffU);
		aucCommand[4] = (uint8_t)((ulNetxAddress>> 8U) & 0xffU);
		aucCommand[5] = (uint8_t)((ulNetxAddress>>16U) & 0xffU);
		aucCommand[6] = (uint8_t)((ulNetxAddress>>24U) & 0xffU);
		aucCommand[7] = (uint8_t)( usData     & 0xffU);
		aucCommand[8] = (uint8_t)((usData>>8U)& 0xffU);
		tResult = execute_command(aucCommand, 9);
		if( tResult!=UARTSTATUS_OK )
		{
			MUHKUH_PLUGIN_PUSH_ERROR(ptClientData, "%s(%p): failed to execute command!", m_pcName, this);
		}
		else
		{
			if( m_sizPacketInputBuffer!=4+1 )
			{
				MUHKUH_PLUGIN_PUSH_ERROR(ptClientData, "%s(%p): answer to write_data16 has wrong packet size of %d!", m_pcName, this, m_sizPacketInputBuffer);
			}
			else
			{
				fOk = true;
			}
		}
#endif
	}

	if( fOk!=true )
	{
		MUHKUH_PLUGIN_EXIT_ERROR(ptClientData);
	}
}



void romloader_dpm::write_data32(lua_State *ptClientData, uint32_t ulNetxAddress, uint32_t ulData)
{
	uint8_t aucCommand[11];
	uint32_t ulValue;
	bool fOk;


	/* Expect error. */
	fOk = false;

	ulValue = 0;

	if( m_fIsConnected==false )
	{
		MUHKUH_PLUGIN_PUSH_ERROR(ptClientData, "%s(%p): not connected!", m_pcName, this);
	}
	else
	{
#if 0
		/* Get the next sequence number. */
		m_uiMonitorSequence = (m_uiMonitorSequence + 1) & (MONITOR_SEQUENCE_MSK>>MONITOR_SEQUENCE_SRT);

		aucCommand[0]  = MONITOR_COMMAND_Write |
		                 (MONITOR_ACCESSSIZE_Long<<MONITOR_ACCESSSIZE_SRT) |
		                 (uint8_t)(m_uiMonitorSequence << MONITOR_SEQUENCE_SRT);
		aucCommand[1]  = 4;
		aucCommand[2]  = 0;
		aucCommand[3]  = (uint8_t)( ulNetxAddress       & 0xffU);
		aucCommand[4]  = (uint8_t)((ulNetxAddress>> 8U) & 0xffU);
		aucCommand[5]  = (uint8_t)((ulNetxAddress>>16U) & 0xffU);
		aucCommand[6]  = (uint8_t)((ulNetxAddress>>24U) & 0xffU);
		aucCommand[7]  = (uint8_t)( ulData       & 0xffU);
		aucCommand[8]  = (uint8_t)((ulData>> 8U) & 0xffU);
		aucCommand[9]  = (uint8_t)((ulData>>16U) & 0xffU);
		aucCommand[10] = (uint8_t)((ulData>>24U) & 0xffU);
		tResult = execute_command(aucCommand, 11);
		if( tResult!=UARTSTATUS_OK )
		{
			MUHKUH_PLUGIN_PUSH_ERROR(ptClientData, "%s(%p): failed to execute command!", m_pcName, this);
		}
		else
		{
			if( m_sizPacketInputBuffer!=4+1 )
			{
				MUHKUH_PLUGIN_PUSH_ERROR(ptClientData, "%s(%p): answer to write_data32 has wrong packet size of %d!", m_pcName, this, m_sizPacketInputBuffer);
			}
			else
			{
				fOk = true;
			}
		}
#endif
	}

	if( fOk!=true )
	{
		MUHKUH_PLUGIN_EXIT_ERROR(ptClientData);
	}
}



void romloader_dpm::write_image(uint32_t ulNetxAddress, const char *pcBUFFER_IN, size_t sizBUFFER_IN, SWIGLUA_REF tLuaFn, long lCallbackUserData)
{
	bool fOk;
	size_t sizChunk;
	uint8_t aucCommand[m_sizMaxPacketSizeHost];
	bool fIsRunning;
	long lBytesProcessed;


	/* Be optimistic. */
	fOk = true;

	if( m_fIsConnected==false )
	{
		MUHKUH_PLUGIN_PUSH_ERROR(tLuaFn.L, "%s(%p): not connected!", m_pcName, this);
		fOk = false;
	}
	else if( sizBUFFER_IN!=0 )
	{
#if 0
		lBytesProcessed = 0;
		do
		{
			sizChunk = sizBUFFER_IN;
			if( sizChunk>m_sizMaxPacketSizeClient-12 )
			{
				sizChunk = m_sizMaxPacketSizeClient-12;
			}

			/* Get the next sequence number. */
			m_uiMonitorSequence = (m_uiMonitorSequence + 1) & (MONITOR_SEQUENCE_MSK>>MONITOR_SEQUENCE_SRT);

			aucCommand[0] = MONITOR_COMMAND_Write |
			                (MONITOR_ACCESSSIZE_Byte<<MONITOR_ACCESSSIZE_SRT) |
			                (uint8_t)(m_uiMonitorSequence << MONITOR_SEQUENCE_SRT);
			aucCommand[1] = (uint8_t)( sizChunk       & 0xffU);
			aucCommand[2] = (uint8_t)((sizChunk>> 8U) & 0xffU);
			aucCommand[3] = (uint8_t)( ulNetxAddress       & 0xffU);
			aucCommand[4] = (uint8_t)((ulNetxAddress>> 8U) & 0xffU);
			aucCommand[5] = (uint8_t)((ulNetxAddress>>16U) & 0xffU);
			aucCommand[6] = (uint8_t)((ulNetxAddress>>24U) & 0xffU);
			memcpy(aucCommand+7, pcBUFFER_IN, sizChunk);
			tResult = execute_command(aucCommand, sizChunk+7);
			if( tResult!=UARTSTATUS_OK )
			{
				MUHKUH_PLUGIN_PUSH_ERROR(tLuaFn.L, "%s(%p): failed to execute command!", m_pcName, this);
				fOk = false;
				break;
			}
			else
			{
				if( m_sizPacketInputBuffer!=4+1 )
				{
					MUHKUH_PLUGIN_PUSH_ERROR(tLuaFn.L, "%s(%p): answer to write_data08 has wrong packet size of %d!", m_pcName, this, m_sizPacketInputBuffer);
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
						MUHKUH_PLUGIN_PUSH_ERROR(tLuaFn.L, "%s(%p): write_image canceled!", m_pcName, this);
						fOk = false;
						break;
					}
				}
			}
		} while( sizBUFFER_IN!=0 );
#endif
	}

	if( fOk!=true )
	{
		MUHKUH_PLUGIN_EXIT_ERROR(tLuaFn.L);
	}
}



void romloader_dpm::call(uint32_t ulNetxAddress, uint32_t ulParameterR0, SWIGLUA_REF tLuaFn, long lCallbackUserData)
{
	bool fOk;
	uint8_t aucCommand[9];
	const uint8_t aucCancelBuf[1] = { 0x2b };
	uint8_t ucStatus;
	bool fIsRunning;
	char *pcProgressData;
	size_t sizProgressData;


	if( m_fIsConnected==false )
	{
		MUHKUH_PLUGIN_PUSH_ERROR(tLuaFn.L, "%s(%p): not connected!", m_pcName, this);
		fOk = false;
	}
	else
	{
#if 0
		/* Get the next sequence number. */
		m_uiMonitorSequence = (m_uiMonitorSequence + 1) & (MONITOR_SEQUENCE_MSK>>MONITOR_SEQUENCE_SRT);

		/* Construct the command packet. */
		aucCommand[0x00] = MONITOR_COMMAND_Execute |
		                   (uint8_t)(m_uiMonitorSequence << MONITOR_SEQUENCE_SRT);
		aucCommand[0x01] = (uint8_t)( ulNetxAddress      & 0xffU);
		aucCommand[0x02] = (uint8_t)((ulNetxAddress>>8 ) & 0xffU);
		aucCommand[0x03] = (uint8_t)((ulNetxAddress>>16) & 0xffU);
		aucCommand[0x04] = (uint8_t)((ulNetxAddress>>24) & 0xffU);
		aucCommand[0x05] = (uint8_t)( ulParameterR0      & 0xffU);
		aucCommand[0x06] = (uint8_t)((ulParameterR0>>8 ) & 0xffU);
		aucCommand[0x07] = (uint8_t)((ulParameterR0>>16) & 0xffU);
		aucCommand[0x08] = (uint8_t)((ulParameterR0>>24) & 0xffU);

//		printf("Executing call command:\n");
//		hexdump(aucCommand, 9);
		tResult = execute_command(aucCommand, 9);
		if( tResult!=UARTSTATUS_OK )
		{
			MUHKUH_PLUGIN_PUSH_ERROR(tLuaFn.L, "%s(%p): failed to execute command!", m_pcName, this);
			fOk = false;
		}
		else
		{
			if( m_sizPacketInputBuffer!=4+1 )
			{
				MUHKUH_PLUGIN_PUSH_ERROR(tLuaFn.L, "%s(%p): answer to call command has wrong packet size of %d!", m_pcName, this, m_sizPacketInputBuffer);
				fOk = false;
			}
			else
			{
				/* Get the next sequence number. */
				//m_uiMonitorSequence = (m_uiMonitorSequence + 1) & (MONITOR_SEQUENCE_MSK>>MONITOR_SEQUENCE_SRT);

				/* Receive message packets. */
				while(1)
				{
					pcProgressData = NULL;
					sizProgressData = 0;

					tResult = receive_packet();
					if( tResult==UARTSTATUS_TIMEOUT )
					{
						/* Do nothing in case of timeout. The application is just running quietly. */
					}
					else if( tResult!=UARTSTATUS_OK )
					{
						MUHKUH_PLUGIN_PUSH_ERROR(tLuaFn.L, "%s(%p): Failed to receive a packet: %d", m_pcName, this, tResult);
						fOk = false;
					}
					else
					{
						if( m_sizPacketInputBuffer<5 )
						{
							MUHKUH_PLUGIN_PUSH_ERROR(tLuaFn.L, "%s(%p): Received a packet without any user data!", m_pcName, this);
							fOk = false;
						}
						else
						{
							ucStatus = m_aucPacketInputBuffer[2] & MONITOR_STATUS_MSK;
							if( ucStatus==MONITOR_STATUS_CallMessage )
							{
								pcProgressData = (char*)m_aucPacketInputBuffer+3;
								sizProgressData = m_sizPacketInputBuffer-5;
							}
							else if( ucStatus==MONITOR_STATUS_CallFinished )
							{
								fprintf(stderr, "%s(%p): Call has finished!", m_pcName, this);
								fOk = true;
								break;
							}
						}
					}

					if (pcProgressData != NULL)
					{
						fIsRunning = callback_string(&tLuaFn, pcProgressData, sizProgressData, lCallbackUserData);
						if( fIsRunning!=true )
						{
							/* Send a cancel request to the device. */
							tResult = send_packet(aucCancelBuf, 1);

							MUHKUH_PLUGIN_PUSH_ERROR(tLuaFn.L, "%s(%p): the call was canceled!", m_pcName, this);
							fOk = false;
							break;
						}
					}

				}
			}
		}
#endif
	}

	if( fOk!=true )
	{
		MUHKUH_PLUGIN_EXIT_ERROR(tLuaFn.L);
	}
}


/*-------------------------------------*/


romloader_dpm_reference::romloader_dpm_reference(void)
 : muhkuh_plugin_reference()
{
}


romloader_dpm_reference::romloader_dpm_reference(const char *pcName, const char *pcTyp, bool fIsUsed, romloader_dpm_provider *ptProvider)
 : muhkuh_plugin_reference(pcName, pcTyp, fIsUsed, ptProvider)
{
}


romloader_dpm_reference::romloader_dpm_reference(const romloader_dpm_reference *ptCloneMe)
 : muhkuh_plugin_reference(ptCloneMe)
{
}


/*-------------------------------------*/

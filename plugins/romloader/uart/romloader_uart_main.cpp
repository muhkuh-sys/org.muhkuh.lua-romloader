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

#include "romloader_uart_read_functinoid_aboot.h"
#include "romloader_uart_read_functinoid_hboot1.h"
#include "romloader_uart_read_functinoid_mi1.h"

#define UART_BASE_TIMEOUT_MS 500
#define UART_CHAR_TIMEOUT_MS 50

#if defined(_MSC_VER)
#       define snprintf _snprintf
#endif


/*-------------------------------------*/

/* Endianness macros translating 16 and 32bit from the host to the netX byte order.
 * FIXME: Detect the host endianness. More infos here:
 *          http://www.boost.org/doc/libs/1_43_0/boost/detail/endian.hpp
 *          https://cmake.org/cmake/help/v3.5/module/TestBigEndian.html
 */
#define HTONETX16(a) (a)
#define HTONETX32(a) (a)

#define NETXTOH16(a) (a)
#define NETXTOH32(a) (a)


struct MIV3_PACKET_COMMAND_READWRITE_DATA_STRUCT
{
	uint8_t ucPacketType;
	uint16_t usDataSize;
	uint32_t ulAddress;
} __attribute__ ((packed));

typedef union MIV3_PACKET_COMMAND_READWRITE_DATA_UNION
{
	struct MIV3_PACKET_COMMAND_READWRITE_DATA_STRUCT s;
	uint8_t auc[7];
} __attribute__ ((packed)) MIV3_PACKET_COMMAND_READWRITE_DATA_T;


struct MIV3_PACKET_COMMAND_CALL_STRUCT
{
	uint8_t ucPacketType;
	uint32_t ulAddress;
	uint32_t ulR0;
} __attribute__ ((packed));

typedef union MIV3_PACKET_COMMAND_CALL_UNION
{
	struct MIV3_PACKET_COMMAND_CALL_STRUCT s;
	uint8_t auc[7];
} __attribute__ ((packed)) MIV3_PACKET_COMMAND_CALL_T;


/*-------------------------------------*/

const char *romloader_uart_provider::m_pcPluginNamePattern = "romloader_uart_%s";

romloader_uart_provider::romloader_uart_provider(swig_type_info *p_romloader_uart, swig_type_info *p_romloader_uart_reference)
 : muhkuh_plugin_provider("romloader_uart")
{
	/* get the romloader_uart lua type */
	m_ptPluginTypeInfo = p_romloader_uart;
	m_ptReferenceTypeInfo = p_romloader_uart_reference;
}



romloader_uart_provider::~romloader_uart_provider(void)
{
}



int romloader_uart_provider::DetectInterfaces(lua_State *ptLuaStateForTableAccess)
{
	size_t sizDeviceNames;
	char **ppcDeviceNames;
	char **ppcDeviceNamesCnt;
	char **ppcDeviceNamesEnd;
	romloader_uart_reference *ptReference;
	bool fDeviceIsBusy;


	/* detect all interfaces */
	sizDeviceNames = romloader_uart_device_platform::ScanForPorts(&ppcDeviceNames);
	//printf("found %d devs, %p\n", sizDeviceNames, ppcDeviceNames);

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
			//printf("create instance '%s'\n", *ppcDeviceNamesCnt);
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
//			printf("%s(%p): claim_interface(): claimed interface %s.\n", m_pcPluginId, this, pcName);
		}
	}

	return ptPlugin;
}



bool romloader_uart_provider::ReleaseInterface(muhkuh_plugin *ptPlugin)
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


romloader_uart::romloader_uart(const char *pcName, const char *pcTyp, romloader_uart_provider *ptProvider, const char *pcDeviceName)
 : romloader(pcName, pcTyp, ptProvider)
 , m_ptUartDev(NULL)
{
	m_ptUartDev = new romloader_uart_device_platform(pcDeviceName);

	m_ucMonitorSequence = 0;

	packet_ringbuffer_init();
}


romloader_uart::~romloader_uart(void)
{
	if( m_ptUartDev!=NULL )
	{
		m_ptUartDev->Close();
		delete m_ptUartDev;
	}
}


void romloader_uart::hexdump(const uint8_t *pucData, uint32_t ulSize)
{
	const uint8_t *pucDumpCnt, *pucDumpEnd;
	unsigned long ulAddressCnt;
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


bool romloader_uart::identify_loader(ROMLOADER_COMMANDSET_T *ptCmdSet)
{
	bool fResult = false;
	const uint8_t aucKnock[5] = { '*', 0x00, 0x00, '*', '#' };
	const uint8_t aucMagicMooh[4] = { 0x4d, 0x4f, 0x4f, 0x48 };
	size_t sizCnt;
	size_t sizTransfered;
	uint32_t ulMiVersionMaj;
	uint32_t ulMiVersionMin;
	ROMLOADER_COMMANDSET_T tCmdSet;
	uint16_t usCrc;
	size_t sizPacket;
	uint8_t aucData[32];
	uint8_t ucStatus;


	/* The command set is unknown by default. */
	tCmdSet = ROMLOADER_COMMANDSET_UNKNOWN;

	/* Send knock sequence with 500 milliseconds timeout. */
	if( m_ptUartDev->SendRaw(aucKnock, sizeof(aucKnock), 500)!=sizeof(aucKnock) )
	{
		/* Failed to send knock sequence to device. */
		fprintf(stderr, "Failed to send knock sequence to device.\n");
	}
	else if( m_ptUartDev->Flush()!=true )
	{
		/* Failed to flush the knock sequence. */
		fprintf(stderr, "Failed to flush the knock sequence.\n");
	}
	else
	{
		sizTransfered = m_ptUartDev->RecvRaw(aucData, 1, 1000);
		if( sizTransfered!=1 )
		{
			/* Failed to receive first char of knock response. */
			fprintf(stderr, "Failed to receive first char of knock response: %ld.\n", sizTransfered);
		}
		else
		{
//			printf("received knock response: 0x%02x\n", aucData[0]);
			/* Knock echoed -> this is the prompt or the machine interface. */
			if( aucData[0]==MONITOR_STREAM_PACKET_START )
			{
				sizTransfered = m_ptUartDev->RecvRaw(aucData, 2, 500);
				if( sizTransfered!=2 )
				{
					fprintf(stderr, "Failed to receive the size information after the stream packet start!\n");
				}
				else
				{
					if( aucData[0]=='*' && aucData[1]=='#' )
					{
						printf("OK, received '*#'!\n");
						fResult = m_ptUartDev->SendBlankLineAndDiscardResponse();
						if( fResult==true )
						{
							tCmdSet = ROMLOADER_COMMANDSET_ABOOT_OR_HBOOT1;
						}
					}
					else if( aucData[0]==0x00 && aucData[1]==0x00 )
					{
						sizTransfered = m_ptUartDev->RecvRaw(aucData+2, 2, 500);
						if( sizTransfered!=2 )
						{
							fprintf(stderr, "Failed to receive the rest of the knock response after 0x00 0x00!\n");
						}
						else if( aucData[2]=='*' && aucData[3]=='#' )
						{
							printf("OK, received '<null><null>*#'!\n");
							fResult = m_ptUartDev->SendBlankLineAndDiscardResponse();
							if( fResult==true )
							{
								tCmdSet = ROMLOADER_COMMANDSET_ABOOT_OR_HBOOT1;
							}
						}
						else
						{
							fprintf(stderr, "Received strange response after 0x00 0x00:\n");
							hexdump(aucData+2, 2);
						}
					}
					else
					{
						/* Get the size of the data part. */
						sizPacket  = ((size_t)aucData[0]);
						sizPacket |= ((size_t)aucData[1]) << 8U;
						
						/* The size information does not include the CRC. Add the 2 bytes here. */
						sizPacket += 2;
						
						/* Is the size OK? */
						if( sizPacket<11 )
						{
							fprintf(stderr, "The received packet is too small. It must be at least 11 bytes, but it is %ld bytes.\n", sizPacket);
							hexdump(aucData, sizPacket);
						}
						else if( sizPacket>(sizeof(aucData)-2) )
						{
							fprintf(stderr, "The received packet is too big for a buffer of %ld bytes. It has %ld bytes.\n", sizeof(aucData)-2, sizPacket);
						}
						else
						{
							/* Get rest of the response. */
							sizTransfered = m_ptUartDev->RecvRaw(aucData+2, sizPacket, 500);
							if( sizTransfered!=sizPacket )
							{
								fprintf(stderr, "Failed to receive the rest of the packet after the size information!\n");
							}
							else
							{
								ucStatus = aucData[3];
								if( ucStatus!=MONITOR_STATUS_Ok )
								{
									fprintf(stderr, "The status of the response is not OK but 0x%02x.\n", ucStatus);
								}
								else
								{
									/* The complete size of the packet includes the 2 bytes of size information. */
									sizPacket += 2;
									
									/* Build the CRC for the packet. */
									usCrc = 0;
									for(sizCnt=0; sizCnt<sizPacket; ++sizCnt)
									{
										usCrc = crc16(usCrc, aucData[sizCnt]);
									}
									if( usCrc!=0 )
									{
										fprintf(stderr, "CRC of version packet is invalid!\n");
										hexdump(aucData, sizPacket);
									}
									else
									{
										if( memcmp(aucData+3, aucMagicMooh, sizeof(aucMagicMooh))==0 )
										{
											hexdump(aucData, sizPacket);
											ulMiVersionMin = (uint32_t)(aucData[7] | (aucData[8]<<8));
											ulMiVersionMaj = (uint32_t)(aucData[9] | (aucData[10]<<8));
											printf("Found new machine interface V%d.%d.\n", ulMiVersionMaj, ulMiVersionMin);
											
											if( ulMiVersionMaj==1 )
											{
												tCmdSet = ROMLOADER_COMMANDSET_MI1;
												fResult = true;
											}
											else if( ulMiVersionMaj==2 )
											{
												tCmdSet = ROMLOADER_COMMANDSET_MI2;
												fResult = true;
											}
/* This is an invalid combination.
											else if( ulMiVersionMaj==3 )
											{
												tCmdSet = ROMLOADER_COMMANDSET_MI3;
												fResult = true;
											}
*/
											else
											{
												fprintf(stderr, "Unknown machine interface version %d.%d\n", ulMiVersionMaj, ulMiVersionMin);
											}
										}
										else if( memcmp(aucData+4, aucMagicMooh, sizeof(aucMagicMooh))==0 )
										{
											ulMiVersionMin = (uint32_t)(aucData[8] | (aucData[9]<<8));
											ulMiVersionMaj = (uint32_t)(aucData[10] | (aucData[11]<<8));
											printf("Found new machine interface V%d.%d.\n", ulMiVersionMaj, ulMiVersionMin);

/* These combinations are invald.
											if( ulMiVersionMaj==1 )
											{
												tCmdSet = ROMLOADER_COMMANDSET_MI1;
												fResult = true;
											}
											else if( ulMiVersionMaj==2 )
											{
												tCmdSet = ROMLOADER_COMMANDSET_MI2;
												fResult = true;
											}
											else 
*/
											if( ulMiVersionMaj==3 )
											{
												tCmdSet = ROMLOADER_COMMANDSET_MI3;
												fResult = true;
											}
											else
											{
												fprintf(stderr, "Unknown machine interface version %d.%d\n", ulMiVersionMaj, ulMiVersionMin);
											}
										}
										else
										{
											fprintf(stderr, "The response packet has no MOOH magic!\n");
										}
									}
								}
							}
						}
					}
				}
			}
			else if( aucData[0]=='#' )
			{
				printf("OK, received hash!\n");

				tCmdSet = ROMLOADER_COMMANDSET_ABOOT_OR_HBOOT1;
				fResult = true;
			}
			else
			{
				/* This seems to be the welcome message. */

				/* The welcome message can be quite trashed depending on the driver. Just discard the characters until the first timeout and send enter. */

				/* Discard all data until timeout. */
				do
				{
					sizTransfered = m_ptUartDev->RecvRaw(aucData, 1, 200);
				} while( sizTransfered==1 );

				fResult = m_ptUartDev->SendBlankLineAndDiscardResponse();
				if( fResult==true )
				{
					tCmdSet = ROMLOADER_COMMANDSET_ABOOT_OR_HBOOT1;
				}
			}
		}
	}

	*ptCmdSet = tCmdSet;
	return fResult;
}



romloader::TRANSPORTSTATUS_T romloader_uart::send_sync_command(void)
{
	const uint8_t aucKnock[5] = { '*', 0x00, 0x00, '*', '#' };
	size_t sizSend;
	TRANSPORTSTATUS_T tResult;
	uint8_t ucStatus;
	unsigned int uiRetryCnt;


	uiRetryCnt = 10;
	do
	{
		sizSend = m_ptUartDev->SendRaw(aucKnock, sizeof(aucKnock), UART_BASE_TIMEOUT_MS + sizeof(aucKnock)*UART_CHAR_TIMEOUT_MS);
		if( sizSend!=sizeof(aucKnock) )
		{
			fprintf(stderr, "! send_sync_command: failed to send the packet!\n");
			tResult = TRANSPORTSTATUS_SEND_FAILED;
		}
		else
		{
			tResult = receive_packet();
			if( tResult!=TRANSPORTSTATUS_OK )
			{
				fprintf(stderr, "! send_sync_command: receive_packet failed with error code %d\n", tResult);
			}
			else
			{
				if( m_sizPacketInputBuffer<5 )
				{
					fprintf(stderr, "! send_sync_command: received no user data!\n");
					tResult = TRANSPORTSTATUS_MISSING_USERDATA;
				}
				else
				{
					ucStatus = m_aucPacketInputBuffer[3];
					if( ucStatus==0 )
					{
						tResult = TRANSPORTSTATUS_OK;
					}
					else
					{
						fprintf(stderr, "! send_sync_command: status is not OK: %d\n", ucStatus);
						tResult = TRANSPORTSTATUS_COMMAND_EXECUTION_FAILED;
					}
				}
			}
		}

		if( tResult!=TRANSPORTSTATUS_OK )
		{
			--uiRetryCnt;
			if( uiRetryCnt==0 )
			{
				fprintf(stderr, "! send_sync_command: Retried 10 times and nothing happened. Giving up!\n");
				break;
			}
			else
			{
				fprintf(stderr, "***************************************\n");
				fprintf(stderr, "*                                     *\n");
				fprintf(stderr, "*            retry                    *\n");
				fprintf(stderr, "*                                     *\n");
				fprintf(stderr, "***************************************\n");
			}
		}

	} while( tResult!=TRANSPORTSTATUS_OK );

	return tResult;
}



bool romloader_uart::synchronize(void)
{
	const uint8_t aucMagicMooh[4] = { 0x4d, 0x4f, 0x4f, 0x48 };
	TRANSPORTSTATUS_T tResult;
	bool fResult;


	/* The expected size of a V3 knock response is 17 bytes:
	 *    2 size bytes
	 *    1 Current sequence number
	 *    1 status byte
	 *    4 MOOH magic
	 *    2 MI version minor
	 *    2 MI version major
	 *    1 Chip type
	 *    2 Maximum packet size
	 *    2 bytes CRC
	 */
	const size_t sizExpectedResponse = 17;
	uint8_t ucSequence;
	uint32_t ulMiVersionMin;
	uint32_t ulMiVersionMaj;
	ROMLOADER_CHIPTYP tChipType;
	size_t sizMaxPacketSize;


	fprintf(stderr, "synchronize\n");
	fResult = false;

	tResult = send_sync_command();
	if( tResult!=TRANSPORTSTATUS_OK )
	{
		/* Failed to send knock sequence to device. */
		fprintf(stderr, "Failed to send knock sequence to device.\n");
	}
	else if( m_sizPacketInputBuffer==sizExpectedResponse && memcmp(m_aucPacketInputBuffer+4, aucMagicMooh, sizeof(aucMagicMooh))==0 )
	{
		fprintf(stderr, "Packet:\n");
		hexdump(m_aucPacketInputBuffer, m_sizPacketInputBuffer);

		/* Get the sequence number. */
		ucSequence = m_aucPacketInputBuffer[0x02];
		fprintf(stderr, "Sequence number: 0x%02x\n", ucSequence);

		ulMiVersionMin =  ((uint32_t)(m_aucPacketInputBuffer[0x09])) |
		                 (((uint32_t)(m_aucPacketInputBuffer[0x08]))<<8U);
		ulMiVersionMaj =  ((uint32_t)(m_aucPacketInputBuffer[0x0a])) |
		                 (((uint32_t)(m_aucPacketInputBuffer[0x0b]))<<8U);
		printf("Machine interface V%d.%d.\n", ulMiVersionMaj, ulMiVersionMin);

		tChipType = (ROMLOADER_CHIPTYP)(m_aucPacketInputBuffer[0x0c]);
		printf("Chip type : %d\n", tChipType);

		/* sizMaxPacketSizeClient = min(max. packet size from response, sizMaxPacketSizeHost) */
		sizMaxPacketSize =  ((size_t)(m_aucPacketInputBuffer[0x0d])) |
		                   (((size_t)(m_aucPacketInputBuffer[0x0e]))<<8U);
		printf("Maximum packet size: 0x%04lx\n", sizMaxPacketSize);
		/* Limit the packet size to the buffer size. */
		if( sizMaxPacketSize>sizMaxPacketSizeHost )
		{
			sizMaxPacketSize = sizMaxPacketSizeHost;
			printf("Limit maximum packet size to 0x%04lx\n", sizMaxPacketSize);
		}

		/* Set the new values. */
		m_ucMonitorSequence = ucSequence;
		m_tChiptyp = tChipType;
		m_sizMaxPacketSizeClient = sizMaxPacketSize;

		fResult = true;
	}
	else
	{
		fprintf(stderr, "Received an invalid knock response.\n");
		hexdump(m_aucPacketInputBuffer, m_sizPacketInputBuffer);

		/* TODO: Send a "Cancel Operation" packet here and retry. */
	}

	return fResult;
}



void romloader_uart::Connect(lua_State *ptClientData)
{
	romloader_uart_read_functinoid *ptFn;
	romloader_uart_read_functinoid_aboot tFnABoot(m_ptUartDev, m_pcName);
	romloader_uart_read_functinoid_hboot1 tFnHBoot1(m_ptUartDev, m_pcName);
	romloader_uart_read_functinoid_mi1 tFnMi1(m_ptUartDev, m_pcName);
	bool fResult;
	ROMLOADER_COMMANDSET_T tCmdSet;
	int iResult;


	/* Expect error. */
	fResult = false;
	ptFn = NULL;

//	printf("%s(%p): connect\n", m_pcName, this);

	if( m_ptUartDev!=NULL && m_fIsConnected==false )
	{
		// detect_chip type and chip_init call read/write_data which require m_fIsConnected == true
		m_fIsConnected = true;

		fResult = m_ptUartDev->Open();
		if( fResult!=true )
		{
			MUHKUH_PLUGIN_PUSH_ERROR(ptClientData, "%s(%p): failed to open device!", m_pcName, this);
		}
		else
		{
			fResult = identify_loader(&tCmdSet);
			if( fResult!=true )
			{
				MUHKUH_PLUGIN_PUSH_ERROR(ptClientData, "%s(%p): failed to identify loader!", m_pcName, this);
			}
			else
			{
				switch(tCmdSet)
				{
				case ROMLOADER_COMMANDSET_UNKNOWN:
					fprintf(stderr, "Unknown command set.\n");
					fResult = false;
					break;


				case ROMLOADER_COMMANDSET_ABOOT_OR_HBOOT1:
					fprintf(stderr, "ABOOT or HBOOT1.\n");
					/* Try to detect the chip type with the old command set ("DUMP"). */
					ptFn = &tFnABoot;
					fResult = detect_chiptyp(ptFn);
					if( fResult!=true )
					{
						/* Failed to get the info with the old command set. Now try the new command ("D"). */
						ptFn = &tFnHBoot1;
						fResult = detect_chiptyp(ptFn);
						if( fResult!=true )
						{
							MUHKUH_PLUGIN_PUSH_ERROR(ptClientData, "%s(%p): failed to detect chip type!", m_pcName, this);
						}
					}
					
					if( fResult==true && ptFn!=NULL )
					{
						iResult = ptFn->update_device(m_tChiptyp);
						if( iResult!=0 )
						{
							fResult = false;
							MUHKUH_PLUGIN_PUSH_ERROR(ptClientData, "%s(%p): failed to update the device!", m_pcName, this);
						}
					}
					break;


				case ROMLOADER_COMMANDSET_MI1:
					fprintf(stderr, "Command set MI1.\n");
					/* Try to detect the chip type with the old MI command set. */
					ptFn = &tFnMi1;
					fResult = detect_chiptyp(ptFn);
					if( fResult==true && ptFn!=NULL )
					{
						iResult = ptFn->update_device(m_tChiptyp);
						if( iResult!=0 )
						{
							fResult = false;
							MUHKUH_PLUGIN_PUSH_ERROR(ptClientData, "%s(%p): failed to update the device!", m_pcName, this);
						}
					}
					else
					{
						MUHKUH_PLUGIN_PUSH_ERROR(ptClientData, "%s(%p): failed to detect chip type!", m_pcName, this);
					}
					break;


				case ROMLOADER_COMMANDSET_MI2:
					fprintf(stderr, "TODO: Update the V2 device to V3.\n");
					fResult = false;
					break;


				case ROMLOADER_COMMANDSET_MI3:
					fprintf(stderr, "The device does not need an update.\n");
					fResult = true;
					break;
				}

				if( fResult==true )
				{
					/* Synchronize with the client to get these settings:
					 *   chip type
					 *   sequence number
					 *   maximum packet size
					 */
					fResult = synchronize();
					if( fResult!=true )
					{
						MUHKUH_PLUGIN_PUSH_ERROR(ptClientData, "%s(%p): failed to synchronize with the client!", m_pcName, this);
					}
				}
			}
		}

		if( fResult!=true )
		{
			m_fIsConnected = false;
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
		/* Flush all waiting data. */
		m_ptUartDev->Flush();

		/* Close the UART device. */
		m_ptUartDev->Close();
	}

	m_fIsConnected = false;
}



void romloader_uart::packet_ringbuffer_init(void)
{
	m_sizPacketRingBufferHead = 0;
	m_sizPacketRingBufferFill = 0;
}



romloader::TRANSPORTSTATUS_T romloader_uart::packet_ringbuffer_fill(size_t sizRequestedFillLevel)
{
	size_t sizWritePosition;
	TRANSPORTSTATUS_T tResult;
	size_t sizReceiveCnt;
	size_t sizChunk;
	size_t sizRead;


	/* Expect success. */
	tResult = TRANSPORTSTATUS_OK;

	/* Does the buffer contain enough data? */
	if( m_sizPacketRingBufferFill<sizRequestedFillLevel )
	{
		/* No -> receive the remaining amount of bytes. */
		sizReceiveCnt = sizRequestedFillLevel - m_sizPacketRingBufferFill;
		do
		{
			/* Get the write position. */
			sizWritePosition = m_sizPacketRingBufferHead + m_sizPacketRingBufferFill;
			if( sizWritePosition>=sizMaxPacketSizeHost )
			{
				sizWritePosition -= sizMaxPacketSizeHost;
			}

			/* Get the size of the remaining continuous buffer space. */
			/* This is the maximum chunk which can be read in one piece. */
			sizChunk = sizMaxPacketSizeHost - sizWritePosition;
			/* Limit the chunk size to the requested size. */
			if( sizChunk>sizReceiveCnt )
			{
				sizChunk = sizReceiveCnt;
			}

			/* Receive the chunk. */
			sizRead = m_ptUartDev->RecvRaw(m_aucPacketRingBuffer+sizWritePosition, sizChunk, UART_BASE_TIMEOUT_MS + sizChunk*UART_CHAR_TIMEOUT_MS);

			m_sizPacketRingBufferFill += sizRead;
			sizReceiveCnt -= sizRead;

			if( sizRead!=sizChunk )
			{
//				fprintf(stderr, "TIMEOUT: requested %d bytes, got only %d.\n", sizChunk, sizRead);
				tResult = TRANSPORTSTATUS_TIMEOUT;
				break;
			}
		} while( sizReceiveCnt>0 );
	}

	return tResult;
}



uint8_t romloader_uart::packet_ringbuffer_get(void)
{
	uint8_t ucByte;


	ucByte = m_aucPacketRingBuffer[m_sizPacketRingBufferHead];

	++m_sizPacketRingBufferHead;
	if( m_sizPacketRingBufferHead>=sizMaxPacketSizeHost )
	{
		m_sizPacketRingBufferHead -= sizMaxPacketSizeHost;
	}

	--m_sizPacketRingBufferFill;

	return ucByte;
}



int romloader_uart::packet_ringbuffer_peek(size_t sizOffset)
{
	size_t sizReadPosition;


	sizReadPosition = m_sizPacketRingBufferHead + sizOffset;
	if( sizReadPosition>=sizMaxPacketSizeHost )
	{
		sizReadPosition -= sizMaxPacketSizeHost;
	}

	return m_aucPacketRingBuffer[sizReadPosition];
}



void romloader_uart::packet_ringbuffer_discard(void)
{
	if( m_sizPacketRingBufferFill>0 )
	{
		printf("Warning: discarding %ld bytes in ringbuffer!\n", m_sizPacketRingBufferFill);
	}
	packet_ringbuffer_init();
}



romloader::TRANSPORTSTATUS_T romloader_uart::send_packet(const uint8_t *pucData, size_t sizData)
{
	TRANSPORTSTATUS_T tResult;
	uint8_t aucPacketBuffer[sizMaxPacketSizeHost];
	uint16_t usCrc;
	const uint8_t *pucCnt;
	const uint8_t *pucEnd;
	size_t sizSend;
	size_t sizPacket;
	size_t sizMaxUserData;


	/* The maximum packet size is reduced by...
	 *   the start char (1 byte),
	 *   the size information (2 bytes)
	 *   the sequence number (1 byte)
	 *   and the CRC (2 bytes).
	 */
	sizMaxUserData = m_sizMaxPacketSizeClient - 6U;
	if( sizData<=sizMaxUserData )
	{
		/* Set the packet start character. */
		aucPacketBuffer[0] = MONITOR_STREAM_PACKET_START;
		/* Set the size of the user data.
		 * Add 1 byte for the sequence number.
		 */
		++sizData;
		aucPacketBuffer[1] = (uint8_t)( sizData        & 0xffU);
		aucPacketBuffer[2] = (uint8_t)((sizData >> 8U) & 0xffU);

		/* Copy the sequence number. */
		aucPacketBuffer[3] = m_ucMonitorSequence;
		--sizData;

		/* Copy the user data. */
		memcpy(aucPacketBuffer+4, pucData, sizData);

		/* Generate the CRC. */
		usCrc = 0;
		pucCnt = aucPacketBuffer + 1;
		pucEnd = aucPacketBuffer + 1 + 2 + 1 + sizData;
		while( pucCnt<pucEnd )
		{
			usCrc = crc16(usCrc, *pucCnt);
			++pucCnt;
		}

		/* Append the CRC. */
		aucPacketBuffer[sizData+4] = (uint8_t)((usCrc>>8U) & 0xffU);
		aucPacketBuffer[sizData+5] = (uint8_t)( usCrc      & 0xffU);

		/* Send the buffer. */
		sizPacket = sizData + 6;
		sizSend = m_ptUartDev->SendRaw(aucPacketBuffer, sizPacket, UART_BASE_TIMEOUT_MS + sizPacket*UART_CHAR_TIMEOUT_MS);
		if( sizSend==sizPacket )
		{
			m_ptUartDev->Flush();
			tResult = TRANSPORTSTATUS_OK;
		}
		else
		{
			fprintf(stderr, "! send_packet: failed to send the packet!\n");
			tResult = TRANSPORTSTATUS_SEND_FAILED;
		}
	}
	else
	{
		fprintf(stderr, "! send_packet: packet too large: %zd bytes! Maximum size is %zd.\n", sizData, sizMaxUserData);
		tResult = TRANSPORTSTATUS_PACKET_TOO_LARGE;
	}

	return tResult;
}



romloader::TRANSPORTSTATUS_T romloader_uart::receive_packet(void)
{
	unsigned int uiRetries;
	uint8_t ucData;
	bool fFound;
	TRANSPORTSTATUS_T tResult;
	size_t sizData;
	size_t sizPacket;
	uint16_t usCrc;
	size_t sizCnt;


//	fprintf(stderr, "receive_packet\n");
	/* Wait for the start character. */
	fFound = false;
	uiRetries = 64;
	do
	{
		tResult = packet_ringbuffer_fill(1);
		if( tResult==TRANSPORTSTATUS_OK )
		{
			ucData = packet_ringbuffer_get();
			if( ucData==MONITOR_STREAM_PACKET_START )
			{
				fFound = true;
				break;
			}
			else
			{
				fprintf(stderr, "WARNING: Skipping char 0x%02x.\n", ucData);
			}
		}
		else if( tResult==TRANSPORTSTATUS_TIMEOUT )
		{
			/* Nothing received. */
			break;
		}
		--uiRetries;
	} while( uiRetries>0 );

	if( fFound!=true )
	{
		fprintf(stderr, "receive_packet: no start char found!\n");
		tResult = TRANSPORTSTATUS_FAILED_TO_SYNC;
	}
	else
	{
//		fprintf(stderr, "Got start char\n");

		/* Get the size information. */
		tResult = packet_ringbuffer_fill(2);
		if( tResult==TRANSPORTSTATUS_OK )
		{
			sizData  =  (size_t)(packet_ringbuffer_peek(0));
			sizData |= ((size_t)(packet_ringbuffer_peek(1))) << 8U;
//			fprintf(stderr, "Expected packet size: %zd bytes\n", sizData);

			/* The complete packet consists of the size info, the user data and the CRC. The sequence number is already part of the user data. */
			sizPacket = 2 + sizData + 2;

			/* Get the rest of the packet. */
			tResult = packet_ringbuffer_fill(sizPacket);
			if( tResult==TRANSPORTSTATUS_OK )
			{
				/* Generate the CRC. */
				usCrc = 0;
				sizCnt = 0;
				do
				{
					ucData = packet_ringbuffer_peek(sizCnt);
					usCrc = crc16(usCrc, ucData);
					++sizCnt;
				} while( sizCnt<sizPacket );
				if( usCrc==0 )
				{
					/* Get the complete packet. */
					sizCnt = 0;
					do
					{
						m_aucPacketInputBuffer[sizCnt] = packet_ringbuffer_get();
						++sizCnt;
					} while( sizCnt<sizPacket );

					m_sizPacketInputBuffer = sizPacket;
				}
				else
				{
					fprintf(stderr, "! receive_packet: CRC failed.\n");

					/* Debug: get the complete packet and dump it.
					 * NOTE: Do not remove the data from the buffer.
					 */
//					printf("packet size: 0x%08lx bytes\n", sizPacket);
					sizCnt = 0;
					do
					{
						m_aucPacketInputBuffer[sizCnt] = packet_ringbuffer_peek(sizCnt);
						++sizCnt;
					} while( sizCnt<sizPacket );
//					printf("Packet data:\n");
//					hexdump(m_aucPacketInputBuffer, sizPacket);

					tResult = TRANSPORTSTATUS_CRC_MISMATCH;
				}
			}
			else
			{
				fprintf(stderr, "receive_packet: Failed to get 0x%02lx bytes of packet data info: %d\n", sizPacket, tResult);
			}
		}
		else
		{
			fprintf(stderr, "receive_packet: Failed to get size info: %d\n", tResult);
		}
	}

	return tResult;
}



struct PACKET_ACK_STRUCT
{
	uint8_t  ucStreamStart;
	uint16_t usDataSize;
	uint8_t  ucSequenceNumber;
	uint8_t  ucPacketType;
	uint8_t  ucCrcHi;
	uint8_t  ucCrcLo;
} __attribute__ ((packed));

typedef union PACKET_ACK_UNION
{
	struct PACKET_ACK_STRUCT s;
	uint8_t auc[7];
} PACKET_ACK_T;


romloader::TRANSPORTSTATUS_T romloader_uart::send_ack(unsigned char ucSequenceToAck)
{
	TRANSPORTSTATUS_T tResult;
	PACKET_ACK_T tPacketAck;
	uint16_t usCrc;
	size_t sizCnt;
	size_t sizPacket;
	size_t sizSend;


//	fprintf(stderr, "Sending ACK for sequence number %d.\n", ucSequenceToAck);

	/* Set the packet start character. */
	tPacketAck.s.ucStreamStart = MONITOR_STREAM_PACKET_START;

	/* Set the size of the user data.
	 *  1 byte sequence number
	 *  1 byte packet type
	 */
	tPacketAck.s.usDataSize = 2;

	/* Set the sequence number. */
	tPacketAck.s.ucSequenceNumber = ucSequenceToAck;

	/* Set the packet typ. */
	tPacketAck.s.ucPacketType = MONITOR_PACKET_TYP_ACK;

	/* Generate the CRC. */
	usCrc = 0;
	sizCnt = offsetof(struct PACKET_ACK_STRUCT, usDataSize);
	while( sizCnt<offsetof(struct PACKET_ACK_STRUCT, ucCrcHi) )
	{
		usCrc = crc16(usCrc, tPacketAck.auc[sizCnt]);
		++sizCnt;
	}

	/* Append the CRC. */
	tPacketAck.s.ucCrcHi = (uint8_t)((usCrc>>8U) & 0xffU);
	tPacketAck.s.ucCrcLo = (uint8_t)( usCrc      & 0xffU);

	/* Send the buffer. */
	sizPacket = sizeof(PACKET_ACK_T);
//	hexdump(tPacketAck.auc, sizPacket);
	sizSend = m_ptUartDev->SendRaw(tPacketAck.auc, sizPacket, UART_BASE_TIMEOUT_MS + sizPacket*UART_CHAR_TIMEOUT_MS);
	if( sizSend==sizPacket )
	{
		m_ptUartDev->Flush();
		tResult = TRANSPORTSTATUS_OK;
	}
	else
	{
		fprintf(stderr, "! send_packet: failed to send the ACK packet!\n");
		tResult = TRANSPORTSTATUS_SEND_FAILED;
	}

	return tResult;
}



romloader::TRANSPORTSTATUS_T romloader_uart::execute_command(const unsigned char *aucCommand, size_t sizCommand)
{
	TRANSPORTSTATUS_T tResult;
	unsigned char ucStatus;
	unsigned char ucPacketSequence;
	unsigned int uiRetryCnt;


	uiRetryCnt = 10;
	do
	{
		/* Discard all old data in the ring buffer. */
		packet_ringbuffer_discard();
		m_ptUartDev->discardCards();

		/* Send the command. */
//		fprintf(stderr, "Sending packet...\n");
		tResult = send_packet(aucCommand, sizCommand);
		if( tResult!=TRANSPORTSTATUS_OK )
		{
			fprintf(stderr, "! execute_command: send_packet failed with error code %d\n", tResult);
		}
		else
		{
			/* Receive the ACK for the command. */
//			fprintf(stderr, "Wait for ACK...\n");
			tResult = receive_packet();
			if( tResult!=TRANSPORTSTATUS_OK )
			{
				fprintf(stderr, "! execute_command: receive_packet failed with error code %d\n", tResult);
			}
			else
			{
				if( m_sizPacketInputBuffer==6 && m_aucPacketInputBuffer[3]==MONITOR_PACKET_TYP_ACK )
				{
					/* Get the sequence number. */
					ucPacketSequence = m_aucPacketInputBuffer[2];
					if( ucPacketSequence!=m_ucMonitorSequence )
					{
						fprintf(stderr, "! execute_command: the sequence does not match: %d / %d\n", ucPacketSequence, m_ucMonitorSequence);
						synchronize();
						tResult = TRANSPORTSTATUS_SEQUENCE_MISMATCH;
					}
					else
					{
//						fprintf(stderr, "ACK OK...\n");

						/* The packet was acknowledged.
						 * Now increase the sequence number .
						 */
						++m_ucMonitorSequence;

						tResult = TRANSPORTSTATUS_OK;
					}
				}
				else
				{
					fprintf(stderr, "! execute_command: expected an ACK packet with 6 bytes, but received %ld bytes:\n", m_sizPacketInputBuffer);
					hexdump(m_aucPacketInputBuffer, m_sizPacketInputBuffer);
/* FIXME: change the result. */
					tResult = TRANSPORTSTATUS_MISSING_USERDATA;
				}
			}
		}

		if( tResult!=TRANSPORTSTATUS_OK )
		{
			--uiRetryCnt;
			if( uiRetryCnt==0 )
			{
				fprintf(stderr, "! execute_command: Retried 10 times and nothing happened. Giving up!\n");
				break;
			}
			else
			{
				fprintf(stderr, "***************************************\n");
				fprintf(stderr, "*                                     *\n");
				fprintf(stderr, "*            retry                    *\n");
				fprintf(stderr, "*                                     *\n");
				fprintf(stderr, "***************************************\n");
			}
		}

	} while( tResult!=TRANSPORTSTATUS_OK );

	return tResult;
}



romloader::TRANSPORTSTATUS_T romloader_uart::read_data(uint32_t ulNetxAddress, MONITOR_ACCESSSIZE_T tAccessSize, uint16_t sizDataInBytes)
{
	MIV3_PACKET_COMMAND_READWRITE_DATA_T tPacketReadData;
	TRANSPORTSTATUS_T tResult;
	uint8_t ucPacketTyp;


	if( m_fIsConnected==false )
	{
		tResult = TRANSPORTSTATUS_NOT_CONNECTED;
	}
	else
	{
		tPacketReadData.s.ucPacketType =  MONITOR_PACKET_TYP_Command_Read |
		                                 (tAccessSize<<MONITOR_ACCESSSIZE_SRT);
		tPacketReadData.s.usDataSize = HTONETX16(sizDataInBytes);
		tPacketReadData.s.ulAddress = HTONETX32(ulNetxAddress);
		tResult = execute_command(tPacketReadData.auc, sizeof(tPacketReadData));
		if( tResult==TRANSPORTSTATUS_OK )
		{
			/* Receive the data. */
			tResult = receive_packet();
			if( tResult==TRANSPORTSTATUS_OK )
			{
				/* A minimum packet must have 6 bytes:
				 *   2 bytes packet size
				 *   1 byte sequence number
				 *   1 byte packet type
				 *   0 bytes data
				 *   2 bytes CRC
				 */
				if( m_sizPacketInputBuffer<6 )
				{
					/* The packet is too small for 1 byte of user data. */
					tResult = TRANSPORTSTATUS_PACKET_TOO_SMALL;
				}
				else
				{
					/* Get the packet type. */
					ucPacketTyp = m_aucPacketInputBuffer[3];

					if( ucPacketTyp==MONITOR_PACKET_TYP_Read_Data )
					{
						if( m_sizPacketInputBuffer==6+sizDataInBytes )
						{
							/* The response matches the number of requested bytes. */
							send_ack(m_ucMonitorSequence);
							/* Increase the sequence number. */
							++m_ucMonitorSequence;
						}
						else
						{
							/* The packet type is set to "read_data", but the response does
							 * not match the requested number of bytes.
							 */
							tResult = TRANSPORTSTATUS_UNEXPECTED_PACKET_SIZE;
						}
					}
					else if( ucPacketTyp==MONITOR_PACKET_TYP_Status )
					{
						if( m_sizPacketInputBuffer==5+2 )
						{
							/* The netX sent a status code in response to the read command. */
							fprintf(stderr, "Status package received. Status %d.\n", m_aucPacketInputBuffer[4]);
							send_ack(m_ucMonitorSequence);
							/* Increase the sequence number. */
							++m_ucMonitorSequence;
							tResult = TRANSPORTSTATUS_NETX_ERROR;
						}
						else
						{
							/* The packet type is set to "status", but the size of the packet does not match a valid status packet. */
							tResult = TRANSPORTSTATUS_INVALID_PACKET_SIZE;
						}
					}
					else
					{
						hexdump(m_aucPacketInputBuffer, m_sizPacketInputBuffer);
						fprintf(stderr, "Answer to read_data has unexpected packet typ 0x%02x!", ucPacketTyp);
						tResult = TRANSPORTSTATUS_UNEXPECTED_PACKET_TYP;
					}
				}
			}
		}
	}

	return tResult;
}



romloader::TRANSPORTSTATUS_T romloader_uart::write_data(uint32_t ulNetxAddress, MONITOR_ACCESSSIZE_T tAccessSize, const void *pvData, uint16_t sizDataInBytes)
{
	union WRITE_DATA_UNION
	{
		struct WRITE_DATA_STRUCT
		{
			MIV3_PACKET_COMMAND_READWRITE_DATA_T s;
			uint8_t auc[sizMaxPacketSizeHost - sizeof(MIV3_PACKET_COMMAND_READWRITE_DATA_T)];
		} __attribute__ ((packed)) s;
		uint8_t auc[sizMaxPacketSizeHost];
	} __attribute__ ((packed)) uWriteData;
	TRANSPORTSTATUS_T tResult;
	uint8_t ucPacketTyp;
	uint8_t ucStatus;


	if( m_fIsConnected==false )
	{
		tResult = TRANSPORTSTATUS_NOT_CONNECTED;
	}
	else
	{
		/* Set the header fields. */
		uWriteData.s.s.s.ucPacketType =  MONITOR_PACKET_TYP_Command_Write |
		                                 (tAccessSize<<MONITOR_ACCESSSIZE_SRT);
		uWriteData.s.s.s.usDataSize = HTONETX16(sizDataInBytes);
		uWriteData.s.s.s.ulAddress = HTONETX32(ulNetxAddress);

		/* Add the data. */
		memcpy(uWriteData.s.auc, pvData, sizDataInBytes);

		tResult = execute_command(uWriteData.auc, sizeof(MIV3_PACKET_COMMAND_READWRITE_DATA_T)+sizDataInBytes);
		if( tResult==TRANSPORTSTATUS_OK )
		{
			/* Receive the status. */
			tResult = receive_packet();
			if( tResult==TRANSPORTSTATUS_OK )
			{
				/* A minimum packet must have 6 bytes:
				 *   2 bytes packet size
				 *   1 byte sequence number
				 *   1 byte packet type
				 *   0 bytes data
				 *   2 bytes CRC
				 */
				if( m_sizPacketInputBuffer<6 )
				{
					/* The packet is too small for 1 byte of user data. */
					tResult = TRANSPORTSTATUS_PACKET_TOO_SMALL;
				}
				else
				{
					/* Get the packet type. */
					ucPacketTyp = m_aucPacketInputBuffer[3];

					if( ucPacketTyp==MONITOR_PACKET_TYP_Status )
					{
						if( m_sizPacketInputBuffer==5+2 )
						{
							/* The netX sent a status code. */
							ucStatus = m_aucPacketInputBuffer[4];
							send_ack(m_ucMonitorSequence);
							/* Increase the sequence number. */
							++m_ucMonitorSequence;
							if( ucStatus!=MONITOR_STATUS_Ok )
							{
								/* The netX sent an error. */
								fprintf(stderr, "Status != OK received. Status %d.\n", ucStatus);
								tResult = TRANSPORTSTATUS_NETX_ERROR;
							}
						}
						else
						{
							/* The packet type is set to "status", but the size of the packet does not match a valid status packet. */
							tResult = TRANSPORTSTATUS_INVALID_PACKET_SIZE;
						}
					}
					else
					{
						hexdump(m_aucPacketInputBuffer, m_sizPacketInputBuffer);
						fprintf(stderr, "Answer to read_data has unexpected packet typ 0x%02x!", ucPacketTyp);
						tResult = TRANSPORTSTATUS_UNEXPECTED_PACKET_TYP;
					}
				}
			}
		}
	}

	return tResult;
}



uint8_t romloader_uart::read_data08(lua_State *ptClientData, uint32_t ulNetxAddress)
{
	TRANSPORTSTATUS_T tResult;
	uint8_t ucValue;
	bool fOk;


	/* Expect error. */
	fOk = false;
	ucValue = 0;

	tResult = read_data(ulNetxAddress, MONITOR_ACCESSSIZE_Byte, 1);
	if( tResult!=TRANSPORTSTATUS_OK )
	{
		MUHKUH_PLUGIN_PUSH_ERROR(ptClientData, "%s(%p): %s!", m_pcName, this, get_error_message(tResult));
	}
	else
	{
		ucValue = m_aucPacketInputBuffer[4];
		fOk = true;
	}

	if( fOk!=true )
	{
		MUHKUH_PLUGIN_EXIT_ERROR(ptClientData);
	}

	return ucValue;
}



uint16_t romloader_uart::read_data16(lua_State *ptClientData, uint32_t ulNetxAddress)
{
	TRANSPORTSTATUS_T tResult;
	uint16_t usValue;
	bool fOk;


	/* Expect error. */
	fOk = false;
	usValue = 0;

	tResult = read_data(ulNetxAddress, MONITOR_ACCESSSIZE_Word, 2);
	if( tResult!=TRANSPORTSTATUS_OK )
	{
		MUHKUH_PLUGIN_PUSH_ERROR(ptClientData, "%s(%p): %s!", m_pcName, this, get_error_message(tResult));
	}
	else
	{
		usValue  = ((uint16_t)(m_aucPacketInputBuffer[4]));
		usValue |= ((uint16_t)(m_aucPacketInputBuffer[5]))<< 8U;
		fOk = true;
	}

	if( fOk!=true )
	{
		MUHKUH_PLUGIN_EXIT_ERROR(ptClientData);
	}

	return usValue;
}



uint32_t romloader_uart::read_data32(lua_State *ptClientData, uint32_t ulNetxAddress)
{
	TRANSPORTSTATUS_T tResult;
	uint32_t ulValue;
	bool fOk;


	/* Expect error. */
	fOk = false;
	ulValue = 0;

	tResult = read_data(ulNetxAddress, MONITOR_ACCESSSIZE_Long, 4);
	if( tResult!=TRANSPORTSTATUS_OK )
	{
		MUHKUH_PLUGIN_PUSH_ERROR(ptClientData, "%s(%p): %s!", m_pcName, this, get_error_message(tResult));
	}
	else
	{
		ulValue  = ((uint32_t)(m_aucPacketInputBuffer[4]));
		ulValue |= ((uint32_t)(m_aucPacketInputBuffer[5]))<< 8U;
		ulValue |= ((uint32_t)(m_aucPacketInputBuffer[6]))<<16U;
		ulValue |= ((uint32_t)(m_aucPacketInputBuffer[7]))<<24U;
		fOk = true;
	}

	if( fOk!=true )
	{
		MUHKUH_PLUGIN_EXIT_ERROR(ptClientData);
	}

	return ulValue;
}



void romloader_uart::read_image(uint32_t ulNetxAddress, uint32_t ulSize, char **ppcBUFFER_OUT, size_t *psizBUFFER_OUT, SWIGLUA_REF tLuaFn, long lCallbackUserData)
{
	char *pcBufferStart;
	char *pcBuffer;
	size_t sizBuffer;
	bool fOk;
	size_t sizChunk;
	size_t sizChunkMaxSize;
	TRANSPORTSTATUS_T tResult;
	bool fIsRunning;
	long lBytesProcessed;


	/* Be optimistic. */
	fOk = true;

	/* The maximum size for a data chunk is the buffer size minus the
	 * protocol fields:
	 *  1 byte start char
	 *  2 byte packet size
	 *  1 byte sequence number
	 *  1 byte packet type
	 *  2 byte data size
	 *  4 byte netX address
	 *  2 byte CRC
	 */
//	sizChunkMaxSize = m_sizMaxPacketSizeClient - 13U;
	sizChunkMaxSize = m_sizMaxPacketSizeClient - 256U;

	pcBufferStart = NULL;
	sizBuffer = 0;

	/* if ulSize == 0, we return with fOk == true, pcBufferStart == NULL and sizBuffer == 0 */
	if( ulSize > 0 )
	{
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
				if( sizChunk>sizChunkMaxSize)
				{
					sizChunk = sizChunkMaxSize;
				}

				tResult = read_data(ulNetxAddress, MONITOR_ACCESSSIZE_Any, sizChunk);
				if( tResult!=TRANSPORTSTATUS_OK )
				{
					MUHKUH_PLUGIN_PUSH_ERROR(tLuaFn.L, "%s(%p): %s!", m_pcName, this, get_error_message(tResult));
				}
				else
				{
					memcpy(pcBuffer, m_aucPacketInputBuffer+4, sizChunk);
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
			} while( ulSize!=0 );
		}
	}

	if( fOk==true )
	{
		*ppcBUFFER_OUT = pcBufferStart;
		*psizBUFFER_OUT = sizBuffer;
	}
	else
	{
		if( pcBufferStart!=NULL )
		{
			free(pcBufferStart);
		}
		MUHKUH_PLUGIN_EXIT_ERROR(tLuaFn.L);
	}
}



void romloader_uart::write_data08(lua_State *ptClientData, uint32_t ulNetxAddress, uint8_t ucData)
{
	TRANSPORTSTATUS_T tResult;
	uint8_t aucData[1];
	bool fOk;


	/* Be optimistic. */
	fOk = true;

	aucData[0] = ucData;
	tResult = write_data(ulNetxAddress, MONITOR_ACCESSSIZE_Byte, aucData, 1);
	if( tResult!=TRANSPORTSTATUS_OK )
	{
		MUHKUH_PLUGIN_PUSH_ERROR(ptClientData, "%s(%p): %s", m_pcName, this, get_error_message(tResult));
		fOk = false;
	}

	if( fOk!=true )
	{
		MUHKUH_PLUGIN_EXIT_ERROR(ptClientData);
	}
}


void romloader_uart::write_data16(lua_State *ptClientData, uint32_t ulNetxAddress, uint16_t usData)
{
	TRANSPORTSTATUS_T tResult;
	uint16_t ausData[1];
	bool fOk;


	/* Be optimistic. */
	fOk = true;

	ausData[0] = HTONETX16(usData);
	tResult = write_data(ulNetxAddress, MONITOR_ACCESSSIZE_Word, ausData, 2);
	if( tResult!=TRANSPORTSTATUS_OK )
	{
		MUHKUH_PLUGIN_PUSH_ERROR(ptClientData, "%s(%p): %s", m_pcName, this, get_error_message(tResult));
		fOk = false;
	}

	if( fOk!=true )
	{
		MUHKUH_PLUGIN_EXIT_ERROR(ptClientData);
	}
}



void romloader_uart::write_data32(lua_State *ptClientData, uint32_t ulNetxAddress, uint32_t ulData)
{
	TRANSPORTSTATUS_T tResult;
	uint32_t aulData[1];
	bool fOk;


	/* Be optimistic. */
	fOk = true;

	aulData[0] = HTONETX32(ulData);
	tResult = write_data(ulNetxAddress, MONITOR_ACCESSSIZE_Long, aulData, 4);
	if( tResult!=TRANSPORTSTATUS_OK )
	{
		MUHKUH_PLUGIN_PUSH_ERROR(ptClientData, "%s(%p): %s", m_pcName, this, get_error_message(tResult));
		fOk = false;
	}

	if( fOk!=true )
	{
		MUHKUH_PLUGIN_EXIT_ERROR(ptClientData);
	}
}



void romloader_uart::write_image(uint32_t ulNetxAddress, const char *pcBUFFER_IN, size_t sizBUFFER_IN, SWIGLUA_REF tLuaFn, long lCallbackUserData)
{
	bool fOk;
	size_t sizChunk;
	size_t sizChunkMaxSize;
	TRANSPORTSTATUS_T tResult;
	bool fIsRunning;
	long lBytesProcessed;


	/* Be optimistic. */
	fOk = true;

	/* The maximum size for a data chunk is the buffer size minus the
	 * protocol fields:
	 *  1 byte start char
	 *  2 byte packet size
	 *  1 byte sequence number
	 *  1 byte packet type
	 *  2 byte data size
	 *  4 byte netX address
	 *  2 byte CRC
	 */
//	sizChunkMaxSize = m_sizMaxPacketSizeClient - 13U;
	sizChunkMaxSize = m_sizMaxPacketSizeClient - 256U;

	if( sizBUFFER_IN!=0 )
	{
		lBytesProcessed = 0;
		do
		{
			sizChunk = sizBUFFER_IN;
			if( sizChunk>sizChunkMaxSize )
			{
				sizChunk = sizChunkMaxSize;
			}

			tResult = write_data(ulNetxAddress, MONITOR_ACCESSSIZE_Any, pcBUFFER_IN, sizChunk);
			if( tResult!=TRANSPORTSTATUS_OK )
			{
				MUHKUH_PLUGIN_PUSH_ERROR(tLuaFn.L, "%s(%p): %s", m_pcName, this, get_error_message(tResult));
				fOk = false;
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
		} while( sizBUFFER_IN!=0 );
	}

	if( fOk!=true )
	{
		MUHKUH_PLUGIN_EXIT_ERROR(tLuaFn.L);
	}
}



void romloader_uart::call(uint32_t ulNetxAddress, uint32_t ulParameterR0, SWIGLUA_REF tLuaFn, long lCallbackUserData)
{
	bool fOk;
	TRANSPORTSTATUS_T tResult;
	MIV3_PACKET_COMMAND_CALL_T tCallCommand;
	const uint8_t aucCancelBuf[1] = { 0x2b };
	uint8_t ucStatus;
	bool fIsRunning;
	char *pcProgressData;
	size_t sizProgressData;
	uint8_t ucPacketTyp;


	if( m_fIsConnected==false )
	{
		MUHKUH_PLUGIN_PUSH_ERROR(tLuaFn.L, "%s(%p): not connected!", m_pcName, this);
		fOk = false;
	}
	else
	{
		tCallCommand.s.ucPacketType = MONITOR_COMMAND_Execute;
		tCallCommand.s.ulAddress = HTONETX32(ulNetxAddress);
		tCallCommand.s.ulR0 = HTONETX32(ulParameterR0);
		tResult = execute_command(tCallCommand.auc, sizeof(tCallCommand));
		if( tResult!=TRANSPORTSTATUS_OK )
		{
			MUHKUH_PLUGIN_PUSH_ERROR(tLuaFn.L, "%s(%p): failed to execute command!", m_pcName, this);
			fOk = false;
		}
		else
		{
			/* Receive message packets. */
			while(1)
			{
				pcProgressData = NULL;
				sizProgressData = 0;

				tResult = receive_packet();
				if( tResult==TRANSPORTSTATUS_TIMEOUT )
				{
					/* Do nothing in case of timeout. The application is just running quietly. */
				}
				else if( tResult!=TRANSPORTSTATUS_OK )
				{
					MUHKUH_PLUGIN_PUSH_ERROR(tLuaFn.L, "%s(%p): Failed to receive a call message packet: %s (%d)", m_pcName, this, get_error_message(tResult), tResult);
					fOk = false;
				}
				else
				{
					/* A minimum packet must have 6 bytes:
					 *   2 bytes packet size
					 *   1 byte sequence number
					 *   1 byte packet type
					 *   0 bytes data
					 *   2 bytes CRC
					 */
					if( m_sizPacketInputBuffer<6 )
					{
						/* The packet is too small for 1 byte of user data. */
						tResult = TRANSPORTSTATUS_PACKET_TOO_SMALL;
					}
					else
					{
						/* Get the packet type. */
						ucPacketTyp = m_aucPacketInputBuffer[3];

						if( ucPacketTyp==MONITOR_PACKET_TYP_Call_Message )
						{
							/* Acknowledge the packet. */
							send_ack(m_ucMonitorSequence);
							/* Increase the sequence number. */
							++m_ucMonitorSequence;

							/* NOTE: Do not check the size of the user data here. It should be possible to send 0 bytes. */
							pcProgressData = ((char*)m_aucPacketInputBuffer) + 4U;
							sizProgressData = m_sizPacketInputBuffer - 6U;
						}
						else if( ucPacketTyp==MONITOR_PACKET_TYP_Status )
						{
							if( m_sizPacketInputBuffer==5+2 )
							{
								/* The netX sent a status code. */

								/* Acknowledge the packet. */
								send_ack(m_ucMonitorSequence);
								/* Increase the sequence number. */
								++m_ucMonitorSequence;

								ucStatus = m_aucPacketInputBuffer[4];
								if( ucStatus==MONITOR_STATUS_Call_Finished )
								{
									fprintf(stderr, "%s(%p): Call has finished!", m_pcName, this);
									fOk = true;
									break;
								}
								else
								{
									/* The netX sent an error. */
									fprintf(stderr, "Status != call_finished received. Status %d.\n", ucStatus);
									tResult = TRANSPORTSTATUS_NETX_ERROR;
								}
							}
							else
							{
								/* The packet type is set to "status", but the size of the packet does not match a valid status packet. */
								tResult = TRANSPORTSTATUS_INVALID_PACKET_SIZE;
							}
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

	if( fOk!=true )
	{
		MUHKUH_PLUGIN_EXIT_ERROR(tLuaFn.L);
	}
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

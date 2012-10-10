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

const char *romloader_uart_provider::m_pcPluginNamePattern = "romloader_uart_%s";

romloader_uart_provider::romloader_uart_provider(swig_type_info *p_romloader_uart, swig_type_info *p_romloader_uart_reference)
 : muhkuh_plugin_provider("romloader_uart")
{
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
			printf("%s(%p): claim_interface(): claimed interface %s.\n", m_pcPluginId, this, pcName);
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
	printf("%s(%p): created in romloader_uart\n", m_pcName, this);

	m_ptUartDev = new romloader_uart_device_platform(pcDeviceName);

	m_uiMonitorSequence = 0;

	packet_ringbuffer_init();
}


romloader_uart::~romloader_uart(void)
{
	printf("%s(%p): deleted in romloader_uart\n", m_pcName, this);

	if( m_ptUartDev!=NULL )
	{
		m_ptUartDev->Close();
	}
}


void romloader_uart::hexdump(const unsigned char *pucData, unsigned long ulSize)
{
	const unsigned char *pucDumpCnt, *pucDumpEnd;
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
	const unsigned char aucKnock[5] = { '*', 0x00, 0x00, '*', '#' };
	const unsigned char aucMagicMooh[4] = { 0x4d, 0x4f, 0x4f, 0x48 };
	size_t sizCnt;
	size_t sizTransfered;
	unsigned long ulMiVersionMaj;
	unsigned long ulMiVersionMin;
	ROMLOADER_COMMANDSET_T tCmdSet;
	unsigned short usCrc;
	size_t sizPacket;
	unsigned char aucData[32];
	unsigned char ucStatus;


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
			fprintf(stderr, "Failed to receive first char of knock response: %d.\n", sizTransfered);
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
						sizPacket  = ((unsigned long)aucData[0]);
						sizPacket |= ((unsigned long)aucData[1]) << 8U;
						
						/* The size information does not include the CRC. Add the 2 bytes here. */
						sizPacket += 2;
						
						/* Is the size ok? */
						if( sizPacket<11 )
						{
							fprintf(stderr, "The received packet is too small. It must be at least 11 bytes, but it is %d bytes.\n", sizPacket);
						}
						else if( sizPacket>(sizeof(aucData)-2) )
						{
							fprintf(stderr, "The received packet is too big for a buffer of %d bytes. It has %d bytes.\n", sizeof(aucData)-2, sizPacket);
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
								ucStatus = aucData[2] & MONITOR_STATUS_MSK;
								if( ucStatus!=MONITOR_STATUS_Ok )
								{
									fprintf(stderr, "The status of the response is not OK but 0x%02x.\n", ucStatus);
								}
								else if( memcmp(aucData+3, aucMagicMooh, sizeof(aucMagicMooh))!=0 )
								{
									fprintf(stderr, "The response packet has no MOOH magic!\n");
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
										ulMiVersionMin = aucData[7] | (aucData[8]<<8);;
										ulMiVersionMaj = aucData[9] | (aucData[10]<<8);
										printf("Found new machine interface V%ld.%ld.\n", ulMiVersionMaj, ulMiVersionMin);
										
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
										{
											fprintf(stderr, "Unknown machine interface version %ld.%ld\n", ulMiVersionMaj, ulMiVersionMin);
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



romloader_uart::UARTSTATUS_T romloader_uart::send_sync_command(void)
{
	const unsigned char aucKnock[5] = { '*', 0x00, 0x00, '*', '#' };
	size_t sizSend;
	UARTSTATUS_T tResult;
	unsigned char ucStatus;
	unsigned int uiRetryCnt;


	uiRetryCnt = 10;
	do
	{
		sizSend = m_ptUartDev->SendRaw(aucKnock, sizeof(aucKnock), UART_BASE_TIMEOUT_MS + sizeof(aucKnock)*UART_CHAR_TIMEOUT_MS);
		if( sizSend!=sizeof(aucKnock) )
		{
			fprintf(stderr, "! send_sync_command: failed to send the packet!\n");
			tResult = UARTSTATUS_SEND_FAILED;
		}
		else
		{
			tResult = receive_packet();
			if( tResult!=UARTSTATUS_OK )
			{
				fprintf(stderr, "! send_sync_command: receive_packet failed with error code %d\n", tResult);
			}
			else
			{
				if( m_sizPacketInputBuffer<5 )
				{
					fprintf(stderr, "! send_sync_command: received no user data!\n");
					tResult = UARTSTATUS_MISSING_USERDATA;
				}
				else
				{
					ucStatus = m_aucPacketInputBuffer[2] & MONITOR_STATUS_MSK;
					if( ucStatus==0 )
					{
						tResult = UARTSTATUS_OK;
					}
					else
					{
						fprintf(stderr, "! send_sync_command: status is not OK: %d\n", ucStatus);
						tResult = UARTSTATUS_COMMAND_EXECUTION_FAILED;
					}
				}
			}
		}

		if( tResult!=UARTSTATUS_OK )
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

	} while( tResult!=UARTSTATUS_OK );

	return tResult;
}



bool romloader_uart::synchronize(void)
{
	const unsigned char aucMagicMooh[4] = { 0x4d, 0x4f, 0x4f, 0x48 };
	UARTSTATUS_T tResult;
	bool fResult;
	/* The expected knock response is 16 bytes:
	 *    2 size bytes
	 *    1 status byte
	 *   11 data bytes (see monitor_commands.h for more information)
	 *    2 bytes CRC
	 */
	const size_t sizExpectedResponse = 16;
	unsigned char ucSequence;
	unsigned long ulMiVersionMin;
	unsigned long ulMiVersionMaj;
	ROMLOADER_CHIPTYP tChipType;
	size_t sizMaxPacketSize;


	fResult = false;

	tResult = send_sync_command();
	if( tResult!=UARTSTATUS_OK )
	{
		/* Failed to send knock sequence to device. */
		fprintf(stderr, "Failed to send knock sequence to device.\n");
	}
	else if( m_sizPacketInputBuffer!=sizExpectedResponse )
	{
		fprintf(stderr, "Received knock sequence with invalid size of %d. Expected: %d.\n", m_sizPacketInputBuffer, sizExpectedResponse);
		hexdump(m_aucPacketInputBuffer, m_sizPacketInputBuffer);
	}
	else if( memcmp(m_aucPacketInputBuffer+3, aucMagicMooh, sizeof(aucMagicMooh))!=0 )
	{
		fprintf(stderr, "Received knock sequence has no magic.\n");
		hexdump(m_aucPacketInputBuffer, m_sizPacketInputBuffer);
	}
	else
	{
		fprintf(stderr, "Packet:\n");
		hexdump(m_aucPacketInputBuffer, m_sizPacketInputBuffer);

		/* Get the sequence number from the status byte. */
		ucSequence = (m_aucPacketInputBuffer[0x02] & MONITOR_SEQUENCE_MSK) >> MONITOR_SEQUENCE_SRT;
		fprintf(stderr, "Sequence number: 0x%02x\n", ucSequence);

		ulMiVersionMin =  ((unsigned long)(m_aucPacketInputBuffer[0x07])) |
		                 (((unsigned long)(m_aucPacketInputBuffer[0x08]))<<8U);
		ulMiVersionMaj =  ((unsigned long)(m_aucPacketInputBuffer[0x09])) |
		                 (((unsigned long)(m_aucPacketInputBuffer[0x0a]))<<8U);
		printf("Machine interface V%ld.%ld.\n", ulMiVersionMaj, ulMiVersionMin);

		tChipType = (ROMLOADER_CHIPTYP)(m_aucPacketInputBuffer[0x0b]);
		printf("Chip type : %d\n", tChipType);

		sizMaxPacketSize =  ((size_t)(m_aucPacketInputBuffer[0x0c])) |
		                   (((size_t)(m_aucPacketInputBuffer[0x0d]))<<8U);
		printf("Maximum packet size: 0x%04x\n", sizMaxPacketSize);
		/* Limit the packet size to the buffer size. */
		if( sizMaxPacketSize>sizMaxPacketSizeHost )
		{
			sizMaxPacketSize = sizMaxPacketSizeHost;
			printf("Limit maximum packet size to 0x%04x\n", sizMaxPacketSize);
		}

		/* Set the new values. */
		m_uiMonitorSequence = ucSequence;
		m_tChiptyp = tChipType;
		m_sizMaxPacketSizeClient = sizMaxPacketSize;
		
		fResult = true;
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

	printf("%s(%p): connect\n", m_pcName, this);

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
		m_ptUartDev->Close();
	}

	m_fIsConnected = false;
}



void romloader_uart::packet_ringbuffer_init(void)
{
	m_sizPacketRingBufferHead = 0;
	m_sizPacketRingBufferFill = 0;
}



romloader_uart::UARTSTATUS_T romloader_uart::packet_ringbuffer_fill(size_t sizRequestedFillLevel)
{
	size_t sizWritePosition;
	UARTSTATUS_T tResult;
	size_t sizReceiveCnt;
	size_t sizChunk;
	size_t sizRead;


	/* Expect success. */
	tResult = UARTSTATUS_OK;

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
				tResult = UARTSTATUS_TIMEOUT;
				break;
			}
		} while( sizReceiveCnt>0 );
	}

	return tResult;
}



unsigned char romloader_uart::packet_ringbuffer_get(void)
{
	unsigned char ucByte;


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



romloader_uart::UARTSTATUS_T romloader_uart::send_packet(const unsigned char *pucData, size_t sizData)
{
	UARTSTATUS_T tResult;
	unsigned char aucPacketBuffer[sizMaxPacketSizeHost];
	unsigned short usCrc;
	const unsigned char *pucCnt;
	const unsigned char *pucEnd;
	size_t sizSend;
	size_t sizPacket;


	/* The maximum packet size is reduced by...
	 *   the start char (1 byte),
	 *   the size information (2 bytes)
	 *   and the CRC (2 bytes).
	 */
	if( sizData<=m_sizMaxPacketSizeClient-5 )
	{
		/* Set the packet start character. */
		aucPacketBuffer[0] = MONITOR_STREAM_PACKET_START;
		/* Set the size of the user data. */
		aucPacketBuffer[1] = (unsigned char)( sizData        & 0xffU);
		aucPacketBuffer[2] = (unsigned char)((sizData >> 8U) & 0xffU);

		/* Copy the user data. */
		memcpy(aucPacketBuffer+3, pucData, sizData);

		/* Generate the CRC. */
		usCrc = 0;
		pucCnt = aucPacketBuffer + 1;
		pucEnd = aucPacketBuffer + 1 + 2 + sizData;
		while( pucCnt<pucEnd )
		{
			usCrc = crc16(usCrc, *pucCnt);
			++pucCnt;
		}

		/* Append the CRC. */
		aucPacketBuffer[sizData+3] = (unsigned char)((usCrc>>8U) & 0xffU);
		aucPacketBuffer[sizData+4] = (unsigned char)( usCrc      & 0xffU);

		/* Send the buffer. */
		sizPacket = sizData + 5;
		sizSend = m_ptUartDev->SendRaw(aucPacketBuffer, sizPacket, UART_BASE_TIMEOUT_MS + sizPacket*UART_CHAR_TIMEOUT_MS);
		if( sizSend==sizPacket )
		{
			tResult = UARTSTATUS_OK;
		}
		else
		{
			fprintf(stderr, "! send_packet: failed to send the packet!\n");
			tResult = UARTSTATUS_SEND_FAILED;
		}
	}
	else
	{
		fprintf(stderr, "! send_packet: packet too large: %d bytes!\n", sizData);
		tResult = UARTSTATUS_PACKET_TOO_LARGE;
	}

	return tResult;
}



romloader_uart::UARTSTATUS_T romloader_uart::receive_packet(void)
{
	unsigned int uiRetries;
	unsigned char ucData;
	bool fFound;
	UARTSTATUS_T tResult;
	size_t sizData;
	size_t sizPacket;
	unsigned short usCrc;
	size_t sizCnt;


	/* Wait for the start character. */
	fFound = false;
	uiRetries = 10;
	do
	{
		tResult = packet_ringbuffer_fill(1);
		if( tResult==UARTSTATUS_OK )
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
		--uiRetries;
	} while( uiRetries>0 );

	if( fFound!=true )
	{
		fprintf(stderr, "receive_packet: no start char found!\n");
		tResult = UARTSTATUS_FAILED_TO_SYNC;
	}
	else
	{
		/* Get the size information. */
		tResult = packet_ringbuffer_fill(2);
		if( tResult==UARTSTATUS_OK )
		{
			sizData  =  (size_t)(packet_ringbuffer_peek(0));
			sizData |= ((size_t)(packet_ringbuffer_peek(1))) << 8U;

			/* The complete packet consists of the size info, the user data and the CRC.*/
			sizPacket = 2 + sizData + 2;

			/* Get the rest of the packet. */
			tResult = packet_ringbuffer_fill(sizPacket);
			if( tResult==UARTSTATUS_OK )
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

					/* Debug: get the complete packet and dump it. */
					printf("packet size: 0x%08x bytes\n", sizPacket);
					sizCnt = 0;
					do
					{
						m_aucPacketInputBuffer[sizCnt] = packet_ringbuffer_get();
						++sizCnt;
					} while( sizCnt<sizPacket );
					printf("Packet data:\n");
					hexdump(m_aucPacketInputBuffer, sizPacket);

					tResult = UARTSTATUS_CRC_MISMATCH;
				}
			}
			else
			{
				fprintf(stderr, "receive_packet: Failed to get 0x%02x bytes of packet data info: %d\n", sizPacket, tResult);
			}
		}
		else
		{
			fprintf(stderr, "receive_packet: Failed to get size info: %d\n", tResult);
		}
	}

	return tResult;
}



romloader_uart::UARTSTATUS_T romloader_uart::execute_command(const unsigned char *aucCommand, size_t sizCommand)
{
	UARTSTATUS_T tResult;
	unsigned char ucStatus;
	unsigned int uiSequence;
	unsigned int uiRetryCnt;


	uiRetryCnt = 10;
	do
	{
		tResult = send_packet(aucCommand, sizCommand);
		if( tResult!=UARTSTATUS_OK )
		{
			fprintf(stderr, "! execute_command: send_packet failed with error code %d\n", tResult);
		}
		else
		{
			tResult = receive_packet();
			if( tResult!=UARTSTATUS_OK )
			{
				fprintf(stderr, "! execute_command: receive_packet failed with error code %d\n", tResult);
			}
			else
			{
				if( m_sizPacketInputBuffer<5 )
				{
					fprintf(stderr, "! execute_command: packet size too small: %d. It has no user data!\n", m_sizPacketInputBuffer);
					tResult = UARTSTATUS_MISSING_USERDATA;
				}
				else
				{
					ucStatus = (m_aucPacketInputBuffer[2] & MONITOR_STATUS_MSK) >> MONITOR_STATUS_SRT;
					uiSequence = (m_aucPacketInputBuffer[2] & MONITOR_SEQUENCE_MSK) >> MONITOR_SEQUENCE_SRT;
					if( ucStatus!=0 )
					{
						fprintf(stderr, "! execute_command: status is not OK: 0x%02x\n", ucStatus);
						switch( ((MONITOR_STATUS_T)ucStatus) )
						{
						case MONITOR_STATUS_Ok:
							fprintf(stderr, "OK\n");
							break;
						case MONITOR_STATUS_CallMessage:
							fprintf(stderr, "CallMessage\n");
							break;
						case MONITOR_STATUS_CallFinished:
							fprintf(stderr, "CallFinished\n");
							break;
						case MONITOR_STATUS_InvalidCommand:
							fprintf(stderr, "InvalidCommand\n");
							break;
						case MONITOR_STATUS_InvalidPacketSize:
							fprintf(stderr, "InvalidPacketSize\n");
							break;
						case MONITOR_STATUS_InvalidSizeParameter:
							fprintf(stderr, "InvalidSizeParameter\n");
							break;
						case MONITOR_STATUS_InvalidSequenceNumber:
							fprintf(stderr, "InvalidSequenceNumber\n");
							break;
						}
						
						if( ((MONITOR_STATUS_T)ucStatus)==MONITOR_STATUS_InvalidSequenceNumber )
						{
							synchronize();
						}
						
						tResult = UARTSTATUS_COMMAND_EXECUTION_FAILED;
					}
					else if( uiSequence!=m_uiMonitorSequence )
					{
						fprintf(stderr, "! execute_command: the sequence does not match: %d / %d\n", uiSequence, m_uiMonitorSequence);
						synchronize();
						tResult = UARTSTATUS_SEQUENCE_MISMATCH;
					}
					else
					{
						tResult = UARTSTATUS_OK;
					}
				}
			}
		}

		if( tResult!=UARTSTATUS_OK )
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

	} while( tResult!=UARTSTATUS_OK );

	return tResult;
}



unsigned char romloader_uart::read_data08(lua_State *ptClientData, unsigned long ulNetxAddress)
{
	unsigned char aucCommand[7];
	UARTSTATUS_T tResult;
	unsigned char ucValue;
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
		/* Get the next sequence number. */
		m_uiMonitorSequence = (m_uiMonitorSequence + 1) & (MONITOR_SEQUENCE_MSK>>MONITOR_SEQUENCE_SRT);

		aucCommand[0] = MONITOR_COMMAND_Read |
		                (MONITOR_ACCESSSIZE_Byte<<MONITOR_ACCESSSIZE_SRT) |
		                (unsigned char)(m_uiMonitorSequence << MONITOR_SEQUENCE_SRT);
		aucCommand[1] = 1;
		aucCommand[2] = 0;
		aucCommand[3] = (unsigned char)( ulNetxAddress       & 0xffU);
		aucCommand[4] = (unsigned char)((ulNetxAddress>> 8U) & 0xffU);
		aucCommand[5] = (unsigned char)((ulNetxAddress>>16U) & 0xffU);
		aucCommand[6] = (unsigned char)((ulNetxAddress>>24U) & 0xffU);
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
	}

	if( fOk!=true )
	{
		MUHKUH_PLUGIN_EXIT_ERROR(ptClientData);
	}

	return ucValue;
}



unsigned short romloader_uart::read_data16(lua_State *ptClientData, unsigned long ulNetxAddress)
{
	unsigned char aucCommand[7];
	UARTSTATUS_T tResult;
	unsigned short usValue;
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
		/* Get the next sequence number. */
		m_uiMonitorSequence = (m_uiMonitorSequence + 1) & (MONITOR_SEQUENCE_MSK>>MONITOR_SEQUENCE_SRT);

		aucCommand[0] = MONITOR_COMMAND_Read |
		                (MONITOR_ACCESSSIZE_Word<<MONITOR_ACCESSSIZE_SRT) |
		                (unsigned char)(m_uiMonitorSequence << MONITOR_SEQUENCE_SRT);
		aucCommand[1] = 2;
		aucCommand[2] = 0;
		aucCommand[3] = (unsigned char)( ulNetxAddress       & 0xffU);
		aucCommand[4] = (unsigned char)((ulNetxAddress>> 8U) & 0xffU);
		aucCommand[5] = (unsigned char)((ulNetxAddress>>16U) & 0xffU);
		aucCommand[6] = (unsigned char)((ulNetxAddress>>24U) & 0xffU);
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
				usValue = ((unsigned long)(m_aucPacketInputBuffer[3])) |
				          ((unsigned long)(m_aucPacketInputBuffer[4]))<<8U;
				fOk = true;
			}
		}
	}

	if( fOk!=true )
	{
		MUHKUH_PLUGIN_EXIT_ERROR(ptClientData);
	}

	return usValue;
}



unsigned long romloader_uart::read_data32(lua_State *ptClientData, unsigned long ulNetxAddress)
{
	unsigned char aucCommand[7];
	UARTSTATUS_T tResult;
	unsigned long ulValue;
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
		/* Get the next sequence number. */
		m_uiMonitorSequence = (m_uiMonitorSequence + 1) & (MONITOR_SEQUENCE_MSK>>MONITOR_SEQUENCE_SRT);

		aucCommand[0] = MONITOR_COMMAND_Read |
		                (MONITOR_ACCESSSIZE_Long<<MONITOR_ACCESSSIZE_SRT) |
		                (unsigned char)(m_uiMonitorSequence << MONITOR_SEQUENCE_SRT);
		aucCommand[1] = 4;
		aucCommand[2] = 0;
		aucCommand[3] = (unsigned char)( ulNetxAddress       & 0xffU);
		aucCommand[4] = (unsigned char)((ulNetxAddress>> 8U) & 0xffU);
		aucCommand[5] = (unsigned char)((ulNetxAddress>>16U) & 0xffU);
		aucCommand[6] = (unsigned char)((ulNetxAddress>>24U) & 0xffU);
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
				ulValue = ((unsigned long)(m_aucPacketInputBuffer[3]))      |
				          ((unsigned long)(m_aucPacketInputBuffer[4]))<< 8U |
				          ((unsigned long)(m_aucPacketInputBuffer[5]))<<16U |
				          ((unsigned long)(m_aucPacketInputBuffer[6]))<<24U;
				fOk = true;
			}
		}
	}

	if( fOk!=true )
	{
		MUHKUH_PLUGIN_EXIT_ERROR(ptClientData);
	}

	return ulValue;
}



void romloader_uart::read_image(unsigned long ulNetxAddress, unsigned long ulSize, char **ppcBUFFER_OUT, size_t *psizBUFFER_OUT, SWIGLUA_REF tLuaFn, long lCallbackUserData)
{
	char *pcBufferStart;
	char *pcBuffer;
	size_t sizBuffer;
	bool fOk;
	size_t sizChunk;
	UARTSTATUS_T tResult;
	unsigned char aucCommand[sizMaxPacketSizeHost];
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
				if( sizChunk>m_sizMaxPacketSizeClient-6 )
				{
					sizChunk = m_sizMaxPacketSizeClient-6;
				}

				/* Get the next sequence number. */
				m_uiMonitorSequence = (m_uiMonitorSequence + 1) & (MONITOR_SEQUENCE_MSK>>MONITOR_SEQUENCE_SRT);

				aucCommand[0] = MONITOR_COMMAND_Read |
				                (MONITOR_ACCESSSIZE_Byte<<MONITOR_ACCESSSIZE_SRT) |
				                (unsigned char)(m_uiMonitorSequence << MONITOR_SEQUENCE_SRT);
				aucCommand[1] = (unsigned char)( sizChunk       & 0xffU);
				aucCommand[2] = (unsigned char)((sizChunk>> 8U) & 0xffU);
				aucCommand[3] = (unsigned char)( ulNetxAddress       & 0xffU);
				aucCommand[4] = (unsigned char)((ulNetxAddress>> 8U) & 0xffU);
				aucCommand[5] = (unsigned char)((ulNetxAddress>>16U) & 0xffU);
				aucCommand[6] = (unsigned char)((ulNetxAddress>>24U) & 0xffU);

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



void romloader_uart::write_data08(lua_State *ptClientData, unsigned long ulNetxAddress, unsigned char ucData)
{
	unsigned char aucCommand[8];
	UARTSTATUS_T tResult;
	unsigned long ulValue;
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
		/* Get the next sequence number. */
		m_uiMonitorSequence = (m_uiMonitorSequence + 1) & (MONITOR_SEQUENCE_MSK>>MONITOR_SEQUENCE_SRT);

		aucCommand[0] = MONITOR_COMMAND_Write |
		                (MONITOR_ACCESSSIZE_Byte<<MONITOR_ACCESSSIZE_SRT) |
		                (unsigned char)(m_uiMonitorSequence << MONITOR_SEQUENCE_SRT);
		aucCommand[1] = 1;
		aucCommand[2] = 0;
		aucCommand[3] = (unsigned char)( ulNetxAddress       & 0xffU);
		aucCommand[4] = (unsigned char)((ulNetxAddress>> 8U) & 0xffU);
		aucCommand[5] = (unsigned char)((ulNetxAddress>>16U) & 0xffU);
		aucCommand[6] = (unsigned char)((ulNetxAddress>>24U) & 0xffU);
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
	}

	if( fOk!=true )
	{
		MUHKUH_PLUGIN_EXIT_ERROR(ptClientData);
	}
}


void romloader_uart::write_data16(lua_State *ptClientData, unsigned long ulNetxAddress, unsigned short usData)
{
	unsigned char aucCommand[9];
	UARTSTATUS_T tResult;
	unsigned long ulValue;
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
		/* Get the next sequence number. */
		m_uiMonitorSequence = (m_uiMonitorSequence + 1) & (MONITOR_SEQUENCE_MSK>>MONITOR_SEQUENCE_SRT);

		aucCommand[0] = MONITOR_COMMAND_Write |
		                (MONITOR_ACCESSSIZE_Word<<MONITOR_ACCESSSIZE_SRT) |
		                (unsigned char)(m_uiMonitorSequence << MONITOR_SEQUENCE_SRT);
		aucCommand[1] = 2;
		aucCommand[2] = 0;
		aucCommand[3] = (unsigned char)( ulNetxAddress       & 0xffU);
		aucCommand[4] = (unsigned char)((ulNetxAddress>> 8U) & 0xffU);
		aucCommand[5] = (unsigned char)((ulNetxAddress>>16U) & 0xffU);
		aucCommand[6] = (unsigned char)((ulNetxAddress>>24U) & 0xffU);
		aucCommand[7] = (unsigned char)( usData     & 0xffU);
		aucCommand[8] = (unsigned char)((usData>>8U)& 0xffU);
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
	}

	if( fOk!=true )
	{
		MUHKUH_PLUGIN_EXIT_ERROR(ptClientData);
	}
}



void romloader_uart::write_data32(lua_State *ptClientData, unsigned long ulNetxAddress, unsigned long ulData)
{
	unsigned char aucCommand[11];
	UARTSTATUS_T tResult;
	unsigned long ulValue;
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
		/* Get the next sequence number. */
		m_uiMonitorSequence = (m_uiMonitorSequence + 1) & (MONITOR_SEQUENCE_MSK>>MONITOR_SEQUENCE_SRT);

		aucCommand[0]  = MONITOR_COMMAND_Write |
		                 (MONITOR_ACCESSSIZE_Long<<MONITOR_ACCESSSIZE_SRT) |
		                 (unsigned char)(m_uiMonitorSequence << MONITOR_SEQUENCE_SRT);
		aucCommand[1]  = 4;
		aucCommand[2]  = 0;
		aucCommand[3]  = (unsigned char)( ulNetxAddress       & 0xffU);
		aucCommand[4]  = (unsigned char)((ulNetxAddress>> 8U) & 0xffU);
		aucCommand[5]  = (unsigned char)((ulNetxAddress>>16U) & 0xffU);
		aucCommand[6]  = (unsigned char)((ulNetxAddress>>24U) & 0xffU);
		aucCommand[7]  = (unsigned char)( ulData       & 0xffU);
		aucCommand[8]  = (unsigned char)((ulData>> 8U) & 0xffU);
		aucCommand[9]  = (unsigned char)((ulData>>16U) & 0xffU);
		aucCommand[10] = (unsigned char)((ulData>>24U) & 0xffU);
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
	}

	if( fOk!=true )
	{
		MUHKUH_PLUGIN_EXIT_ERROR(ptClientData);
	}
}



void romloader_uart::write_image(unsigned long ulNetxAddress, const char *pcBUFFER_IN, size_t sizBUFFER_IN, SWIGLUA_REF tLuaFn, long lCallbackUserData)
{
	bool fOk;
	size_t sizChunk;
	UARTSTATUS_T tResult;
	unsigned char aucCommand[sizMaxPacketSizeHost];
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
			                (unsigned char)(m_uiMonitorSequence << MONITOR_SEQUENCE_SRT);
			aucCommand[1] = (unsigned char)( sizChunk       & 0xffU);
			aucCommand[2] = (unsigned char)((sizChunk>> 8U) & 0xffU);
			aucCommand[3] = (unsigned char)( ulNetxAddress       & 0xffU);
			aucCommand[4] = (unsigned char)((ulNetxAddress>> 8U) & 0xffU);
			aucCommand[5] = (unsigned char)((ulNetxAddress>>16U) & 0xffU);
			aucCommand[6] = (unsigned char)((ulNetxAddress>>24U) & 0xffU);
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
	}

	if( fOk!=true )
	{
		MUHKUH_PLUGIN_EXIT_ERROR(tLuaFn.L);
	}
}



void romloader_uart::call(unsigned long ulNetxAddress, unsigned long ulParameterR0, SWIGLUA_REF tLuaFn, long lCallbackUserData)
{
	bool fOk;
	UARTSTATUS_T tResult;
	unsigned char aucCommand[9];
	const unsigned char aucCancelBuf[1] = { 0x2b };
	unsigned char ucStatus;
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
		/* Get the next sequence number. */
		m_uiMonitorSequence = (m_uiMonitorSequence + 1) & (MONITOR_SEQUENCE_MSK>>MONITOR_SEQUENCE_SRT);

		/* Construct the command packet. */
		aucCommand[0x00] = MONITOR_COMMAND_Execute |
		                   (unsigned char)(m_uiMonitorSequence << MONITOR_SEQUENCE_SRT);
		aucCommand[0x01] = (unsigned char)( ulNetxAddress      & 0xffU);
		aucCommand[0x02] = (unsigned char)((ulNetxAddress>>8 ) & 0xffU);
		aucCommand[0x03] = (unsigned char)((ulNetxAddress>>16) & 0xffU);
		aucCommand[0x04] = (unsigned char)((ulNetxAddress>>24) & 0xffU);
		aucCommand[0x05] = (unsigned char)( ulParameterR0      & 0xffU);
		aucCommand[0x06] = (unsigned char)((ulParameterR0>>8 ) & 0xffU);
		aucCommand[0x07] = (unsigned char)((ulParameterR0>>16) & 0xffU);
		aucCommand[0x08] = (unsigned char)((ulParameterR0>>24) & 0xffU);

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

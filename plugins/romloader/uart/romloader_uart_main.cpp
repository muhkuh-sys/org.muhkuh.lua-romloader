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
#include "romloader_uart_read_functinoid_mi2.h"

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
	/* get the romloader_uart lua type */
	m_ptPluginTypeInfo = p_romloader_uart;
	m_ptReferenceTypeInfo = p_romloader_uart_reference;
}



romloader_uart_provider::~romloader_uart_provider(void)
{
}



int romloader_uart_provider::DetectInterfaces(lua_State *ptLuaStateForTableAccess, lua_State *ptLuaStateForTableAccessOptional)
{
	size_t sizDeviceNames;
	char **ppcDeviceNames;
	char **ppcDeviceNamesCnt;
	char **ppcDeviceNamesEnd;
	romloader_uart_reference *ptReference;
	bool fDeviceIsBusy;


	/* Process the plugin options. */
	if( ptLuaStateForTableAccessOptional!=NULL )
	{
		processOptions(ptLuaStateForTableAccessOptional, 3);
	}

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
		m_ptLog->error("claim_interface(): missing reference!");
	}
	else
	{
		pcName = ptReference->GetName();
		if( pcName==NULL )
		{
			m_ptLog->error("claim_interface(): missing name!");
		}
		else if( sscanf(pcName, m_pcPluginNamePattern, acDevice)!=1 )
		{
			m_ptLog->error("claim_interface(): invalid name: %s", pcName);
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
		m_ptLog->error("release_interface(): missing plugin!");
	}
	else
	{
		pcName = ptPlugin->GetName();
		if( pcName==NULL )
		{
			m_ptLog->error("release_interface(): missing name!");
		}
		else if( sscanf(pcName, m_pcPluginNamePattern, acDevice)!=1 )
		{
			m_ptLog->error("release_interface(): invalid name: %s", pcName);
		}
		else
		{
			m_ptLog->debug("released interface %s.", pcName);
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


bool romloader_uart::identify_loader(ROMLOADER_COMMANDSET_T *ptCmdSet, romloader_uart_read_functinoid_mi2 *ptFnMi2)
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
	unsigned int uiSequence;
	size_t sizMaxPacketSizeClient;


	/* The command set is unknown by default. */
	tCmdSet = ROMLOADER_COMMANDSET_UNKNOWN;

	/* Send knock sequence with 500 milliseconds timeout. */
	if( m_ptUartDev->SendRaw(aucKnock, sizeof(aucKnock), 500)!=sizeof(aucKnock) )
	{
		/* Failed to send knock sequence to device. */
		m_ptLog->error("Failed to send knock sequence to device.");
	}
	else if( m_ptUartDev->Flush()!=true )
	{
		/* Failed to flush the knock sequence. */
		m_ptLog->error("Failed to flush the knock sequence.");
	}
	else
	{
		sizTransfered = m_ptUartDev->RecvRaw(aucData, 1, 1000);
		if( sizTransfered!=1 )
		{
			/* Failed to receive first char of knock response. */
			m_ptLog->error("Failed to receive first char of knock response: %ld.", sizTransfered);
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
					m_ptLog->error("Failed to receive the size information after the stream packet start!");
				}
				else
				{
					if( aucData[0]=='*' && aucData[1]=='#' )
					{
						m_ptLog->debug("OK, received '*#'!");
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
							m_ptLog->error("Failed to receive the rest of the knock response after 0x00 0x00!");
						}
						else if( aucData[2]=='*' && aucData[3]=='#' )
						{
							m_ptLog->debug("OK, received '<null><null>*#'!");
							fResult = m_ptUartDev->SendBlankLineAndDiscardResponse();
							if( fResult==true )
							{
								tCmdSet = ROMLOADER_COMMANDSET_ABOOT_OR_HBOOT1;
							}
						}
						else
						{
							m_ptLog->error("Received strange response after 0x00 0x00.");
							m_ptLog->hexdump(muhkuh_log::MUHKUH_LOG_LEVEL_DEBUG, aucData+2, 2);
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
							m_ptLog->error("The received packet is too small. It must be at least 11 bytes, but it is %ld bytes.", sizPacket);
							m_ptLog->hexdump(muhkuh_log::MUHKUH_LOG_LEVEL_DEBUG, aucData, sizPacket);
						}
						else if( sizPacket>(sizeof(aucData)-2) )
						{
							m_ptLog->error("The received packet is too big for a buffer of %ld bytes. It has %ld bytes.", sizeof(aucData)-2, sizPacket);
						}
						else
						{
							/* Get rest of the response. */
							sizTransfered = m_ptUartDev->RecvRaw(aucData+2, sizPacket, 500);
							if( sizTransfered!=sizPacket )
							{
								m_ptLog->error("Failed to receive the rest of the packet after the size information!");
							}
							else
							{
								m_ptLog->debug("Got %zd bytes.", sizPacket+2);
								m_ptLog->hexdump(muhkuh_log::MUHKUH_LOG_LEVEL_DEBUG, aucData, sizPacket+2);

								/* Is this a MI V1 sync packet? */
								if( memcmp(aucData+3, aucMagicMooh, sizeof(aucMagicMooh))==0 )
								{
									ucStatus = aucData[2];
									if( ucStatus!=MONITOR_STATUS_Ok )
									{
										m_ptLog->error("The status of the response is not OK but 0x%02x.", ucStatus);
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
											m_ptLog->error("CRC of version packet is invalid!");
											m_ptLog->hexdump(muhkuh_log::MUHKUH_LOG_LEVEL_DEBUG, aucData, sizPacket);
										}
										else
										{
											m_ptLog->hexdump(muhkuh_log::MUHKUH_LOG_LEVEL_DEBUG, aucData, sizPacket);
											ulMiVersionMin = (uint32_t)(aucData[7] | (aucData[8]<<8));
											ulMiVersionMaj = (uint32_t)(aucData[9] | (aucData[10]<<8));
											m_ptLog->debug("Found new machine interface V%d.%d.\n", ulMiVersionMaj, ulMiVersionMin);

											if( ulMiVersionMaj==1 )
											{
												tCmdSet = ROMLOADER_COMMANDSET_MI1;
												fResult = true;
											}
											else if( ulMiVersionMaj==2 )
											{
												tCmdSet = ROMLOADER_COMMANDSET_MI2;

												/* Get the sequence number. */
												uiSequence = aucData[2];

												/* Get the maximum packet size. */
												sizMaxPacketSizeClient = (size_t)(aucData[12] | (aucData[13]<<8U));;

												ptFnMi2->set_sync_data(uiSequence, sizMaxPacketSizeClient);


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
												m_ptLog->error("Unknown machine interface version %d.%d", ulMiVersionMaj, ulMiVersionMin);
											}
										}
									}
								}
								/* Is this a MI V2 sync packet? */
								else if( memcmp(aucData+4, aucMagicMooh, sizeof(aucMagicMooh))==0 )
								{
									ucStatus = aucData[3];
									if( ucStatus!=MONITOR_STATUS_Ok )
									{
										m_ptLog->error("The status of the response is not OK but 0x%02x.", ucStatus);
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
											m_ptLog->error("CRC of version packet is invalid!");
											m_ptLog->hexdump(muhkuh_log::MUHKUH_LOG_LEVEL_DEBUG, aucData, sizPacket);
										}
										else
										{
											ulMiVersionMin = (uint32_t)(aucData[8] | (aucData[9]<<8));
											ulMiVersionMaj = (uint32_t)(aucData[10] | (aucData[11]<<8));
											m_ptLog->debug("Found new machine interface V%d.%d.", ulMiVersionMaj, ulMiVersionMin);

/* These combinations are invalid.
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
												m_ptLog->error("Unknown machine interface version %d.%d", ulMiVersionMaj, ulMiVersionMin);
											}
										}
									}
								}
								else
								{
									m_ptLog->error("The response packet has no MOOH magic!");
								}
							}
						}
					}
				}
			}
			else if( aucData[0]=='#' )
			{
				m_ptLog->debug("OK, received hash!");

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



void romloader_uart::Connect(lua_State *ptClientData)
{
	romloader_uart_read_functinoid *ptFn;
	romloader_uart_read_functinoid_aboot tFnABoot(m_ptUartDev, m_pcName);
	romloader_uart_read_functinoid_hboot1 tFnHBoot1(m_ptUartDev, m_pcName);
	romloader_uart_read_functinoid_mi1 tFnMi1(m_ptUartDev, m_pcName);
	romloader_uart_read_functinoid_mi2 tFnMi2(m_ptUartDev, m_pcName);
	bool fResult;
	ROMLOADER_COMMANDSET_T tCmdSet;
	int iResult;
	ROMLOADER_CHIPTYP tChiptyp;


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
			fResult = identify_loader(&tCmdSet, &tFnMi2);
			if( fResult!=true )
			{
				MUHKUH_PLUGIN_PUSH_ERROR(ptClientData, "%s(%p): failed to identify loader!", m_pcName, this);
			}
			else
			{
				switch(tCmdSet)
				{
				case ROMLOADER_COMMANDSET_UNKNOWN:
					m_ptLog->error("Unknown command set.");
					fResult = false;
					break;


				case ROMLOADER_COMMANDSET_ABOOT_OR_HBOOT1:
					m_ptLog->debug("ABOOT or HBOOT1.");
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
					m_ptLog->debug("Command set MI1.");
					/* Try to detect the chip type with the old MI command set. */
					ptFn = &tFnMi1;
					fResult = detect_chiptyp(ptFn);
					if( fResult==true )
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
					m_ptLog->debug("Command set MI2.");
					ptFn = &tFnMi2;
					fResult = detect_chiptyp(ptFn);
					if( fResult==true )
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


				case ROMLOADER_COMMANDSET_MI3:
					m_ptLog->debug("The device does not need an update.");
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
					fResult = synchronize(&tChiptyp);
					if( fResult==true )
					{
						/* The chip type reported by the ROM code of the netX 90 MPW/Rev0/Rev1 is incorrect:
						 * netx90 MPW  reports MI V2, chip type netX90 MPW  (0x02 0x0a)
						 * netx90 Rev0 reports MI V3, chip type netX90 MPW  (0x03 0x0a)
						 * next90 Rev1 reports MI V3, chip type netX90 Rev0 (0x03 0x0d)
						 */
						if( tChiptyp==ROMLOADER_CHIPTYP_NETX90_MPW 
							|| tChiptyp==ROMLOADER_CHIPTYP_NETX90
							|| tChiptyp==ROMLOADER_CHIPTYP_NETX90C
							|| tChiptyp==ROMLOADER_CHIPTYP_NETX90C_INTRAM
							|| tChiptyp==ROMLOADER_CHIPTYP_NETX90B)
						{
							m_ptLog->debug("Got suspicious chip type %d, detecting chip type.", tChiptyp);
							fResult = detect_chiptyp();
						}
						else
						{
							m_tChiptyp = tChiptyp;
						}
					}
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
	m_ptLog->debug("disconnect");

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
			if( sizWritePosition>=m_sizMaxPacketSizeHost )
			{
				sizWritePosition -= m_sizMaxPacketSizeHost;
			}

			/* Get the size of the remaining continuous buffer space. */
			/* This is the maximum chunk which can be read in one piece. */
			sizChunk = m_sizMaxPacketSizeHost - sizWritePosition;
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
	if( m_sizPacketRingBufferHead>=m_sizMaxPacketSizeHost )
	{
		m_sizPacketRingBufferHead -= m_sizMaxPacketSizeHost;
	}

	--m_sizPacketRingBufferFill;

	return ucByte;
}



int romloader_uart::packet_ringbuffer_peek(size_t sizOffset)
{
	size_t sizReadPosition;


	sizReadPosition = m_sizPacketRingBufferHead + sizOffset;
	if( sizReadPosition>=m_sizMaxPacketSizeHost )
	{
		sizReadPosition -= m_sizMaxPacketSizeHost;
	}

	return m_aucPacketRingBuffer[sizReadPosition];
}



void romloader_uart::packet_ringbuffer_skip(size_t sizSkip)
{
	size_t sizReadPosition;
	size_t sizFill;


	/* Are enough chars in the ring buffer? */
	sizFill = m_sizPacketRingBufferFill;
	if( sizSkip>sizFill )
	{
		/* Not enough chars left. Limit the number of bytes to skip. */
		sizSkip = sizFill;
	}

	/* Move the read position and wrap around at the end of the buffer. */
	sizReadPosition = m_sizPacketRingBufferHead + sizSkip;
	if( sizReadPosition>=m_sizMaxPacketSizeHost )
	{
		sizReadPosition -= m_sizMaxPacketSizeHost;
	}
	m_sizPacketRingBufferHead = sizReadPosition;

	/* The skipped bytes are not in use anymore. */
	sizFill -= sizSkip;
	m_sizPacketRingBufferFill = sizFill;
}



void romloader_uart::packet_ringbuffer_discard(void)
{
	if( m_sizPacketRingBufferFill>0 )
	{
		m_ptLog->warning("discarding %ld bytes in ring buffer!", m_sizPacketRingBufferFill);
	}
	packet_ringbuffer_init();
}



romloader::TRANSPORTSTATUS_T romloader_uart::send_raw_packet(const void *pvPacket, size_t sizPacket)
{
	TRANSPORTSTATUS_T tResult;
	size_t sizSend;


	if( m_ptUartDev==NULL )
	{
		tResult = TRANSPORTSTATUS_NOT_CONNECTED;
	}
	else
	{
		sizSend = m_ptUartDev->SendRaw(pvPacket, sizPacket, UART_BASE_TIMEOUT_MS + sizPacket*UART_CHAR_TIMEOUT_MS);
		if( sizSend==sizPacket )
		{
			tResult = TRANSPORTSTATUS_OK;
		}
		else
		{
			m_ptLog->error("send_raw_packet: failed to send the packet!");
			tResult = TRANSPORTSTATUS_SEND_FAILED;
		}
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
	size_t sizPacketWithoutStartChar;
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
				m_aucPacketInputBuffer[0] = MONITOR_STREAM_PACKET_START;
				fFound = true;
				break;
			}
			else
			{
				m_ptLog->warning("Skipping char 0x%02x.", ucData);
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
		m_ptLog->debug("receive_packet: no start char found!");
		tResult = TRANSPORTSTATUS_FAILED_TO_SYNC;
	}
	else
	{
//		fprintf(stderr, "Got start char\n");

		/* Get the size information. */
		tResult = packet_ringbuffer_fill(2);
		if( tResult==TRANSPORTSTATUS_OK )
		{
			/* NOTE: Do not store the size information in the receive buffer here.
			 *       This will be done later when the CRC is calculated.
			 */
			ucData = packet_ringbuffer_peek(0);
			sizData  =  (size_t)ucData;
			ucData = packet_ringbuffer_peek(1);
			sizData |= ((size_t)ucData) << 8U;
//			fprintf(stderr, "Expected packet size: %zd bytes\n", sizData);

			/* The complete packet consists of...
			 *   1 byte start char
			 *   2 bytes packet size
			 *   the user data including the sequence number and the packet type
			 *   2 bytes CRC
			 */
			sizPacket = 1U + 2U + sizData + 2U;

			/* Get the rest of the packet.
			 * NOTE: 1 byte is subtracted as the start char was already taken from the buffer.
			 */
			sizPacketWithoutStartChar = sizPacket - 1U;
			tResult = packet_ringbuffer_fill(sizPacketWithoutStartChar);
			if( tResult==TRANSPORTSTATUS_OK )
			{
				/* Generate the CRC and store the complete packet in the receive buffer. */
				usCrc = 0;
				sizCnt = 0;
				do
				{
					ucData = packet_ringbuffer_peek(sizCnt);
					m_aucPacketInputBuffer[1U + sizCnt] = ucData;
					usCrc = crc16(usCrc, ucData);
					++sizCnt;
				} while( sizCnt<sizPacketWithoutStartChar );
				if( usCrc==0 )
				{
					/* Skip the complete packet.
					 * NOTE: 1 byte is subtracted as the start char was already taken from the buffer.
					 */
					packet_ringbuffer_skip(sizPacket - 1U);

					m_sizPacketInputBuffer = sizPacket;
				}
				else
				{
					m_ptLog->error("receive_packet: CRC failed.");

					/* Debug: get the complete packet and dump it.
					 * NOTE: Do not remove the data from the buffer.
					 */
					m_ptLog->debug("packet size: 0x%08lx bytes", sizPacket);
					m_ptLog->debug("Packet data:");
					m_ptLog->hexdump(muhkuh_log::MUHKUH_LOG_LEVEL_DEBUG, m_aucPacketInputBuffer, sizPacket);

					tResult = TRANSPORTSTATUS_CRC_MISMATCH;
				}
			}
			else
			{
				m_ptLog->error("receive_packet: Failed to get 0x%02lx bytes of packet data info: %d", sizPacket, tResult);
			}
		}
		else
		{
			m_ptLog->error("receive_packet: Failed to get size info: %d", tResult);
		}
	}

	return tResult;
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

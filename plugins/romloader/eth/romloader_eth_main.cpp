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
#include <stddef.h>
#include <stdio.h>


#include "romloader_eth_main.h"

#define ETH_REPLY_TIMEOUT_MS 500

#if defined(_MSC_VER)
#       define snprintf _snprintf
#endif


/*-------------------------------------*/

const char *romloader_eth_provider::m_pcPluginNamePattern = "romloader_eth_%s";

romloader_eth_provider::romloader_eth_provider(swig_type_info *p_romloader_eth, swig_type_info *p_romloader_eth_reference)
 : muhkuh_plugin_provider("romloader_eth")
{
	/* get the romloader_eth lua type */
	m_ptPluginTypeInfo = p_romloader_eth;
	m_ptReferenceTypeInfo = p_romloader_eth_reference;
}


romloader_eth_provider::~romloader_eth_provider(void)
{
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


void romloader_eth::Connect(lua_State *ptClientData)
{
	int iResult;
	bool fResult;


	/* Expect error. */
	iResult = -1;

	printf("%s(%p): connect\n", m_pcName, this);

	if( m_ptEthDev!=NULL && m_fIsConnected==false )
	{
		if( m_ptEthDev->Open()!=true )
		{
			MUHKUH_PLUGIN_PUSH_ERROR(ptClientData, "%s(%p): failed to open device!", m_pcName, this);
		}
		else
		{
			/* Set a default maximum packet size which should be enough for a sync packet. */
			m_sizMaxPacketSizeClient = 32;
			fResult = synchronize();
			if( fResult==true )
			{
				m_fIsConnected = true;
				iResult = 0;
			}
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



romloader::TRANSPORTSTATUS_T romloader_eth::send_raw_packet(const void *pvPacket, size_t sizPacket)
{
	TRANSPORTSTATUS_T tResult;


	if( m_ptEthDev==NULL )
	{
		tResult = TRANSPORTSTATUS_NOT_CONNECTED;
	}
	else
	{
		tResult = m_ptEthDev->SendPacket(pvPacket, sizPacket);
		if( tResult!=TRANSPORTSTATUS_OK )
		{
			fprintf(stderr, "! send_packet: failed to send the packet!\n");
		}
	}

	return tResult;
}



romloader::TRANSPORTSTATUS_T romloader_eth::receive_packet(void)
{
	unsigned int uiRetries;
	uint8_t ucData;
	TRANSPORTSTATUS_T tResult;
	size_t sizData;
	size_t sizCompletePacket;
	uint16_t usCrc;
	size_t sizCnt;
	MIV3_PACKET_HEADER_T *ptPacket;


//	fprintf(stderr, "receive_packet\n");
	uiRetries = 8;
	do
	{
		tResult = m_ptEthDev->RecvPacket(m_aucPacketInputBuffer, sizeof(m_aucPacketInputBuffer), 1000, &m_sizPacketInputBuffer);
		if( tResult==TRANSPORTSTATUS_OK )
		{
			/* The ACK packet is the smallest packet possible. */
			if( m_sizPacketInputBuffer<sizeof(MIV3_PACKET_ACK_T) )
			{
				fprintf(stderr, "Packet too small. Packet size: %zd, minimum packet size: %zd.\n", m_sizPacketInputBuffer, sizeof(MIV3_PACKET_ACK_T));
				tResult = TRANSPORTSTATUS_PACKET_TOO_SMALL;
			}
			/* The packet size must not exceed the negotiated maximum. */
			else if( m_sizPacketInputBuffer>m_sizMaxPacketSizeClient )
			{
				fprintf(stderr, "Packet too large. Packet size: %zd, maximum packet size: %zd.\n", m_sizPacketInputBuffer, m_sizMaxPacketSizeClient);
				tResult = TRANSPORTSTATUS_PACKET_TOO_LARGE;
			}
			else
			{
				ptPacket = (MIV3_PACKET_HEADER_T*)m_aucPacketInputBuffer;

				/* The packet must start with the stream start character. */
				ucData = ptPacket->s.ucStreamStart;
				if( ucData!=MONITOR_STREAM_PACKET_START )
				{
					fprintf(stderr, "receive_packet: no start char found!\n");
					tResult = TRANSPORTSTATUS_FAILED_TO_SYNC;
				}
				else
				{
					/* Get the size information. */
					sizData = ptPacket->s.usDataSize;

					/* The size information covers only the sequence number, the type and the user data.
					 * The complete package has the following additional fields:
					 *   1 byte start char
					 *   2 bytes packet size
					 *   2 bytes CRC
					 */
					sizCompletePacket = sizData + 5U;
					if( m_sizPacketInputBuffer<sizCompletePacket )
					{
						fprintf(stderr, "The size information in the packet does not match the number of received bytes.\n");
						fprintf(stderr, "Received %zd bytes. Expected packet size: %zd bytes\n", m_sizPacketInputBuffer, sizCompletePacket);
						tResult = TRANSPORTSTATUS_INVALID_PACKET_SIZE;
					}
					else
					{
						/* Generate the CRC. */
						usCrc = 0;
						sizCnt = offsetof(struct MIV3_PACKET_HEADER_STRUCT, usDataSize);
						do
						{
							usCrc = crc16(usCrc, ptPacket->auc[sizCnt]);
							++sizCnt;
						} while( sizCnt<sizCompletePacket );
						if( usCrc==0 )
						{
							tResult = TRANSPORTSTATUS_OK;
						}
						else
						{
							fprintf(stderr, "! receive_packet: CRC failed.\n");

							/* Debug: get the complete packet and dump it.
							 * NOTE: Do not remove the data from the buffer.
							 */
							fprintf(stderr, "raw packet size: 0x%08lx bytes\n", m_sizPacketInputBuffer);
							hexdump(m_aucPacketInputBuffer, m_sizPacketInputBuffer);

							tResult = TRANSPORTSTATUS_CRC_MISMATCH;
						}
					}
				}
			}
		}
		else
		{
			fprintf(stderr, "receive_packet: Failed to receive a packet: %d\n", tResult);
		}

		if( tResult!=TRANSPORTSTATUS_OK )
		{
			/* Are some retries left? */
			if( uiRetries==0 )
			{
				/* No more retries... */
				fprintf(stderr, "No more retries left, giving up.\n");
				break;
			}
			else
			{
				--uiRetries;
				fprintf(stderr, "Retry\n");
			}
		}
	} while( tResult!=TRANSPORTSTATUS_OK );

	return tResult;
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

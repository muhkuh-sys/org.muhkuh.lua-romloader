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
 , m_fIsConnected(false)
 , m_ptEthDev(NULL)
 , m_ucMonitorSequence(0)
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



romloader::TRANSPORTSTATUS_T romloader_eth::send_packet(MIV3_PACKET_HEADER_T *ptPacket, size_t sizPacket)
{
	TRANSPORTSTATUS_T tResult;
	uint16_t usCrc;
	size_t sizUserData;
	uint8_t *pucCnt;
	uint8_t *pucEnd;


	/* The maximum packet size must not exceed the maximum size negotiated with the client. */
	if( sizPacket>m_sizMaxPacketSizeClient )
	{
		fprintf(stderr, "! send_packet: packet too large: %zd bytes! Maximum size is %zd.\n", m_sizMaxPacketSizeClient, m_sizMaxPacketSizeClient);
		tResult = TRANSPORTSTATUS_PACKET_TOO_LARGE;
	}
	else if( sizPacket<sizeof(MIV3_PACKET_HEADER_T) )
	{
		fprintf(stderr, "! send_packet: packet too small: %zd bytes! Minimum size is %zd.\n", m_sizMaxPacketSizeClient, sizeof(MIV3_PACKET_HEADER_T));
		tResult = TRANSPORTSTATUS_PACKET_TOO_SMALL;
	}
	else
	{
		/* Set the packet start character. */
		ptPacket->s.ucStreamStart = MONITOR_STREAM_PACKET_START;

		/* Set the size of the user data. The user data is the packet size minus...
		 *   1 byte start char
		 *   2 bytes packet size
		 *   2 bytes for the CRC
		 */
		sizUserData = sizPacket - 5U;
		ptPacket->s.usDataSize = sizUserData;

		/* Copy the sequence number. */
		ptPacket->s.ucSequenceNumber = m_ucMonitorSequence;

		/* Generate the CRC. */
		usCrc = 0;
		pucCnt = ptPacket->auc + offsetof(struct MIV3_PACKET_HEADER_STRUCT, usDataSize);
		pucEnd = ptPacket->auc + sizPacket - 2U;
		while( pucCnt<pucEnd )
		{
			usCrc = crc16(usCrc, *pucCnt);
			++pucCnt;
		}

		/* Append the CRC. */
		*(pucCnt++) = (uint8_t)((usCrc>>8U) & 0xffU);
		*(pucCnt++) = (uint8_t)( usCrc      & 0xffU);

		/* Send the buffer. */
		tResult = m_ptEthDev->SendPacket(ptPacket->auc, sizPacket);
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


	fprintf(stderr, "receive_packet\n");
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



romloader::TRANSPORTSTATUS_T romloader_eth::send_sync_command(void)
{
	const uint8_t aucKnock[5] = { '*', 0x00, 0x00, '*', '#' };
	size_t sizSend;
	TRANSPORTSTATUS_T tResult;
	uint8_t ucStatus;
	unsigned int uiRetryCnt;
	int iResult;


	uiRetryCnt = 10;
	do
	{
		tResult = m_ptEthDev->SendPacket(aucKnock, sizeof(aucKnock));
		if( tResult!=TRANSPORTSTATUS_OK )
		{
			fprintf(stderr, "! send_sync_command: failed to send the packet!\n");
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
				tResult = TRANSPORTSTATUS_OK;
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



bool romloader_eth::synchronize(void)
{
	const uint8_t aucMagicMooh[4] = { 0x4d, 0x4f, 0x4f, 0x48 };
	TRANSPORTSTATUS_T tResult;
	bool fResult;
	uint8_t ucSequence;
	uint32_t ulMiVersionMin;
	uint32_t ulMiVersionMaj;
	ROMLOADER_CHIPTYP tChipType;
	size_t sizMaxPacketSize;
	MIV3_PACKET_SYNC_T *ptSyncPacket;


	fprintf(stderr, "synchronize\n");
	fResult = false;

	/* Get the received packet as a sync packet for later. */
	ptSyncPacket = (MIV3_PACKET_SYNC_T*)m_aucPacketInputBuffer;

	tResult = send_sync_command();
	if( tResult!=TRANSPORTSTATUS_OK )
	{
		/* Failed to send knock sequence to device. */
		fprintf(stderr, "Failed to send knock sequence to device.\n");
	}
	else if( m_sizPacketInputBuffer==sizeof(MIV3_PACKET_SYNC_T) && memcmp(ptSyncPacket->s.aucMacic, aucMagicMooh, sizeof(aucMagicMooh))==0 )
	{
		fprintf(stderr, "Packet:\n");
		hexdump(m_aucPacketInputBuffer, m_sizPacketInputBuffer);

		/* Get the sequence number. */
		ucSequence = ptSyncPacket->s.tHeader.s.ucSequenceNumber;
		fprintf(stderr, "Sequence number: 0x%02x\n", ucSequence);

		ulMiVersionMin = NETXTOH16(ptSyncPacket->s.usVersionMinor);
		ulMiVersionMaj = NETXTOH16(ptSyncPacket->s.usVersionMajor);
		printf("Machine interface V%d.%d.\n", ulMiVersionMaj, ulMiVersionMin);

		tChipType = (ROMLOADER_CHIPTYP)(ptSyncPacket->s.ucChipType);
		printf("Chip type : %d\n", tChipType);

		/* sizMaxPacketSizeClient = min(max. packet size from response, sizMaxPacketSizeHost) */
		sizMaxPacketSize = NETXTOH16(ptSyncPacket->s.usMaximumPacketSize);
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


romloader::TRANSPORTSTATUS_T romloader_eth::send_ack(unsigned char ucSequenceToAck)
{
	TRANSPORTSTATUS_T tResult;
	MIV3_PACKET_ACK_T tPacketAck;
	uint16_t usCrc;
	size_t sizCnt;


//	fprintf(stderr, "Sending ACK for sequence number %d.\n", ucSequenceToAck);

	/* Set the packet start character. */
	tPacketAck.s.tHeader.s.ucStreamStart = MONITOR_STREAM_PACKET_START;

	/* Set the size of the user data.
	 *  1 byte sequence number
	 *  1 byte packet type
	 */
	tPacketAck.s.tHeader.s.usDataSize = 2;

	/* Set the sequence number. */
	tPacketAck.s.tHeader.s.ucSequenceNumber = ucSequenceToAck;

	/* Set the packet type. */
	tPacketAck.s.tHeader.s.ucPacketType = MONITOR_PACKET_TYP_ACK;

	/* Generate the CRC. */
	usCrc = 0;
	sizCnt = offsetof(struct MIV3_PACKET_HEADER_STRUCT, usDataSize);
	while( sizCnt<offsetof(struct MIV3_PACKET_ACK_STRUCT, ucCrcHi) )
	{
		usCrc = crc16(usCrc, tPacketAck.auc[sizCnt]);
		++sizCnt;
	}

	/* Append the CRC. */
	tPacketAck.s.ucCrcHi = (uint8_t)((usCrc>>8U) & 0xffU);
	tPacketAck.s.ucCrcLo = (uint8_t)( usCrc      & 0xffU);

	/* Send the buffer. */
//	hexdump(tPacketAck.auc, sizPacket);
	tResult = send_packet(&(tPacketAck.s.tHeader), sizeof(MIV3_PACKET_ACK_T));
	if( tResult!=TRANSPORTSTATUS_OK )
	{
		fprintf(stderr, "! send_packet: failed to send the ACK packet!\n");
	}

	return tResult;
}



romloader::TRANSPORTSTATUS_T romloader_eth::execute_command(MIV3_PACKET_HEADER_T *ptPacket, size_t sizPacket)
{
	TRANSPORTSTATUS_T tResult;
	unsigned char ucStatus;
	unsigned char ucPacketSequence;
	unsigned int uiRetryCnt;
	int iResult;
	MIV3_PACKET_ACK_T *ptAckPacket;


	uiRetryCnt = 10;
	do
	{
		/* Send the command. */
		fprintf(stderr, "Sending packet...\n");
		tResult = send_packet(ptPacket, sizPacket);
		if( tResult!=TRANSPORTSTATUS_OK )
		{
			fprintf(stderr, "! execute_command: send_packet failed with error code %d\n", tResult);
		}
		else
		{
			/* Receive the ACK for the command. */
			fprintf(stderr, "Wait for ACK...\n");
			tResult = receive_packet();
			if( tResult!=TRANSPORTSTATUS_OK )
			{
				fprintf(stderr, "! execute_command: receive_packet failed with error code %d\n", tResult);
			}
			else
			{
				ptAckPacket = (MIV3_PACKET_ACK_T*)m_aucPacketInputBuffer;

				if( m_sizPacketInputBuffer==sizeof(MIV3_PACKET_ACK_T) && ptAckPacket->s.tHeader.s.ucPacketType==MONITOR_PACKET_TYP_ACK )
				{
					/* Get the sequence number. */
					ucPacketSequence = ptAckPacket->s.tHeader.s.ucSequenceNumber;
					if( ucPacketSequence!=m_ucMonitorSequence )
					{
						fprintf(stderr, "! execute_command: the sequence does not match: %d / %d\n", ucPacketSequence, m_ucMonitorSequence);
						synchronize();
						tResult = TRANSPORTSTATUS_SEQUENCE_MISMATCH;
					}
					else
					{
						fprintf(stderr, "ACK OK...\n");

						/* The packet was acknowledged.
						 * Now increase the sequence number .
						 */
						++m_ucMonitorSequence;

						tResult = TRANSPORTSTATUS_OK;
					}
				}
				else
				{
					fprintf(stderr, "! execute_command: expected an ACK packet with %zd bytes, but received %zd bytes:\n", sizeof(MIV3_PACKET_ACK_T), m_sizPacketInputBuffer);
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



romloader::TRANSPORTSTATUS_T romloader_eth::read_data(uint32_t ulNetxAddress, MONITOR_ACCESSSIZE_T tAccessSize, uint16_t sizDataInBytes)
{
	MIV3_PACKET_COMMAND_READ_DATA_T tPacketReadData;
	TRANSPORTSTATUS_T tResult;
	uint8_t ucPacketTyp;
	int iResult;
	MIV3_PACKET_HEADER_T *ptPacketHeader;
	MIV3_PACKET_STATUS_T *ptPacketStatus;


	fprintf(stderr, "read_data\n");
	if( m_fIsConnected==false )
	{
		tResult = TRANSPORTSTATUS_NOT_CONNECTED;
	}
	else
	{
		tPacketReadData.s.tHeader.s.ucPacketType =  MONITOR_PACKET_TYP_Command_Read |
		                                         (tAccessSize<<MONITOR_ACCESSSIZE_SRT);
		tPacketReadData.s.usDataSize = HTONETX16(sizDataInBytes);
		tPacketReadData.s.ulAddress = HTONETX32(ulNetxAddress);

		tResult = execute_command(&(tPacketReadData.s.tHeader), sizeof(MIV3_PACKET_COMMAND_READ_DATA_T));
		if( tResult==TRANSPORTSTATUS_OK )
		{
			/* Receive the data. */
			tResult = receive_packet();
			if( tResult==TRANSPORTSTATUS_OK )
			{
				/* The ACK packet is the smallest possible packet. It has only 2 bytes of user data
				 * (the sequence number and the packet type).
				 */
				if( m_sizPacketInputBuffer<sizeof(MIV3_PACKET_ACK_T) )
				{
					/* The packet is too small. */
					tResult = TRANSPORTSTATUS_PACKET_TOO_SMALL;
				}
				else
				{
					/* Get the packet type. */
					ptPacketHeader = (MIV3_PACKET_HEADER_T*)m_aucPacketInputBuffer;
					ucPacketTyp = ptPacketHeader->s.ucPacketType;

					if( ucPacketTyp==MONITOR_PACKET_TYP_Read_Data )
					{
						/* The expected packet size is...
						 *   the packet header
						 *   the data
						 *   2 CRC bytes
						 */
						if( m_sizPacketInputBuffer==sizeof(MIV3_PACKET_HEADER_T)+sizDataInBytes+2U )
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
						ptPacketStatus = (MIV3_PACKET_STATUS_T*)m_aucPacketInputBuffer;

						if( m_sizPacketInputBuffer==sizeof(MIV3_PACKET_STATUS_T) )
						{
							/* The netX sent a status code in response to the read command. */
							fprintf(stderr, "Status package received. Status %d.\n", ptPacketStatus->s.ucStatus);
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
						fprintf(stderr, "Answer to read_data has unexpected packet type 0x%02x!\n", ucPacketTyp);
						tResult = TRANSPORTSTATUS_UNEXPECTED_PACKET_TYP;
					}
				}
			}
		}
	}

	return tResult;
}



romloader::TRANSPORTSTATUS_T romloader_eth::write_data(uint32_t ulNetxAddress, MONITOR_ACCESSSIZE_T tAccessSize, const void *pvData, uint16_t sizDataInBytes)
{
	union WRITE_DATA_UNION
	{
		struct WRITE_DATA_STRUCT
		{
			MIV3_PACKET_COMMAND_WRITE_DATA_HEADER_T s;
			uint8_t auc[sizMaxPacketSizeHost - sizeof(MIV3_PACKET_COMMAND_WRITE_DATA_HEADER_T)];
		} __attribute__ ((packed)) s;
		uint8_t auc[sizMaxPacketSizeHost];
	} __attribute__ ((packed)) uWriteData;
	TRANSPORTSTATUS_T tResult;
	size_t sizPacket;
	uint8_t ucPacketTyp;
	uint8_t ucStatus;
	int iResult;
	MIV3_PACKET_HEADER_T *ptPacketHeader;
	MIV3_PACKET_STATUS_T *ptPacketStatus;


	if( m_fIsConnected==false )
	{
		tResult = TRANSPORTSTATUS_NOT_CONNECTED;
	}
	else
	{
		/* Set the header fields. */
		uWriteData.s.s.s.tHeader.s.ucPacketType =  MONITOR_PACKET_TYP_Command_Write |
		                                          (tAccessSize<<MONITOR_ACCESSSIZE_SRT);
		uWriteData.s.s.s.usDataSize = HTONETX16(sizDataInBytes);
		uWriteData.s.s.s.ulAddress = HTONETX32(ulNetxAddress);

		/* Add the data. */
		memcpy(uWriteData.s.auc, pvData, sizDataInBytes);

		/* The size of the packet is the "write" header with the data and 2 bytes of CRC. */
		sizPacket = sizeof(MIV3_PACKET_COMMAND_WRITE_DATA_HEADER_T) + sizDataInBytes + 2U;
		tResult = execute_command(&(uWriteData.s.s.s.tHeader), sizPacket);
		if( tResult==TRANSPORTSTATUS_OK )
		{
			/* Receive the status. */
			tResult = receive_packet();
			if( tResult==TRANSPORTSTATUS_OK )
			{
				/* The ACK packet is the smallest possible packet. It has only 2 bytes of user data
				 * (the sequence number and the packet type).
				 */
				if( m_sizPacketInputBuffer<sizeof(MIV3_PACKET_ACK_T) )
				{
					/* The packet is too small. */
					tResult = TRANSPORTSTATUS_PACKET_TOO_SMALL;
				}
				else
				{
					/* Get the packet type. */
					ptPacketHeader = (MIV3_PACKET_HEADER_T*)m_aucPacketInputBuffer;
					ucPacketTyp = ptPacketHeader->s.ucPacketType;

					if( ucPacketTyp==MONITOR_PACKET_TYP_Status )
					{
						ptPacketStatus = (MIV3_PACKET_STATUS_T*)m_aucPacketInputBuffer;

						if( m_sizPacketInputBuffer==sizeof(MIV3_PACKET_STATUS_T) )
						{
							/* The netX sent a status code. */
							ucStatus = ptPacketStatus->s.ucStatus;
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



uint8_t romloader_eth::read_data08(lua_State *ptClientData, uint32_t ulNetxAddress)
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


uint16_t romloader_eth::read_data16(lua_State *ptClientData, uint32_t ulNetxAddress)
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


uint32_t romloader_eth::read_data32(lua_State *ptClientData, uint32_t ulNetxAddress)
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


void romloader_eth::read_image(uint32_t ulNetxAddress, uint32_t ulSize, char **ppcBUFFER_OUT, size_t *psizBUFFER_OUT, SWIGLUA_REF tLuaFn, long lCallbackUserData)
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



void romloader_eth::write_data08(lua_State *ptClientData, uint32_t ulNetxAddress, uint8_t ucData)
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



void romloader_eth::write_data16(lua_State *ptClientData, uint32_t ulNetxAddress, uint16_t usData)
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



void romloader_eth::write_data32(lua_State *ptClientData, uint32_t ulNetxAddress, uint32_t ulData)
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



void romloader_eth::write_image(uint32_t ulNetxAddress, const char *pcBUFFER_IN, size_t sizBUFFER_IN, SWIGLUA_REF tLuaFn, long lCallbackUserData)
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



void romloader_eth::call(uint32_t ulNetxAddress, uint32_t ulParameterR0, SWIGLUA_REF tLuaFn, long lCallbackUserData)
{
	bool fOk;
	TRANSPORTSTATUS_T tResult;
	MIV3_PACKET_COMMAND_CALL_T tCallCommand;
	MIV3_PACKET_CANCEL_CALL_T tCancelCallPacket;
//	const uint8_t aucCancelBuf[1] = { 0x2b };
	uint8_t ucStatus;
	bool fIsRunning;
	char *pcProgressData;
	size_t sizProgressData;
	uint8_t ucPacketTyp;
	MIV3_PACKET_HEADER_T *ptPacketHeader;
	MIV3_PACKET_STATUS_T *ptPacketStatus;


	if( m_fIsConnected==false )
	{
		MUHKUH_PLUGIN_PUSH_ERROR(tLuaFn.L, "%s(%p): not connected!", m_pcName, this);
		fOk = false;
	}
	else
	{
		tCallCommand.s.tHeader.s.ucPacketType = MONITOR_COMMAND_Execute;
		tCallCommand.s.ulAddress = HTONETX32(ulNetxAddress);
		tCallCommand.s.ulR0 = HTONETX32(ulParameterR0);
		tResult = execute_command(&(tCallCommand.s.tHeader), sizeof(tCallCommand));
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
					ptPacketHeader = (MIV3_PACKET_HEADER_T*)m_aucPacketInputBuffer;

					/* Get the packet type. */
					ucPacketTyp = ptPacketHeader->s.ucPacketType;

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
						ptPacketStatus = (MIV3_PACKET_STATUS_T*)m_aucPacketInputBuffer;
						if( m_sizPacketInputBuffer==sizeof(MIV3_PACKET_STATUS_T) )
						{
							/* The netX sent a status code. */

							/* Acknowledge the packet. */
							send_ack(m_ucMonitorSequence);
							/* Increase the sequence number. */
							++m_ucMonitorSequence;

							ucStatus = ptPacketStatus->s.ucStatus;
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

				if (pcProgressData != NULL)
				{
					fIsRunning = callback_string(&tLuaFn, pcProgressData, sizProgressData, lCallbackUserData);
					if( fIsRunning!=true )
					{
						/* Send a cancel request to the device. */
						tCancelCallPacket.s.tHeader.s.ucPacketType = MONITOR_PACKET_TYP_Call_Message;
						tCancelCallPacket.s.ucData = 0x2b;
						tResult = send_packet(&(tCancelCallPacket.s.tHeader), sizeof(MIV3_PACKET_CANCEL_CALL_T));

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

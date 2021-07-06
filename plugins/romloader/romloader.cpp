/***************************************************************************
 *   Copyright (C) 2007 by Christoph Thelen                                *
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


#include <stdio.h>

#include "romloader.h"


romloader::romloader(const char *pcName, const char *pcTyp, muhkuh_plugin_provider *ptProvider)
 : muhkuh_plugin(pcName, pcTyp, ptProvider)
 , m_tChiptyp(ROMLOADER_CHIPTYP_UNKNOWN)
 , m_sizMaxPacketSizeClient(0)
 , m_sizPacketInputBuffer(0)
 , m_ucMonitorSequence(0)
{
/*	m_ptLog->trace("created in romloader"); */
}


romloader::romloader(const char *pcName, const char *pcTyp, const char *pcLocation, muhkuh_plugin_provider *ptProvider)
 : muhkuh_plugin(pcName, pcTyp, pcLocation, ptProvider)
 , m_tChiptyp(ROMLOADER_CHIPTYP_UNKNOWN)
 , m_sizMaxPacketSizeClient(0)
, m_sizPacketInputBuffer(0)
 , m_ucMonitorSequence(0)
{
/*	printf("%s(%p): created in romloader\n", m_pcName, this); */
}


romloader::~romloader(void)
{
/*	printf("%s(%p): deleted in romloader\n", m_pcName, this); */
}



bool romloader::synchronize(ROMLOADER_CHIPTYP *ptChiptyp, uint16_t *usMiVersionMin, uint16_t *usMiVersionMaj)
{
	const uint8_t aucKnock[5] = { '*', 0x00, 0x00, '*', '#' };
	const uint8_t aucMagicMooh[4] = { 0x4d, 0x4f, 0x4f, 0x48 };
	unsigned int uiRetryCnt;
	TRANSPORTSTATUS_T tResult;
	bool fResult;
	MIV3_PACKET_SYNC_T *ptSyncPacket;
	uint8_t ucSequence;
	//uint32_t ulMiVersionMin;
	//uint32_t ulMiVersionMaj;
	ROMLOADER_CHIPTYP tChipType;
	size_t sizMaxPacketSize;

	m_ptLog->info("synchronize!");
	fResult = false;

	uiRetryCnt = 10;
	do
	{
		tResult = send_raw_packet(aucKnock, sizeof(aucKnock));
		if( tResult!=TRANSPORTSTATUS_OK )
		{
			m_ptLog->error("send_sync_command: failed to send the packet!");
		}
		else
		{
			tResult = receive_packet();
			if( tResult!=TRANSPORTSTATUS_OK )
			{
				m_ptLog->error("send_sync_command: receive_packet failed with error code %d", tResult);
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
				m_ptLog->error("send_sync_command: Retried 10 times and nothing happened. Giving up!");
				break;
			}
			else
			{
				m_ptLog->error("***************************************");
				m_ptLog->error("*                                     *");
				m_ptLog->error("*            retry                    *");
				m_ptLog->error("*                                     *");
				m_ptLog->error("***************************************");
			}
		}

	} while( tResult!=TRANSPORTSTATUS_OK );

	if( tResult!=TRANSPORTSTATUS_OK )
	{
		/* Failed to send knock sequence to device. */
		m_ptLog->error("Failed to send knock sequence to device.");
	}
	else
	{
		/* Get the received packet as a sync packet for later. */
		ptSyncPacket = (MIV3_PACKET_SYNC_T*)m_aucPacketInputBuffer;

		if( m_sizPacketInputBuffer==sizeof(MIV3_PACKET_SYNC_T) && memcmp(ptSyncPacket->s.aucMagic, aucMagicMooh, sizeof(aucMagicMooh))==0 )
		{
			m_ptLog->debug("Packet:");
			m_ptLog->hexdump(muhkuh_log::MUHKUH_LOG_LEVEL_DEBUG, m_aucPacketInputBuffer, m_sizPacketInputBuffer);

			/* Get the sequence number. */
			ucSequence = ptSyncPacket->s.tHeader.s.ucSequenceNumber;
			m_ptLog->debug("Sequence number: 0x%02x", ucSequence);
			
			// check if usMiVersionMaj and usMiVersionMin are no NULL before writing
			*usMiVersionMin = NETXTOH16(ptSyncPacket->s.usVersionMinor);
			*usMiVersionMaj = NETXTOH16(ptSyncPacket->s.usVersionMajor);
			m_ptLog->debug("Machine interface V%d.%d .", ptSyncPacket->s.usVersionMinor, ptSyncPacket->s.usVersionMajor);

			tChipType = (ROMLOADER_CHIPTYP)(ptSyncPacket->s.ucChipType);
			m_ptLog->debug("Chip type : %d", tChipType);

			/* sizMaxPacketSizeClient = min(max. packet size from response, sizMaxPacketSizeHost) */
			sizMaxPacketSize = NETXTOH16(ptSyncPacket->s.usMaximumPacketSize);
			m_ptLog->debug("Maximum packet size: 0x%04lx", sizMaxPacketSize);
			/* Limit the packet size to the buffer size. */
			if( sizMaxPacketSize>m_sizMaxPacketSizeHost )
			{
				sizMaxPacketSize = m_sizMaxPacketSizeHost;
				m_ptLog->debug("Limit maximum packet size to 0x%04lx", sizMaxPacketSize);
			}

			/* Set the new values. */
			m_ucMonitorSequence = ucSequence;
			if( ptChiptyp!=NULL )
			{
				*ptChiptyp = tChipType;
			}
			m_sizMaxPacketSizeClient = sizMaxPacketSize;

			fResult = true;
		}
		else
		{
			m_ptLog->error("Received an invalid knock response.");
			m_ptLog->hexdump(muhkuh_log::MUHKUH_LOG_LEVEL_DEBUG, m_aucPacketInputBuffer, m_sizPacketInputBuffer);

			/* TODO: Send a "Cancel Operation" packet here and retry. */
		}
	}

	return fResult;
}



romloader::TRANSPORTSTATUS_T romloader::send_packet(MIV3_PACKET_HEADER_T *ptPacket, size_t sizPacket)
{
	TRANSPORTSTATUS_T tResult;
	uint16_t usCrc;
	size_t sizUserData;
	uint8_t *pucCnt;
	uint8_t *pucEnd;


	/* The maximum packet size must not exceed the maximum size negotiated with the client. */
	if( sizPacket>m_sizMaxPacketSizeClient )
	{
		m_ptLog->error("send_packet: packet too large: %zd bytes! Maximum size is %zd.", m_sizMaxPacketSizeClient, m_sizMaxPacketSizeClient);
		tResult = TRANSPORTSTATUS_PACKET_TOO_LARGE;
	}
	else if( sizPacket<sizeof(MIV3_PACKET_HEADER_T) )
	{
		m_ptLog->error("send_packet: packet too small: %zd bytes! Minimum size is %zd.", m_sizMaxPacketSizeClient, sizeof(MIV3_PACKET_HEADER_T));
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
		tResult = send_raw_packet(ptPacket->auc, sizPacket);
		if( tResult!=TRANSPORTSTATUS_OK )
		{
			m_ptLog->error("send_packet: failed to send the packet!");
		}
	}

	return tResult;
}


romloader::TRANSPORTSTATUS_T romloader::send_ack(unsigned char ucSequenceToAck)
{
	TRANSPORTSTATUS_T tResult;
	MIV3_PACKET_ACK_T tPacketAck;
	uint16_t usCrc;
	size_t sizCnt;


	/* Set the packet start character. */
	tPacketAck.s.tHeader.s.ucStreamStart = MONITOR_STREAM_PACKET_START;

	/* Set the size of the user data.
	 *  1 byte sequence number
	 *  1 byte packet type
	 */
	tPacketAck.s.tHeader.s.usDataSize = 2;

	/* Set the sequence number. */
//	fprintf(stderr, "ACK 0x%02x\n", ucSequenceToAck);
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
	tResult = send_raw_packet(tPacketAck.auc, sizeof(MIV3_PACKET_ACK_T));
	if( tResult!=TRANSPORTSTATUS_OK )
	{
		m_ptLog->error("send_packet: failed to send the ACK packet!");
	}

	return tResult;
}



romloader::TRANSPORTSTATUS_T romloader::execute_command(MIV3_PACKET_HEADER_T *ptPacket, size_t sizPacket)
{
	TRANSPORTSTATUS_T tResult;
	unsigned char ucStatus;
	unsigned char ucPacketSequence;
	unsigned char ucLastMonitorSequence;
	unsigned int uiRetryCnt;
	int iResult;
	MIV3_PACKET_ACK_T *ptAckPacket;


	uiRetryCnt = 10;
	do
	{
		/* Send the command. */
//		fprintf(stderr, "Sending packet...\n");
		tResult = send_packet(ptPacket, sizPacket);
		if( tResult!=TRANSPORTSTATUS_OK )
		{
			m_ptLog->error("execute_command: send_packet failed with error code %d", tResult);
		}
		else
		{
			/* Receive the ACK for the command. */
//			fprintf(stderr, "Wait for ACK...\n");
			tResult = receive_packet();
			if( tResult!=TRANSPORTSTATUS_OK )
			{
				m_ptLog->error("execute_command: receive_packet failed with error code %d", tResult);
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
						m_ptLog->error("execute_command: the sequence does not match: %d / %d", ucPacketSequence, m_ucMonitorSequence);
						synchronize(NULL, NULL, NULL);
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
					/* Is this a re-send of the last packet from the netX? */
					ucPacketSequence = ptAckPacket->s.tHeader.s.ucSequenceNumber;
					ucLastMonitorSequence = m_ucMonitorSequence - 1U;
					if( ucLastMonitorSequence==ucPacketSequence )
					{
						/* The netX sent the last packet again.
						 * This is the case if the last ACK from the PC got lost.
						 * Re-transmit an ACK packet for the last sequence number.
						 */
						m_ptLog->debug("Re-sending ACK for sequence number 0x%02x.", ucLastMonitorSequence);
						send_ack(ucLastMonitorSequence);

/* FIXME: change the result. */
						tResult = TRANSPORTSTATUS_MISSING_USERDATA;
					}
					else
					{
						m_ptLog->error("execute_command: expected an ACK packet with %zd bytes, but received %zd bytes:", sizeof(MIV3_PACKET_ACK_T), m_sizPacketInputBuffer);
						m_ptLog->hexdump(muhkuh_log::MUHKUH_LOG_LEVEL_DEBUG, m_aucPacketInputBuffer, m_sizPacketInputBuffer);
/* FIXME: change the result. */
						tResult = TRANSPORTSTATUS_MISSING_USERDATA;
					}
				}
			}
		}

		if( tResult!=TRANSPORTSTATUS_OK )
		{
			--uiRetryCnt;
			if( uiRetryCnt==0 )
			{
				m_ptLog->error("execute_command: Retried 10 times and nothing happened. Giving up!");
				break;
			}
			else
			{
				m_ptLog->debug("***************************************");
				m_ptLog->debug("*                                     *");
				m_ptLog->debug("*            retry                    *");
				m_ptLog->debug("*                                     *");
				m_ptLog->debug("***************************************");
			}
		}

	} while( tResult!=TRANSPORTSTATUS_OK );

	return tResult;
}



romloader::TRANSPORTSTATUS_T romloader::read_data(uint32_t ulNetxAddress, MONITOR_ACCESSSIZE_T tAccessSize, uint16_t sizDataInBytes)
{
	MIV3_PACKET_COMMAND_READ_DATA_T tPacketReadData;
	TRANSPORTSTATUS_T tResult;
	uint8_t ucPacketTyp;
	int iResult;
	MIV3_PACKET_HEADER_T *ptPacketHeader;
	MIV3_PACKET_STATUS_T *ptPacketStatus;


	if( m_fIsConnected==false )
	{
		tResult = TRANSPORTSTATUS_NOT_CONNECTED;
	}
	else
	{
		tPacketReadData.s.tHeader.s.ucPacketType =  MONITOR_PACKET_TYP_CommandRead |
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

					if( ucPacketTyp==MONITOR_PACKET_TYP_ReadData )
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
							m_ptLog->debug("Status package received. Status %d.", ptPacketStatus->s.ucStatus);
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
						m_ptLog->error("Answer to read_data has unexpected packet type 0x%02x!", ucPacketTyp);
						m_ptLog->hexdump(muhkuh_log::MUHKUH_LOG_LEVEL_DEBUG, m_aucPacketInputBuffer, m_sizPacketInputBuffer);
						tResult = TRANSPORTSTATUS_UNEXPECTED_PACKET_TYP;
					}
				}
			}
		}
	}

	return tResult;
}

void romloader::cmd_usip(SWIGLUA_REF tLuaFn, long lCallbackUserData)
{
	bool fOk;
	TRANSPORTSTATUS_T tResult;
	MIV3_PACKET_INFO_COMMAND_USIP_T tCommandUsip;
	
//	MIV3_PACKET_COMMAND_CALL_T tCallCommand;
	MIV3_PACKET_CANCEL_CALL_T tCancelCallPacket;
//	const uint8_t aucCancelBuf[1] = { 0x2b };
	uint8_t ucStatus;
	bool fIsRunning;
	char *pcProgressData;
	size_t sizProgressData;
	uint8_t ucPacketTyp;
	MIV3_PACKET_HEADER_T *ptPacketHeader;
	MIV3_PACKET_STATUS_T *ptPacketStatus;
	int i = 10;

	if( m_fIsConnected==false )
	{
		MUHKUH_PLUGIN_PUSH_ERROR(tLuaFn.L, "%s(%p): not connected!", m_pcName, this);
		fOk = false;
	}
	else
	{	
		m_ptLog->info("use MONITOR_PACKET_TYP_Command_Start_USIP");
		tCommandUsip.s.tHeader.s.ucPacketType = MONITOR_PACKET_TYP_Command_Start_USIP;
		tResult = execute_command(&(tCommandUsip.s.tHeader), sizeof(tCommandUsip));
		if( tResult!=TRANSPORTSTATUS_OK )
		{
			MUHKUH_PLUGIN_PUSH_ERROR(tLuaFn.L, "%s(%p): failed to execute command!", m_pcName, this);
			fOk = false;
		}
		else
		{
			/* Receive message packets. */
			while(i>1)
			{
				pcProgressData = NULL;
				sizProgressData = 0;

				tResult = receive_packet();
				printf("result: %s ", tResult);
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

					if( ucPacketTyp==MONITOR_PACKET_TYP_CallMessage )
					{
						/* Acknowledge the packet. */
						send_ack(m_ucMonitorSequence);
						/* Increase the sequence number. */
						++m_ucMonitorSequence;

						/* NOTE: Do not check the size of the user data here. It should be possible to send 0 bytes. */
						pcProgressData = ((char*)m_aucPacketInputBuffer) + sizeof(MIV3_PACKET_HEADER_T);
						/* The size of the user data is the size of the packet - the header size - 2 bytes for the CRC16. */
						sizProgressData = m_sizPacketInputBuffer - (sizeof(MIV3_PACKET_HEADER_T) + 2U);
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
							if( ucStatus==MONITOR_STATUS_CallFinished )
							{
								fOk = true;
								break;
							}
							else if(ucStatus==MONITOR_STATUS_Ok)
							{
								m_ptLog->info("Status: MONITOR_STATUS_Ok");
								fOk = true;
								break;
							}
							else
							{
								/* The netX sent an error. */
								m_ptLog->error("Status != call_finished received. Status %d.", ucStatus);
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
						tCancelCallPacket.s.tHeader.s.ucPacketType = MONITOR_PACKET_TYP_Command_Start_USIP;
						tCancelCallPacket.s.ucData = 0x2b;
						tResult = send_packet(&(tCancelCallPacket.s.tHeader), sizeof(MIV3_PACKET_CANCEL_CALL_T));

						MUHKUH_PLUGIN_PUSH_ERROR(tLuaFn.L, "%s(%p): the call was canceled!", m_pcName, this);
						fOk = false;
						break;
					}
				}
				i--;
			}
		}
	}

	if( fOk!=true )
	{
		MUHKUH_PLUGIN_EXIT_ERROR(tLuaFn.L);
	}
}


uint32_t romloader::get_info(uint32_t *ptNetxVersion, uint32_t *ptInfoFlags)
{
	MIV3_PACKET_INFO_COMMAND_T tPacketInfo;
	MIV3_PACKET_INFO_DATA_T* ptRecPacketInfo;
	uint32_t tResult;
	uint8_t ucPacketTyp;
	int iResult;
	MIV3_PACKET_HEADER_T *ptPacketHeader;
	MIV3_PACKET_STATUS_T *ptPacketStatus;
	uint32_t  ulNetxVersion;
	uint32_t  ulInfoFlags;


	if( m_fIsConnected==false )
	{
		tResult = TRANSPORTSTATUS_NOT_CONNECTED;
	}
	else
	{
		m_ptLog->info("use MONITOR_PACKET_TYP_Command_Info");
		tPacketInfo.s.tHeader.s.ucPacketType =  MONITOR_PACKET_TYP_Command_Info;

		tResult = execute_command(&(tPacketInfo.s.tHeader), sizeof(MIV3_PACKET_INFO_DATA_T));
		if( tResult==TRANSPORTSTATUS_OK )
		{
			/* Receive the data. */
			tResult = receive_packet();
			
			if( tResult!=TRANSPORTSTATUS_OK )
			{
				/* Failed to send knock sequence to device. */
				m_ptLog->error("Failed to send knock sequence to device.");
			}
			else
			{
				/* Get the received packet as a sync packet for later. */
				ptRecPacketInfo = (MIV3_PACKET_INFO_DATA_T*)m_aucPacketInputBuffer;
				
				m_ptLog->hexdump(muhkuh_log::MUHKUH_LOG_LEVEL_DEBUG, m_aucPacketInputBuffer, m_sizPacketInputBuffer);
				
				ulNetxVersion = NETXTOH32(ptRecPacketInfo->s.ulNetxVersion);
				ulInfoFlags = NETXTOH32(ptRecPacketInfo->s.ulInfoFlags);
	
				m_ptLog->info("ulNetxVersion: 0x%08x", ulNetxVersion);
				m_ptLog->info("ulInfoFlags: 0x%08x", ulInfoFlags);
									
			}
		}
	}
	
	*ptNetxVersion = ulNetxVersion;
	*ptInfoFlags = ulInfoFlags;

	return tResult;
}

romloader::TRANSPORTSTATUS_T romloader::write_data(uint32_t ulNetxAddress, MONITOR_ACCESSSIZE_T tAccessSize, const void *pvData, uint16_t sizDataInBytes)
{
/* NOTE: Use "pragma pack" instead of "attribute packed" as the latter does not work on MinGW.
 *       See here for details: https://sourceforge.net/p/mingw-w64/bugs/588/
 */
#pragma pack(push, 1)
	union WRITE_DATA_UNION
	{
		struct WRITE_DATA_STRUCT
		{
			MIV3_PACKET_COMMAND_WRITE_DATA_HEADER_T s;
			uint8_t auc[m_sizMaxPacketSizeHost - sizeof(MIV3_PACKET_COMMAND_WRITE_DATA_HEADER_T)];
		} s;
		uint8_t auc[m_sizMaxPacketSizeHost];
	} uWriteData;
MUHKUH_STATIC_ASSERT( sizeof(uWriteData)==m_sizMaxPacketSizeHost, "Packing of WRITE_DATA_UNION does not work.");
#pragma pack(pop)
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
		uWriteData.s.s.s.tHeader.s.ucPacketType =  MONITOR_PACKET_TYP_CommandWrite |
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
					m_ptLog->error("the packet is too small: %zd", m_sizPacketInputBuffer);
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
								m_ptLog->error("Status != OK received. Status %d.", ucStatus);
								tResult = TRANSPORTSTATUS_NETX_ERROR;
							}
						}
						else
						{
							/* The packet type is set to "status", but the size of the packet does not match a valid status packet. */
							m_ptLog->error("invalid size: %zd", m_sizPacketInputBuffer);
							tResult = TRANSPORTSTATUS_INVALID_PACKET_SIZE;
						}
					}
					else
					{
						m_ptLog->error("Answer to read_data has unexpected packet typ 0x%02x!", ucPacketTyp);
						m_ptLog->hexdump(muhkuh_log::MUHKUH_LOG_LEVEL_DEBUG, m_aucPacketInputBuffer, m_sizPacketInputBuffer);
						tResult = TRANSPORTSTATUS_UNEXPECTED_PACKET_TYP;
					}
				}
			}
			else
			{
				m_ptLog->error("receive_packet failed with %d", tResult);
			}
		}
		else
		{
			m_ptLog->error("execute_command failed with %d", tResult);
		}
	}

	return tResult;
}



uint8_t romloader::read_data08(lua_State *ptClientData, uint32_t ulNetxAddress)
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
		ucValue = m_aucPacketInputBuffer[5];
		fOk = true;
	}

	if( fOk!=true )
	{
		MUHKUH_PLUGIN_EXIT_ERROR(ptClientData);
	}

	return ucValue;
}


uint16_t romloader::read_data16(lua_State *ptClientData, uint32_t ulNetxAddress)
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
		usValue  = ((uint16_t)(m_aucPacketInputBuffer[5]));
		usValue |= ((uint16_t)(m_aucPacketInputBuffer[6]))<< 8U;
		fOk = true;
	}

	if( fOk!=true )
	{
		MUHKUH_PLUGIN_EXIT_ERROR(ptClientData);
	}

	return usValue;
}


uint32_t romloader::read_data32(lua_State *ptClientData, uint32_t ulNetxAddress)
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
		ulValue  = ((uint32_t)(m_aucPacketInputBuffer[5]));
		ulValue |= ((uint32_t)(m_aucPacketInputBuffer[6]))<< 8U;
		ulValue |= ((uint32_t)(m_aucPacketInputBuffer[7]))<<16U;
		ulValue |= ((uint32_t)(m_aucPacketInputBuffer[8]))<<24U;
		fOk = true;
	}

	if( fOk!=true )
	{
		MUHKUH_PLUGIN_EXIT_ERROR(ptClientData);
	}

	return ulValue;
}


void romloader::read_image(uint32_t ulNetxAddress, uint32_t ulSize, char **ppcBUFFER_OUT, size_t *psizBUFFER_OUT, SWIGLUA_REF tLuaFn, long lCallbackUserData)
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
					fOk = false;
					break;
				}
				else
				{
					memcpy(pcBuffer, m_aucPacketInputBuffer+sizeof(MIV3_PACKET_HEADER_T), sizChunk);
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



void romloader::write_data08(lua_State *ptClientData, uint32_t ulNetxAddress, uint8_t ucData)
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



void romloader::write_data16(lua_State *ptClientData, uint32_t ulNetxAddress, uint16_t usData)
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



void romloader::write_data32(lua_State *ptClientData, uint32_t ulNetxAddress, uint32_t ulData)
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



void romloader::write_image(uint32_t ulNetxAddress, const char *pcBUFFER_IN, size_t sizBUFFER_IN, SWIGLUA_REF tLuaFn, long lCallbackUserData)
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
	sizChunkMaxSize = m_sizMaxPacketSizeClient - 13U;

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
		} while( sizBUFFER_IN!=0 );
	}

	if( fOk!=true )
	{
		MUHKUH_PLUGIN_EXIT_ERROR(tLuaFn.L);
	}
}


void romloader::call(uint32_t ulNetxAddress, uint32_t ulParameterR0, SWIGLUA_REF tLuaFn, long lCallbackUserData)
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
		tCallCommand.s.tHeader.s.ucPacketType = MONITOR_PACKET_TYP_CommandExecute;
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

					if( ucPacketTyp==MONITOR_PACKET_TYP_CallMessage )
					{
						/* Acknowledge the packet. */
						send_ack(m_ucMonitorSequence);
						/* Increase the sequence number. */
						++m_ucMonitorSequence;

						/* NOTE: Do not check the size of the user data here. It should be possible to send 0 bytes. */
						pcProgressData = ((char*)m_aucPacketInputBuffer) + sizeof(MIV3_PACKET_HEADER_T);
						/* The size of the user data is the size of the packet - the header size - 2 bytes for the CRC16. */
						sizProgressData = m_sizPacketInputBuffer - (sizeof(MIV3_PACKET_HEADER_T) + 2U);
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
							if( ucStatus==MONITOR_STATUS_CallFinished )
							{
								fOk = true;
								break;
							}
							else
							{
								/* The netX sent an error. */
								m_ptLog->error("Status != call_finished received. Status %d.", ucStatus);
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
						tCancelCallPacket.s.tHeader.s.ucPacketType = MONITOR_PACKET_TYP_CallMessage;
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

void romloader::call_quick(uint32_t ulNetxAddress, uint32_t ulParameterR0, SWIGLUA_REF tLuaFn, long lCallbackUserData)
{
	/*
	this function is used for test purposes only!
	it sends a call package to the netx and does not check for a reply.
	we use it to start a binary that resets the netx and does not return with an answer!
	*/
	
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
		tCallCommand.s.tHeader.s.ucPacketType = MONITOR_PACKET_TYP_CommandExecute;
		tCallCommand.s.ulAddress = HTONETX32(ulNetxAddress);
		tCallCommand.s.ulR0 = HTONETX32(ulParameterR0);
		tResult = execute_command(&(tCallCommand.s.tHeader), sizeof(tCallCommand));
		if( tResult!=TRANSPORTSTATUS_OK )
		{
			MUHKUH_PLUGIN_PUSH_ERROR(tLuaFn.L, "%s(%p): failed to execute command!", m_pcName, this);
			fOk = false;
		}

	}

	if( fOk!=true )
	{
		MUHKUH_PLUGIN_EXIT_ERROR(tLuaFn.L);
	}
}

void romloader::call_hboot(uint32_t ulNetxAddress, uint32_t ulParameterR0, SWIGLUA_REF tLuaFn, long lCallbackUserData)
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
//	int i = 10;

	if( m_fIsConnected==false )
	{
		MUHKUH_PLUGIN_PUSH_ERROR(tLuaFn.L, "%s(%p): not connected!", m_pcName, this);
		fOk = false;
	}
	else
	{	
		m_ptLog->info("use MONITOR_PACKET_TYP_Command_Start_Hboot");
		tCallCommand.s.tHeader.s.ucPacketType = MONITOR_PACKET_TYP_Command_Start_Hboot;
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
				printf("result: %s ", tResult);
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

					if( ucPacketTyp==MONITOR_PACKET_TYP_CallMessage )
					{
						/* Acknowledge the packet. */
						send_ack(m_ucMonitorSequence);
						/* Increase the sequence number. */
						++m_ucMonitorSequence;

						/* NOTE: Do not check the size of the user data here. It should be possible to send 0 bytes. */
						pcProgressData = ((char*)m_aucPacketInputBuffer) + sizeof(MIV3_PACKET_HEADER_T);
						/* The size of the user data is the size of the packet - the header size - 2 bytes for the CRC16. */
						sizProgressData = m_sizPacketInputBuffer - (sizeof(MIV3_PACKET_HEADER_T) + 2U);
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
							if( ucStatus==MONITOR_STATUS_CallFinished )
							{
								fOk = true;
								break;
							}
							else if(ucStatus==MONITOR_STATUS_Ok)
							{
								m_ptLog->info("Status: MONITOR_STATUS_Ok");
							}
							else
							{
								/* The netX sent an error. */
								m_ptLog->error("Status != call_finished received. Status %d.", ucStatus);
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
						tCancelCallPacket.s.tHeader.s.ucPacketType = MONITOR_PACKET_TYP_Command_Start_Hboot;
						tCancelCallPacket.s.ucData = 0x2b;
						tResult = send_packet(&(tCancelCallPacket.s.tHeader), sizeof(MIV3_PACKET_CANCEL_CALL_T));

						MUHKUH_PLUGIN_PUSH_ERROR(tLuaFn.L, "%s(%p): the call was canceled!", m_pcName, this);
						fOk = false;
						break;
					}
				}
//				i--;
			}
		}
	}

	if( fOk!=true )
	{
		MUHKUH_PLUGIN_EXIT_ERROR(tLuaFn.L);
	}
}



ROMLOADER_CHIPTYP romloader::GetChiptyp(void) const
{
	return m_tChiptyp;
}



const char *romloader::GetChiptypName(ROMLOADER_CHIPTYP tChiptyp) const
{
	const char *pcChiptyp;
	const ROMLOADER_RESET_ID_T *ptCnt, *ptEnd;


	// init chip name with unknown name
	pcChiptyp = "unknown chip";

	// loop over all known romcodes and search the romcode typ
	ptCnt = atResIds;
	ptEnd = ptCnt + (sizeof(atResIds)/sizeof(atResIds[0]));
	while( ptCnt<ptEnd )
	{
		if( ptCnt->tChiptyp==tChiptyp )
		{
			pcChiptyp = ptCnt->pcChiptypName;
			break;
		}
		++ptCnt;
	}

	return pcChiptyp;
}


// wrapper functions for compatibility with old function names
ROMLOADER_CHIPTYP  romloader::get_chiptyp(void) const                             {return GetChiptyp();}
unsigned int       romloader::get_romcode(void) const                             {return 0;}
const char        *romloader::get_chiptyp_name(ROMLOADER_CHIPTYP tChiptyp) const  {return GetChiptypName(tChiptyp);}
const char        *romloader::get_romcode_name(unsigned int tRomcode) const       {return "Unknown";}

bool romloader::detect_chiptyp(romloader_read_functinoid *ptFn)
{
	uint32_t ulResetVector;
	const ROMLOADER_RESET_ID_T *ptRstCnt, *ptRstEnd;
	uint32_t ulVersionAddr;
	uint32_t ulVersion;
	
	uint32_t ulCheckAddr;
	uint32_t ulCheckVal;
	uint32_t ulCheckValMasked;
	bool fResult;
	ROMLOADER_CHIPTYP tChiptyp;
	//const char *pcChiptypName;


	tChiptyp = ROMLOADER_CHIPTYP_UNKNOWN;

	/* Read the reset vector at 0x00000000. */
	fResult = ptFn->read_data32(0, &ulResetVector);
	if (fResult!=true)
	{
		m_ptLog->debug("Failed to read reset vector!");
	}
	else
	{
		m_ptLog->debug("reset vector: 0x%08X", ulResetVector);
	
		// match the reset vector to all known chipfamilies
		ptRstCnt = atResIds;
		ptRstEnd = ptRstCnt + (sizeof(atResIds)/sizeof(atResIds[0]));
		ulVersionAddr = 0xffffffff;
		while( ptRstCnt<ptRstEnd )
		{
			if( ptRstCnt->ulResetVector==ulResetVector )
			{
				ulVersionAddr = ptRstCnt->ulVersionAddress;
				// read version address
				fResult = ptFn->read_data32(ulVersionAddr, &ulVersion);
				if (fResult!=true)
				{
					m_ptLog->debug("Failed to read version info!");
				}
				else
				{
					m_ptLog->debug("version value: 0x%08X", ulVersion);
					if( ptRstCnt->ulVersionValue==ulVersion )
					{
						ulCheckAddr = ptRstCnt->ulCheckAddress;
						ulCheckVal = 0;
		
						if (ulCheckAddr != 0)
						{
							fResult = ptFn->read_data32(ulCheckAddr, &ulCheckVal);
							if (fResult!=true)
							{
								m_ptLog->debug("Failed to read 3rd check value!");
							}
							else
							{
								ulCheckValMasked = ulCheckVal & ptRstCnt->ulCheckMask;
								m_ptLog->debug("check address: 0x%08X  value: 0x%08X masked: 0x%08X compare to: 0x%08X", ulCheckAddr, ulCheckVal, ulCheckValMasked, ptRstCnt->ulCheckCmpValue);
							}
						}
							
						if ((ulCheckAddr==0) || (ulCheckValMasked==ptRstCnt->ulCheckCmpValue))
						{
							// found chip!
							tChiptyp = ptRstCnt->tChiptyp;
							m_ptLog->debug("found chip %s.", ptRstCnt->pcChiptypName);
							break;
						}
						else{
							m_ptLog->debug("chip does not match: %s.", ptRstCnt->pcChiptypName);
						}
					}
				}
			}
			++ptRstCnt;
		}
	}

	/* Found something? */
	fResult = ( tChiptyp!=ROMLOADER_CHIPTYP_UNKNOWN );

	if( fResult==true )
	{
		/* Accept new chiptype and romcode. */
		m_tChiptyp = tChiptyp;
//
//		pcChiptypName = GetChiptypName(tChiptyp);
//		printf("%s(%p): found chip %s.\n", m_pcName, this, pcChiptypName);
	}

	return fResult;
}


bool romloader::__read_data32(uint32_t ulNetxAddress, uint32_t *pulData)
{
	TRANSPORTSTATUS_T tResult;
	uint32_t ulValue;
	bool fOk;


	/* Expect error. */
	fOk = false;

	tResult = read_data(ulNetxAddress, MONITOR_ACCESSSIZE_Long, 4);
	if( tResult==TRANSPORTSTATUS_OK )
	{
		ulValue  = ((uint32_t)(m_aucPacketInputBuffer[5]));
		ulValue |= ((uint32_t)(m_aucPacketInputBuffer[6]))<< 8U;
		ulValue |= ((uint32_t)(m_aucPacketInputBuffer[7]))<<16U;
		ulValue |= ((uint32_t)(m_aucPacketInputBuffer[8]))<<24U;
		*pulData = ulValue;
		fOk = true;
	}

	return fOk;
}

bool romloader::new_detect_chiptyp(void)
{
	const ROMLOADER_RESET_ID_T *ptRstCnt, *ptRstEnd;
	bool fResult;
	ROMLOADER_CHIPTYP tChiptyp;
	uint32_t ulNetxVersion;
	uint32_t ulInfoFlags;
	const ROMLOADER_RESET_ID_T *ptCnt, *ptEnd;
	uint32_t ulVersionRegAddr = 0xFF401298;
	uint32_t ulMaskedVal;
	uint32_t ulCompVal;
	
	fResult = get_info(&ulNetxVersion, &ulInfoFlags);
	m_ptLog->debug("ulNetxVersion:  0x%08X", ulNetxVersion);
	m_ptLog->debug("ulInfoFlags:  0x%08X", ulInfoFlags);
	if( fResult!=TRANSPORTSTATUS_OK )
	{

		m_ptLog->error("Failed to get Info Packet");
	}
	else
	{
		m_ptLog->debug("check for matching chip type ");
	
		/* Compare the reset vector to all known chip families. */
		ptRstCnt = atResIds;
		ptRstEnd = ptRstCnt + (sizeof(atResIds)/sizeof(atResIds[0]));
		
		while( ptRstCnt<ptRstEnd )
		{
			if (ulNetxVersion & ptRstCnt->ulCheckMask == (ptRstCnt->ulCheckCmpValue))
			{
				tChiptyp = ptRstCnt->tChiptyp;
				m_ptLog->debug("found chip %s.", ptRstCnt->pcChiptypName);
				break;
			}	
			++ptRstCnt;
		}
	}
	
	/* Found something? */
	if( fResult==TRANSPORTSTATUS_OK && tChiptyp!=ROMLOADER_CHIPTYP_UNKNOWN )
	{
		/* Accept the new chip type. */
		m_tChiptyp = tChiptyp;
		fResult = true;
	}
	else
	{
		fResult = false;
	}
	return fResult;
	
}

bool romloader::detect_chiptyp(void)
{
	TRANSPORTSTATUS_T tResult;
	uint32_t ulResetVector;
	const ROMLOADER_RESET_ID_T *ptRstCnt, *ptRstEnd;
	uint32_t ulVersionAddr;
	uint32_t ulVersion;
	uint32_t ulCheckAddr;
	uint32_t ulCheckVal;
	uint32_t ulCheckValMasked;
	bool fResult;
	ROMLOADER_CHIPTYP tChiptyp;


	tChiptyp = ROMLOADER_CHIPTYP_UNKNOWN;

	/* Read the reset vector at 0x00000000. */
	fResult = __read_data32(0x00000000, &ulResetVector);
	
	if( fResult!=true )
	{

		m_ptLog->error("Failed to read the reset vector.");
	}
	else
	{
		m_ptLog->debug("reset vector: 0x%08X", ulResetVector);

		/* Compare the reset vector to all known chip families. */
		ptRstCnt = atResIds;
		ptRstEnd = ptRstCnt + (sizeof(atResIds)/sizeof(atResIds[0]));
		ulVersionAddr = 0xffffffff;
		while( ptRstCnt<ptRstEnd )
		{
			
			m_ptLog->debug("check if it is a %s", ptRstCnt->pcChiptypName);
			m_ptLog->debug("ptRstCnt->ulVersionAddress: 0x%08X", ptRstCnt->ulVersionAddress);
			if( ptRstCnt->ulResetVector==ulResetVector )
			{
				ulVersionAddr = ptRstCnt->ulVersionAddress;
				/* Read the version address. */
				fResult = __read_data32(ulVersionAddr, &ulVersion);
				if( fResult!=true )
				{
					m_ptLog->error("Failed to read the version data at 0x%08x.", ulVersionAddr);
				}
				else
				{
					m_ptLog->debug("version value: 0x%08X", ulVersion);
					if( ptRstCnt->ulVersionValue==ulVersion )
					{
						ulCheckAddr = ptRstCnt->ulCheckAddress;
						ulCheckVal = 0;

						if (ulCheckAddr != 0)
						{
							fResult = __read_data32(ulCheckAddr, &ulCheckVal);
							if( fResult!=true )
							{
								m_ptLog->error("Failed to read the check data at 0x%08x.", ulCheckAddr);
								break;
							}
							else
							{
								ulCheckValMasked = ulCheckVal & ptRstCnt->ulCheckMask;
								m_ptLog->debug("check address: 0x%08X  value: 0x%08X masked: 0x%08X compare to: 0x%08X", ulCheckAddr, ulCheckVal, ulCheckValMasked, ptRstCnt->ulCheckCmpValue);
							}
						}

						if ((ulCheckAddr==0) || (ulCheckValMasked==ptRstCnt->ulCheckCmpValue))
						{
							/* Found chip! */
							tChiptyp = ptRstCnt->tChiptyp;
							m_ptLog->debug("found chip %s.", ptRstCnt->pcChiptypName);
							break;
						}
						else{
							m_ptLog->debug("chip does not match: %s.", ptRstCnt->pcChiptypName);
						}
					}
				}
			}
			++ptRstCnt;
		}
	}

	/* Found something? */
	if( fResult==true && tChiptyp!=ROMLOADER_CHIPTYP_UNKNOWN )
	{
		/* Accept the new chip type. */
		m_tChiptyp = tChiptyp;
	}

	return fResult;
}


const romloader::ROMLOADER_RESET_ID_T romloader::atResIds[16] =
{

	{
		0xea080001,
		0x00200008,
		0x00001000,
		0,
		0,
		0,
		ROMLOADER_CHIPTYP_NETX500,
		"netX500"
	},

	{
		0xea080002,
		0x00200008,
		0x00003002,
		0,
		0,
		0,
		ROMLOADER_CHIPTYP_NETX100,
		"netX100"
	},

	{
		0xeac83ffc,
		0x08200008,
		0x00002001,
		0,
		0,
		0,
		ROMLOADER_CHIPTYP_NETX50,
		"netX50"
	},

	{
		0xeafdfffa,
		0x08070008,
		0x00005003,
		0,
		0,
		0,
		ROMLOADER_CHIPTYP_NETX10,
		"netX10"
	},

	{
		0xeafbfffa,
		0x080f0008,
		0x00006003,
		0,
		0,
		0,
		ROMLOADER_CHIPTYP_NETX56,
		"netX51/52 Step A"
	},

	{
		0xeafbfffa,
		0x080f0008,
		0x00106003,
		0,
		0,
		0,
		ROMLOADER_CHIPTYP_NETX56B,
		"netX51/52 Step B"
	},

	{
		0xe59ff00c,
		0x04100020,
		0x00108004,
		0,
		0,
		0,
		ROMLOADER_CHIPTYP_NETX4000_RELAXED,
		"netX4000 RLXD"
	},
	
	{
		0xe59ff00c,
		0x04100020,
		0x0010b004,
		0xf80000c0, /* RAP_SYSCTRL_OTP_CONFIG_0 bit 0 is the package type */
		0x00000001,
		0x00000000, /* 0 = netx 4000 Full */
		ROMLOADER_CHIPTYP_NETX4000_FULL,
		"netX4000 Full"
	},

	{
		0xe59ff00c,
		0x04100020,
		0x0010b004,
		0xf80000c0, /* RAP_SYSCTRL_OTP_CONFIG_0 bit 0 is the package type */
		0x00000001,
		0x00000001, /* 1 = netx 4100 /netx 4000 Small */
		ROMLOADER_CHIPTYP_NETX4100_SMALL,
		"netX4100 Small"
	},
	
	{
		0x2009fff0,
		0x000000c0,
		0x0010a005,
		0x00005110,
		0xffffffff,
		0x1f13933b,
		ROMLOADER_CHIPTYP_NETX90_MPW,
		"netX90MPW"
	},

	{
		0x2009fff0,
		0x000000c0,
		0x0010a005,
		0x00005110,
		0xffffffff,
		0xe001200c,
		ROMLOADER_CHIPTYP_NETX90,
		"netX90 Rev0"
	},
	{
		0x2009fff0,
		0x000400c0,
		0x0010d005,
		0, // not used for new_detect_chiptyp() routine
		0xffffffff,
		0x0981020d,
		ROMLOADER_CHIPTYP_NETX90D_INTRAM,
		"netX90 Rev2 intram"
	},
	{
		0x2009fff0,
		0x000000c0,
		0x0010d005,
		0, // not used for MI new_detect_chiptyp() routine but for jtag??
		0xFF7FFFFF,
		0x0901020d,
		ROMLOADER_CHIPTYP_NETX90D,
		"netX90 Rev2"
	},
	{
		0x2009fff0,
		0x000000c0,
		0x0010d005,
		0,
		0,
		0,
		ROMLOADER_CHIPTYP_NETX90B,
		"netX90 Rev1"
	},
	{
		0x00000000, /* reserved location 0 reads as 0 */
		0x2000, 
		0x0, /* should be 0x7c80006f, can't read rom */
		0, 
		0,
		0,
		ROMLOADER_CHIPTYP_NETIOLA,
		"netIOL MPW"
	},
	
	{
		0x00000000, /* reserved location 0 reads as 0 */
		0x2000, 
		0x64b0006f,
		0, 
		0,
		0,
		ROMLOADER_CHIPTYP_NETIOLB,
		"netIOL Rev0"
	} 
};


uint16_t romloader::crc16(uint16_t usCrc, uint8_t ucData)
{
	usCrc  = (usCrc >> 8) | ((usCrc & 0xff) << 8);
	usCrc ^= ucData;
	usCrc ^= (usCrc & 0xff) >> 4;
	usCrc ^= (usCrc & 0x0f) << 12;
	usCrc ^= ((usCrc & 0xff) << 4) << 1;

	return usCrc;
}



bool romloader::callback_long(SWIGLUA_REF *ptLuaFn, long lProgressData, long lCallbackUserData)
{
	bool fStillRunning;
	int iOldTopOfStack;


	// default value
	fStillRunning = false;

	// check lua state and callback tag
	if( ptLuaFn->L!=NULL && ptLuaFn->ref!=LUA_NOREF && ptLuaFn->ref!=LUA_REFNIL )
	{
		// get the current stack position
		iOldTopOfStack = lua_gettop(ptLuaFn->L);
		lua_rawgeti(ptLuaFn->L, LUA_REGISTRYINDEX, ptLuaFn->ref);
		// push the arguments on the stack
		lua_pushnumber(ptLuaFn->L, lProgressData);
		fStillRunning = callback_common(ptLuaFn, lCallbackUserData, iOldTopOfStack);
	}

	return fStillRunning;
}


bool romloader::callback_string(SWIGLUA_REF *ptLuaFn, const char *pcProgressData, size_t sizProgressData, long lCallbackUserData)
{
	bool fStillRunning;
	int iOldTopOfStack;


	// default value
	fStillRunning = false;

	// check lua state and callback tag
	if( ptLuaFn->L!=NULL && ptLuaFn->ref!=LUA_NOREF && ptLuaFn->ref!=LUA_REFNIL )
	{
		// get the current stack position
		iOldTopOfStack = lua_gettop(ptLuaFn->L);
		lua_rawgeti(ptLuaFn->L, LUA_REGISTRYINDEX, ptLuaFn->ref);
		// push the arguments on the stack
		lua_pushlstring(ptLuaFn->L, pcProgressData, sizProgressData);
		fStillRunning = callback_common(ptLuaFn, lCallbackUserData, iOldTopOfStack);
	}

	return fStillRunning;
}


bool romloader::callback_common(SWIGLUA_REF *ptLuaFn, long lCallbackUserData, int iOldTopOfStack)
{
	bool fStillRunning;
	int iResult;
	double dResult;
	int iLuaType;
	const char *pcErrMsg;
	const char *pcErrDetails;


	// check lua state and callback tag
	if( ptLuaFn->L!=NULL && ptLuaFn->ref!=LUA_NOREF && ptLuaFn->ref!=LUA_REFNIL )
	{
		lua_pushnumber(ptLuaFn->L, lCallbackUserData);
		// call the function
		iResult = lua_pcall(ptLuaFn->L, 2, 1, 0);
		if( iResult!=0 )
		{
			switch( iResult )
			{
			case LUA_ERRRUN:
				pcErrMsg = "runtime error";
				break;
			case LUA_ERRMEM:
				pcErrMsg = "memory allocation error";
				break;
			default:
				pcErrMsg = "unknown errorcode";
				break;
			}
			pcErrDetails = lua_tostring(ptLuaFn->L, -1);
			MUHKUH_PLUGIN_ERROR(ptLuaFn->L, "callback function failed: %s (%d): %s", pcErrMsg, iResult, pcErrDetails);
			fStillRunning = false;
		}
		else
		{
			// get the function's return value
			iLuaType = lua_type(ptLuaFn->L, -1);
			if( iLuaType!=LUA_TNUMBER && iLuaType!=LUA_TBOOLEAN )
			{
				MUHKUH_PLUGIN_ERROR(ptLuaFn->L, "callback function returned a non-boolean type: %d", iLuaType);
				fStillRunning = false;
			}
			else
			{
				if( iLuaType==LUA_TNUMBER )
				{
					dResult = lua_tonumber(ptLuaFn->L, -1);
					fStillRunning = (dResult!=0);
				}
				else
				{
					iResult = lua_toboolean(ptLuaFn->L, -1);
					fStillRunning = (iResult!=0);
				}
			}
		}
		// return old stack top
		lua_settop(ptLuaFn->L, iOldTopOfStack);
	}
	else
	{
		// no callback function -> keep running
		fStillRunning = true;
	}

	return fStillRunning;
}



const char *romloader::get_error_message(TRANSPORTSTATUS_T tStatus)
{
	const char *pcMessage = "Unknown error code";


	switch(tStatus)
	{
	case TRANSPORTSTATUS_OK:
		pcMessage = "OK";
		break;

	case TRANSPORTSTATUS_TIMEOUT:
		pcMessage = "timeout";
		break;

	case TRANSPORTSTATUS_PACKET_TOO_LARGE:
		pcMessage = "packet too large";
		break;

	case TRANSPORTSTATUS_RECEIVE_FAILED:
		pcMessage = "receive failed";
		break;

	case TRANSPORTSTATUS_SEND_FAILED:
		pcMessage = "send failed";
		break;

	case TRANSPORTSTATUS_FAILED_TO_SYNC:
		pcMessage = "failed to sync";
		break;

	case TRANSPORTSTATUS_CRC_MISMATCH:
		pcMessage = "CRC mismatch";
		break;

	case TRANSPORTSTATUS_MISSING_USERDATA:
		pcMessage = "missing user data";
		break;

	case TRANSPORTSTATUS_COMMAND_EXECUTION_FAILED:
		pcMessage = "command execution failed";
		break;

	case TRANSPORTSTATUS_SEQUENCE_MISMATCH:
		pcMessage = "sequence mismatch";
		break;

	case TRANSPORTSTATUS_NOT_CONNECTED:
		pcMessage = "not connected";
		break;

	case TRANSPORTSTATUS_NETX_ERROR:
		pcMessage = "the netX returned an error code";
		break;

	case TRANSPORTSTATUS_PACKET_TOO_SMALL:
		pcMessage = "the packet does not have enough data for 1 byte of user data";
		break;

	case TRANSPORTSTATUS_INVALID_PACKET_SIZE:
		pcMessage = "the packet size does not match the expected size for the packet type";
		break;

	case TRANSPORTSTATUS_UNEXPECTED_PACKET_SIZE:
		pcMessage = "the packet size does not match the number of requested bytes";
		break;

	case TRANSPORTSTATUS_UNEXPECTED_PACKET_TYP:
		pcMessage = "unexpected packet type";
		break;
	}

	return pcMessage;
}


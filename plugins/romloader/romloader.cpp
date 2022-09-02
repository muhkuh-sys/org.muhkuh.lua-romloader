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

#if defined(_MSC_VER)
#	define SLEEP_MS(ms) Sleep(ms)
#else
#	include <sys/time.h>
#	include <unistd.h>
#	define SLEEP_MS(ms) usleep(ms*1000)
#endif

romloader::romloader(const char *pcName, const char *pcTyp, muhkuh_plugin_provider *ptProvider)
 : muhkuh_plugin(pcName, pcTyp, ptProvider)
 , m_tChiptyp(ROMLOADER_CHIPTYP_UNKNOWN)
 , m_ulInfoFlags(0)
 , m_sizMaxPacketSizeClient(0)
 , m_sizPacketInputBuffer(0)
 , m_ucMonitorSequence(0)
{
/*	m_ptLog->trace("created in romloader"); */
}


romloader::romloader(const char *pcName, const char *pcTyp, const char *pcLocation, muhkuh_plugin_provider *ptProvider)
 : muhkuh_plugin(pcName, pcTyp, pcLocation, ptProvider)
 , m_tChiptyp(ROMLOADER_CHIPTYP_UNKNOWN)
 , m_ulInfoFlags(0)
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


/* Wrapper to call the functions directly from lua scripts */
bool romloader::send_packet_wrapper(const char *pcBuffer, size_t sizData)
{	
	TRANSPORTSTATUS_T tResult;
	tResult = send_packet((MIV3_PACKET_HEADER_T*)pcBuffer, sizData);
	
	return tResult;
}

/* Wrapper to call the functions directly from lua scripts */
bool romloader::receive_packet_wrapper()
{
	TRANSPORTSTATUS_T tResult;
	tResult = receive_packet();
	
	return tResult;
}
/* Wrapper for increasing, getting and setting the sequence number */
void romloader::increase_sequence_number()
{
	m_ucMonitorSequence++;
}
uint8_t romloader::get_sequence_number()
{
	return m_ucMonitorSequence;
}
void romloader::set_sequence_number(uint8_t ucSequenceNumber)
{
	m_ucMonitorSequence = ucSequenceNumber;
}
/*
* wrapper functions to set test counter that will delay the acknowlede packages
* for received packages during commands 
*/
void romloader::set_call_skip_counter(uint32_t ulCallMessageSkip, uint32_t ulStatusSkip)
{
	sRomlTestVars.ulCnt_SkipAckMessageCall = ulCallMessageSkip;
	sRomlTestVars.ulCnt_SkipAckStatusCall = ulStatusSkip;
	m_ptLog->debug("Set sRomlTestVars.ulCnt_SkipAckMessageCall to %d", sRomlTestVars.ulCnt_SkipAckMessageCall);
	m_ptLog->debug("Set sRomlTestVars.ulCnt_SkipAckStatusCall to %d", sRomlTestVars.ulCnt_SkipAckStatusCall);
}

void romloader::set_write_skip_counter(uint32_t ulStatusSkip)
{
	sRomlTestVars.ulCnt_SkipAckStatusWrite = ulStatusSkip;
	
	m_ptLog->debug("Set sRomlTestVars.ulCnt_SkipAckStatusWrite to %d", sRomlTestVars.ulCnt_SkipAckStatusWrite);
}

void romloader::set_read_skip_counter(uint32_t ulReadDataSkip,uint32_t ulStatusSkip)
{
	sRomlTestVars.ulCnt_SkipAckReadData = ulReadDataSkip;
	sRomlTestVars.ulCnt_SkipAckStatusRead = ulStatusSkip;
	m_ptLog->debug("Set sRomlTestVars.ulCnt_SkipAckReadData to %d", sRomlTestVars.ulCnt_SkipAckReadData);
	m_ptLog->debug("Set sRomlTestVars.ulCnt_SkipAckStatusRead to %d", sRomlTestVars.ulCnt_SkipAckStatusRead);
}


bool romloader::cancel_operation()
{
	/*
	* send a MONITOR_PACKET_TYP_CancelOperation packet to the netX
	*/
	TRANSPORTSTATUS_T tResult;
	
	m_ptLog->debug("Send MONITOR_PACKET_TYP_CancelOperation.");
	/* create a MIV3_PACKET_CANCEL_CALL_T packet */
	MIV3_PACKET_CANCEL_CALL_T tCancelCallPacket;
	tCancelCallPacket.s.tHeader.s.ucPacketType = MONITOR_PACKET_TYP_CancelOperation;
	// tCancelCallPacket.s.tHeader.s.ucSequenceNumber = ucSequenceNumber; // Setting the sequence number is not necessary (it will be set automatically in send_packet)
	//tCancelCallPacket.s.ucData = ucCallData;  // Not needed in MIV3_PACKET_CANCEL_CALL_T packet (netX does not know a cancel packet with that size)
	
	tResult = send_packet(&(tCancelCallPacket.s.tHeader), sizeof(MIV3_PACKET_CANCEL_CALL_T));
	
	return tResult;
	
}


bool romloader::synchronize(ROMLOADER_CHIPTYP *ptChiptyp, uint16_t *pusMiVersionMin, uint16_t *pusMiVersionMaj)
{
	/*
	Send a Knock packet to the netX and wait for a MagicData packet in return.
	
	NEW:
		If instead of a MagicData packet an other packet is received,
		we assume that the netX is still waitong for an ACK for that packet.
		In this case we send a packet 'cancel_operation' to get the netX out of the endless loop.
	*/
	const uint8_t aucKnock[5] = { '*', 0x00, 0x00, '*', '#' };
	const uint8_t aucMagicMooh[4] = { 0x4d, 0x4f, 0x4f, 0x48 };
	unsigned int uiRetryCnt;
	TRANSPORTSTATUS_T tResult;
	bool fResult;
	MIV3_PACKET_SYNC_T *ptSyncPacket;
	uint8_t ucSequence;
	uint32_t ulMiVersionMin;
	uint32_t ulMiVersionMaj;
	ROMLOADER_CHIPTYP tChipType;
	size_t sizMaxPacketSize;
	MIV3_PACKET_HEADER_T* ptPacketHeader;
	uint8_t ucSequenceNumber;


//	fprintf(stderr, "synchronize\n");
	fResult = false;

	uiRetryCnt = 10;
	do
	{
		/*
		* Send the Knock packet 
		*/
		tResult = send_raw_packet(aucKnock, sizeof(aucKnock));
		if( tResult!=TRANSPORTSTATUS_OK )
		{
			m_ptLog->error("send_sync_command: failed to send the packet!");
		}
		else
		{
			/*
			* Get an answer from the netX
			*/
			tResult = receive_packet();
			if( tResult!=TRANSPORTSTATUS_OK )
			{
				m_ptLog->error("send_sync_command: receive_packet failed with error code %d", tResult);
			}
			else
			{
				tResult = TRANSPORTSTATUS_OK;
				

				/* Treat the received packet as a sync packet at first */
				ptSyncPacket = (MIV3_PACKET_SYNC_T*)m_aucPacketInputBuffer;
				
				/* Check for the aucMagicMooh and the correct size*/
				if( m_sizPacketInputBuffer==sizeof(MIV3_PACKET_SYNC_T) && memcmp(ptSyncPacket->s.aucMagic, aucMagicMooh, sizeof(aucMagicMooh))==0 )
				{
					m_ptLog->debug("Packet:");
					m_ptLog->hexdump(muhkuh_log::MUHKUH_LOG_LEVEL_DEBUG, m_aucPacketInputBuffer, m_sizPacketInputBuffer);

					/* Get the sequence number. */
					ucSequence = ptSyncPacket->s.tHeader.s.ucSequenceNumber;
					m_ptLog->debug("Sequence number: 0x%02x", ucSequence);

					ulMiVersionMin = NETXTOH16(ptSyncPacket->s.usVersionMinor);
					ulMiVersionMaj = NETXTOH16(ptSyncPacket->s.usVersionMajor);
					m_ptLog->debug("Machine interface V%d.%d .", ulMiVersionMaj, ulMiVersionMin);

					tChipType = (ROMLOADER_CHIPTYP)(ptSyncPacket->s.ucChipType);
					m_ptLog->debug("Suspicious chip type reported via MI: %d", tChipType);

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
					if( pusMiVersionMaj!=NULL )
					{
						*pusMiVersionMaj = (uint16_t) ulMiVersionMaj;
					}
					if( pusMiVersionMin!=NULL )
					{
						*pusMiVersionMin = (uint16_t) ulMiVersionMin;
					}
					
					m_sizMaxPacketSizeClient = sizMaxPacketSize;

					fResult = true;
				}
				else
				{
					/* 
					* If the packet is not a MagicData packet we assume that the netX is still in a state where it waits for a response.
					* To get the netX out of that state we send a MONITOR_PACKET_TYP_CancelOperation packet to get it back in the main Loop.
					*/
					m_ptLog->error("Received an invalid knock response.");
					m_ptLog->hexdump(muhkuh_log::MUHKUH_LOG_LEVEL_DEBUG, m_aucPacketInputBuffer, m_sizPacketInputBuffer);
					
					/* send cancel operation packet*/
					cancel_operation();
					tResult = TRANSPORTSTATUS_FAILED_TO_SYNC;
				}
				
				
			}
		}
		
		/*
		* If transportstatus is not OK decrement the retries and print a message.
		* If no retries are left break from the loop
		*/
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
		m_ptLog->error("send_packet: packet too large: %zd bytes! Maximum size is %zd.", sizPacket, m_sizMaxPacketSizeClient);
		tResult = TRANSPORTSTATUS_PACKET_TOO_LARGE;
	}
	else if( sizPacket<sizeof(MIV3_PACKET_HEADER_T) )
	{
		m_ptLog->error("send_packet: packet too small: %zd bytes! Minimum size is %zd.", sizPacket, sizeof(MIV3_PACKET_HEADER_T));
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

/* 
* The skip_ack_test function tests if the last packet,
* that was received and is still in the buffer will be re-send.
* 
* To test that, the current packet is saved in a buffer and will be compared to new received packets.
* The parameter ulCnt_ignore_packets indicated how many packets will be compared.
*/
bool romloader::skip_ack_test(uint32_t ulCnt_ignore_packets)
{
	bool fResult;
	TRANSPORTSTATUS_T tResult;
	uint8_t aucSavePacketBuffer[m_sizMaxPacketSizeHost];
	uint32_t ulCnt_ignore;
	
	fResult = true;
	ulCnt_ignore = ulCnt_ignore_packets;
	
	/*save the current packet inside the buffer*/
	memcpy(&aucSavePacketBuffer, &m_aucPacketInputBuffer, m_sizPacketInputBuffer);
	
	m_ptLog->debug("Ignore the current packet in the receive packet %d times before sending an acknowledge", ulCnt_ignore_packets);
	
	while(ulCnt_ignore>0)
	{
		SLEEP_MS(1000);
		tResult = receive_packet();
		if (tResult != TRANSPORTSTATUS_OK)
		{
			continue;
		}
		
		if(memcmp(&aucSavePacketBuffer, &m_aucPacketInputBuffer, m_sizPacketInputBuffer)!=0 )
		{
			fResult = false;
			break;
		}
		
		ulCnt_ignore--;
		m_ptLog->debug("ignored packets remaining %d", ulCnt_ignore);
	}
	if(fResult)
	{
		m_ptLog->debug("skip_ack_test result: OK (received %d times the same packet)", ulCnt_ignore_packets);
	}
	else
	{
		m_ptLog->error("skip_ack_test result: FAILED (packet %d was wrong)", uint32_t(ulCnt_ignore_packets-ulCnt_ignore));
	}
	return fResult;
}



romloader::TRANSPORTSTATUS_T romloader::execute_command(MIV3_PACKET_HEADER_T *ptPacket, size_t sizPacket, bool *pfPacketStillValid)
{
	TRANSPORTSTATUS_T tResult;
	unsigned char ucStatus;
	unsigned char ucPacketSequence;
	unsigned char ucLastMonitorSequence;
	unsigned char ucNextMonitorSequence;
	unsigned int uiRetryCnt;
	int iResult;
	MIV3_PACKET_ACK_T *ptAckPacket;

	*pfPacketStillValid = false; //default to false

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
					ucNextMonitorSequence = m_ucMonitorSequence + 1U;
					m_ptLog->debug("Current Sequence-Number : %d  Received Sequence-Number : %d", m_ucMonitorSequence, ucPacketSequence);
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
					else if (ucNextMonitorSequence == ucPacketSequence)
					{
						/* When we get a packet from the netX where the sequence number is already one number ahead
						 * while we are waiting for the ACK, we assume that the ACK packet got lost.
						 * Since the netX won't send another ACK packet we have to continue with out process to avoid a deadlock.
						 */
						 
						m_ptLog->debug("Received a packet with a Sequence number one ahead of us (%d).", ucPacketSequence);
						m_ptLog->debug("Continue process instead of waiting for ACK packet to avoid deadlock.");
						*pfPacketStillValid = true; // set the flag to keep the packet in the buffer and use it for further use
						tResult = TRANSPORTSTATUS_OK;
						++m_ucMonitorSequence;
						
					}
					else
					{
						m_ptLog->error("execute_command: expected an ACK packet with %zd bytes, but received %zd bytes:", sizeof(MIV3_PACKET_ACK_T), m_sizPacketInputBuffer);
						m_ptLog->hexdump(muhkuh_log::MUHKUH_LOG_LEVEL_DEBUG, m_aucPacketInputBuffer, m_sizPacketInputBuffer);
						m_ptLog->error("Current Sequence-Number : %d  Received Sequence-Number : %d", m_ucMonitorSequence, ucPacketSequence);
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
	bool fPacketStillValid;  


	if( m_fIsConnected==false )
	{
		tResult = TRANSPORTSTATUS_NOT_CONNECTED;
	}
	else
	{
		m_ptLog->info("use MONITOR_PACKET_TYP_Command_Info");
		tPacketInfo.s.tHeader.s.ucPacketType =  MONITOR_PACKET_TYP_Command_Info;

		tResult = execute_command(&(tPacketInfo.s.tHeader), sizeof(MIV3_PACKET_INFO_COMMAND_T), &fPacketStillValid);
		if( tResult==TRANSPORTSTATUS_OK )
		{
			/* Receive the data. */
			if (!fPacketStillValid){
				/* only recceive the packet if the packet in the buffer is not still valid*/
				tResult = receive_packet();
			}
			else{
				fPacketStillValid = false; // set the flag back to False so the next packet will be received as usual
			}
			
			if( tResult!=TRANSPORTSTATUS_OK )
			{
				m_ptLog->error("get_info(): Failed to receive Info_Data packet.");
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


romloader::TRANSPORTSTATUS_T romloader::read_data(uint32_t ulNetxAddress, MONITOR_ACCESSSIZE_T tAccessSize, uint16_t sizDataInBytes)
{
	MIV3_PACKET_COMMAND_READ_DATA_T tPacketReadData;
	TRANSPORTSTATUS_T tResult;
	bool fResult;
	uint8_t ucPacketTyp;
	int iResult;
	MIV3_PACKET_HEADER_T *ptPacketHeader;
	MIV3_PACKET_STATUS_T *ptPacketStatus;

	
	/* The following flag is used to tell if the packet in the buffer is still valid.
	 * In this case the buffer should not be overwritten by receiving a new packet.
	 */
	bool fPacketStillValid;  

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

		tResult = execute_command(&(tPacketReadData.s.tHeader), sizeof(MIV3_PACKET_COMMAND_READ_DATA_T) , &fPacketStillValid);
		if( tResult==TRANSPORTSTATUS_OK )
		{
			
			/* Receive the data. */
			if (!fPacketStillValid){
				/* only recceive the packet if the packet in the buffer is not still valid*/
				tResult = receive_packet();
			}
			else{
				fPacketStillValid = false; // set the flag back to False so the next packet will be received as usual
			}
						
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
						if(sRomlTestVars.ulCnt_SkipAckReadData > 0)
						{
							fResult = skip_ack_test(sRomlTestVars.ulCnt_SkipAckReadData);
							if(fResult==false)
							{
								return TRANSPORTSTATUS_REPETATION_TEST_FAILES;
							}
						}
	
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

						if(sRomlTestVars.ulCnt_SkipAckStatusRead > 0)
						{
							fResult = skip_ack_test(sRomlTestVars.ulCnt_SkipAckStatusRead);
							if(fResult==false)
							{
								return TRANSPORTSTATUS_REPETATION_TEST_FAILES;
							}
						}
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
	bool fResult;
	size_t sizPacket;
	uint8_t ucPacketTyp;
	uint8_t ucStatus;
	int iResult;
	MIV3_PACKET_HEADER_T *ptPacketHeader;
	MIV3_PACKET_STATUS_T *ptPacketStatus;
	
	/* The following flag is used to tell if the packet in the buffer is still valid.
	 * In this case the buffer should not be overwritten by receiving a new packet.
	 */
	bool fPacketStillValid; 
	
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
		tResult = execute_command(&(uWriteData.s.s.s.tHeader), sizPacket, &fPacketStillValid);
				
		if( tResult==TRANSPORTSTATUS_OK )
		{
			/* Receive the status. */
			if (!fPacketStillValid){
				/* only recceive the packet if the packet in the buffer is not still valid*/
				tResult = receive_packet();
			}
			else{
				fPacketStillValid = false; // set the flash back to False so the next packet will be received as usual
			}
						
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

						if(sRomlTestVars.ulCnt_SkipAckStatusWrite > 0)
						{
							fResult = skip_ack_test(sRomlTestVars.ulCnt_SkipAckStatusWrite);
							if(fResult==false)
							{
								return TRANSPORTSTATUS_REPETATION_TEST_FAILES;
							}
						}
						ptPacketStatus = (MIV3_PACKET_STATUS_T*)m_aucPacketInputBuffer;

						if( m_sizPacketInputBuffer==sizeof(MIV3_PACKET_STATUS_T))
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
	bool fResult;
	TRANSPORTSTATUS_T tResult;
	MIV3_PACKET_COMMAND_CALL_T tCallCommand;
	MIV3_PACKET_CANCEL_CALL_T tCancelCallPacket;
//	const uint8_t aucCancelBuf[1] = { 0x2b };
	uint8_t ucStatus;
	bool fIsRunning;
	char *pcProgressData;
	size_t sizProgressData;
	uint8_t ucPacketTyp;
	uint8_t ucPacketSequenceNumber;
	MIV3_PACKET_HEADER_T *ptPacketHeader;
	MIV3_PACKET_STATUS_T *ptPacketStatus;
	
	/* The following flag is used to tell if the packet in the buffer is still valid.
	 * In this case the buffer should not be overwritten by receiving a new packet.
	 */
	bool fPacketStillValid; 

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
		tResult = execute_command(&(tCallCommand.s.tHeader), sizeof(tCallCommand), &fPacketStillValid);
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

				if (!fPacketStillValid){
					/* only recceive the packet if the packet in the buffer is not still valid*/
					tResult = receive_packet();
				}
				else{
					fPacketStillValid = false; // set the flash back to False so the next packet will be received as usual
				}
				
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
					ucPacketSequenceNumber =  ptPacketHeader->s.ucSequenceNumber;
					
					if( ucPacketTyp==MONITOR_PACKET_TYP_CallMessage)
					{
						if(sRomlTestVars.ulCnt_SkipAckMessageCall > 0)
						{
							fResult = skip_ack_test(sRomlTestVars.ulCnt_SkipAckMessageCall);
							if(fResult==false)
							{
								MUHKUH_PLUGIN_PUSH_ERROR(tLuaFn.L, "skip_ack_test failed");
								fOk = false;
								break;
							}
						}
						/* Acknowledge the packet. */
						send_ack(ucPacketSequenceNumber);
						
						if (ucPacketSequenceNumber == m_ucMonitorSequence){
							/* Increase the sequence number. */
							++m_ucMonitorSequence;

							/* NOTE: Do not check the size of the user data here. It should be possible to send 0 bytes. */
							pcProgressData = ((char*)m_aucPacketInputBuffer) + sizeof(MIV3_PACKET_HEADER_T);
							/* The size of the user data is the size of the packet - the header size - 2 bytes for the CRC16. */
							sizProgressData = m_sizPacketInputBuffer - (sizeof(MIV3_PACKET_HEADER_T) + 2U);
						}
						

					}
					else if( ucPacketTyp==MONITOR_PACKET_TYP_Status )
					{
						if(sRomlTestVars.ulCnt_SkipAckStatusCall > 0)
						{
							fResult = skip_ack_test(sRomlTestVars.ulCnt_SkipAckStatusCall);
							if(fResult==false)
							{
								MUHKUH_PLUGIN_PUSH_ERROR(tLuaFn.L, "skip_ack_test failed");
								fOk = false;
								break;
							}
						}
						ptPacketStatus = (MIV3_PACKET_STATUS_T*)m_aucPacketInputBuffer;
						if( m_sizPacketInputBuffer==sizeof(MIV3_PACKET_STATUS_T) )
						{
							/* The netX sent a status code. */
							/* Acknowledge the packet. */
							send_ack(ucPacketSequenceNumber);
						
							if (ucPacketSequenceNumber == m_ucMonitorSequence){
															

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
						cancel_operation();

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
	bool fPacketStillValid;
	
	if( m_fIsConnected==false )
	{
		MUHKUH_PLUGIN_PUSH_ERROR(tLuaFn.L, "%s(%p): not connected!", m_pcName, this);
		fOk = false;
	}
	else
	{	
		m_ptLog->info("use MONITOR_PACKET_TYP_Command_Start_USIP");
		tCommandUsip.s.tHeader.s.ucPacketType = MONITOR_PACKET_TYP_Command_Start_USIP;
		tResult = execute_command(&(tCommandUsip.s.tHeader), sizeof(tCommandUsip) , &fPacketStillValid);
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

				if (!fPacketStillValid){
					/* only recceive the packet if the packet in the buffer is not still valid*/
					tResult = receive_packet();
				}
				else{
					fPacketStillValid = false; // set the flash back to False so the next packet will be received as usual
				}
				
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
						cancel_operation();

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
	bool fPacketStillValid;
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
		tResult = execute_command(&(tCallCommand.s.tHeader), sizeof(tCallCommand), &fPacketStillValid);
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

				if (!fPacketStillValid){
					/* only recceive the packet if the packet in the buffer is not still valid*/
					tResult = receive_packet();
				}
				else{
					fPacketStillValid = false; // set the flash back to False so the next packet will be received as usual
				}
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
						cancel_operation();

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


void romloader::found_chiptyp_message()
{
	const char *pcChiptypName;
	const char *pcChiptypName90B;
	const char *pcChiptypName90C;
	
	pcChiptypName = GetChiptypName(m_tChiptyp);
	if ((m_tChiptyp == ROMLOADER_CHIPTYP_NETX90B) || (m_tChiptyp == ROMLOADER_CHIPTYP_NETX90C))
	{
		pcChiptypName90B = GetChiptypName(ROMLOADER_CHIPTYP_NETX90B);
		pcChiptypName90C = GetChiptypName(ROMLOADER_CHIPTYP_NETX90C);
		m_ptLog->debug("Suspicious chip type: %s (%d). Might be %s (%d) or %s (%d).", 
			pcChiptypName, m_tChiptyp,
			pcChiptypName90B, ROMLOADER_CHIPTYP_NETX90B,
			pcChiptypName90C, ROMLOADER_CHIPTYP_NETX90C
			);
	}
	else
	{
		m_ptLog->debug("Chip type: %s (%d).", pcChiptypName, m_tChiptyp);
	}
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

	// read the reset vector at 0x00000000
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
								m_ptLog->debug("check address: 0x%08X  value: 0x%08X masked: 0x%08X", ulCheckAddr, ulCheckVal, ulCheckValMasked);
							}
						}
							
						if ((ulCheckAddr==0) || (ulCheckValMasked==ptRstCnt->ulCheckCmpValue))
						{
							// found chip!
							tChiptyp = ptRstCnt->tChiptyp;
							//m_ptLog->debug("found chip %s.", ptRstCnt->pcChiptypName);
							break;
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
		found_chiptyp_message();
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
	fResult = __read_data32(0, &ulResetVector);
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
			if( ptRstCnt->ulResetVector==ulResetVector )
			{
				ulVersionAddr = ptRstCnt->ulVersionAddress;
				/* Read the version address. */
				fResult = __read_data32(ulVersionAddr, &ulVersion);
				if( fResult!=true )
				{
					m_ptLog->error("Failed to read the version data at 0x%08x.", ulVersionAddr);
					break;
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
								m_ptLog->debug("check address: 0x%08X  value: 0x%08X masked: 0x%08X", ulCheckAddr, ulCheckVal, ulCheckValMasked);
							}
						}

						if ((ulCheckAddr==0) || (ulCheckValMasked==ptRstCnt->ulCheckCmpValue))
						{
							/* Found chip! */
							tChiptyp = ptRstCnt->tChiptyp;
							//m_ptLog->debug("found chip %s.", ptRstCnt->pcChiptypName);
							break;
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
		found_chiptyp_message();
	}

	return fResult;
}

/* If bit 23 of netx_version is set, the romcode is
 * a development version running from RAM.
 */
#define MSK_NETX_VERSION_INTRAM_ROMCODE 0x00800000UL

/* Use the info command in MI v3.1 to get the netx_version.
 * Map this value to a chip type using atInfoIds.
 */
bool romloader::detect_chiptyp_via_info(void)
{
	const ROMLOADER_INFO_ID_T *ptInfoCnt, *ptInfoEnd;
	bool fResult;
	ROMLOADER_CHIPTYP tChiptyp;
	uint32_t ulInfoNetxVersion;
	uint32_t ulInfoFlags;
	uint32_t ulIntramRomcode;
	
	tChiptyp = ROMLOADER_CHIPTYP_UNKNOWN;
	
	fResult = get_info(&ulInfoNetxVersion, &ulInfoFlags);
	if( fResult!=TRANSPORTSTATUS_OK )
	{
		m_ptLog->error("Info command failed.");
	}
	else
	{
		/* Ignore bit 23 (Romcode running from RAM) */
		ulIntramRomcode = ulInfoNetxVersion & MSK_NETX_VERSION_INTRAM_ROMCODE;
		if (ulIntramRomcode != 0)
		{
			m_ptLog->debug("Romcode is running in RAM");
			/* clear the flag before matching the netx_version value */
			ulInfoNetxVersion &= (~MSK_NETX_VERSION_INTRAM_ROMCODE);
		}
		
		m_ptLog->debug("Mapping netx_version value to chip type");
	
		ptInfoCnt = atInfoIds;
		ptInfoEnd = ptInfoCnt + (sizeof(atInfoIds)/sizeof(atInfoIds[0]));
		
		while( ptInfoCnt<ptInfoEnd )
		{
			if (ptInfoCnt->ulNetxVersion == ulInfoNetxVersion)
			{
				tChiptyp = ptInfoCnt->tChiptyp;
				break;
			}
			++ptInfoCnt;
		}
	}
	
	/* Found something? */
	if( fResult==TRANSPORTSTATUS_OK && tChiptyp!=ROMLOADER_CHIPTYP_UNKNOWN )
	{
		/* Accept the new chip type. */
		//m_ptLog->debug("Found chip type %s.", GetChiptypName(tChiptyp));
		m_tChiptyp = tChiptyp;
		m_ulInfoFlags = ulInfoFlags;
		found_chiptyp_message();
		fResult = true;
	}
	else
	{
		m_ptLog->error("No Chiptype detected.");
		fResult = false;
	}
	return fResult;
	
}

const romloader::ROMLOADER_INFO_ID_T romloader::atInfoIds[1] =
{
	{
		0x0901020d,
		ROMLOADER_CHIPTYP_NETX90D
	},
};

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
		0x000000c0,
		0x0010d005,
		0x00005110,
		0xffffffff,
		0xe001200c,
		ROMLOADER_CHIPTYP_NETX90B,
		"netX90 Rev1"
	},
	
	{/* Note: 
			The netx90C cannot be detected by detect_chiptyp(), 
			as its detection pattern is the same as the previous one for step B. 
			The only way it might be detected is if the pattern for step B fails to 
			match due to an error. 
			In this case, a netx 90 step B might be also detected as step C. */
		0x2009fff0,
		0x000000c0,
		0x0010d005,
		0x00005110,
		0xffffffff,
		0xe001200c,
		ROMLOADER_CHIPTYP_NETX90C,
		"netX90 Rev1 (PHY V3)"
	},
	
	{
		0x2009fff0,
		0x000000c0,
		0x0010d005,
		0xff401298,
		0xff7fffff, /* mask out bit 23 which indicates that the ROM code is running in RAM */
		0x0901020d,
		ROMLOADER_CHIPTYP_NETX90D,
		"netX90 Rev2"
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
	}, 
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
	case TRANSPORTSTATUS_REPETATION_TEST_FAILES:
		pcMessage = "packet repetition test failed";
		break;
	}

	return pcMessage;
}


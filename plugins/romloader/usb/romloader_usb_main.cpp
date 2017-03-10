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


#include <ctype.h>
#include <stdio.h>

#include "romloader_usb_main.h"
#include "../machine_interface/netx/src/monitor_commands.h"

#if defined(_MSC_VER)
#       define snprintf _snprintf
#       define ssize_t size_t
#endif

/*-------------------------------------*/

#if CFG_DEBUGMSG!=0
	/* show all messages by default */
	static unsigned long s_ulCurSettings = 0xffffffff;

	#define DEBUGZONE(n)  (s_ulCurSettings&(0x00000001<<(n)))

	/* NOTE: These defines must match the ZONE_* defines. */
	#define DBG_ZONE_ERROR      0
	#define DBG_ZONE_WARNING    1
	#define DBG_ZONE_FUNCTION   2
	#define DBG_ZONE_INIT       3
	#define DBG_ZONE_VERBOSE    7

	#define ZONE_ERROR          DEBUGZONE(DBG_ZONE_ERROR)
	#define ZONE_WARNING        DEBUGZONE(DBG_ZONE_WARNING)
	#define ZONE_FUNCTION       DEBUGZONE(DBG_ZONE_FUNCTION)
	#define ZONE_INIT           DEBUGZONE(DBG_ZONE_INIT)
	#define ZONE_VERBOSE        DEBUGZONE(DBG_ZONE_VERBOSE)

	#define DEBUGMSG(cond,printf_exp) ((void)((cond)?(printf printf_exp),1:0))
#else  /* CFG_DEBUGMSG!=0 */
	#define DEBUGMSG(cond,printf_exp) ((void)0)
#endif /* CFG_DEBUGMSG!=0 */


/*-------------------------------------*/

const char *romloader_usb_provider::m_pcPluginNamePattern = "romloader_usb_%02x_%02x";

romloader_usb_provider::romloader_usb_provider(swig_type_info *p_romloader_usb, swig_type_info *p_romloader_usb_reference)
 : muhkuh_plugin_provider("romloader_usb")
 , m_ptUsbDevice(NULL)
{
	DEBUGMSG(ZONE_FUNCTION, ("+romloader_usb_provider::romloader_usb_provider(): p_romloader_usb=%p, p_romloader_usb_reference=%p\n", p_romloader_usb, p_romloader_usb_reference));

	/* Get the romloader_usb LUA type. */
	m_ptPluginTypeInfo = p_romloader_usb;
	m_ptReferenceTypeInfo = p_romloader_usb_reference;

	/* Create a new libusb context */
	m_ptUsbDevice = new romloader_usb_device_libusb(m_pcPluginId);

	DEBUGMSG(ZONE_FUNCTION, ("-romloader_usb_provider::romloader_usb_provider()\n"));
}


romloader_usb_provider::~romloader_usb_provider(void)
{
	DEBUGMSG(ZONE_FUNCTION, ("+romloader_usb_provider::~romloader_usb_provider()\n"));

	if( m_ptUsbDevice!=NULL )
	{
		delete m_ptUsbDevice;
	}

	DEBUGMSG(ZONE_FUNCTION, ("-romloader_usb_provider::~romloader_usb_provider()\n"));
}


int romloader_usb_provider::DetectInterfaces(lua_State *ptLuaStateForTableAccess)
{
	int iResult;
	romloader_usb_reference **pptReferences;
	romloader_usb_reference **pptRefCnt;
	romloader_usb_reference **pptRefEnd;
	romloader_usb_reference *ptRef;
	size_t sizReferences;


	DEBUGMSG(ZONE_FUNCTION, ("+romloader_usb_provider::DetectInterfaces(): ptLuaStateForTableAccess=%p\n", ptLuaStateForTableAccess));

	sizReferences = 0;

	/* check the libusb context */
	if( m_ptUsbDevice==NULL )
	{
		/* libusb was not initialized */
		printf("%s(%p): libusb was not initialized!\n", m_pcPluginId, this);
	}
	else
	{
		/* detect devices */
		pptReferences = NULL;
		iResult = m_ptUsbDevice->detect_interfaces(&pptReferences, &sizReferences, this);
		if( iResult==0 && pptReferences!=NULL )
		{
			pptRefCnt = pptReferences;
			pptRefEnd = pptReferences + sizReferences;
			while( pptRefCnt<pptRefEnd )
			{
				ptRef = *pptRefCnt;
				if( ptRef!=NULL )
				{
					add_reference_to_table(ptLuaStateForTableAccess, ptRef);
				}
				++pptRefCnt;
			}
			/* Free the list of references.
			 * NOTE: Do not free the entries of the list. They are already linked to a LUA table.
			 */
			free(pptReferences);
		}
	}

	DEBUGMSG(ZONE_FUNCTION, ("-romloader_usb_provider::DetectInterfaces(): sizReferences=%zd\n", sizReferences));

	return sizReferences;
}


romloader_usb *romloader_usb_provider::ClaimInterface(const muhkuh_plugin_reference *ptReference)
{
	romloader_usb *ptPlugin;
	const char *pcName;
	unsigned int uiBusNr;
	unsigned int uiDevAdr;


	DEBUGMSG(ZONE_FUNCTION, ("+romloader_usb_provider::ClaimInterface(): ptReference=%p\n", ptReference));

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
		else if( sscanf(pcName, m_pcPluginNamePattern, &uiBusNr, &uiDevAdr)!=2 )
		{
			fprintf(stderr, "%s(%p): claim_interface(): invalid name: %s\n", m_pcPluginId, this, pcName);
		}
		else if( m_ptUsbDevice==NULL )
		{
			/* libusb was not initialized */
			fprintf(stderr, "%s(%p): libusb was not initialized!\n", m_pcPluginId, this);
		}
		else
		{
			ptPlugin = new romloader_usb(pcName, m_pcPluginId, this, uiBusNr, uiDevAdr);
			printf("%s(%p): claim_interface(): claimed interface %s.\n", m_pcPluginId, this, pcName);
		}
	}

	DEBUGMSG(ZONE_FUNCTION, ("-romloader_usb_provider::ClaimInterface(): ptPlugin=%p\n", ptPlugin));

	return ptPlugin;
}


bool romloader_usb_provider::ReleaseInterface(muhkuh_plugin *ptPlugin)
{
	bool fOk;
	const char *pcName;
	unsigned int uiBusNr;
	unsigned int uiDevAdr;


	DEBUGMSG(ZONE_FUNCTION, ("+romloader_usb_provider::ReleaseInterface(): ptPlugin=%p\n", ptPlugin));

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

	DEBUGMSG(ZONE_FUNCTION, ("-romloader_usb_provider::ReleaseInterface(): fOk=%d\n", fOk));

	return fOk;
}


/*-------------------------------------*/

romloader_usb::romloader_usb(const char *pcName, const char *pcTyp, romloader_usb_provider *ptProvider, unsigned int uiBusNr, unsigned int uiDeviceAdr)
 : romloader(pcName, pcTyp, ptProvider)
 , m_ptUsbProvider(ptProvider)
 , m_uiBusNr(uiBusNr)
 , m_uiDeviceAdr(uiDeviceAdr)
 , m_ptUsbDevice(NULL)
{
	DEBUGMSG(ZONE_FUNCTION, ("+romloader_usb::romloader_usb(): pcName='%s', pcTyp='%s', ptProvider=%p, uiBusNr=%d, uiDeviceAdr=%d\n", pcName, pcTyp, ptProvider, uiBusNr, uiDeviceAdr));

	/* create a new libusb context */
	m_ptUsbDevice = new romloader_usb_device_libusb(m_pcName);

	DEBUGMSG(ZONE_FUNCTION, ("-romloader_usb::romloader_usb()\n"));
}


romloader_usb::romloader_usb(const char *pcName, const char *pcTyp, const char *pcLocation, romloader_usb_provider *ptProvider, unsigned int uiBusNr, unsigned int uiDeviceAdr)
 : romloader(pcName, pcTyp, pcLocation, ptProvider)
 , m_ptUsbProvider(ptProvider)
 , m_uiBusNr(uiBusNr)
 , m_uiDeviceAdr(uiDeviceAdr)
 , m_ptUsbDevice(NULL)
{
	DEBUGMSG(ZONE_FUNCTION, ("+romloader_usb::romloader_usb(): pcName='%s', pcTyp='%s', ptProvider=%p, uiBusNr=%d, uiDeviceAdr=%d\n", pcName, pcTyp, ptProvider, uiBusNr, uiDeviceAdr));
	
	/* create a new libusb context */
	m_ptUsbDevice = new romloader_usb_device_libusb(m_pcName);
	
	DEBUGMSG(ZONE_FUNCTION, ("-romloader_usb::romloader_usb()\n"));
}


romloader_usb::~romloader_usb(void)
{
	DEBUGMSG(ZONE_FUNCTION, ("-romloader_usb::~romloader_usb()\n"));

	if( m_ptUsbDevice!=NULL )
	{
		delete(m_ptUsbDevice);
	}

	DEBUGMSG(ZONE_FUNCTION, ("-romloader_usb::~romloader_usb()\n"));
}


/* open the connection to the device */
void romloader_usb::Connect(lua_State *ptClientData)
{
	int iResult;


	DEBUGMSG(ZONE_FUNCTION, ("+romloader_usb::Connect(): ptClientData=%p\n", ptClientData));

	if( m_fIsConnected!=false )
	{
		printf("%s(%p): already connected, ignoring new connect request\n", m_pcName, this);
	}
	else
	{
		iResult = m_ptUsbDevice->Connect(m_uiBusNr, m_uiDeviceAdr);
		if( iResult!=LIBUSB_SUCCESS )
		{
			MUHKUH_PLUGIN_PUSH_ERROR(ptClientData, "%s(%p): failed to connect to device", m_pcName, this);
		}
		else
		{
			/* NOTE: set m_fIsConnected to true here or detect_chiptyp and chip_init will fail! */
			m_fIsConnected = true;

			/* Synchronize with the client to get these settings:
			 *   chip type
			 *   sequence number
			 *   maximum packet size
			 */
			if( synchronize()!=true )
			{
				fprintf(stderr, "%s(%p): failed to synchronize!", m_pcName, this);
				MUHKUH_PLUGIN_PUSH_ERROR(ptClientData, "%s(%p): failed to synchronize!", m_pcName, this);
				m_fIsConnected = false;
				m_ptUsbDevice->Disconnect();
				iResult = -1;
			}
		}

		DEBUGMSG(ZONE_FUNCTION, ("-romloader_usb::Connect(): iResult=%d\n", iResult));

		if( iResult!=LIBUSB_SUCCESS )
		{
			MUHKUH_PLUGIN_EXIT_ERROR(ptClientData);
		}
	}
}



/* close the connection to the device */
void romloader_usb::Disconnect(lua_State *ptClientData)
{
	DEBUGMSG(ZONE_FUNCTION, ("+romloader_usb::Disconnect(): ptClientData=%p\n", ptClientData));

	/* NOTE: allow disconnects even if the plugin was already disconnected. */

	if( m_ptUsbDevice!=NULL )
	{
		m_ptUsbDevice->Disconnect();
	}

	m_fIsConnected = false;

	DEBUGMSG(ZONE_FUNCTION, ("-romloader_usb::Disconnect()\n"));
}



bool romloader_usb::synchronize(void)
{
	const uint8_t aucMagicMooh[4] = { 0x4d, 0x4f, 0x4f, 0x48 };
	bool fResult;
	int iResult;
	uint8_t ucData;
	uint8_t aucInBuf[64];
	size_t sizInBuf;
	/* The expected knock response is 13 bytes:
	 *    1 status byte
	 *   11 data bytes (see monitor_commands.h for more information)
	 */
	const size_t sizExpectedResponse = 12;
	uint8_t ucSequence;
	uint32_t ulMiVersionMin;
	uint32_t ulMiVersionMaj;
	ROMLOADER_CHIPTYP tChipType;
	size_t sizMaxPacketSize;


	DEBUGMSG(ZONE_FUNCTION, ("+romloader_usb::synchronize()\n"));

	fResult = false;

	if( m_fIsConnected==false )
	{
		fprintf(stderr, "%s(%p): synchronize: not connected!\n", m_pcName, this);
	}
	else
	{
		ucData = MONITOR_COMMAND_Magic;
		iResult = m_ptUsbDevice->execute_command(&ucData, 1, aucInBuf, sizeof(aucInBuf), &sizInBuf);
		if( iResult!=0 )
		{
			fprintf(stderr, "%s(%p): synchronize: failed to transfer command!\n", m_pcName, this);
		}
		else if( sizInBuf==0 )
		{
			fprintf(stderr, "%s(%p): synchronize: received empty answer!\n", m_pcName, this);
		}
		else if( sizInBuf!=sizExpectedResponse )
		{
			fprintf(stderr, "synchronize: Received knock sequence with invalid size of %ld. Expected: %ld.\n", sizInBuf, sizExpectedResponse);
			hexdump(aucInBuf, sizInBuf, 0);
		}
		else if( memcmp(aucInBuf+1, aucMagicMooh, sizeof(aucMagicMooh))!=0 )
		{
			fprintf(stderr, "Received knock sequence has no magic.\n");
			hexdump(aucInBuf, sizInBuf, 0);
		}
		else
		{
			fprintf(stderr, "Packet:\n");
			hexdump(aucInBuf, sizInBuf, 0);

			/* Get the sequence number from the status byte. */
			ucSequence = (aucInBuf[0x00] & MONITOR_SEQUENCE_MSK) >> MONITOR_SEQUENCE_SRT;
			fprintf(stderr, "Sequence number: 0x%02x\n", ucSequence);

			ulMiVersionMin =  ((uint32_t)(aucInBuf[0x05])) |
			                 (((uint32_t)(aucInBuf[0x06]))<<8U);
			ulMiVersionMaj =  ((uint32_t)(aucInBuf[0x07])) |
			                 (((uint32_t)(aucInBuf[0x08]))<<8U);
			printf("Machine interface V%d.%d.\n", ulMiVersionMaj, ulMiVersionMin);

			tChipType = (ROMLOADER_CHIPTYP)(aucInBuf[0x09]);
			printf("Chip type : %d\n", tChipType);

			sizMaxPacketSize =  ((size_t)(aucInBuf[0x0a])) |
			                   (((size_t)(aucInBuf[0x0b]))<<8U);
			printf("Maximum packet size: 0x%04lx\n", sizMaxPacketSize);
			/* Limit the packet size to the buffer size. */
			if( sizMaxPacketSize>m_sizMaxPacketSizeHost )
			{
				sizMaxPacketSize = m_sizMaxPacketSizeHost;
				printf("Limit maximum packet size to 0x%04lx\n", sizMaxPacketSize);
			}

			/* Set the new values. */
			m_uiMonitorSequence = ucSequence;
			m_tChiptyp = tChipType;
			m_sizMaxPacketSizeClient = sizMaxPacketSize;

			next_sequence_number();
			fResult = true;
		}
	}

	DEBUGMSG(ZONE_FUNCTION, ("-romloader_usb::synchronize(): fResult=%d\n", fResult));

	return fResult;
}


/* 
Increment the sequence number
Call when a packet has been received from the netX (the sequence number is incremented on the netX side, too).
- after synchronize()
- after receiving a response packet
Do not increment if the packet send/receive operation failed or no response was received.
If the netX did not receive the packet, it is still assuming the current sequence number.
*/
void romloader_usb::next_sequence_number()
{
	m_uiMonitorSequence = (m_uiMonitorSequence + 1) & (MONITOR_SEQUENCE_MSK>>MONITOR_SEQUENCE_SRT);
}


/* Update the sequence number in the packet */
void romloader_usb::packet_update_sequence_number(uint8_t *aucCommand)
{
	aucCommand[0] &= ((uint8_t) ~MONITOR_SEQUENCE_MSK);
	aucCommand[0] |= (uint8_t)(m_uiMonitorSequence << MONITOR_SEQUENCE_SRT);
}


romloader_usb::USBSTATUS_T romloader_usb::execute_command(uint8_t *aucCommand, size_t sizCommand, size_t *psizReceivePacket)
{
	int iResult;
	USBSTATUS_T tResult;
	uint8_t ucStatus;
	unsigned int uiSequence;
	unsigned int uiRetryCnt;
	size_t sizPacketInputBuffer;


	DEBUGMSG(ZONE_FUNCTION, ("+romloader_usb::execute_command(): aucCommand=%p, sizCommand=%zd, psizReceivePacket=%p\n", aucCommand, sizCommand, psizReceivePacket));

	uiRetryCnt = 10;
	do
	{
		packet_update_sequence_number(aucCommand);
		iResult = m_ptUsbDevice->execute_command(aucCommand, sizCommand, m_aucPacketInputBuffer, m_sizMaxPacketSizeHost, &sizPacketInputBuffer);
		if( iResult!=0 )
		{
			fprintf(stderr, "%s(%p): Failed to exchange data.\n", m_pcName, this);
			tResult = USBSTATUS_SEND_FAILED;
		}
		/* The packet must be at least 1 byte long.
		 *  0 : status.
		 */
		else if( sizPacketInputBuffer<1 )
		{
			fprintf(stderr, "! execute_command: packet size too small: %ld. It has no user data!\n", sizPacketInputBuffer);
			tResult = USBSTATUS_MISSING_USERDATA;
		}
		else
		{
			ucStatus = (m_aucPacketInputBuffer[0] & MONITOR_STATUS_MSK) >> MONITOR_STATUS_SRT;
			uiSequence = (m_aucPacketInputBuffer[0] & MONITOR_SEQUENCE_MSK) >> MONITOR_SEQUENCE_SRT;
			if( ucStatus!=0 )
			{
				/* got a response from netX with an error code */
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
				else
				{
					/* got response from netx -> increment sequence number */
					next_sequence_number();
				}

				tResult = USBSTATUS_COMMAND_EXECUTION_FAILED;
			}
			else if( uiSequence!=m_uiMonitorSequence )
			{
				/* got a response from netX with status OK but incorrect sequence number -> synchronize */
				fprintf(stderr, "! execute_command: the sequence does not match: %d / %d\n", uiSequence, m_uiMonitorSequence);
				synchronize();

				tResult = USBSTATUS_SEQUENCE_MISMATCH;
			}
			else
			{
				/* got a response from netX with status OK -> success */
				*psizReceivePacket = sizPacketInputBuffer;
				tResult = USBSTATUS_OK;

				/* got response from netx -> increment sequence number */
				next_sequence_number();
			}
		}

		if( tResult!=USBSTATUS_OK )
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

	} while( tResult!=USBSTATUS_OK );

	DEBUGMSG(ZONE_FUNCTION, ("-romloader_usb::execute_command(): tResult=0x%d\n", tResult));

	return tResult;
}



/* read a byte (8bit) from the netx to the pc */
uint8_t romloader_usb::read_data08(lua_State *ptClientData, uint32_t ulNetxAddress)
{
	uint8_t ucData;
	bool fOk;
	int iResult;
	size_t sizInBuf;
	uint8_t ucCommand;
	uint8_t ucStatus;
	USBSTATUS_T tResult;


	DEBUGMSG(ZONE_FUNCTION, ("+romloader_usb::read_data08(): ptClientData=%p, ulNetxAddress=0x%08x\n", ptClientData, ulNetxAddress));

	if( m_fIsConnected==false )
	{
		MUHKUH_PLUGIN_PUSH_ERROR(ptClientData, "%s(%p): read_data08: not connected!", m_pcName, this);
		fOk = false;
	}
	else
	{
		/* Construct the command packet. */
		m_aucPacketOutputBuffer[0x00] = MONITOR_COMMAND_Read |
		                                (MONITOR_ACCESSSIZE_Byte<<MONITOR_ACCESSSIZE_SRT);
		m_aucPacketOutputBuffer[0x01] = 1;  /* Read 8 bit -> 1 byte. */
		m_aucPacketOutputBuffer[0x02] = 0x00;
		m_aucPacketOutputBuffer[0x03] = (uint8_t)( ulNetxAddress       & 0xffU);
		m_aucPacketOutputBuffer[0x04] = (uint8_t)((ulNetxAddress>> 8U) & 0xffU);
		m_aucPacketOutputBuffer[0x05] = (uint8_t)((ulNetxAddress>>16U) & 0xffU);
		m_aucPacketOutputBuffer[0x06] = (uint8_t)((ulNetxAddress>>24U) & 0xffU);

		tResult = execute_command(m_aucPacketOutputBuffer, 7, &sizInBuf);
		if( tResult!=USBSTATUS_OK )
		{
			MUHKUH_PLUGIN_PUSH_ERROR(ptClientData, "%s(%p): failed to execute command!", m_pcName, this);
			fOk = false;
		}
		else
		{
			if( sizInBuf!=2 )
			{
				MUHKUH_PLUGIN_PUSH_ERROR(ptClientData, "%s(%p): read_data08: answer has invalid size: %d", m_pcName, this, sizInBuf);
				hexdump(m_aucPacketInputBuffer, sizInBuf, 0);
				fOk = false;
			}
			else
			{
				ucData  = m_aucPacketInputBuffer[1];
				fOk = true;
			}
		}
	}

	DEBUGMSG(ZONE_FUNCTION, ("-romloader_usb::read_data08(): fOk=%d, ucData=0x%02x\n", fOk, ucData));

	if( fOk!=true )
	{
		printf("Exit Error\n");
		MUHKUH_PLUGIN_EXIT_ERROR(ptClientData);
	}

	return ucData;
}


/* read a word (16bit) from the netx to the pc */
uint16_t romloader_usb::read_data16(lua_State *ptClientData, uint32_t ulNetxAddress)
{
	uint16_t usData;
	bool fOk;
	int iResult;
	size_t sizInBuf;
	uint8_t ucCommand;
	uint8_t ucStatus;
	USBSTATUS_T tResult;


	DEBUGMSG(ZONE_FUNCTION, ("+romloader_usb::read_data16(): ptClientData=%p, ulNetxAddress=0x%08x\n", ptClientData, ulNetxAddress));

	if( m_fIsConnected==false )
	{
		MUHKUH_PLUGIN_PUSH_ERROR(ptClientData, "%s(%p): read_data16: not connected!", m_pcName, this);
		fOk = false;
	}
	else
	{
		/* Construct the command packet. */
		m_aucPacketOutputBuffer[0x00] = MONITOR_COMMAND_Read |
		                                (MONITOR_ACCESSSIZE_Word<<MONITOR_ACCESSSIZE_SRT);
		m_aucPacketOutputBuffer[0x01] = 2;  /* Read 16 bits -> 2 bytes. */
		m_aucPacketOutputBuffer[0x02] = 0;
		m_aucPacketOutputBuffer[0x03] = (uint8_t)( ulNetxAddress       & 0xffU);
		m_aucPacketOutputBuffer[0x04] = (uint8_t)((ulNetxAddress>> 8U) & 0xffU);
		m_aucPacketOutputBuffer[0x05] = (uint8_t)((ulNetxAddress>>16U) & 0xffU);
		m_aucPacketOutputBuffer[0x06] = (uint8_t)((ulNetxAddress>>24U) & 0xffU);

		tResult = execute_command(m_aucPacketOutputBuffer, 7, &sizInBuf);
		if( tResult!=USBSTATUS_OK )
		{
			MUHKUH_PLUGIN_PUSH_ERROR(ptClientData, "%s(%p): failed to execute command!", m_pcName, this);
			fOk = false;
		}
		else
		{
			if( sizInBuf!=3 )
			{
				MUHKUH_PLUGIN_PUSH_ERROR(ptClientData, "%s(%p): read_data16: answer has invalid size!", m_pcName, this);
				hexdump(m_aucPacketInputBuffer, sizInBuf, 0);
				fOk = false;
			}
			else
			{
				usData =  ((uint16_t)(m_aucPacketInputBuffer[1])) |
				         (((uint16_t)(m_aucPacketInputBuffer[2]))<<8U);
				fOk = true;
			}
		}
	}

	DEBUGMSG(ZONE_FUNCTION, ("-romloader_usb::read_data16(): fOk=%d, usData=0x%04x\n", fOk, usData));

	if( fOk!=true )
	{
		printf("Exit Error\n");
		MUHKUH_PLUGIN_EXIT_ERROR(ptClientData);
	}

	return usData;
}


/* read a long (32bit) from the netx to the pc */
uint32_t romloader_usb::read_data32(lua_State *ptClientData, uint32_t ulNetxAddress)
{
	uint32_t ulData;
	bool fOk;
	int iResult;
	size_t sizInBuf;
	uint8_t ucCommand;
	uint8_t ucStatus;
	USBSTATUS_T tResult;


	DEBUGMSG(ZONE_FUNCTION, ("+romloader_usb::read_data32(): ptClientData=%p, ulNetxAddress=0x%08x\n", ptClientData, ulNetxAddress));

	if( m_fIsConnected==false )
	{
		MUHKUH_PLUGIN_PUSH_ERROR(ptClientData, "%s(%p): read_data32: not connected!", m_pcName, this);
		fOk = false;
	}
	else
	{
		/* Construct the command packet. */
		m_aucPacketOutputBuffer[0x00] = MONITOR_COMMAND_Read |
		                                (MONITOR_ACCESSSIZE_Long<<MONITOR_ACCESSSIZE_SRT);
		m_aucPacketOutputBuffer[0x01] = 4;  /* Read 32 bits -> 4 bytes. */
		m_aucPacketOutputBuffer[0x02] = 0;
		m_aucPacketOutputBuffer[0x03] = (uint8_t)( ulNetxAddress       & 0xffU);
		m_aucPacketOutputBuffer[0x04] = (uint8_t)((ulNetxAddress>> 8U) & 0xffU);
		m_aucPacketOutputBuffer[0x05] = (uint8_t)((ulNetxAddress>>16U) & 0xffU);
		m_aucPacketOutputBuffer[0x06] = (uint8_t)((ulNetxAddress>>24U) & 0xffU);

		tResult = execute_command(m_aucPacketOutputBuffer, 7, &sizInBuf);
		if( tResult!=USBSTATUS_OK )
		{
			MUHKUH_PLUGIN_PUSH_ERROR(ptClientData, "%s(%p): failed to execute command!", m_pcName, this);
			fOk = false;
		}
		else
		{
			if( sizInBuf!=5 )
			{
				MUHKUH_PLUGIN_PUSH_ERROR(ptClientData, "%s(%p): read_data32: answer has invalid size!", m_pcName, this);
				hexdump(m_aucPacketInputBuffer, sizInBuf, 0);
				fOk = false;
			}
			else
			{
				ulData =  ((uint32_t)m_aucPacketInputBuffer[1]) |
				         (((uint32_t)m_aucPacketInputBuffer[2]) <<  8U) |
				         (((uint32_t)m_aucPacketInputBuffer[3]) << 16U) |
				         (((uint32_t)m_aucPacketInputBuffer[4]) << 24U);
				fOk = true;
			}
		}
	}

	/* Print the function end marker already here as the MUHKUH_PLUGIN_EXIT_ERROR might leave the function. */
	DEBUGMSG(ZONE_FUNCTION, ("-romloader_usb::read_data32(): fOk=%d, ulData=0x%08x\n", fOk, ulData));

	if( fOk!=true )
	{
		printf("Exit Error\n");
		MUHKUH_PLUGIN_EXIT_ERROR(ptClientData);
	}

	return ulData;
}


/* read a byte array from the netx to the pc  */
void romloader_usb::read_image(uint32_t ulNetxAddress, uint32_t ulSize, char **ppcBUFFER_OUT, size_t *psizBUFFER_OUT, SWIGLUA_REF tLuaFn, long lCallbackUserData)
{
	char *pcBufferStart;
	char *pcBuffer;
	size_t sizBuffer;
	bool fOk;
	int iResult;
	size_t sizChunk;
	size_t sizInBuf;
	uint8_t ucCommand;
	uint8_t ucStatus;
	bool fIsRunning;
	long lBytesProcessed;
	USBSTATUS_T tResult;


	DEBUGMSG(ZONE_FUNCTION, ("+romloader_usb::read_image(): ulNetxAddress=0x%08x, ulSize=0x%08x, ppcBUFFER_OUT=%p, psizBUFFER_OUT=%p, tLuaFn.L=%p, lCallbackUserData=0x%08lx\n", ulNetxAddress, ulSize, ppcBUFFER_OUT, psizBUFFER_OUT, tLuaFn.L, lCallbackUserData));

	/* Be optimistic. */
	fOk = true;

	pcBufferStart = NULL;
	sizBuffer = 0;

	if( m_fIsConnected==false )
	{
		MUHKUH_PLUGIN_PUSH_ERROR(tLuaFn.L, "%s(%p): read_image: not connected!", m_pcName, this);
		fOk = false;
	}
	/* if ulSize == 0, we return with fOk == true, pcBufferStart == NULL and sizBuffer == 0 */
	else if ( ulSize>0 )
	{
		pcBufferStart = (char*)malloc(ulSize);
		if( pcBufferStart==NULL )
		{
			MUHKUH_PLUGIN_PUSH_ERROR(tLuaFn.L, "%s(%p): read_image: failed to allocate %d bytes!", m_pcName, this, ulSize);
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
				if( sizChunk>m_sizMaxPacketSizeClient-1 )
				{
					sizChunk = m_sizMaxPacketSizeClient - 1;
				}

				/* Construct the command packet. */
				m_aucPacketOutputBuffer[0x00] = MONITOR_COMMAND_Read |
				                                (MONITOR_ACCESSSIZE_Byte<<MONITOR_ACCESSSIZE_SRT);
				m_aucPacketOutputBuffer[0x01] = (uint8_t)( sizChunk       & 0xffU);
				m_aucPacketOutputBuffer[0x02] = (uint8_t)((sizChunk>> 8U) & 0xffU);
				m_aucPacketOutputBuffer[0x03] = (uint8_t)( ulNetxAddress       & 0xffU);
				m_aucPacketOutputBuffer[0x04] = (uint8_t)((ulNetxAddress>> 8U) & 0xffU);
				m_aucPacketOutputBuffer[0x05] = (uint8_t)((ulNetxAddress>>16U) & 0xffU);
				m_aucPacketOutputBuffer[0x06] = (uint8_t)((ulNetxAddress>>24U) & 0xffU);

				tResult = execute_command(m_aucPacketOutputBuffer, 7, &sizInBuf);
				if( tResult!=USBSTATUS_OK )
				{
					MUHKUH_PLUGIN_PUSH_ERROR(tLuaFn.L, "%s(%p): failed to execute command!", m_pcName, this);
					fOk = false;
					break;
				}
				else
				{
					if( sizInBuf!=sizChunk+1 )
					{
						MUHKUH_PLUGIN_PUSH_ERROR(tLuaFn.L, "%s(%p): read_image: answer has invalid size!", m_pcName, this);
						hexdump(m_aucPacketInputBuffer, sizInBuf, 0);
						fOk = false;
						break;
					}
					else
					{
						memcpy(pcBuffer, m_aucPacketInputBuffer+1, sizChunk);
						pcBuffer += sizChunk;
						ulSize -= sizChunk;
						ulNetxAddress += sizChunk;
						lBytesProcessed += sizChunk;

						fIsRunning = callback_long(&tLuaFn, lBytesProcessed, lCallbackUserData);
						if( fIsRunning!=true )
						{
							MUHKUH_PLUGIN_PUSH_ERROR(tLuaFn.L, "%s(%p): read_image cancelled!", m_pcName, this);
							fOk = false;
							break;
						}
					}
				}
			} while( ulSize!=0 );
		}
	}

	DEBUGMSG(ZONE_FUNCTION, ("-romloader_usb::read_image(): fOk=%d, pcBufferStart=%p, sizBuffer=%zd\n", fOk, pcBufferStart, sizBuffer));

	if( fOk == true )
	{
		*ppcBUFFER_OUT = pcBufferStart;
		*psizBUFFER_OUT = sizBuffer;
	}
	else
	{
		if ( pcBufferStart!=NULL)
		{
			free(pcBufferStart);
		}
		printf("Exit Error\n");
		MUHKUH_PLUGIN_EXIT_ERROR(tLuaFn.L);
	}
}


/* write a byte (8bit) from the pc to the netx */
void romloader_usb::write_data08(lua_State *ptClientData, uint32_t ulNetxAddress, uint8_t ucData)
{
	bool fOk;
	int iResult;
	size_t sizInBuf;
	USBSTATUS_T tResult;


	DEBUGMSG(ZONE_FUNCTION, ("+romloader_usb::write_data08(): ptClientData=%p, ulNetxAddress=0x%08x, ucData=0x%02x\n", ptClientData, ulNetxAddress, ucData));

	if( m_fIsConnected==false )
	{
		MUHKUH_PLUGIN_PUSH_ERROR(ptClientData, "%s(%p): write_data08: not connected!", m_pcName, this);
		fOk = false;
	}
	else
	{
		/* Construct the command packet. */
		m_aucPacketOutputBuffer[0x00] = MONITOR_COMMAND_Write |
		                                (MONITOR_ACCESSSIZE_Byte<<MONITOR_ACCESSSIZE_SRT);
		m_aucPacketOutputBuffer[0x01] = 1;  /* Write 8 bit -> 1 byte. */
		m_aucPacketOutputBuffer[0x02] = 0;
		m_aucPacketOutputBuffer[0x03] = (uint8_t)( ulNetxAddress       & 0xffU);
		m_aucPacketOutputBuffer[0x04] = (uint8_t)((ulNetxAddress>> 8U) & 0xffU);
		m_aucPacketOutputBuffer[0x05] = (uint8_t)((ulNetxAddress>>16U) & 0xffU);
		m_aucPacketOutputBuffer[0x06] = (uint8_t)((ulNetxAddress>>24U) & 0xffU);
		m_aucPacketOutputBuffer[0x07] = ucData;

		tResult = execute_command(m_aucPacketOutputBuffer, 8, &sizInBuf);
		if( tResult!=USBSTATUS_OK )
		{
			MUHKUH_PLUGIN_PUSH_ERROR(ptClientData, "%s(%p): failed to execute command!", m_pcName, this);
			fOk = false;
		}
		else if( sizInBuf!=1 )
		{
			MUHKUH_PLUGIN_PUSH_ERROR(ptClientData, "%s(%p): write_data08: answer has invalid size!", m_pcName, this);
			hexdump(m_aucPacketInputBuffer, sizInBuf, 0);
			fOk = false;
		}
		else
		{
			fOk = true;
		}
	}

	DEBUGMSG(ZONE_FUNCTION, ("-romloader_usb::write_data08(): fOk=%d\n", fOk));

	if( fOk!=true )
	{
		printf("Exit Error\n");
		MUHKUH_PLUGIN_EXIT_ERROR(ptClientData);
	}
}


/* write a word (16bit) from the pc to the netx */
void romloader_usb::write_data16(lua_State *ptClientData, uint32_t ulNetxAddress, uint16_t usData)
{
	bool fOk;
	int iResult;
	size_t sizInBuf;
	USBSTATUS_T tResult;


	DEBUGMSG(ZONE_FUNCTION, ("+romloader_usb::write_data16(): ptClientData=%p, ulNetxAddress=0x%08x, usData=0x%04x\n", ptClientData, ulNetxAddress, usData));

	if( m_fIsConnected==false )
	{
		MUHKUH_PLUGIN_PUSH_ERROR(ptClientData, "%s(%p): write_data16: not connected!", m_pcName, this);
		fOk = false;
	}
	else
	{
		/* Construct the command packet. */
		m_aucPacketOutputBuffer[0x00] = MONITOR_COMMAND_Write |
		                                (MONITOR_ACCESSSIZE_Word<<MONITOR_ACCESSSIZE_SRT);
		m_aucPacketOutputBuffer[0x01] = 2;  /* Write 16 bit -> 2 bytes. */
		m_aucPacketOutputBuffer[0x02] = 0;
		m_aucPacketOutputBuffer[0x03] = (uint8_t)( ulNetxAddress       & 0xffU);
		m_aucPacketOutputBuffer[0x04] = (uint8_t)((ulNetxAddress>> 8U) & 0xffU);
		m_aucPacketOutputBuffer[0x05] = (uint8_t)((ulNetxAddress>>16U) & 0xffU);
		m_aucPacketOutputBuffer[0x06] = (uint8_t)((ulNetxAddress>>24U) & 0xffU);
		m_aucPacketOutputBuffer[0x07] = (uint8_t)( usData      & 0xffU);
		m_aucPacketOutputBuffer[0x08] = (uint8_t)((usData>>8U) & 0xffU);

		tResult = execute_command(m_aucPacketOutputBuffer, 9, &sizInBuf);
		if( tResult!=USBSTATUS_OK )
		{
			MUHKUH_PLUGIN_PUSH_ERROR(ptClientData, "%s(%p): failed to execute command!", m_pcName, this);
			fOk = false;
		}
		else if( sizInBuf!=1 )
		{
			MUHKUH_PLUGIN_PUSH_ERROR(ptClientData, "%s(%p): write_data16: answer has invalid size!", m_pcName, this);
			hexdump(m_aucPacketInputBuffer, sizInBuf, 0);
			fOk = false;
		}
		else
		{
			fOk = true;
		}
	}

	DEBUGMSG(ZONE_FUNCTION, ("-romloader_usb::write_data16(): fOk=%d\n", fOk));

	if( fOk!=true )
	{
		printf("Exit Error\n");
		MUHKUH_PLUGIN_EXIT_ERROR(ptClientData);
	}
}


/* write a long (32bit) from the pc to the netx */
void romloader_usb::write_data32(lua_State *ptClientData, uint32_t ulNetxAddress, uint32_t ulData)
{
	bool fOk;
	int iResult;
	size_t sizInBuf;
	USBSTATUS_T tResult;


	DEBUGMSG(ZONE_FUNCTION, ("+romloader_usb::write_data32(): ptClientData=%p, ulNetxAddress=0x%08x, ulData=0x%08x\n", ptClientData, ulNetxAddress, ulData));

	if( m_fIsConnected==false )
	{
		MUHKUH_PLUGIN_PUSH_ERROR(ptClientData, "%s(%p): write_data32: not connected!", m_pcName, this);
		fOk = false;
	}
	else
	{
		/* Construct the command packet. */
		m_aucPacketOutputBuffer[0x00] = MONITOR_COMMAND_Write |
		                                (MONITOR_ACCESSSIZE_Long<<MONITOR_ACCESSSIZE_SRT);
		m_aucPacketOutputBuffer[0x01] = 4;  /* Write 32 bit -> 4 bytes. */
		m_aucPacketOutputBuffer[0x02] = 0;
		m_aucPacketOutputBuffer[0x03] = (uint8_t)( ulNetxAddress       & 0xffU);
		m_aucPacketOutputBuffer[0x04] = (uint8_t)((ulNetxAddress>> 8U) & 0xffU);
		m_aucPacketOutputBuffer[0x05] = (uint8_t)((ulNetxAddress>>16U) & 0xffU);
		m_aucPacketOutputBuffer[0x06] = (uint8_t)((ulNetxAddress>>24u) & 0xffU);
		m_aucPacketOutputBuffer[0x07] = (uint8_t)( ulData       & 0xffU);
		m_aucPacketOutputBuffer[0x08] = (uint8_t)((ulData>> 8U) & 0xffU);
		m_aucPacketOutputBuffer[0x09] = (uint8_t)((ulData>>16U) & 0xffU);
		m_aucPacketOutputBuffer[0x0a] = (uint8_t)((ulData>>24U) & 0xffU);

		tResult = execute_command(m_aucPacketOutputBuffer, 11, &sizInBuf);
		if( tResult!=USBSTATUS_OK )
		{
			MUHKUH_PLUGIN_PUSH_ERROR(ptClientData, "%s(%p): failed to execute command!", m_pcName, this);
			fOk = false;
		}
		else if( sizInBuf!=1 )
		{
			MUHKUH_PLUGIN_PUSH_ERROR(ptClientData, "%s(%p): write_data32: answer has invalid size!", m_pcName, this);
			hexdump(m_aucPacketInputBuffer, sizInBuf, 0);
			fOk = false;
		}
		else
		{
			fOk = true;
		}
	}

	DEBUGMSG(ZONE_FUNCTION, ("-romloader_usb::write_data32(): fOk=%d\n", fOk));

	if( fOk!=true )
	{
		printf("Exit Error\n");
		MUHKUH_PLUGIN_EXIT_ERROR(ptClientData);
	}
}


/* write a byte array from the pc to the netx */
void romloader_usb::write_image(uint32_t ulNetxAddress, const char *pcBUFFER_IN, size_t sizBUFFER_IN, SWIGLUA_REF tLuaFn, long lCallbackUserData)
{
	bool fOk;
	size_t sizChunk;
	int iResult;
	size_t sizInBuf;
	bool fIsRunning;
	long lBytesProcessed;
	USBSTATUS_T tResult;


	DEBUGMSG(ZONE_FUNCTION, ("+romloader_usb::write_image(): ulNetxAddress=0x%08x, pcBUFFER_IN=%p, sizBUFFER_IN=%zd, tLuaFn.L=%p, lCallbackUserData=0x%08lx\n", ulNetxAddress, pcBUFFER_IN, sizBUFFER_IN, tLuaFn.L, lCallbackUserData));

	/* Be optimistic. */
	fOk = true;

	if( m_fIsConnected==false )
	{
		MUHKUH_PLUGIN_PUSH_ERROR(tLuaFn.L, "%s(%p): write_image: not connected!", m_pcName, this);
		fOk = false;
	}
	else if( sizBUFFER_IN!=0 )
	{
		lBytesProcessed = 0;
		do
		{
			sizChunk = sizBUFFER_IN;
			if( sizChunk>(m_sizMaxPacketSizeClient-7) )
			{
				sizChunk = m_sizMaxPacketSizeClient - 7;
			}

			/* Construct the command packet. */
			m_aucPacketOutputBuffer[0x00] = MONITOR_COMMAND_Write |
			                                (MONITOR_ACCESSSIZE_Byte<<MONITOR_ACCESSSIZE_SRT);
			m_aucPacketOutputBuffer[0x01] = (uint8_t)( sizChunk       & 0xffU);
			m_aucPacketOutputBuffer[0x02] = (uint8_t)((sizChunk>> 8U) & 0xffU);
			m_aucPacketOutputBuffer[0x03] = (uint8_t)( ulNetxAddress       & 0xffU);
			m_aucPacketOutputBuffer[0x04] = (uint8_t)((ulNetxAddress>> 8U) & 0xffU);
			m_aucPacketOutputBuffer[0x05] = (uint8_t)((ulNetxAddress>>16U) & 0xffU);
			m_aucPacketOutputBuffer[0x06] = (uint8_t)((ulNetxAddress>>24U) & 0xffU);
			memcpy(m_aucPacketOutputBuffer+7, pcBUFFER_IN, sizChunk);

			tResult = execute_command(m_aucPacketOutputBuffer, sizChunk+7, &sizInBuf);
			if( tResult!=USBSTATUS_OK )
			{
				MUHKUH_PLUGIN_PUSH_ERROR(tLuaFn.L, "%s(%p): failed to execute command!", m_pcName, this);
				fOk = false;
				break;
			}
			else if( sizInBuf!=1 )
			{
				MUHKUH_PLUGIN_PUSH_ERROR(tLuaFn.L, "%s(%p): write_image: answer has invalid size!", m_pcName, this);
				hexdump(m_aucPacketInputBuffer, sizInBuf, 0);
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
					MUHKUH_PLUGIN_PUSH_ERROR(tLuaFn.L, "%s(%p): write_image cancelled!", m_pcName, this);
					fOk = false;
					break;
				}
			}
		} while( sizBUFFER_IN!=0 );
	}

	DEBUGMSG(ZONE_FUNCTION, ("-romloader_usb::write_image(): fOk=%d\n", fOk));

	if( fOk!=true )
	{
		printf("Exit Error\n");
		MUHKUH_PLUGIN_EXIT_ERROR(tLuaFn.L);
	}
}


/* call routine */
void romloader_usb::call(uint32_t ulNetxAddress, uint32_t ulParameterR0, SWIGLUA_REF tLuaFn, long lCallbackUserData)
{
	bool fOk;
	int iResult;
	size_t sizInBuf;
	uint8_t ucStatus;
	bool fIsRunning;
	char *pcProgressData;
	size_t sizProgressData;
	USBSTATUS_T tResult;


	DEBUGMSG(ZONE_FUNCTION, ("+romloader_usb::call(): ulNetxAddress=0x%08x, ulParameterR0=0x%08x, tLuaFn.L=%p, lCallbackUserData=0x%08lx\n", ulNetxAddress, ulParameterR0, tLuaFn.L, lCallbackUserData));

	if( m_fIsConnected==false )
	{
		MUHKUH_PLUGIN_PUSH_ERROR(tLuaFn.L, "%s(%p): call: not connected!", m_pcName, this);
		fOk = false;
	}
	else
	{
		/* Construct the command packet. */
		m_aucPacketOutputBuffer[0x00] = MONITOR_COMMAND_Execute;
		m_aucPacketOutputBuffer[0x01] = (uint8_t)( ulNetxAddress      & 0xff);
		m_aucPacketOutputBuffer[0x02] = (uint8_t)((ulNetxAddress>>8 ) & 0xff);
		m_aucPacketOutputBuffer[0x03] = (uint8_t)((ulNetxAddress>>16) & 0xff);
		m_aucPacketOutputBuffer[0x04] = (uint8_t)((ulNetxAddress>>24) & 0xff);
		m_aucPacketOutputBuffer[0x05] = (uint8_t)( ulParameterR0      & 0xff);
		m_aucPacketOutputBuffer[0x06] = (uint8_t)((ulParameterR0>>8 ) & 0xff);
		m_aucPacketOutputBuffer[0x07] = (uint8_t)((ulParameterR0>>16) & 0xff);
		m_aucPacketOutputBuffer[0x08] = (uint8_t)((ulParameterR0>>24) & 0xff);

		tResult = execute_command(m_aucPacketOutputBuffer, 9, &sizInBuf);
		if( tResult!=USBSTATUS_OK )
		{
			MUHKUH_PLUGIN_PUSH_ERROR(tLuaFn.L, "%s(%p): failed to execute command!", m_pcName, this);
			fOk = false;
		}
		else if( sizInBuf!=1 )
		{
			MUHKUH_PLUGIN_PUSH_ERROR(tLuaFn.L, "%s(%p): call: answer has invalid size!", m_pcName, this);
			hexdump(m_aucPacketInputBuffer, sizInBuf, 0);
			fOk = false;
		}
		else
		{
			/* Receive message packets. */
			while(1)
			{
				pcProgressData = NULL;
				sizProgressData = 0;

				iResult = m_ptUsbDevice->receive_packet(m_aucPacketInputBuffer, m_sizMaxPacketSizeClient, &sizInBuf, 500);
				if( iResult==LIBUSB_ERROR_TIMEOUT )
				{ // should we print anything if a timeout occurs?
				}
				else if( iResult!=0 )
				{
					MUHKUH_PLUGIN_PUSH_ERROR(tLuaFn.L, "%s(%p): call: failed to receive packet! (error %d)", m_pcName, this, iResult);
					fOk = false;
					break;
				}
				else
				{
					ucStatus = m_aucPacketInputBuffer[0] & MONITOR_STATUS_MSK;
					if( sizInBuf==1 && ucStatus==MONITOR_STATUS_CallFinished )
					{
						fOk = true;
						break;
					}
					else if( sizInBuf>=1 && ucStatus==MONITOR_STATUS_CallMessage )
					{
//						printf("Received message:\n");
//						hexdump(aucInBuf+1, sizInBuf-1, 0);
						pcProgressData = (char*)m_aucPacketInputBuffer+1;
						sizProgressData = sizInBuf-1;
					}
					else if( sizInBuf!=0 )
					{
						printf("Received invalid packet:\n");
						hexdump(m_aucPacketInputBuffer, sizInBuf, 0);

						MUHKUH_PLUGIN_PUSH_ERROR(tLuaFn.L, "%s(%p): call: received invalid packet!", m_pcName, this);
						fOk = false;
						break;
					}
				}

				if (pcProgressData != NULL)
				{
					fIsRunning = callback_string(&tLuaFn, pcProgressData, sizProgressData, lCallbackUserData);
					if( fIsRunning!=true )
					{
						/* Send a cancel request to the device. */
						m_aucPacketOutputBuffer[0] = 0x2b;
						iResult = m_ptUsbDevice->send_packet(m_aucPacketOutputBuffer, 1, 100);

						MUHKUH_PLUGIN_PUSH_ERROR(tLuaFn.L, "%s(%p): the call was cancelled!", m_pcName, this);
						fOk = false;
						break;
					}
				}

			}
		}
	}

	DEBUGMSG(ZONE_FUNCTION, ("-romloader_usb::call(): fOk=%d\n", fOk));

	if( fOk!=true )
	{
		MUHKUH_PLUGIN_EXIT_ERROR(tLuaFn.L);
	}
}


void romloader_usb::hexdump(const uint8_t *pucData, uint32_t ulSize, uint32_t ulNetxAddress)
{
	const uint8_t *pucDumpCnt, *pucDumpEnd;
	uint32_t ulAddressCnt;
	size_t sizBytesLeft;
	size_t sizChunkSize;
	size_t sizChunkCnt;


	// show a hexdump of the data
	pucDumpCnt = pucData;
	pucDumpEnd = pucData + ulSize;
	ulAddressCnt = ulNetxAddress;
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


/*-------------------------------------*/


romloader_usb_reference::romloader_usb_reference(void)
 : muhkuh_plugin_reference()
{
}


romloader_usb_reference::romloader_usb_reference(const char *pcName, const char *pcTyp, bool fIsUsed, romloader_usb_provider *ptProvider)
 : muhkuh_plugin_reference(pcName, pcTyp, fIsUsed, ptProvider)
{
}


romloader_usb_reference::romloader_usb_reference(const char *pcName, const char *pcTyp, const char *pcLocation, bool fIsUsed, romloader_usb_provider *ptProvider)
 : muhkuh_plugin_reference(pcName, pcTyp, pcLocation, fIsUsed, ptProvider)
{
}


romloader_usb_reference::romloader_usb_reference(const romloader_usb_reference *ptCloneMe)
 : muhkuh_plugin_reference(ptCloneMe)
{
}


/*-------------------------------------*/


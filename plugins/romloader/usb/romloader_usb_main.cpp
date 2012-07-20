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
	#define snprintf _snprintf
	#define ssize_t size_t
#endif

/*-------------------------------------*/

class romloader_usb_read_functinoid : public romloader_read_functinoid
{
public:
	romloader_usb_read_functinoid(romloader_usb *ptDevice, lua_State *ptClientData)
	{
		m_ptDevice = ptDevice;
		m_ptClientData = ptClientData;
	}

	unsigned long read_data32(unsigned long ulAddress)
	{
		unsigned long ulValue;


		ulValue = m_ptDevice->read_data32(m_ptClientData, ulAddress);
		return ulValue;
	}

private:
	romloader_usb *m_ptDevice;
	lua_State *m_ptClientData;
};


/*-------------------------------------*/

const char *romloader_usb_provider::m_pcPluginNamePattern = "romloader_usb_%02x_%02x";

romloader_usb_provider::romloader_usb_provider(swig_type_info *p_romloader_usb, swig_type_info *p_romloader_usb_reference)
 : muhkuh_plugin_provider("romloader_usb")
 , m_ptUsbDevice(NULL)
{
    printf("%s(%p): provider create\n", m_pcPluginId, this);

	/* get the romloader_usb lua type */
	m_ptPluginTypeInfo = p_romloader_usb;
	m_ptReferenceTypeInfo = p_romloader_usb_reference;

	libusb_load();

	/* create a new libusb context */
	m_ptUsbDevice = new romloader_usb_device_libusb(m_pcPluginId);
}


romloader_usb_provider::~romloader_usb_provider(void)
{
	printf("%s(%p): provider delete\n", m_pcPluginId, this); 

	libusb_unload();

	if( m_ptUsbDevice!=NULL )
	{
		delete m_ptUsbDevice;
	}
}


int romloader_usb_provider::DetectInterfaces(lua_State *ptLuaStateForTableAccess)
{
	int iResult;
	romloader_usb_reference **pptReferences;
	romloader_usb_reference **pptRefCnt;
	romloader_usb_reference **pptRefEnd;
	romloader_usb_reference *ptRef;
	size_t sizReferences;


	sizReferences = 0;

	if (!libusb_isloaded())
	{
		return sizReferences;
	}

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
		}
	}

	return sizReferences;
}


romloader_usb *romloader_usb_provider::ClaimInterface(const muhkuh_plugin_reference *ptReference)
{
	romloader_usb *ptPlugin;
	const char *pcName;
	unsigned int uiBusNr;
	unsigned int uiDevAdr;


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
			printf("%s(%p): libusb was not initialized!\n", m_pcPluginId, this);
		}
		else
		{
			ptPlugin = new romloader_usb(pcName, m_pcPluginId, this, uiBusNr, uiDevAdr);
			printf("%s(%p): claim_interface(): claimed interface %s.\n", m_pcPluginId, this, pcName);
		}
	}

	return ptPlugin;
}


bool romloader_usb_provider::ReleaseInterface(muhkuh_plugin *ptPlugin)
{
	bool fOk;
	const char *pcName;
	unsigned int uiBusNr;
	unsigned int uiDevAdr;


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
	/* create a new libusb context */
	m_ptUsbDevice = new romloader_usb_device_libusb(m_pcName);
}


romloader_usb::~romloader_usb(void)
{
	if( m_ptUsbDevice!=NULL )
	{
		delete(m_ptUsbDevice);
	}
}


/* open the connection to the device */
void romloader_usb::Connect(lua_State *ptClientData)
{
	int iResult;
	romloader_usb_read_functinoid tFn(this, ptClientData);


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

		if( iResult!=LIBUSB_SUCCESS )
		{
			MUHKUH_PLUGIN_EXIT_ERROR(ptClientData);
		}
	}
}


/* close the connection to the device */
void romloader_usb::Disconnect(lua_State *ptClientData)
{
	printf("%s(%p): disconnect()\n", m_pcName, this);

	/* NOTE: allow disconnects even if the plugin was already disconnected. */

	if( m_ptUsbDevice!=NULL )
	{
		m_ptUsbDevice->Disconnect();
	}

	m_fIsConnected = false;
}



bool romloader_usb::synchronize(void)
{
	const unsigned char aucMagicMooh[4] = { 0x4d, 0x4f, 0x4f, 0x48 };
	bool fResult;
	int iResult;
	unsigned char ucData;
	unsigned char aucInBuf[64];
	size_t sizInBuf;
	/* The expected knock response is 13 bytes:
	 *    1 status byte
	 *   12 data bytes (see monitor_commands.h for more information)
	 */
	const size_t sizExpectedResponse = 13;
	unsigned char ucSequence;
	unsigned long ulMiVersionMin;
	unsigned long ulMiVersionMaj;
	ROMLOADER_CHIPTYP tChipType;
	ROMLOADER_ROMCODE tRomCode;
	size_t sizMaxPacketSize;


	printf("+synchronize()\n");

	fResult = false;

	if( m_fIsConnected==false )
	{
		fprintf(stderr, "%s(%p): synchronize: not connected!\n", m_pcName, this);
	}
	else
	{
		ucData = 0xffU;
		iResult = m_ptUsbDevice->execute_command(&ucData, 1, aucInBuf, &sizInBuf);
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
			fprintf(stderr, "synchronize: Received knock sequence with invalid size of %d. Expected: %d.\n", sizInBuf, sizExpectedResponse);
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

			ulMiVersionMin =  ((unsigned long)(aucInBuf[0x05])) |
			                 (((unsigned long)(aucInBuf[0x06]))<<8U);
			ulMiVersionMaj =  ((unsigned long)(aucInBuf[0x07])) |
			                 (((unsigned long)(aucInBuf[0x08]))<<8U);
			printf("Machine interface V%ld.%ld.\n", ulMiVersionMaj, ulMiVersionMin);

			tChipType = (ROMLOADER_CHIPTYP)(aucInBuf[0x09]);
			tRomCode  = (ROMLOADER_ROMCODE)(aucInBuf[0x0a]);
			printf("Chip type : %d\n", tChipType);
			printf("ROM code : %d\n", tRomCode);

			sizMaxPacketSize =  ((size_t)(aucInBuf[0x0b])) |
			                   (((size_t)(aucInBuf[0x0c]))<<8U);
			printf("Maximum packet size: 0x%04x\n", sizMaxPacketSize);
			/* Limit the packet size to the buffer size. */
			if( sizMaxPacketSize>m_sizMaxPacketSizeHost )
			{
				sizMaxPacketSize = m_sizMaxPacketSizeHost;
				printf("Limit maximum packet size to 0x%04x\n", sizMaxPacketSize);
			}

			/* Set the new values. */
			m_uiMonitorSequence = ucSequence;
			m_tChiptyp = tChipType;
			m_tRomcode = tRomCode;
			m_sizMaxPacketSizeClient = sizMaxPacketSize;

			fResult = true;
		}
	}

	printf("-synchronize(): fResult=%d\n", fResult);

	return fResult;
}



romloader_usb::USBSTATUS_T romloader_usb::execute_command(const unsigned char *aucCommand, size_t sizCommand, size_t *psizReceivePacket)
{
	int iResult;
	USBSTATUS_T tResult;
	unsigned char ucStatus;
	unsigned int uiSequence;
	unsigned int uiRetryCnt;
	size_t sizPacketInputBuffer;


	uiRetryCnt = 10;
	do
	{
		iResult = m_ptUsbDevice->execute_command(aucCommand, sizCommand, m_aucPacketInputBuffer, &sizPacketInputBuffer);
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
			fprintf(stderr, "! execute_command: packet size too small: %d. It has no user data!\n", sizPacketInputBuffer);
			tResult = USBSTATUS_MISSING_USERDATA;
		}
		else
		{
			ucStatus = (m_aucPacketInputBuffer[0] & MONITOR_STATUS_MSK) >> MONITOR_STATUS_SRT;
			uiSequence = (m_aucPacketInputBuffer[0] & MONITOR_SEQUENCE_MSK) >> MONITOR_SEQUENCE_SRT;
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

				tResult = USBSTATUS_COMMAND_EXECUTION_FAILED;
			}
			else if( uiSequence!=m_uiMonitorSequence )
			{
				fprintf(stderr, "! execute_command: the sequence does not match: %d / %d\n", uiSequence, m_uiMonitorSequence);
				synchronize();
				tResult = USBSTATUS_SEQUENCE_MISMATCH;
			}
			else
			{
				*psizReceivePacket = sizPacketInputBuffer;
				tResult = USBSTATUS_OK;
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

	printf("-execute_command(): tResult=%d\n", tResult);

	return tResult;
}



/* read a byte (8bit) from the netx to the pc */
unsigned char romloader_usb::read_data08(lua_State *ptClientData, unsigned long ulNetxAddress)
{
	unsigned char ucData;
	bool fOk;
	int iResult;
	size_t sizInBuf;
	unsigned char ucCommand;
	unsigned char ucStatus;
	USBSTATUS_T tResult;


	printf("+read_data08(): ptClientData=0x%p, ulNetxAddress=0x%08lx\n", ptClientData, ulNetxAddress);

	if( m_fIsConnected==false )
	{
		MUHKUH_PLUGIN_PUSH_ERROR(ptClientData, "%s(%p): read_data08: not connected!", m_pcName, this);
		fOk = false;
	}
	else
	{
		/* Get the next sequence number. */
		m_uiMonitorSequence = (m_uiMonitorSequence + 1) & (MONITOR_SEQUENCE_MSK>>MONITOR_SEQUENCE_SRT);

		/* Construct the command packet. */
		m_aucPacketOutputBuffer[0x00] = MONITOR_COMMAND_Read |
		                                (MONITOR_ACCESSSIZE_Byte<<MONITOR_ACCESSSIZE_SRT) |
		                                (unsigned char)(m_uiMonitorSequence << MONITOR_SEQUENCE_SRT);
		m_aucPacketOutputBuffer[0x01] = sizeof(unsigned char);
		m_aucPacketOutputBuffer[0x02] = (unsigned char)( ulNetxAddress       & 0xffU);
		m_aucPacketOutputBuffer[0x03] = (unsigned char)((ulNetxAddress>> 8U) & 0xffU);
		m_aucPacketOutputBuffer[0x04] = (unsigned char)((ulNetxAddress>>16U) & 0xffU);
		m_aucPacketOutputBuffer[0x05] = (unsigned char)((ulNetxAddress>>24U) & 0xffU);

		tResult = execute_command(m_aucPacketOutputBuffer, 6, &sizInBuf);
		printf("execute_command returned %d\n", tResult);
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

	printf("-read_data08(): fOk=%d, ucData=0x%02x\n", fOk, ucData);

	if( fOk!=true )
	{
		printf("Exit Error\n");
		MUHKUH_PLUGIN_EXIT_ERROR(ptClientData);
	}

	return ucData;
}


/* read a word (16bit) from the netx to the pc */
unsigned short romloader_usb::read_data16(lua_State *ptClientData, unsigned long ulNetxAddress)
{
	unsigned short usData;
	bool fOk;
	int iResult;
	size_t sizInBuf;
	unsigned char ucCommand;
	unsigned char ucStatus;
	USBSTATUS_T tResult;


	printf("+read_data16(): ptClientData=0x%p, ulNetxAddress=0x%08lx\n", ptClientData, ulNetxAddress);

	if( m_fIsConnected==false )
	{
		MUHKUH_PLUGIN_PUSH_ERROR(ptClientData, "%s(%p): read_data16: not connected!", m_pcName, this);
		fOk = false;
	}
	else
	{
		/* Get the next sequence number. */
		m_uiMonitorSequence = (m_uiMonitorSequence + 1) & (MONITOR_SEQUENCE_MSK>>MONITOR_SEQUENCE_SRT);

		/* Construct the command packet. */
		m_aucPacketOutputBuffer[0x00] = MONITOR_COMMAND_Read |
		                                (MONITOR_ACCESSSIZE_Word<<MONITOR_ACCESSSIZE_SRT) |
		                                (unsigned char)(m_uiMonitorSequence << MONITOR_SEQUENCE_SRT);
		m_aucPacketOutputBuffer[0x01] = sizeof(unsigned short);
		m_aucPacketOutputBuffer[0x02] = (unsigned char)( ulNetxAddress       & 0xffU);
		m_aucPacketOutputBuffer[0x03] = (unsigned char)((ulNetxAddress>> 8U) & 0xffU);
		m_aucPacketOutputBuffer[0x04] = (unsigned char)((ulNetxAddress>>16U) & 0xffU);
		m_aucPacketOutputBuffer[0x05] = (unsigned char)((ulNetxAddress>>24U) & 0xffU);

		tResult = execute_command(m_aucPacketOutputBuffer, 6, &sizInBuf);
		printf("execute_command returned %d\n", tResult);
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
				usData =  ((unsigned short)(m_aucPacketInputBuffer[1])) |
				         (((unsigned short)(m_aucPacketInputBuffer[2]))<<8U);
				fOk = true;
			}
		}
	}

	printf("-read_data16(): fOk=%d, usData=0x%04x\n", fOk, usData);

	if( fOk!=true )
	{
		printf("Exit Error\n");
		MUHKUH_PLUGIN_EXIT_ERROR(ptClientData);
	}

	return usData;
}


/* read a long (32bit) from the netx to the pc */
unsigned long romloader_usb::read_data32(lua_State *ptClientData, unsigned long ulNetxAddress)
{
	unsigned long ulData;
	bool fOk;
	int iResult;
	size_t sizInBuf;
	unsigned char ucCommand;
	unsigned char ucStatus;
	USBSTATUS_T tResult;


	printf("+read_data32(): ptClientData=0x%p, ulNetxAddress=0x%08lx\n", ptClientData, ulNetxAddress);

	if( m_fIsConnected==false )
	{
		MUHKUH_PLUGIN_PUSH_ERROR(ptClientData, "%s(%p): read_data32: not connected!", m_pcName, this);
		fOk = false;
	}
	else
	{
		/* Get the next sequence number. */
		m_uiMonitorSequence = (m_uiMonitorSequence + 1) & (MONITOR_SEQUENCE_MSK>>MONITOR_SEQUENCE_SRT);

		/* Construct the command packet. */
		m_aucPacketOutputBuffer[0x00] = MONITOR_COMMAND_Read |
		                                (MONITOR_ACCESSSIZE_Long<<MONITOR_ACCESSSIZE_SRT) |
		                                (unsigned char)(m_uiMonitorSequence << MONITOR_SEQUENCE_SRT);
		m_aucPacketOutputBuffer[0x01] = sizeof(unsigned long);
		m_aucPacketOutputBuffer[0x02] = (unsigned char)( ulNetxAddress       & 0xffU);
		m_aucPacketOutputBuffer[0x03] = (unsigned char)((ulNetxAddress>> 8U) & 0xffU);
		m_aucPacketOutputBuffer[0x04] = (unsigned char)((ulNetxAddress>>16U) & 0xffU);
		m_aucPacketOutputBuffer[0x05] = (unsigned char)((ulNetxAddress>>24U) & 0xffU);

		tResult = execute_command(m_aucPacketOutputBuffer, 6, &sizInBuf);
		printf("execute_command returned %d\n", tResult);
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
				ulData =  ((unsigned long)m_aucPacketInputBuffer[1]) |
				         (((unsigned long)m_aucPacketInputBuffer[2]) <<  8U) |
				         (((unsigned long)m_aucPacketInputBuffer[3]) << 16U) |
				         (((unsigned long)m_aucPacketInputBuffer[4]) << 24U);
				fOk = true;
			}
		}
	}

	/* Print the function end marker already here as the MUHKUH_PLUGIN_EXIT_ERROR might leave the function. */
	printf("-read_data32(): fOk=%d, ulData=0x%08lx\n", fOk, ulData);

	if( fOk!=true )
	{
		printf("Exit Error\n");
		MUHKUH_PLUGIN_EXIT_ERROR(ptClientData);
	}

	return ulData;
}


/* read a byte array from the netx to the pc  */
void romloader_usb::read_image(unsigned long ulNetxAddress, unsigned long ulSize, char **ppcBUFFER_OUT, size_t *psizBUFFER_OUT, SWIGLUA_REF tLuaFn, long lCallbackUserData)
{
	char *pcBufferStart;
	char *pcBuffer;
	size_t sizBuffer;
	bool fOk;
	int iResult;
	size_t sizChunk;
	size_t sizInBuf;
	unsigned char ucCommand;
	unsigned char ucStatus;
	bool fIsRunning;
	long lBytesProcessed;
	USBSTATUS_T tResult;


	printf("+read_image(): ulNetxAddress=0x%08lx, ulSize=0x%08lx, ppcBUFFER_OUT=0x%p, psizBUFFER_OUT=0x%p, tLuaFn.L=0x%p, lCallbackUserData=0x%08lx\n", ulNetxAddress, ulSize, ppcBUFFER_OUT, psizBUFFER_OUT, tLuaFn.L, lCallbackUserData);

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

				/* Get the next sequence number. */
				m_uiMonitorSequence = (m_uiMonitorSequence + 1) & (MONITOR_SEQUENCE_MSK>>MONITOR_SEQUENCE_SRT);

				/* Construct the command packet. */
				m_aucPacketOutputBuffer[0x00] = MONITOR_COMMAND_Read |
				                                (MONITOR_ACCESSSIZE_Byte<<MONITOR_ACCESSSIZE_SRT) |
				                                (unsigned char)(m_uiMonitorSequence << MONITOR_SEQUENCE_SRT);
				m_aucPacketOutputBuffer[0x01] = (unsigned char)sizChunk;
				m_aucPacketOutputBuffer[0x02] = (unsigned char)( ulNetxAddress       & 0xffU);
				m_aucPacketOutputBuffer[0x03] = (unsigned char)((ulNetxAddress>> 8U) & 0xffU);
				m_aucPacketOutputBuffer[0x04] = (unsigned char)((ulNetxAddress>>16U) & 0xffU);
				m_aucPacketOutputBuffer[0x05] = (unsigned char)((ulNetxAddress>>24U) & 0xffU);

				tResult = execute_command(m_aucPacketOutputBuffer, 6, &sizInBuf);
				printf("execute_command returned %d\n", tResult);
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

	printf("-read_image(): fOk=%d, pcBufferStart=0x%p, sizBuffer=0x%08x\n", fOk, pcBufferStart, sizBuffer);

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
void romloader_usb::write_data08(lua_State *ptClientData, unsigned long ulNetxAddress, unsigned char ucData)
{
	bool fOk;
	int iResult;
	size_t sizInBuf;
	unsigned char ucCommand;
	unsigned char ucStatus;
	USBSTATUS_T tResult;


	printf("+write_data08(): ptClientData=0x%p, ulNetxAddress=0x08lx, ucData=0x%02lx\n", ptClientData, ulNetxAddress, ucData);

	if( m_fIsConnected==false )
	{
		MUHKUH_PLUGIN_PUSH_ERROR(ptClientData, "%s(%p): write_data08: not connected!", m_pcName, this);
		fOk = false;
	}
	else
	{
		/* Get the next sequence number. */
		m_uiMonitorSequence = (m_uiMonitorSequence + 1) & (MONITOR_SEQUENCE_MSK>>MONITOR_SEQUENCE_SRT);

		/* Construct the command packet. */
		m_aucPacketOutputBuffer[0x00] = MONITOR_COMMAND_Write |
		                                (MONITOR_ACCESSSIZE_Byte<<MONITOR_ACCESSSIZE_SRT) |
		                                (unsigned char)(m_uiMonitorSequence << MONITOR_SEQUENCE_SRT);
		m_aucPacketOutputBuffer[0x01] = sizeof(unsigned char);
		m_aucPacketOutputBuffer[0x02] = (unsigned char)( ulNetxAddress       & 0xffU);
		m_aucPacketOutputBuffer[0x03] = (unsigned char)((ulNetxAddress>> 8U) & 0xffU);
		m_aucPacketOutputBuffer[0x04] = (unsigned char)((ulNetxAddress>>16U) & 0xffU);
		m_aucPacketOutputBuffer[0x05] = (unsigned char)((ulNetxAddress>>24U) & 0xffU);
		m_aucPacketOutputBuffer[0x06] = ucData;

		tResult = execute_command(m_aucPacketOutputBuffer, 7, &sizInBuf);
		printf("write_data08: execute_command returned %d\n", tResult);
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

	printf("-write_data08(): fOk=%d\n", fOk);

	if( fOk!=true )
	{
		printf("Exit Error\n");
		MUHKUH_PLUGIN_EXIT_ERROR(ptClientData);
	}
}


/* write a word (16bit) from the pc to the netx */
void romloader_usb::write_data16(lua_State *ptClientData, unsigned long ulNetxAddress, unsigned short usData)
{
	bool fOk;
	int iResult;
	size_t sizInBuf;
	unsigned char ucCommand;
	unsigned char ucStatus;
	USBSTATUS_T tResult;


	printf("+write_data16(): ptClientData=0x%p, ulNetxAddress=0x08lx, usData=0x%04lx\n", ptClientData, ulNetxAddress, usData);

	if( m_fIsConnected==false )
	{
		MUHKUH_PLUGIN_PUSH_ERROR(ptClientData, "%s(%p): write_data16: not connected!", m_pcName, this);
		fOk = false;
	}
	else
	{
		/* Get the next sequence number. */
		m_uiMonitorSequence = (m_uiMonitorSequence + 1) & (MONITOR_SEQUENCE_MSK>>MONITOR_SEQUENCE_SRT);

		/* Construct the command packet. */
		m_aucPacketOutputBuffer[0x00] = MONITOR_COMMAND_Write |
		                                (MONITOR_ACCESSSIZE_Word<<MONITOR_ACCESSSIZE_SRT) |
		                                (unsigned char)(m_uiMonitorSequence << MONITOR_SEQUENCE_SRT);
		m_aucPacketOutputBuffer[0x01] = sizeof(unsigned short);
		m_aucPacketOutputBuffer[0x02] = (unsigned char)( ulNetxAddress       & 0xffU);
		m_aucPacketOutputBuffer[0x03] = (unsigned char)((ulNetxAddress>> 8U) & 0xffU);
		m_aucPacketOutputBuffer[0x04] = (unsigned char)((ulNetxAddress>>16U) & 0xffU);
		m_aucPacketOutputBuffer[0x05] = (unsigned char)((ulNetxAddress>>24U) & 0xffU);
		m_aucPacketOutputBuffer[0x06] = (unsigned char)( usData      & 0xffU);
		m_aucPacketOutputBuffer[0x07] = (unsigned char)((usData>>8U) & 0xffU);

		tResult = execute_command(m_aucPacketOutputBuffer, 8, &sizInBuf);
		printf("write_data16: execute_command returned %d\n", tResult);
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

	printf("-write_data16(): fOk=%d\n", fOk);

	if( fOk!=true )
	{
		printf("Exit Error\n");
		MUHKUH_PLUGIN_EXIT_ERROR(ptClientData);
	}
}


/* write a long (32bit) from the pc to the netx */
void romloader_usb::write_data32(lua_State *ptClientData, unsigned long ulNetxAddress, unsigned long ulData)
{
	bool fOk;
	int iResult;
	size_t sizInBuf;
	unsigned char ucCommand;
	unsigned char ucStatus;
	USBSTATUS_T tResult;


	printf("+write_data32(): ptClientData=0x%p, ulNetxAddress=0x08lx, ulData=0x%08lx\n", ptClientData, ulNetxAddress, ulData);

	if( m_fIsConnected==false )
	{
		MUHKUH_PLUGIN_PUSH_ERROR(ptClientData, "%s(%p): write_data32: not connected!", m_pcName, this);
		fOk = false;
	}
	else
	{
		/* Get the next sequence number. */
		m_uiMonitorSequence = (m_uiMonitorSequence + 1) & (MONITOR_SEQUENCE_MSK>>MONITOR_SEQUENCE_SRT);

		/* Construct the command packet. */
		m_aucPacketOutputBuffer[0x00] = MONITOR_COMMAND_Write |
		                                (MONITOR_ACCESSSIZE_Long<<MONITOR_ACCESSSIZE_SRT) |
		                                (unsigned char)(m_uiMonitorSequence << MONITOR_SEQUENCE_SRT);
		m_aucPacketOutputBuffer[0x01] = sizeof(unsigned long);
		m_aucPacketOutputBuffer[0x02] = (unsigned char)( ulNetxAddress       & 0xffU);
		m_aucPacketOutputBuffer[0x03] = (unsigned char)((ulNetxAddress>> 8U) & 0xffU);
		m_aucPacketOutputBuffer[0x04] = (unsigned char)((ulNetxAddress>>16U) & 0xffU);
		m_aucPacketOutputBuffer[0x05] = (unsigned char)((ulNetxAddress>>24u) & 0xffU);
		m_aucPacketOutputBuffer[0x06] = (unsigned char)( ulData       & 0xffU);
		m_aucPacketOutputBuffer[0x07] = (unsigned char)((ulData>> 8U) & 0xffU);
		m_aucPacketOutputBuffer[0x08] = (unsigned char)((ulData>>16U) & 0xffU);
		m_aucPacketOutputBuffer[0x09] = (unsigned char)((ulData>>24U) & 0xffU);

		tResult = execute_command(m_aucPacketOutputBuffer, 10, &sizInBuf);
		printf("write_data32: execute_command returned %d\n", tResult);
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

	printf("-write_data32(): fOk=%d\n", fOk);

	if( fOk!=true )
	{
		printf("Exit Error\n");
		MUHKUH_PLUGIN_EXIT_ERROR(ptClientData);
	}
}


/* write a byte array from the pc to the netx */
void romloader_usb::write_image(unsigned long ulNetxAddress, const char *pcBUFFER_IN, size_t sizBUFFER_IN, SWIGLUA_REF tLuaFn, long lCallbackUserData)
{
	bool fOk;
	size_t sizChunk;
	int iResult;
	size_t sizInBuf;
	unsigned char ucCommand;
	unsigned char ucStatus;
	bool fIsRunning;
	long lBytesProcessed;
	USBSTATUS_T tResult;


	printf("+write_image(): ulNetxAddress=0x%08lx, pcBUFFER_IN=0x%p, sizBUFFER_IN=0x%08x, tLuaFn.L=0x%p, lCallbackUserData=0x%08lx\n", ulNetxAddress, pcBUFFER_IN, sizBUFFER_IN, tLuaFn.L, lCallbackUserData);

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
			if( sizChunk>58 )
			{
				sizChunk = 58;
			}

			/* Get the next sequence number. */
			m_uiMonitorSequence = (m_uiMonitorSequence + 1) & (MONITOR_SEQUENCE_MSK>>MONITOR_SEQUENCE_SRT);

			/* Construct the command packet. */
			m_aucPacketOutputBuffer[0x00] = MONITOR_COMMAND_Write |
			                                (MONITOR_ACCESSSIZE_Byte<<MONITOR_ACCESSSIZE_SRT) |
			                                (unsigned char)(m_uiMonitorSequence << MONITOR_SEQUENCE_SRT);
			m_aucPacketOutputBuffer[0x01] = (unsigned char)sizChunk;
			m_aucPacketOutputBuffer[0x02] = (unsigned char)( ulNetxAddress       & 0xffU);
			m_aucPacketOutputBuffer[0x03] = (unsigned char)((ulNetxAddress>> 8U) & 0xffU);
			m_aucPacketOutputBuffer[0x04] = (unsigned char)((ulNetxAddress>>16U) & 0xffU);
			m_aucPacketOutputBuffer[0x05] = (unsigned char)((ulNetxAddress>>24U) & 0xffU);
			memcpy(m_aucPacketOutputBuffer+6, pcBUFFER_IN, sizChunk);

			tResult = execute_command(m_aucPacketOutputBuffer, sizChunk+6, &sizInBuf);
			printf("write_image: execute_command returned %d\n", tResult);
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

	printf("-write_image(): fOk=%d\n", fOk);

	if( fOk!=true )
	{
		printf("Exit Error\n");
		MUHKUH_PLUGIN_EXIT_ERROR(tLuaFn.L);
	}
}


/* call routine */
void romloader_usb::call(unsigned long ulNetxAddress, unsigned long ulParameterR0, SWIGLUA_REF tLuaFn, long lCallbackUserData)
{
	bool fOk;
	int iResult;
	size_t sizInBuf;
	unsigned char ucCommand;
	unsigned char ucStatus;
	bool fIsRunning;
	char *pcProgressData;
	size_t sizProgressData;
	USBSTATUS_T tResult;


	printf("+call(): ulNetxAddress=0x%08lx, ulParameterR0=0x%08lx, tLuaFn.L=0x%p, lCallbackUserData=0x%08lx\n", ulNetxAddress, ulParameterR0, tLuaFn.L, lCallbackUserData);

	if( m_fIsConnected==false )
	{
		MUHKUH_PLUGIN_PUSH_ERROR(tLuaFn.L, "%s(%p): call: not connected!", m_pcName, this);
		fOk = false;
	}
	else
	{
		/* Construct the command packet. */
		m_aucPacketOutputBuffer[0x00] = MONITOR_COMMAND_Execute;
		m_aucPacketOutputBuffer[0x01] = (unsigned char)( ulNetxAddress      & 0xff);
		m_aucPacketOutputBuffer[0x02] = (unsigned char)((ulNetxAddress>>8 ) & 0xff);
		m_aucPacketOutputBuffer[0x03] = (unsigned char)((ulNetxAddress>>16) & 0xff);
		m_aucPacketOutputBuffer[0x04] = (unsigned char)((ulNetxAddress>>24) & 0xff);
		m_aucPacketOutputBuffer[0x05] = (unsigned char)( ulParameterR0      & 0xff);
		m_aucPacketOutputBuffer[0x06] = (unsigned char)((ulParameterR0>>8 ) & 0xff);
		m_aucPacketOutputBuffer[0x07] = (unsigned char)((ulParameterR0>>16) & 0xff);
		m_aucPacketOutputBuffer[0x08] = (unsigned char)((ulParameterR0>>24) & 0xff);

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

				iResult = m_ptUsbDevice->receive_packet(m_aucPacketInputBuffer, &sizInBuf, 500);
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
					if( sizInBuf==1 && m_aucPacketInputBuffer[0]==MONITOR_STATUS_CallFinished )
					{
						fOk = true;
						break;
					}
					else if( sizInBuf>=1 && m_aucPacketInputBuffer[0]==MONITOR_STATUS_CallMessage )
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

	printf("-call(): fOk=%d\n", fOk);

	if( fOk!=true )
	{
		MUHKUH_PLUGIN_EXIT_ERROR(tLuaFn.L);
	}
}


void romloader_usb::hexdump(const unsigned char *pucData, unsigned long ulSize, unsigned long ulNetxAddress)
{
	const unsigned char *pucDumpCnt, *pucDumpEnd;
	unsigned long ulAddressCnt;
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


/*-------------------------------------*/


romloader_usb_reference::romloader_usb_reference(void)
 : muhkuh_plugin_reference()
{
}


romloader_usb_reference::romloader_usb_reference(const char *pcName, const char *pcTyp, bool fIsUsed, romloader_usb_provider *ptProvider)
 : muhkuh_plugin_reference(pcName, pcTyp, fIsUsed, ptProvider)
{
}


romloader_usb_reference::romloader_usb_reference(const romloader_usb_reference *ptCloneMe)
 : muhkuh_plugin_reference(ptCloneMe)
{
}


/*-------------------------------------*/


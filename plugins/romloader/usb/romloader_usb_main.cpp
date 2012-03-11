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
#include "netx/src/usbmonitor_commands.h"

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


bool romloader_usb::chip_init(lua_State *ptClientData)
{
	bool fResult;


	switch( m_tChiptyp )
	{
	case ROMLOADER_CHIPTYP_NETX500:
	case ROMLOADER_CHIPTYP_NETX100:
		switch( m_tRomcode )
		{
		case ROMLOADER_ROMCODE_ABOOT:
			// aboot does not set the serial vectors
			write_data32(ptClientData, 0x10001ff0, 0x00200582|1);		// get: usb_receiveChar
			write_data32(ptClientData, 0x10001ff4, 0x0020054e|1);		// put: usb_sendChar
			write_data32(ptClientData, 0x10001ff8, 0);			// peek: none
			write_data32(ptClientData, 0x10001ffc, 0x00200566|1);		// flush: usb_sendFinish
			fResult = true;
			break;
		case ROMLOADER_ROMCODE_HBOOT:
			// hboot needs no special init
			fResult = true;
			break;
		case ROMLOADER_ROMCODE_HBOOT2_SOFT:
			/* hboot2 software emu needs no special init. */
			fResult = true;
			break;
		case ROMLOADER_ROMCODE_HBOOT2:
			// this is an unknown combination
			fResult = false;
			break;
		case ROMLOADER_ROMCODE_UNKNOWN:
			fResult = false;
			break;
		}
		break;

	case ROMLOADER_CHIPTYP_NETX50:
		switch( m_tRomcode )
		{
		case ROMLOADER_ROMCODE_ABOOT:
			// this is an unknown combination
			fResult = false;
			break;
		case ROMLOADER_ROMCODE_HBOOT:
			// hboot needs no special init
			fResult = true;
			break;
		case ROMLOADER_ROMCODE_HBOOT2_SOFT:
			/* hboot2 software emu needs no special init. */
			fResult = true;
			break;
		case ROMLOADER_ROMCODE_HBOOT2:
			// this is an unknown combination
			fResult = false;
			break;
		case ROMLOADER_ROMCODE_UNKNOWN:
			fResult = false;
			break;
		}
		break;

	case ROMLOADER_CHIPTYP_NETX10:
		switch( m_tRomcode )
		{
		case ROMLOADER_ROMCODE_ABOOT:
			// this is an unknown combination
			fResult = false;
			break;
		case ROMLOADER_ROMCODE_HBOOT:
			// hboot needs no special init
			fResult = true;
			break;
		case ROMLOADER_ROMCODE_HBOOT2_SOFT:
			/* hboot2 software emu needs no special init. */
			fResult = true;
			break;
		case ROMLOADER_ROMCODE_HBOOT2:
			// this is an unknown combination
			fResult = false;
			break;
		case ROMLOADER_ROMCODE_UNKNOWN:
			fResult = false;
			break;
		}
		break;

	case ROMLOADER_CHIPTYP_NETX56:
		switch( m_tRomcode )
		{
		case ROMLOADER_ROMCODE_ABOOT:
			// this is an unknown combination
			fResult = false;
			break;
		case ROMLOADER_ROMCODE_HBOOT:
			// this is an unknown combination
			fResult = false;
			break;
		case ROMLOADER_ROMCODE_HBOOT2_SOFT:
			// this is an unknown combination
			fResult = false;
			break;
		case ROMLOADER_ROMCODE_HBOOT2:
			/* HBoot2 needs no special init. */
			fResult = true;
			break;
		case ROMLOADER_ROMCODE_UNKNOWN:
			fResult = false;
			break;
		}
		break;

	case ROMLOADER_CHIPTYP_NETX5:
	case ROMLOADER_CHIPTYP_UNKNOWN:
		fResult = false;
		break;
	}

	return fResult;
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

			if( detect_chiptyp(&tFn)!=true )
			{
				MUHKUH_PLUGIN_PUSH_ERROR(ptClientData, "%s(%p): failed to detect chiptyp!", m_pcName, this);
				m_fIsConnected = false;
				m_ptUsbDevice->Disconnect();
				iResult = -1;
			}
			else if( chip_init(ptClientData)!=true )
			{
				MUHKUH_PLUGIN_PUSH_ERROR(ptClientData, "%s(%p): failed to init chip!", m_pcName, this);
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


/* read a byte (8bit) from the netx to the pc */
unsigned char romloader_usb::read_data08(lua_State *ptClientData, unsigned long ulNetxAddress)
{
	unsigned char ucData;
	bool fOk;
	int iResult;
	unsigned char aucOutBuf[64];
	unsigned char aucInBuf[64];
	size_t sizOutBuf;
	size_t sizInBuf;
	unsigned char ucCommand;
	unsigned char ucStatus;


	if( m_fIsConnected==false )
	{
		MUHKUH_PLUGIN_PUSH_ERROR(ptClientData, "%s(%p): read_data08: not connected!", m_pcName, this);
		fOk = false;
	}
	else
	{
		/* Construct the command packet. */
		ucCommand  = USBMON_COMMAND_Read;
		ucCommand |= USBMON_ACCESSSIZE_Byte << 6U;
		aucOutBuf[0x00] = ucCommand;
		aucOutBuf[0x01] = sizeof(unsigned char);
		aucOutBuf[0x02] = (unsigned char)( ulNetxAddress      & 0xff);
		aucOutBuf[0x03] = (unsigned char)((ulNetxAddress>>8 ) & 0xff);
		aucOutBuf[0x04] = (unsigned char)((ulNetxAddress>>16) & 0xff);
		aucOutBuf[0x05] = (unsigned char)((ulNetxAddress>>24) & 0xff);
		sizOutBuf = 6;

		iResult = m_ptUsbDevice->execute_command(aucOutBuf, sizOutBuf, aucInBuf, &sizInBuf);
		if( iResult!=0 )
		{
			MUHKUH_PLUGIN_PUSH_ERROR(ptClientData, "%s(%p): read_data08: failed to transfer command!", m_pcName, this);
			fOk = false;
		}
		else if( sizInBuf==0 )
		{
			MUHKUH_PLUGIN_PUSH_ERROR(ptClientData, "%s(%p): read_data08: received empty answer!", m_pcName, this);
			fOk = false;
		}
		else
		{
			ucStatus = aucInBuf[0];
			if( ucStatus!=USBMON_STATUS_Ok )
			{
				MUHKUH_PLUGIN_PUSH_ERROR(ptClientData, "%s(%p): read_data08: failed to execute command! Status: %d", m_pcName, this, ucStatus);
				fOk = false;
			}
			else if( sizInBuf!=2 )
			{
				MUHKUH_PLUGIN_PUSH_ERROR(ptClientData, "%s(%p): read_data08: answer has invalid size: %d", m_pcName, this, sizInBuf);
				hexdump(aucInBuf, sizInBuf, 0);
				fOk = false;
			}
			else
			{
				ucData  = aucInBuf[1];
				fOk = true;
			}
		}
	}

	if( fOk!=true )
	{
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
	unsigned char aucOutBuf[64];
	unsigned char aucInBuf[64];
	size_t sizOutBuf;
	size_t sizInBuf;
	unsigned char ucCommand;
	unsigned char ucStatus;


	if( m_fIsConnected==false )
	{
		MUHKUH_PLUGIN_PUSH_ERROR(ptClientData, "%s(%p): read_data16: not connected!", m_pcName, this);
		fOk = false;
	}
	else
	{
		/* Construct the command packet. */
		ucCommand  = USBMON_COMMAND_Read;
		ucCommand |= USBMON_ACCESSSIZE_Word << 6U;
		aucOutBuf[0x00] = ucCommand;
		aucOutBuf[0x01] = sizeof(unsigned short);
		aucOutBuf[0x02] = (unsigned char)( ulNetxAddress      & 0xff);
		aucOutBuf[0x03] = (unsigned char)((ulNetxAddress>>8 ) & 0xff);
		aucOutBuf[0x04] = (unsigned char)((ulNetxAddress>>16) & 0xff);
		aucOutBuf[0x05] = (unsigned char)((ulNetxAddress>>24) & 0xff);
		sizOutBuf = 6;

		iResult = m_ptUsbDevice->execute_command(aucOutBuf, sizOutBuf, aucInBuf, &sizInBuf);
		if( iResult!=0 )
		{
			MUHKUH_PLUGIN_PUSH_ERROR(ptClientData, "%s(%p): read_data16: failed to transfer command!", m_pcName, this);
			fOk = false;
		}
		else if( sizInBuf==0 )
		{
			MUHKUH_PLUGIN_PUSH_ERROR(ptClientData, "%s(%p): read_data16: received empty answer!", m_pcName, this);
			fOk = false;
		}
		else
		{
			ucStatus = aucInBuf[0];
			if( ucStatus!=USBMON_STATUS_Ok )
			{
				MUHKUH_PLUGIN_PUSH_ERROR(ptClientData, "%s(%p): read_data16: failed to execute command! Status: %d", m_pcName, this, ucStatus);
				fOk = false;
			}
			else if( sizInBuf!=3 )
			{
				MUHKUH_PLUGIN_PUSH_ERROR(ptClientData, "%s(%p): read_data16: answer has invalid size!", m_pcName, this);
				hexdump(aucInBuf, sizInBuf, 0);
				fOk = false;
			}
			else
			{
				usData = (unsigned short)(aucInBuf[1] | (aucInBuf[2]<<8U));
				fOk = true;
			}
		}
	}

	if( fOk!=true )
	{
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
	unsigned char aucOutBuf[64];
	unsigned char aucInBuf[64];
	size_t sizOutBuf;
	size_t sizInBuf;
	unsigned char ucCommand;
	unsigned char ucStatus;


	if( m_fIsConnected==false )
	{
		MUHKUH_PLUGIN_PUSH_ERROR(ptClientData, "%s(%p): read_data32: not connected!", m_pcName, this);
		fOk = false;
	}
	else
	{
		/* Construct the command packet. */
		ucCommand  = USBMON_COMMAND_Read;
		ucCommand |= USBMON_ACCESSSIZE_Long << 6U;
		aucOutBuf[0x00] = ucCommand;
		aucOutBuf[0x01] = sizeof(unsigned long);
		aucOutBuf[0x02] = (unsigned char)( ulNetxAddress      & 0xff);
		aucOutBuf[0x03] = (unsigned char)((ulNetxAddress>>8 ) & 0xff);
		aucOutBuf[0x04] = (unsigned char)((ulNetxAddress>>16) & 0xff);
		aucOutBuf[0x05] = (unsigned char)((ulNetxAddress>>24) & 0xff);
		sizOutBuf = 6;

		iResult = m_ptUsbDevice->execute_command(aucOutBuf, sizOutBuf, aucInBuf, &sizInBuf);
		if( iResult!=0 )
		{
			MUHKUH_PLUGIN_PUSH_ERROR(ptClientData, "%s(%p): read_data32: failed to transfer command!", m_pcName, this);
			fOk = false;
		}
		else if( sizInBuf==0 )
		{
			MUHKUH_PLUGIN_PUSH_ERROR(ptClientData, "%s(%p): read_data32: received empty answer!", m_pcName, this);
			fOk = false;
		}
		else
		{
			ucStatus = aucInBuf[0];
			if( ucStatus!=USBMON_STATUS_Ok )
			{
				MUHKUH_PLUGIN_PUSH_ERROR(ptClientData, "%s(%p): read_data32: failed to execute command! Status: %d", m_pcName, this, ucStatus);
				fOk = false;
			}
			else if( sizInBuf!=5 )
			{
				MUHKUH_PLUGIN_PUSH_ERROR(ptClientData, "%s(%p): read_data32: answer has invalid size!", m_pcName, this);
				hexdump(aucInBuf, sizInBuf, 0);
				fOk = false;
			}
			else
			{
				ulData  = aucInBuf[1];
				ulData |= aucInBuf[2] << 8;
				ulData |= aucInBuf[3] << 16;
				ulData |= aucInBuf[4] << 24;
				fOk = true;
			}
		}
	}

	if( fOk!=true )
	{
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
	unsigned char aucOutBuf[64];
	unsigned char aucInBuf[64];
	size_t sizOutBuf;
	size_t sizInBuf;
	unsigned char ucCommand;
	unsigned char ucStatus;
	bool fIsRunning;
	long lBytesProcessed;


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
				if( sizChunk>63 )
				{
					sizChunk = 63;
				}

				/* Construct the command packet. */
				ucCommand  = USBMON_COMMAND_Read;
				ucCommand |= USBMON_ACCESSSIZE_Byte << 6U;
				aucOutBuf[0x00] = ucCommand;
				aucOutBuf[0x01] = (unsigned char)sizChunk;
				aucOutBuf[0x02] = (unsigned char)( ulNetxAddress      & 0xff);
				aucOutBuf[0x03] = (unsigned char)((ulNetxAddress>>8 ) & 0xff);
				aucOutBuf[0x04] = (unsigned char)((ulNetxAddress>>16) & 0xff);
				aucOutBuf[0x05] = (unsigned char)((ulNetxAddress>>24) & 0xff);
				sizOutBuf = 6;

				iResult = m_ptUsbDevice->execute_command(aucOutBuf, sizOutBuf, aucInBuf, &sizInBuf);
				if( iResult!=0 )
				{
					MUHKUH_PLUGIN_PUSH_ERROR(tLuaFn.L, "%s(%p): read_image: failed to transfer command!", m_pcName, this);
					fOk = false;
					break;
				}
				else if( sizInBuf==0 )
				{
					MUHKUH_PLUGIN_PUSH_ERROR(tLuaFn.L, "%s(%p): read_image: received empty answer!", m_pcName, this);
					fOk = false;
					break;
				}
				else
				{
					ucStatus = aucInBuf[0];
					if( ucStatus!=USBMON_STATUS_Ok )
					{
						MUHKUH_PLUGIN_PUSH_ERROR(tLuaFn.L, "%s(%p): read_image: failed to execute command! Status: %d", m_pcName, this, ucStatus);
						fOk = false;
						break;
					}
					else if( sizInBuf!=sizChunk+1 )
					{
						MUHKUH_PLUGIN_PUSH_ERROR(tLuaFn.L, "%s(%p): read_image: answer has invalid size!", m_pcName, this);
						hexdump(aucInBuf, sizInBuf, 0);
						fOk = false;
						break;
					}
					else
					{
						memcpy(pcBuffer, aucInBuf+1, sizChunk);
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


/* write a byte (8bit) from the pc to the netx */
void romloader_usb::write_data08(lua_State *ptClientData, unsigned long ulNetxAddress, unsigned char ucData)
{
	bool fOk;
	int iResult;
	unsigned char aucOutBuf[64];
	unsigned char aucInBuf[64];
	size_t sizOutBuf;
	size_t sizInBuf;
	unsigned char ucCommand;
	unsigned char ucStatus;


	if( m_fIsConnected==false )
	{
		MUHKUH_PLUGIN_PUSH_ERROR(ptClientData, "%s(%p): write_data08: not connected!", m_pcName, this);
		fOk = false;
	}
	else
	{
		/* Construct the command packet. */
		ucCommand  = USBMON_COMMAND_Write;
		ucCommand |= USBMON_ACCESSSIZE_Byte << 6U;
		aucOutBuf[0x00] = ucCommand;
		aucOutBuf[0x01] = sizeof(unsigned char);
		aucOutBuf[0x02] = (unsigned char)( ulNetxAddress      & 0xff);
		aucOutBuf[0x03] = (unsigned char)((ulNetxAddress>>8 ) & 0xff);
		aucOutBuf[0x04] = (unsigned char)((ulNetxAddress>>16) & 0xff);
		aucOutBuf[0x05] = (unsigned char)((ulNetxAddress>>24) & 0xff);
		aucOutBuf[0x06] = ucData;
		sizOutBuf = 7;

		iResult = m_ptUsbDevice->execute_command(aucOutBuf, sizOutBuf, aucInBuf, &sizInBuf);
		if( iResult!=0 )
		{
			MUHKUH_PLUGIN_PUSH_ERROR(ptClientData, "%s(%p): write_data08: failed to transfer command!", m_pcName, this);
			fOk = false;
		}
		else if( sizInBuf==0 )
		{
			MUHKUH_PLUGIN_PUSH_ERROR(ptClientData, "%s(%p): write_data08: received empty answer!", m_pcName, this);
			fOk = false;
		}
		else
		{
			ucStatus = aucInBuf[0];
			if( ucStatus!=USBMON_STATUS_Ok )
			{
				MUHKUH_PLUGIN_PUSH_ERROR(ptClientData, "%s(%p): write_data08: failed to execute command! Status: %d", m_pcName, this, ucStatus);
				fOk = false;
			}
			else if( sizInBuf!=1 )
			{
				MUHKUH_PLUGIN_PUSH_ERROR(ptClientData, "%s(%p): write_data08: answer has invalid size!", m_pcName, this);
				hexdump(aucInBuf, sizInBuf, 0);
				fOk = false;
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


/* write a word (16bit) from the pc to the netx */
void romloader_usb::write_data16(lua_State *ptClientData, unsigned long ulNetxAddress, unsigned short usData)
{
	bool fOk;
	int iResult;
	unsigned char aucOutBuf[64];
	unsigned char aucInBuf[64];
	size_t sizOutBuf;
	size_t sizInBuf;
	unsigned char ucCommand;
	unsigned char ucStatus;


	if( m_fIsConnected==false )
	{
		MUHKUH_PLUGIN_PUSH_ERROR(ptClientData, "%s(%p): write_data16: not connected!", m_pcName, this);
		fOk = false;
	}
	else
	{
		/* Construct the command packet. */
		ucCommand  = USBMON_COMMAND_Write;
		ucCommand |= USBMON_ACCESSSIZE_Word << 6U;
		aucOutBuf[0x00] = ucCommand;
		aucOutBuf[0x01] = sizeof(unsigned short);
		aucOutBuf[0x02] = (unsigned char)( ulNetxAddress      & 0xff);
		aucOutBuf[0x03] = (unsigned char)((ulNetxAddress>>8 ) & 0xff);
		aucOutBuf[0x04] = (unsigned char)((ulNetxAddress>>16) & 0xff);
		aucOutBuf[0x05] = (unsigned char)((ulNetxAddress>>24) & 0xff);
		aucOutBuf[0x06] = (unsigned char)( usData      & 0xff);
		aucOutBuf[0x07] = (unsigned char)((usData>>8 ) & 0xff);
		sizOutBuf = 8;

		iResult = m_ptUsbDevice->execute_command(aucOutBuf, sizOutBuf, aucInBuf, &sizInBuf);
		if( iResult!=0 )
		{
			MUHKUH_PLUGIN_PUSH_ERROR(ptClientData, "%s(%p): write_data16: failed to transfer command!", m_pcName, this);
			fOk = false;
		}
		else if( sizInBuf==0 )
		{
			MUHKUH_PLUGIN_PUSH_ERROR(ptClientData, "%s(%p): write_data16: received empty answer!", m_pcName, this);
			fOk = false;
		}
		else
		{
			ucStatus = aucInBuf[0];
			if( ucStatus!=USBMON_STATUS_Ok )
			{
				MUHKUH_PLUGIN_PUSH_ERROR(ptClientData, "%s(%p): write_data16: failed to execute command! Status: %d", m_pcName, this, ucStatus);
				fOk = false;
			}
			else if( sizInBuf!=1 )
			{
				MUHKUH_PLUGIN_PUSH_ERROR(ptClientData, "%s(%p): write_data16: answer has invalid size!", m_pcName, this);
				hexdump(aucInBuf, sizInBuf, 0);
				fOk = false;
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


/* write a long (32bit) from the pc to the netx */
void romloader_usb::write_data32(lua_State *ptClientData, unsigned long ulNetxAddress, unsigned long ulData)
{
	bool fOk;
	int iResult;
	unsigned char aucOutBuf[64];
	unsigned char aucInBuf[64];
	size_t sizOutBuf;
	size_t sizInBuf;
	unsigned char ucCommand;
	unsigned char ucStatus;


	if( m_fIsConnected==false )
	{
		MUHKUH_PLUGIN_PUSH_ERROR(ptClientData, "%s(%p): write_data32: not connected!", m_pcName, this);
		fOk = false;
	}
	else
	{
		/* Construct the command packet. */
		ucCommand  = USBMON_COMMAND_Write;
		ucCommand |= USBMON_ACCESSSIZE_Long << 6U;
		aucOutBuf[0x00] = ucCommand;
		aucOutBuf[0x01] = sizeof(unsigned long);
		aucOutBuf[0x02] = (unsigned char)( ulNetxAddress      & 0xff);
		aucOutBuf[0x03] = (unsigned char)((ulNetxAddress>>8 ) & 0xff);
		aucOutBuf[0x04] = (unsigned char)((ulNetxAddress>>16) & 0xff);
		aucOutBuf[0x05] = (unsigned char)((ulNetxAddress>>24) & 0xff);
		aucOutBuf[0x06] = (unsigned char)( ulData      & 0xff);
		aucOutBuf[0x07] = (unsigned char)((ulData>>8 ) & 0xff);
		aucOutBuf[0x08] = (unsigned char)((ulData>>16) & 0xff);
		aucOutBuf[0x09] = (unsigned char)((ulData>>24) & 0xff);
		sizOutBuf = 10;

		iResult = m_ptUsbDevice->execute_command(aucOutBuf, sizOutBuf, aucInBuf, &sizInBuf);
		if( iResult!=0 )
		{
			MUHKUH_PLUGIN_PUSH_ERROR(ptClientData, "%s(%p): write_data32: failed to transfer command!", m_pcName, this);
			fOk = false;
		}
		else if( sizInBuf==0 )
		{
			MUHKUH_PLUGIN_PUSH_ERROR(ptClientData, "%s(%p): write_data32: received empty answer!", m_pcName, this);
			fOk = false;
		}
		else
		{
			ucStatus = aucInBuf[0];
			if( ucStatus!=USBMON_STATUS_Ok )
			{
				MUHKUH_PLUGIN_PUSH_ERROR(ptClientData, "%s(%p): write_data32: failed to execute command! Status: %d", m_pcName, this, ucStatus);
				fOk = false;
			}
			else if( sizInBuf!=1 )
			{
				MUHKUH_PLUGIN_PUSH_ERROR(ptClientData, "%s(%p): write_data32: answer has invalid size!", m_pcName, this);
				hexdump(aucInBuf, sizInBuf, 0);
				fOk = false;
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


/* write a byte array from the pc to the netx */
void romloader_usb::write_image(unsigned long ulNetxAddress, const char *pcBUFFER_IN, size_t sizBUFFER_IN, SWIGLUA_REF tLuaFn, long lCallbackUserData)
{
	bool fOk;
	size_t sizChunk;
	int iResult;
	unsigned char aucOutBuf[64];
	unsigned char aucInBuf[64];
	size_t sizOutBuf;
	size_t sizInBuf;
	unsigned char ucCommand;
	unsigned char ucStatus;
	bool fIsRunning;
	long lBytesProcessed;


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

			/* Construct the command packet. */
			ucCommand  = USBMON_COMMAND_Write;
			ucCommand |= USBMON_ACCESSSIZE_Byte << 6U;
			aucOutBuf[0x00] = ucCommand;
			aucOutBuf[0x01] = (unsigned char)sizChunk;
			aucOutBuf[0x02] = (unsigned char)( ulNetxAddress      & 0xff);
			aucOutBuf[0x03] = (unsigned char)((ulNetxAddress>>8 ) & 0xff);
			aucOutBuf[0x04] = (unsigned char)((ulNetxAddress>>16) & 0xff);
			aucOutBuf[0x05] = (unsigned char)((ulNetxAddress>>24) & 0xff);
			memcpy(aucOutBuf+6, pcBUFFER_IN, sizChunk);
			sizOutBuf = sizChunk+6;

			iResult = m_ptUsbDevice->execute_command(aucOutBuf, sizOutBuf, aucInBuf, &sizInBuf);
			if( iResult!=0 )
			{
				MUHKUH_PLUGIN_PUSH_ERROR(tLuaFn.L, "%s(%p): write_image: failed to transfer command!", m_pcName, this);
				fOk = false;
				break;
			}
			else if( sizInBuf==0 )
			{
				MUHKUH_PLUGIN_PUSH_ERROR(tLuaFn.L, "%s(%p): write_image: received empty answer!", m_pcName, this);
				fOk = false;
				break;
			}
			else
			{
				ucStatus = aucInBuf[0];
				if( ucStatus!=USBMON_STATUS_Ok )
				{
					MUHKUH_PLUGIN_PUSH_ERROR(tLuaFn.L, "%s(%p): write_image: failed to execute command! Status: %d", m_pcName, this, ucStatus);
					fOk = false;
					break;
				}
				else if( sizInBuf!=1 )
				{
					MUHKUH_PLUGIN_PUSH_ERROR(tLuaFn.L, "%s(%p): write_image: answer has invalid size!", m_pcName, this);
					hexdump(aucInBuf, sizInBuf, 0);
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
			}
		} while( sizBUFFER_IN!=0 );
	}

	if( fOk!=true )
	{
		MUHKUH_PLUGIN_EXIT_ERROR(tLuaFn.L);
	}
}


/* call routine */
void romloader_usb::call(unsigned long ulNetxAddress, unsigned long ulParameterR0, SWIGLUA_REF tLuaFn, long lCallbackUserData)
{
	bool fOk;
	int iResult;
	unsigned char aucOutBuf[64];
	unsigned char aucInBuf[64];
	size_t sizOutBuf;
	size_t sizInBuf;
	unsigned char ucCommand;
	unsigned char ucStatus;
	bool fIsRunning;
	char *pcProgressData;
	size_t sizProgressData;


	if( m_fIsConnected==false )
	{
		MUHKUH_PLUGIN_PUSH_ERROR(tLuaFn.L, "%s(%p): call: not connected!", m_pcName, this);
		fOk = false;
	}
	else
	{
		/* Construct the command packet. */
		ucCommand = USBMON_COMMAND_Execute;
		aucOutBuf[0x00] = ucCommand;
		aucOutBuf[0x01] = (unsigned char)( ulNetxAddress      & 0xff);
		aucOutBuf[0x02] = (unsigned char)((ulNetxAddress>>8 ) & 0xff);
		aucOutBuf[0x03] = (unsigned char)((ulNetxAddress>>16) & 0xff);
		aucOutBuf[0x04] = (unsigned char)((ulNetxAddress>>24) & 0xff);
		aucOutBuf[0x05] = (unsigned char)( ulParameterR0      & 0xff);
		aucOutBuf[0x06] = (unsigned char)((ulParameterR0>>8 ) & 0xff);
		aucOutBuf[0x07] = (unsigned char)((ulParameterR0>>16) & 0xff);
		aucOutBuf[0x08] = (unsigned char)((ulParameterR0>>24) & 0xff);
		sizOutBuf = 9;

		printf("Executing call command:\n");
		hexdump(aucOutBuf, sizOutBuf, 0);
		iResult = m_ptUsbDevice->execute_command(aucOutBuf, sizOutBuf, aucInBuf, &sizInBuf);
		if( iResult!=0 )
		{
			MUHKUH_PLUGIN_PUSH_ERROR(tLuaFn.L, "%s(%p): call: failed to transfer command!", m_pcName, this);
			fOk = false;
		}
		else if( sizInBuf==0 )
		{
			MUHKUH_PLUGIN_PUSH_ERROR(tLuaFn.L, "%s(%p): call: received empty answer!", m_pcName, this);
			fOk = false;
		}
		else
		{
			printf("Received call status:\n");
			hexdump(aucInBuf, sizInBuf, 0);

			ucStatus = aucInBuf[0];
			if( ucStatus!=USBMON_STATUS_Ok )
			{
				MUHKUH_PLUGIN_PUSH_ERROR(tLuaFn.L, "%s(%p): call: failed to execute command! Status: %d", m_pcName, this, ucStatus);
				fOk = false;
			}
			else if( sizInBuf!=1 )
			{
				MUHKUH_PLUGIN_PUSH_ERROR(tLuaFn.L, "%s(%p): call: answer has invalid size!", m_pcName, this);
				hexdump(aucInBuf, sizInBuf, 0);
				fOk = false;
			}
			else
			{
				/* Receive message packets. */
				while(1)
				{
					pcProgressData = NULL;
					sizProgressData = 0;

					iResult = m_ptUsbDevice->receive_packet(aucInBuf, &sizInBuf, 500);
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
						if( sizInBuf==1 && aucInBuf[0]==USBMON_STATUS_CallFinished )
						{
							fOk = true;
							break;
						}
						else if( sizInBuf>=1 && aucInBuf[0]==USBMON_STATUS_CallMessage )
						{
//							printf("Received message:\n");
//							hexdump(aucInBuf+1, sizInBuf-1, 0);
							pcProgressData = (char*)aucInBuf+1;
							sizProgressData = sizInBuf-1;
						}
						else if( sizInBuf!=0 )
						{
							printf("Received invalid packet:\n");
							hexdump(aucInBuf, sizInBuf, 0);

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
							aucOutBuf[0] = 0x2b;
							iResult = m_ptUsbDevice->send_packet(aucOutBuf, 1, 100);

							MUHKUH_PLUGIN_PUSH_ERROR(tLuaFn.L, "%s(%p): the call was cancelled!", m_pcName, this);
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


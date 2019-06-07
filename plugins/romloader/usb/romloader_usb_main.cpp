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

#define ROMLOADER_USB_TIMEOUT_PACKET_SEND_MS 1000
#define ROMLOADER_USB_TIMEOUT_PACKET_RECEIVE_MS 1000


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
			if( synchronize(&m_tChiptyp)!=true )
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



romloader::TRANSPORTSTATUS_T romloader_usb::send_raw_packet(const void *pvPacket, size_t sizPacket)
{
	romloader::TRANSPORTSTATUS_T tResult;
	const uint8_t *pucPacket;


	tResult = TRANSPORTSTATUS_NOT_CONNECTED;

	pucPacket = (const uint8_t *)pvPacket;

	if( m_ptUsbDevice!=NULL )
	{
		tResult = m_ptUsbDevice->send_packet(pucPacket, sizPacket, ROMLOADER_USB_TIMEOUT_PACKET_SEND_MS);
	}

	return tResult;
}



romloader::TRANSPORTSTATUS_T romloader_usb::receive_packet(void)
{
	romloader::TRANSPORTSTATUS_T tResult;


	tResult = TRANSPORTSTATUS_NOT_CONNECTED;
	if( m_ptUsbDevice!=NULL )
	{
		tResult = m_ptUsbDevice->receive_packet(m_aucPacketInputBuffer, m_sizMaxPacketSizeHost, &m_sizPacketInputBuffer, ROMLOADER_USB_TIMEOUT_PACKET_RECEIVE_MS);
	}

	return tResult;
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


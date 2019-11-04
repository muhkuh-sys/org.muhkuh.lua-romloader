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


#include "../romloader.h"

#ifndef __ROMLOADER_USB_MAIN_H__
#define __ROMLOADER_USB_MAIN_H__

/*-----------------------------------*/

/* The LIBUSB header is only used in the private section of the class.
 * It should not be exported.
 */
#ifndef SWIG
#       include "romloader_usb_device_libusb.h"
#endif

/*-----------------------------------*/

class romloader_usb_provider;

/*-----------------------------------*/

class romloader_usb : public romloader
{
public:
	romloader_usb(const char *pcName, const char *pcTyp, romloader_usb_provider *ptProvider, unsigned int uiBusNr, unsigned int uiDeviceAdr);
	romloader_usb(const char *pcName, const char *pcTyp, const char *pcLocation, romloader_usb_provider *ptProvider, unsigned int uiBusNr, unsigned int uiDeviceAdr);
	~romloader_usb(void);

// *** lua interface start ***
	// open the connection to the device
	virtual void Connect(lua_State *ptClientData);
	// close the connection to the device
	virtual void Disconnect(lua_State *ptClientData);
// *** lua interface end ***

protected:
	virtual TRANSPORTSTATUS_T send_raw_packet(const void *pvPacket, size_t sizPacket);
	virtual TRANSPORTSTATUS_T receive_packet(void);

private:
#ifndef SWIG
//	static const size_t m_sizMaxPacketSizeHost = 8192+64;
//	size_t m_sizMaxPacketSizeClient;
//	uint8_t m_aucPacketOutputBuffer[m_sizMaxPacketSizeHost];
//	uint8_t m_aucPacketInputBuffer[m_sizMaxPacketSizeHost];

//	unsigned int m_uiMonitorSequence;

	romloader_usb_provider *m_ptUsbProvider;

	unsigned int m_uiBusNr;
	unsigned int m_uiDeviceAdr;

	/* Pointer to the USB device and the USB device handle. */
	romloader_usb_device_libusb *m_ptUsbDevice;
#endif
};

/*-----------------------------------*/

class romloader_usb_provider : public muhkuh_plugin_provider
{
public:
	romloader_usb_provider(swig_type_info *p_romloader_usb, swig_type_info *p_romloader_usb_reference);
	~romloader_usb_provider(void);

	int DetectInterfaces(lua_State *ptLuaStateForTableAccess, lua_State *ptLuaStateForTableAccessOptional);

	virtual romloader_usb *ClaimInterface(const muhkuh_plugin_reference *ptReference);
	virtual bool ReleaseInterface(muhkuh_plugin *ptPlugin);

private:
	static const char *m_pcPluginNamePattern;

	romloader_usb_device_libusb *m_ptUsbDevice;
};

/*-----------------------------------*/

class romloader_usb_reference : public muhkuh_plugin_reference
{
public:
	romloader_usb_reference(void);
	romloader_usb_reference(const char *pcName, const char *pcTyp, bool fIsUsed, romloader_usb_provider *ptProvider);
	romloader_usb_reference(const char *pcName, const char *pcTyp, const char *pcLocation, bool fIsUsed, romloader_usb_provider *ptProvider);
	romloader_usb_reference(const romloader_usb_reference *ptCloneMe);
};

/*-----------------------------------*/

#endif	/* __ROMLOADER_USB_MAIN_H__ */


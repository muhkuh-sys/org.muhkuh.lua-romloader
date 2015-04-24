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

#include "romloader_usb_device_libusb.h"

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

	// read a byte (8bit) from the netx to the pc
	virtual uint8_t read_data08(lua_State *ptClientData, uint32_t ulNetxAddress);
	// read a word (16bit) from the netx to the pc
	virtual uint16_t read_data16(lua_State *ptClientData, uint32_t ulNetxAddress);
	// read a long (32bit) from the netx to the pc
	virtual uint32_t read_data32(lua_State *ptClientData, uint32_t ulNetxAddress);
	// read a byte array from the netx to the pc
	virtual void read_image(uint32_t ulNetxAddress, uint32_t ulSize, char **ppcBUFFER_OUT, size_t *psizBUFFER_OUT, SWIGLUA_REF tLuaFn, long lCallbackUserData);

	// write a byte (8bit) from the pc to the netx
	virtual void write_data08(lua_State *ptClientData, uint32_t ulNetxAddress, uint8_t ucData);
	// write a word (16bit) from the pc to the netx
	virtual void write_data16(lua_State *ptClientData, uint32_t ulNetxAddress, uint16_t usData);
	// write a long (32bit) from the pc to the netx
	virtual void write_data32(lua_State *ptClientData, uint32_t ulNetxAddress, uint32_t ulData);
	// write a byte array from the pc to the netx
	virtual void write_image(uint32_t ulNetxAddress, const char *pcBUFFER_IN, size_t sizBUFFER_IN, SWIGLUA_REF tLuaFn, long lCallbackUserData);

	// call routine
	virtual void call(uint32_t ulNetxAddress, uint32_t ulParameterR0, SWIGLUA_REF tLuaFn, long lCallbackUserData);
// *** lua interface end ***

	static void hexdump(const uint8_t *pucData, uint32_t ulSize, uint32_t ulNetxAddress);

private:
	typedef struct
	{
		uint8_t *pucData;
		size_t sizData;
		size_t sizPos;
	} DATA_BUFFER_T;

	typedef enum USBSTATUS_ENUM
	{
		USBSTATUS_OK                        = 0,
		USBSTATUS_TIMEOUT                   = 1,
		USBSTATUS_PACKET_TOO_LARGE          = 2,
		USBSTATUS_SEND_FAILED               = 3,
		USBSTATUS_RECEIVE_FAILED            = 4,
		USBSTATUS_FAILED_TO_SYNC            = 5,
		USBSTATUS_CRC_MISMATCH              = 6,
		USBSTATUS_MISSING_USERDATA          = 7,
		USBSTATUS_COMMAND_EXECUTION_FAILED  = 8,
		USBSTATUS_SEQUENCE_MISMATCH         = 9
	} USBSTATUS_T;

	static const size_t m_sizMaxPacketSizeHost = 8192+64;
	size_t m_sizMaxPacketSizeClient;
	uint8_t m_aucPacketOutputBuffer[m_sizMaxPacketSizeHost];
	uint8_t m_aucPacketInputBuffer[m_sizMaxPacketSizeHost];

	unsigned int m_uiMonitorSequence;

	bool synchronize(void);
	void next_sequence_number();
	void packet_update_sequence_number(uint8_t *aucCommand);

	USBSTATUS_T execute_command(uint8_t *aucCommand, size_t sizCommand, size_t *psizReceivePacket);

	romloader_usb_provider *m_ptUsbProvider;

	unsigned int m_uiBusNr;
	unsigned int m_uiDeviceAdr;

	/* Pointer to the USB device and the USB device handle. */
	romloader_usb_device_libusb *m_ptUsbDevice;
};

/*-----------------------------------*/

class romloader_usb_provider : public muhkuh_plugin_provider
{
public:
	romloader_usb_provider(swig_type_info *p_romloader_usb, swig_type_info *p_romloader_usb_reference);
	~romloader_usb_provider(void);

	int DetectInterfaces(lua_State *ptLuaStateForTableAccess);

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


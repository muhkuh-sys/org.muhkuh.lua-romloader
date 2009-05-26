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


#include <libusb-1.0/libusb.h>

#include "../romloader.h"

#ifndef __ROMLOADER_USB_MAIN_H__
#define __ROMLOADER_USB_MAIN_H__

/*-----------------------------------*/

class romloader_usb_provider;

/*-----------------------------------*/

class romloader_usb : public romloader
{
public:
	romloader_usb(const char *pcName, const char *pcTyp, romloader_usb_provider *ptProvider, libusb_device *ptUsbDevice);
	~romloader_usb(void);

// *** lua interface start ***
	// open the connection to the device
	virtual void Connect(lua_State *ptClientData);
	// close the connection to the device
	virtual void Disconnect(lua_State *ptClientData);

	// read a byte (8bit) from the netx to the pc
	virtual unsigned char read_data08(lua_State *ptClientData, unsigned long ulNetxAddress);
	// read a word (16bit) from the netx to the pc
	virtual unsigned short read_data16(lua_State *ptClientData, unsigned long ulNetxAddress);
	// read a long (32bit) from the netx to the pc
	virtual unsigned long read_data32(lua_State *ptClientData, unsigned long ulNetxAddress);
	// read a byte array from the netx to the pc
	virtual void read_image(unsigned long ulNetxAddress, unsigned long ulSize, char **ppcOutputData, unsigned long *pulOutputData, SWIGLUA_REF tLuaFn, long lCallbackUserData);

	// write a byte (8bit) from the pc to the netx
	virtual void write_data08(lua_State *ptClientData, unsigned long ulNetxAddress, unsigned char ucData);
	// write a word (16bit) from the pc to the netx
	virtual void write_data16(lua_State *ptClientData, unsigned long ulNetxAddress, unsigned short usData);
	// write a long (32bit) from the pc to the netx
	virtual void write_data32(lua_State *ptClientData, unsigned long ulNetxAddress, unsigned long ulData);
	// write a byte array from the pc to the netx
	virtual void write_image(unsigned long ulNetxAddress, const char *pcInputData, unsigned long ulInputData, SWIGLUA_REF tLuaFn, long lCallbackUserData);

	// call routine
	virtual void call(unsigned long ulNetxAddress, unsigned long ulParameterR0, SWIGLUA_REF tLuaFn, long lCallbackUserData);
// *** lua interface end ***

private:
	typedef struct
	{
		unsigned char *pucData;
		size_t sizData;
		size_t sizPos;
	} DATA_BUFFER_T;

	bool chip_init(lua_State *ptClientData);

	bool parse_hex_digit(DATA_BUFFER_T *ptBuffer, size_t sizDigits, unsigned long *pulResult);
	bool expect_string(DATA_BUFFER_T *ptBuffer, const char *pcMatch);

	bool parseDumpLine(DATA_BUFFER_T *ptBuffer, unsigned long ulAddress, unsigned long ulElements, unsigned char *pucBuffer);
/*
	int write_data(wxString &strData, unsigned long ulLoadAdr, lua_State *L, int iLuaCallbackTag, void *pvCallbackUserData);
	

	int getLine(wxString &strData);

	int usb_load(const char *pcData, size_t sizDataLen, unsigned long ulLoadAdr, lua_State *L, int iLuaCallbackTag, void *pvCallbackUserData);
	int usb_call(unsigned long ulNetxAddress, unsigned long ulParameterR0, lua_State *L, int iLuaCallbackTag, void *pvCallbackUserData);
*/
	int usb_executeCommand(const char *pcCommand, DATA_BUFFER_T *ptBuffer);
	int usb_sendCommand(const char *pcCommand);
	int usb_getNetxData(DATA_BUFFER_T *ptBuffer, SWIGLUA_REF *ptLuaFn, long lCallbackUserData);


	// lowlevel functions
	int libusb_closeDevice(void);
	int libusb_resetDevice(void);
	int libusb_readBlock(unsigned char *pucReceiveBuffer, unsigned int uiSize, int iTimeoutMs);
	int libusb_writeBlock(unsigned char *pucSendBuffer, unsigned int uiSize, int iTimeoutMs);
	int libusb_exchange(unsigned char *pucSendBuffer, unsigned char *pucReceiveBuffer);

	romloader_usb_provider *m_ptUsbProvider;

	/* pointer to the usb device and the usb device handle */
	libusb_device *m_ptUsbDev;
	libusb_device_handle *m_ptUsbDevHandle;

	/* bus number and device address for the connection */
	uint8_t m_uiNetxBusNr;
	uint8_t m_uiNetxDeviceAddress;

	// buffer for the read_image command
	size_t sizBufLen;
	size_t sizBufPos;
	bool fEof;
	char acBuf[64];
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

	const char *libusb_strerror(int iError);
private:
	bool isDeviceNetx(libusb_device *ptDev);

	typedef struct
	{
		libusb_error eErrNo;
		const char *pcErrMsg;
	} LIBUSB_STRERROR_T;
	static const LIBUSB_STRERROR_T atStrError[];

	static const char *m_pcPluginNamePattern;

	libusb_context *m_ptLibUsbContext;
};

/*-----------------------------------*/

class romloader_usb_reference : public muhkuh_plugin_reference
{
public:
	romloader_usb_reference(void);
	romloader_usb_reference(const char *pcName, const char *pcTyp, bool fIsUsed, romloader_usb_provider *ptProvider);
	romloader_usb_reference(const romloader_usb_reference *ptCloneMe);
};

/*-----------------------------------*/

#endif	/* __ROMLOADER_USB_MAIN_H__ */


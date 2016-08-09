/***************************************************************************
 *   Copyright (C) 2016 by Christoph Thelen                                *
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

/* The LIBUSB header is only used in the private section of the class.
 * It should not be exported.
 */
#ifndef SWIG
#       include "libusb.h"
#endif

#ifndef __ROMLOADER_PAPA_SCHLUMPF_H__
#define __ROMLOADER_PAPA_SCHLUMPF_H__

/*-----------------------------------*/

class romloader_papa_schlumpf_provider;

/*-----------------------------------*/

class romloader_papa_schlumpf : public romloader
{
public:
	romloader_papa_schlumpf(const char *pcName, const char *pcTyp, romloader_papa_schlumpf_provider *ptProvider, unsigned int uiBusNr, unsigned int uiDeviceAdr);
	romloader_papa_schlumpf(const char *pcName, const char *pcTyp, const char *pcLocation, romloader_papa_schlumpf_provider *ptProvider, unsigned int uiBusNr, unsigned int uiDeviceAdr);
	~romloader_papa_schlumpf(void);

/* *** LUA interface start *** */
	/* Open the connection to the device. */
	virtual void Connect(lua_State *ptClientData);
	/* Close the connection to the device. */
	virtual void Disconnect(lua_State *ptClientData);

	/* Read a byte (8bit) from the netX to the PC. */
	virtual uint8_t read_data08(lua_State *ptClientData, uint32_t ulDataSourceAddress);
	/* Read a word (16bit) from the netX to the PC. */
	virtual uint16_t read_data16(lua_State *ptClientData, uint32_t ulDataSourceAddress);
	/* Read a long (32bit) from the netX to the PC. */
	virtual uint32_t read_data32(lua_State *ptClientData, uint32_t ulDataSourceAddress);
	/* Read a byte array from the netX to the PC. */
	virtual void read_image(uint32_t ulNetxAddress, uint32_t ulSize, char **ppcBUFFER_OUT, size_t *psizBUFFER_OUT, SWIGLUA_REF tLuaFn, long lCallbackUserData);

	/* Write a byte (8bit) from the PC to the netX. */
	virtual void write_data08(lua_State *ptClientData, uint32_t ulNetxAddress, uint8_t ucData);
	/* Write a word (16bit) from the PC to the netX. */
	virtual void write_data16(lua_State *ptClientData, uint32_t ulNetxAddress, uint16_t usData);
	/* Write a long (32bit) from the PC to the netX. */
	virtual void write_data32(lua_State *ptClientData, uint32_t ulNetxAddress, uint32_t ulData);
	/* Write a byte array from the PC to the netX. */
	virtual void write_image(uint32_t ulNetxAddress, const char *pcBUFFER_IN, size_t sizBUFFER_IN, SWIGLUA_REF tLuaFn, long lCallbackUserData);

	/* Call routine. */
	virtual void call(uint32_t ulNetxAddress, uint32_t ulParameterR0, SWIGLUA_REF tLuaFn, long lCallbackUserData);
// *** LUA interface end ***

private:
#ifndef SWIG
	typedef enum PAPA_SCHLUMPF_ERROR_ENUM
	{
		PAPA_SCHLUMPF_ERROR_Ok = 0,
		PAPA_SCHLUMPF_ERROR_LibUsb_Open = 1,
		PAPA_SCHLUMPF_ERROR_NoHardwareFound = 2,
		PAPA_SCHLUMPF_ERROR_FailedToListUSBDevices = 3,
		PAPA_SCHLUMPF_ERROR_FoundMoreThanOneDevice = 4,
		PAPA_SCHLUMPF_ERROR_FailedToOpenDevice = 5,
		PAPA_SCHLUMPF_ERROR_FailedToClaimInterface = 6,
		PAPA_SCHLUMPF_ERROR_TransferError = 7,
		PAPA_SCHLUMPF_ERROR_MemoryError = 8,
		PAPA_SCHLUMPF_ERROR_netxError = 9
	} PAPA_SCHLUMPF_ERROR_T;

	/*Indicates status for the raw usb communication*/
	typedef enum USB_STATUS_ENUM
	{
		USB_STATUS_OK = 0,
		USB_STATUS_NOT_ALL_BYTES_TRANSFERED = 1,
		USB_STATUS_ERROR = 2,
		USB_STATUS_RECEIVED_EMPTY_PACKET = 3
	} USB_STATUS_T;

	libusb_device_handle *g_pLibusbDeviceHandle;
	libusb_context       *g_pLibusbContext;

	USB_STATUS_T sendAndReceivePackets(uint8_t* aucOutBuf, uint32_t sizOutBuf, uint8_t* aucInBuf, uint32_t* sizInBuf);
	USB_STATUS_T sendPacket(uint8_t* aucOutBuf, uint32_t sizOutBuf);
	USB_STATUS_T receivePacket(uint8_t* aucInBuf, uint32_t* pSizInBuf);
#endif
};


/*-----------------------------------*/

class romloader_papa_schlumpf_provider : public muhkuh_plugin_provider
{
public:
	romloader_papa_schlumpf_provider(swig_type_info *p_romloader_papa_schlumpf, swig_type_info *p_romloader_papa_schlumpf_reference);
	~romloader_papa_schlumpf_provider(void);

	int DetectInterfaces(lua_State *ptLuaStateForTableAccess);

	virtual romloader_papa_schlumpf *ClaimInterface(const muhkuh_plugin_reference *ptReference);
	virtual bool ReleaseInterface(muhkuh_plugin *ptPlugin);

private:
	static const char *m_pcPluginNamePattern;

//	romloader_papa_schlumpf_device *m_ptDevice;
};

/*-----------------------------------*/

class romloader_papa_schlumpf_reference : public muhkuh_plugin_reference
{
public:
	romloader_papa_schlumpf_reference(void);
	romloader_papa_schlumpf_reference(const char *pcName, const char *pcTyp, bool fIsUsed, romloader_papa_schlumpf_provider *ptProvider);
	romloader_papa_schlumpf_reference(const char *pcName, const char *pcTyp, const char *pcLocation, bool fIsUsed, romloader_papa_schlumpf_provider *ptProvider);
	romloader_papa_schlumpf_reference(const romloader_papa_schlumpf_reference *ptCloneMe);
};

/*-----------------------------------*/

#endif  /* __ROMLOADER_PAPA_SCHLUMPF_H__ */

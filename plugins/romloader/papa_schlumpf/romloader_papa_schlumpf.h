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
#       include "romloader_papa_schlumpf_device.h"
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
	virtual uint8_t read_data08(lua_State *ptClientData, uint32_t ulNetxAddress);
	/* Read a word (16bit) from the netX to the PC. */
	virtual uint16_t read_data16(lua_State *ptClientData, uint32_t ulNetxAddress);
	/* Read a long (32bit) from the netX to the PC. */
	virtual uint32_t read_data32(lua_State *ptClientData, uint32_t ulNetxAddress);
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

	static int call_progress_data_static(void *pvUser, const char *pcProgressData, size_t sizProgressData);
	int call_progress_data(const char *pcProgressData, size_t sizProgressData);

protected:
	virtual TRANSPORTSTATUS_T send_raw_packet(const void *pvPacket, size_t sizPacket);
	virtual TRANSPORTSTATUS_T receive_packet(void);

private:
#ifndef SWIG
	romloader_papa_schlumpf_device *m_ptDevice;

	romloader_papa_schlumpf_provider *m_ptProvider;
	unsigned int m_uiBusNr;
	unsigned int m_uiDeviceAdr;

	SWIGLUA_REF *m_ptCallBackLuaFn;
	long m_lCallBackUserData;
#endif
};


/*-----------------------------------*/

class romloader_papa_schlumpf_provider : public muhkuh_plugin_provider
{
public:
	romloader_papa_schlumpf_provider(swig_type_info *p_romloader_papa_schlumpf, swig_type_info *p_romloader_papa_schlumpf_reference);
	~romloader_papa_schlumpf_provider(void);

	int DetectInterfaces(lua_State *ptLuaStateForTableAccess, lua_State *ptLuaStateForTableAccessOptional);

	virtual romloader_papa_schlumpf *ClaimInterface(const muhkuh_plugin_reference *ptReference);
	virtual bool ReleaseInterface(muhkuh_plugin *ptPlugin);

private:
	static const char *m_pcPluginNamePattern;

	romloader_papa_schlumpf_device *m_ptDevice;
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

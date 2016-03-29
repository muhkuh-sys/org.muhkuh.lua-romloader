/***************************************************************************
 *   Copyright (C) 2013 by Christoph Thelen                                *
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

#ifndef __ROMLOADER_JTAG_MAIN_H__
#define __ROMLOADER_JTAG_MAIN_H__


extern "C" {
#       include "openocd/romloader_jtag_openocd.h"
}


/*-----------------------------------*/

class romloader_jtag_provider;

/*-----------------------------------*/


class romloader_jtag : public romloader
{
public:
	romloader_jtag(const char *pcName, const char *pcTyp, romloader_jtag_provider *ptProvider, unsigned int uiBusNr, unsigned int uiDeviceAdr);
	romloader_jtag(const char *pcName, const char *pcTyp, const char *pcLocation, romloader_jtag_provider *ptProvider, unsigned int uiBusNr, unsigned int uiDeviceAdr);
	~romloader_jtag(void);

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

	static void hexdump(const unsigned char *pucData, unsigned long ulSize, unsigned long ulNetxAddress);

private:
	/* TODO: remove this. */
	static const size_t m_sizMaxPacketSizeHost = 8192+64;
	size_t m_sizMaxPacketSizeClient;
	unsigned char m_aucPacketOutputBuffer[m_sizMaxPacketSizeHost];
	unsigned char m_aucPacketInputBuffer[m_sizMaxPacketSizeHost];

	unsigned int m_uiMonitorSequence;

	romloader_jtag_provider *m_ptJtagProvider;

	unsigned int m_uiBusNr;
	unsigned int m_uiDeviceAdr;
};

/*-----------------------------------*/

class romloader_jtag_provider : public muhkuh_plugin_provider
{
public:
	romloader_jtag_provider(swig_type_info *p_romloader_jtag, swig_type_info *p_romloader_jtag_reference);
	~romloader_jtag_provider(void);

	int DetectInterfaces(lua_State *ptLuaStateForTableAccess);

	virtual romloader_jtag *ClaimInterface(const muhkuh_plugin_reference *ptReference);
	virtual bool ReleaseInterface(muhkuh_plugin *ptPlugin);

private:
	ROMLOADER_JTAG_DEVICE_T m_tJtagDevice;
	static const char *m_pcPluginNamePattern;
};

/*-----------------------------------*/

class romloader_jtag_reference : public muhkuh_plugin_reference
{
public:
	romloader_jtag_reference(void);
	romloader_jtag_reference(const char *pcName, const char *pcTyp, bool fIsUsed, romloader_jtag_provider *ptProvider);
	romloader_jtag_reference(const char *pcName, const char *pcTyp, const char *pcLocation, bool fIsUsed, romloader_jtag_provider *ptProvider);
	romloader_jtag_reference(const romloader_jtag_reference *ptCloneMe);
};

/*-----------------------------------*/

#endif	/* __ROMLOADER_JTAG_MAIN_H__ */


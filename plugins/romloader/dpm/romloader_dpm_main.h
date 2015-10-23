/***************************************************************************
 *   Copyright (C) 2015 by Christoph Thelen                                *
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

#include "../machine_interface/netx/src/monitor_commands.h"


#ifndef __ROMLOADER_DPM_MAIN_H__
#define __ROMLOADER_DPM_MAIN_H__


/*-----------------------------------*/

class romloader_dpm_provider;

/*-----------------------------------*/

class romloader_dpm : public romloader
{
public:
	romloader_dpm(const char *pcName, const char *pcTyp, romloader_dpm_provider *ptProvider, const char *pcDeviceName);
	~romloader_dpm(void);

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

private:
	size_t m_sizMaxPacketSizeHost;
};

/*-----------------------------------*/

class romloader_dpm_provider : public muhkuh_plugin_provider
{
public:
	romloader_dpm_provider(swig_type_info *p_romloader_dpm, swig_type_info *p_romloader_dpm_reference);
	~romloader_dpm_provider(void);

	int DetectInterfaces(lua_State *ptLuaStateForTableAccess);

	virtual romloader_dpm *ClaimInterface(const muhkuh_plugin_reference *ptReference);
	virtual bool ReleaseInterface(muhkuh_plugin *ptPlugin);

private:
	static const char *m_pcPluginNamePattern;
};

/*-----------------------------------*/

class romloader_dpm_reference : public muhkuh_plugin_reference
{
public:
	romloader_dpm_reference(void);
	romloader_dpm_reference(const char *pcName, const char *pcTyp, bool fIsUsed, romloader_dpm_provider *ptProvider);
	romloader_dpm_reference(const romloader_dpm_reference *ptCloneMe);
};

/*-----------------------------------*/

#endif  /* __ROMLOADER_DPM_MAIN_H__ */


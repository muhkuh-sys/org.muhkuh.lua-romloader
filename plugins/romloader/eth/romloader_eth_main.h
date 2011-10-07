/***************************************************************************
 *   Copyright (C) 2011 by Christoph Thelen                                *
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

#include "netx/src/monitor_commands.h"


#ifndef __ROMLOADER_ETH_MAIN_H__
#define __ROMLOADER_ETH_MAIN_H__


#ifdef _WIN32
	#include "romloader_eth_device_win.h"
	#define romloader_eth_device_platform romloader_eth_device_win
#else
	#include "romloader_eth_device_linux.h"
	#define romloader_eth_device_platform romloader_eth_device_linux
#endif


/*-----------------------------------*/

class romloader_eth_provider;

/*-----------------------------------*/

class romloader_eth : public romloader
{
public:
	romloader_eth(const char *pcName, const char *pcTyp, romloader_eth_provider *ptProvider, char *pcDeviceName);
	~romloader_eth(void);

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
	virtual void read_image(unsigned long ulNetxAddress, unsigned long ulSize, char **ppcBUFFER_OUT, size_t *psizBUFFER_OUT, SWIGLUA_REF tLuaFn, long lCallbackUserData);

	// write a byte (8bit) from the pc to the netx
	virtual void write_data08(lua_State *ptClientData, unsigned long ulNetxAddress, unsigned char ucData);
	// write a word (16bit) from the pc to the netx
	virtual void write_data16(lua_State *ptClientData, unsigned long ulNetxAddress, unsigned short usData);
	// write a long (32bit) from the pc to the netx
	virtual void write_data32(lua_State *ptClientData, unsigned long ulNetxAddress, unsigned long ulData);
	// write a byte array from the pc to the netx
	virtual void write_image(unsigned long ulNetxAddress, const char *pcBUFFER_IN, size_t sizBUFFER_IN, SWIGLUA_REF tLuaFn, long lCallbackUserData);

	// call routine
	virtual void call(unsigned long ulNetxAddress, unsigned long ulParameterR0, SWIGLUA_REF tLuaFn, long lCallbackUserData);
// *** lua interface end ***

private:

	void hexdump(const unsigned char *pucData, unsigned long ulSize);

	bool chip_init(lua_State *ptClientData);

	bool m_fIsConnected;
	romloader_eth_device_platform *m_ptEthDev;
};

/*-----------------------------------*/

class romloader_eth_provider : public muhkuh_plugin_provider
{
public:
	romloader_eth_provider(swig_type_info *p_romloader_eth, swig_type_info *p_romloader_eth_reference);
	~romloader_eth_provider(void);

	int DetectInterfaces(lua_State *ptLuaStateForTableAccess);

	virtual romloader_eth *ClaimInterface(const muhkuh_plugin_reference *ptReference);
	virtual bool ReleaseInterface(muhkuh_plugin *ptPlugin);

private:
	static const char *m_pcPluginNamePattern;
};

/*-----------------------------------*/

class romloader_eth_reference : public muhkuh_plugin_reference
{
public:
	romloader_eth_reference(void);
	romloader_eth_reference(const char *pcName, const char *pcTyp, bool fIsUsed, romloader_eth_provider *ptProvider);
	romloader_eth_reference(const romloader_eth_reference *ptCloneMe);
};

/*-----------------------------------*/

#endif  /* __ROMLOADER_ETH_MAIN_H__ */


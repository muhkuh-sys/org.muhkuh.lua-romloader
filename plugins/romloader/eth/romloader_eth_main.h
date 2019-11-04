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

//#include "machine_interface_commands.h"
#include "../machine_interface/netx/src/monitor_commands.h"


#ifndef __ROMLOADER_ETH_MAIN_H__
#define __ROMLOADER_ETH_MAIN_H__


#if defined(_MSC_VER) || defined(__MINGW32__) || defined(__MINGW64__)
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
	romloader_eth(const char *pcName, const char *pcTyp, romloader_eth_provider *ptProvider, const char *pcServerName);
	~romloader_eth(void);

/* *** LUA interface start *** */
	/* Open the connection to the device. */
	virtual void Connect(lua_State *ptClientData);
	/* Close the connection to the device. */
	virtual void Disconnect(lua_State *ptClientData);
/* *** LUA interface end *** */

protected:
	virtual TRANSPORTSTATUS_T send_raw_packet(const void *pvPacket, size_t sizPacket);
	virtual TRANSPORTSTATUS_T receive_packet(void);

private:
	romloader_eth_device_platform *m_ptEthDev;
};

/*-----------------------------------*/

class romloader_eth_provider : public muhkuh_plugin_provider
{
public:
	romloader_eth_provider(swig_type_info *p_romloader_eth, swig_type_info *p_romloader_eth_reference);
	~romloader_eth_provider(void);

	int DetectInterfaces(lua_State *ptLuaStateForTableAccess, lua_State *ptLuaStateForTableAccessOptional);

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


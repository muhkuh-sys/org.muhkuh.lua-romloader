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


#include <vector>

#include "../romloader.h"

#ifndef __BAKA_MAIN_H__
#define __BAKA_MAIN_H__

/*-----------------------------------*/

class romloader_baka;

/*-----------------------------------*/

class romloader_baka_provider : public muhkuh_plugin_provider
{
public:
	romloader_baka_provider(void);
	~romloader_baka_provider(void);

	int DetectInterfaces(std::vector<muhkuh_plugin_reference*> &vInterfaceList);

	virtual romloader_baka *ClaimInterface(const muhkuh_plugin_reference *ptReference);
	virtual bool ReleaseInterface(romloader_baka *ptPlugin);

private:
	static const char *m_pcPluginNamePattern;

	typedef struct
	{
		bool fIsUsed;
	} BAKA_INSTANCE_CFG_T;

	BAKA_INSTANCE_CFG_T *m_ptInstanceCfg;

	int m_cfg_iInstances;
};

/*-----------------------------------*/

class romloader_baka : public romloader
{
public:
	romloader_baka(const char *pcName, const char *pcTyp, romloader_baka_provider *ptProvider);
	~romloader_baka(void);

// *** lua interface start ***
	// open the connection to the device
	virtual void Connect(void);
	// close the connection to the device
	virtual void Disconnect(void);

	// read a byte (8bit) from the netx to the pc
	virtual unsigned char read_data08(unsigned long ulNetxAddress);
	// read a word (16bit) from the netx to the pc
	virtual unsigned short read_data16(unsigned long ulNetxAddress);
	// read a long (32bit) from the netx to the pc
	virtual unsigned long read_data32(unsigned long ulNetxAddress);
	// read a byte array from the netx to the pc
//	virtual wxString read_image(double dNetxAddress, double dSize, lua_State *L, int iLuaCallbackTag, void *pvCallbackUserData);

	// write a byte (8bit) from the pc to the netx
	virtual void write_data08(unsigned long ulNetxAddress, unsigned char ucData);
	// write a word (16bit) from the pc to the netx
	virtual void write_data16(unsigned long ulNetxAddress, unsigned short usData);
	// write a long (32bit) from the pc to the netx
	virtual void write_data32(unsigned long ulNetxAddress, unsigned long ulData);
	// write a byte array from the pc to the netx
//	virtual void write_image(double dNetxAddress, wxString strData, lua_State *L, int iLuaCallbackTag, void *pvCallbackUserData);

	// call routine
//	virtual void call(double dNetxAddress, double dParameterR0, lua_State *L, int iLuaCallbackTag, void *pvCallbackUserData);

private:
};

/*-----------------------------------*/

#endif	/* __BAKA_MAIN_H__ */


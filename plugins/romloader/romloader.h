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


#include "../muhkuh_plugin_interface.h"
#include "romloader_def.h"


#ifndef __ROMLOADER_H__
#define __ROMLOADER_H__

/*-----------------------------------*/

class MUHKUH_EXPORT romloader_read_functinoid
{
public:
	romloader_read_functinoid(void) {}

	virtual unsigned long read_data32(unsigned long ulAddress) = 0;
};


class MUHKUH_EXPORT romloader : public muhkuh_plugin
{
protected:
	romloader(const char *pcName, const char *pcTyp, muhkuh_plugin_provider *ptProvider);
	romloader(const char *pcName, const char *pcTyp, const char *pcLocation, muhkuh_plugin_provider *ptProvider);
	~romloader(void);

public:
// *** lua interface start ***
	// read a byte (8bit) from the netx to the pc
	virtual unsigned char read_data08(lua_State *ptClientData, unsigned long ulNetxAddress) = 0;
	// read a word (16bit) from the netx to the pc
	virtual unsigned short read_data16(lua_State *ptClientData, unsigned long ulNetxAddress) = 0;
	// read a long (32bit) from the netx to the pc
	virtual unsigned long read_data32(lua_State *ptClientData, unsigned long ulNetxAddress) = 0;
	// read a byte array from the netx to the pc
	virtual void read_image(unsigned long ulNetxAddress, unsigned long ulSize, char **ppcBUFFER_OUT, size_t *psizBUFFER_OUT, SWIGLUA_REF tLuaFn, long lCallbackUserData) = 0;

	// write a byte (8bit) from the pc to the netx
	virtual void write_data08(lua_State *ptClientData, unsigned long ulNetxAddress, unsigned char ucData) = 0;
	// write a word (16bit) from the pc to the netx
	virtual void write_data16(lua_State *ptClientData, unsigned long ulNetxAddress, unsigned short usData) = 0;
	// write a long (32bit) from the pc to the netx
	virtual void write_data32(lua_State *ptClientData, unsigned long ulNetxAddress, unsigned long ulData) = 0;
	// write a byte array from the pc to the netx
	virtual void write_image(unsigned long ulNetxAddress, const char *pcBUFFER_IN, size_t sizBUFFER_IN, SWIGLUA_REF tLuaFn, long lCallbackUserData) = 0;

	// call routine
	virtual void call(unsigned long ulNetxAddress, unsigned long ulParameterR0, SWIGLUA_REF tLuaFn, long lCallbackUserData) = 0;

	// get chiptyp and romcode version
	virtual ROMLOADER_CHIPTYP GetChiptyp(void) const;
	virtual const char *GetChiptypName(ROMLOADER_CHIPTYP tChiptyp) const;

	// wrapper functions for compatibility with old function names
	virtual ROMLOADER_CHIPTYP get_chiptyp(void) const;
	virtual const char *get_chiptyp_name(ROMLOADER_CHIPTYP tChiptyp) const;
	virtual unsigned int get_romcode(void) const;
	virtual const char *get_romcode_name(unsigned int tRomcode) const;

// *** lua interface end ***

protected:
	typedef struct
	{
		unsigned long ulResetVector;
		unsigned long ulVersionAddress;
		unsigned long ulVersionValue;
		ROMLOADER_CHIPTYP tChiptyp;
		const char *pcChiptypName;
	} ROMLOADER_RESET_ID_T;


	bool detect_chiptyp(romloader_read_functinoid *ptFn);
	unsigned int crc16(unsigned short usCrc, unsigned char ucData);
	bool callback_long(SWIGLUA_REF *ptLuaFn, long lProgressData, long lCallbackUserData);
	bool callback_string(SWIGLUA_REF *ptLuaFn, const char *pcProgressData, size_t sizProgressData, long lCallbackUserData);

	ROMLOADER_CHIPTYP m_tChiptyp;

private:

	bool callback_common(SWIGLUA_REF *ptLuaFn, long lCallbackUserData, int iOldTopOfStack);

	static const ROMLOADER_RESET_ID_T atResIds[6];
};

/*-----------------------------------*/

#endif  /* __ROMLOADER_H__ */


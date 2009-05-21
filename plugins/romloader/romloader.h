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


#ifndef __ROMLOADER_H__
#define __ROMLOADER_H__

/*-----------------------------------*/

typedef enum
{
	ROMLOADER_CHIPTYP_UNKNOWN			= 0,
	ROMLOADER_CHIPTYP_NETX500			= 1,
	ROMLOADER_CHIPTYP_NETX100			= 2,
	ROMLOADER_CHIPTYP_NETX50			= 3
} ROMLOADER_CHIPTYP;


typedef enum
{
	ROMLOADER_ROMCODE_UNKNOWN			= 0,
	ROMLOADER_ROMCODE_ABOOT				= 1,
	ROMLOADER_ROMCODE_HBOOT				= 2
} ROMLOADER_ROMCODE;

/*-----------------------------------*/

class romloader : public muhkuh_plugin
{
protected:
	romloader(const char *pcName, const char *pcTyp, muhkuh_plugin_provider *ptProvider);
	~romloader(void);

public:
// *** lua interface start ***
	// read a byte (8bit) from the netx to the pc
	virtual unsigned char read_data08(unsigned long ulNetxAddress) = 0;
	// read a word (16bit) from the netx to the pc
	virtual unsigned short read_data16(unsigned long ulNetxAddress) = 0;
	// read a long (32bit) from the netx to the pc
	virtual unsigned long read_data32(unsigned long ulNetxAddress) = 0;
	// read a byte array from the netx to the pc
//	virtual wxString read_image(double dNetxAddress, double dSize, lua_State *L, int iLuaCallbackTag, void *pvCallbackUserData);

	// write a byte (8bit) from the pc to the netx
	virtual void write_data08(unsigned long ulNetxAddress, unsigned char ucData) = 0;
	// write a word (16bit) from the pc to the netx
	virtual void write_data16(unsigned long ulNetxAddress, unsigned short usData) = 0;
	// write a long (32bit) from the pc to the netx
	virtual void write_data32(unsigned long ulNetxAddress, unsigned long ulData) = 0;
	// write a byte array from the pc to the netx
//	virtual void write_image(double dNetxAddress, wxString strData, lua_State *L, int iLuaCallbackTag, void *pvCallbackUserData);

	// call routine
//	virtual void call(double dNetxAddress, double dParameterR0, lua_State *L, int iLuaCallbackTag, void *pvCallbackUserData);

	// get chiptyp and romcode version
	virtual ROMLOADER_CHIPTYP GetChiptyp(void) const;
	virtual const char *GetChiptypName(ROMLOADER_CHIPTYP tChiptyp) const;
	virtual ROMLOADER_ROMCODE GetRomcode(void) const;
	virtual const char *GetRomcodeName(ROMLOADER_ROMCODE tRomcode) const;

// *** lua interface end ***

protected:
	bool detect_chiptyp(void);
	unsigned int crc16(unsigned int uCrc, unsigned int uData);
//	bool callback_long(lua_State *L, int iLuaCallbackTag, long lProgressData, void *pvCallbackUserData);
//	bool callback_string(lua_State *L, int iLuaCallbackTag, wxString strProgressData, void *pvCallbackUserData);

	// the lua state
//	wxLuaState *m_ptLuaState;

	ROMLOADER_CHIPTYP m_tChiptyp;
	ROMLOADER_ROMCODE m_tRomcode;

private:
//	bool callback_common(lua_State *L, void *pvCallbackUserData, int iOldTopOfStack);

//	muhkuh_plugin_fn_close_instance m_fn_close;


	typedef struct
	{
		unsigned long ulResetVector;
		unsigned long ulVersionAddress;
		unsigned long ulVersionValue;
		ROMLOADER_CHIPTYP tChiptyp;
		const char *pcChiptypName;
		ROMLOADER_ROMCODE tRomcode;
		const char *pcRomcodeName;
	} ROMLOADER_RESET_ID_T;

	static const ROMLOADER_RESET_ID_T atResIds[3];
};

/*-----------------------------------*/

#endif	/* __ROMLOADER_H__ */


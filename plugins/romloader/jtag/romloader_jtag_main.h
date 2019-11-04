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


#include "openocd/romloader_jtag_openocd.h"

extern "C" {
int romloader_jtag_callback_string_c(void *pvCallbackUserData, uint8_t *pucData, unsigned long ulDataSize);
}

/*-----------------------------------*/


class romloader_jtag_options : public muhkuh_plugin_options
{
public:
	romloader_jtag_options(muhkuh_log *ptLog);
	romloader_jtag_options(const romloader_jtag_options *ptCloneMe);
	~romloader_jtag_options(void);

	virtual void set_option(const char *pcKey, lua_State *ptLuaState, int iIndex);

	typedef enum JTAG_RESET_ENUM
	{
		JTAG_RESET_HardReset = 0,
		JTAG_RESET_SoftReset = 1,
		JTAG_RESET_Attach = 2
	} JTAG_RESET_T;

	JTAG_RESET_T getOption_jtagReset(void);
	unsigned long getOption_jtagFrequencyKhz(void);

private:
	typedef struct JTAG_RESET_TO_NAME_STRUCT
	{
		JTAG_RESET_T tJtagReset;
		const char *pcName;
	} JTAG_RESET_TO_NAME_T;

	const JTAG_RESET_TO_NAME_T atJtagResetToName[3] =
	{
		{ JTAG_RESET_HardReset, "HardReset" },
		{ JTAG_RESET_SoftReset, "SoftReset" },
		{ JTAG_RESET_Attach,    "Attach" }
	};

	JTAG_RESET_T m_tOption_jtagReset;
	unsigned long m_tOption_jtagFrequencyKhz;
};


/*-----------------------------------*/

class romloader_jtag_provider;

/*-----------------------------------*/


class romloader_jtag : public romloader
{
public:
	romloader_jtag(const char *pcName, const char *pcTyp, romloader_jtag_provider *ptProvider, const char *pcInterfaceName, const char *pcTargetName, const char *pcLocation);
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

public:

    // public wrapper for romloader::callback_string
    bool callback_string_public(SWIGLUA_REF *ptLuaFn, const char *pcProgressData, size_t sizProgressData, long lCallbackUserData);

protected:
	virtual TRANSPORTSTATUS_T send_raw_packet(const void *pvPacket, size_t sizPacket);
	virtual TRANSPORTSTATUS_T receive_packet(void);

private:
	bool m_fIsInitialized;
	romloader_jtag_provider *m_ptJtagProvider;
	romloader_jtag_openocd *m_ptJtagDevice;

	char *m_pcInterfaceName;
	char *m_pcTargetName;
	char *m_pcLocation;
};

/*-----------------------------------*/

class romloader_jtag_provider : public muhkuh_plugin_provider
{
public:
	romloader_jtag_provider(swig_type_info *p_romloader_jtag, swig_type_info *p_romloader_jtag_reference);
	~romloader_jtag_provider(void);

	virtual romloader_jtag_options *GetOptions(void);
	int DetectInterfaces(lua_State *ptLuaStateForTableAccess, lua_State *ptLuaStateForTableAccessOptional);

	virtual romloader_jtag *ClaimInterface(const muhkuh_plugin_reference *ptReference);
	virtual bool ReleaseInterface(muhkuh_plugin *ptPlugin);

private:
	bool m_fIsInitialized;
	romloader_jtag_openocd *m_ptJtagDevice;
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


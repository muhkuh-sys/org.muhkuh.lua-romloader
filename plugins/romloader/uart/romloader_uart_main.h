/***************************************************************************
 *   Copyright (C) 2010 by Christoph Thelen                                *
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


#ifndef __ROMLOADER_UART_MAIN_H__
#define __ROMLOADER_UART_MAIN_H__


#ifdef _WIN32
	#include "romloader_uart_device_win.h"
	#define romloader_uart_device_platform romloader_uart_device_win
#else
	#include "romloader_uart_device_linux.h"
	#define romloader_uart_device_platform romloader_uart_device_linux
#endif


typedef enum
{
	UARTSTATUS_OK           = 0,
	UARTSTATUS_TIMEOUT      = 1,
	UARTSTATUS_PACKET_TOO_LARGE = 2,
	UARTSTATUS_SEND_FAILED = 3,
	UARTSTATUS_FAILED_TO_SYNC = 4,
	UARTSTATUS_CRC_MISMATCH = 5,
	UARTSTATUS_MISSING_USERDATA = 6,
	UARTSTATUS_COMMAND_EXECUTION_FAILED = 7
} UARTSTATUS_T;


/*-----------------------------------*/

class romloader_uart_provider;

/*-----------------------------------*/

class romloader_uart : public romloader
{
public:
	romloader_uart(const char *pcName, const char *pcTyp, romloader_uart_provider *ptProvider, const char *pcDeviceName);
	~romloader_uart(void);

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

	bool identify_loader(bool *pfNeedsUpdate);
	bool chip_init(lua_State *ptClientData);

	romloader_uart_device_platform *m_ptUartDev;

	size_t m_sizPacketRingBufferHead;
	size_t m_sizPacketRingBufferFill;
	unsigned char m_aucPacketRingBuffer[MONITOR_MAX_PACKET_SIZE];

	size_t m_sizPacketInputBuffer;
	unsigned char m_aucPacketInputBuffer[MONITOR_MAX_PACKET_SIZE];


	void packet_ringbuffer_init(void);
	UARTSTATUS_T packet_ringbuffer_fill(size_t sizRequestedFillLevel);
	unsigned char packet_ringbuffer_get(void);
	int packet_ringbuffer_peek(size_t sizOffset);

	UARTSTATUS_T send_packet(const unsigned char *pucData, size_t sizData);
	UARTSTATUS_T receive_packet(void);
	UARTSTATUS_T execute_command(const unsigned char *aucCommand, size_t sizCommand);
};

/*-----------------------------------*/

class romloader_uart_provider : public muhkuh_plugin_provider
{
public:
	romloader_uart_provider(swig_type_info *p_romloader_uart, swig_type_info *p_romloader_uart_reference);
	~romloader_uart_provider(void);

	int DetectInterfaces(lua_State *ptLuaStateForTableAccess);

	virtual romloader_uart *ClaimInterface(const muhkuh_plugin_reference *ptReference);
	virtual bool ReleaseInterface(muhkuh_plugin *ptPlugin);

private:
	static const char *m_pcPluginNamePattern;
};

/*-----------------------------------*/

class romloader_uart_reference : public muhkuh_plugin_reference
{
public:
	romloader_uart_reference(void);
	romloader_uart_reference(const char *pcName, const char *pcTyp, bool fIsUsed, romloader_uart_provider *ptProvider);
	romloader_uart_reference(const romloader_uart_reference *ptCloneMe);
};

/*-----------------------------------*/

#endif	/* __ROMLOADER_UART_MAIN_H__ */


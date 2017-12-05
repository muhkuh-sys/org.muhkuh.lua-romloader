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

#include "../machine_interface/netx/src/monitor_commands.h"


#ifndef __ROMLOADER_UART_MAIN_H__
#define __ROMLOADER_UART_MAIN_H__


#ifdef _WIN32
#       include "romloader_uart_device_win.h"
#       define romloader_uart_device_platform romloader_uart_device_win
#else
#       include "romloader_uart_device_linux.h"
#       define romloader_uart_device_platform romloader_uart_device_linux
#endif


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
	typedef enum ROMLOADER_COMMANDSET_ENUM
	{
		ROMLOADER_COMMANDSET_UNKNOWN          = 0,
		ROMLOADER_COMMANDSET_ABOOT_OR_HBOOT1  = 1,
		ROMLOADER_COMMANDSET_MI1              = 2,
		ROMLOADER_COMMANDSET_MI2              = 3,
		ROMLOADER_COMMANDSET_MI3              = 4
	} ROMLOADER_COMMANDSET_T;

	static const size_t sizMaxPacketSizeHost = 4096;
	size_t m_sizMaxPacketSizeClient;
	
	void hexdump(const uint8_t *pucData, uint32_t ulSize);

	bool identify_loader(ROMLOADER_COMMANDSET_T *ptCmdSet);
	bool synchronize(void);

	romloader_uart_device_platform *m_ptUartDev;

	unsigned char m_ucMonitorSequence;

	size_t m_sizPacketRingBufferHead;
	size_t m_sizPacketRingBufferFill;
	uint8_t m_aucPacketRingBuffer[sizMaxPacketSizeHost];

	size_t m_sizPacketInputBuffer;
	uint8_t m_aucPacketInputBuffer[sizMaxPacketSizeHost];


	void packet_ringbuffer_init(void);
	TRANSPORTSTATUS_T packet_ringbuffer_fill(size_t sizRequestedFillLevel);
	uint8_t packet_ringbuffer_get(void);
	int packet_ringbuffer_peek(size_t sizOffset);
	void packet_ringbuffer_discard(void);

	TRANSPORTSTATUS_T send_sync_command(void);
	TRANSPORTSTATUS_T send_packet(const uint8_t *pucData, size_t sizData);
	TRANSPORTSTATUS_T receive_packet(void);
	TRANSPORTSTATUS_T send_ack(unsigned char ucSequenceToAck);
	TRANSPORTSTATUS_T execute_command(const uint8_t *aucCommand, size_t sizCommand);
	TRANSPORTSTATUS_T read_data(uint32_t ulNetxAddress, MONITOR_ACCESSSIZE_T tAccessSize, uint16_t sizDataInBytes);
	TRANSPORTSTATUS_T write_data(uint32_t ulNetxAddress, MONITOR_ACCESSSIZE_T tAccessSize, const void *pvData, uint16_t sizDataInBytes);
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


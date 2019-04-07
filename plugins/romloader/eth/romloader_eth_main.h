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

/* Do not use these structures in Swig. */
#if !defined(SWIG)

/* Endianness macros translating 16 and 32bit from the host to the netX byte order.
 * FIXME: Detect the host endianness. More infos here:
 *          http://www.boost.org/doc/libs/1_43_0/boost/detail/endian.hpp
 *          https://cmake.org/cmake/help/v3.5/module/TestBigEndian.html
 */
#define HTONETX16(a) (a)
#define HTONETX32(a) (a)

#define NETXTOH16(a) (a)
#define NETXTOH32(a) (a)


/* This is a packet header up to the packet type. */
struct __attribute__((__packed__)) MIV3_PACKET_HEADER_STRUCT
{
	uint8_t  ucStreamStart;
	uint16_t usDataSize;
	uint8_t  ucSequenceNumber;
	uint8_t  ucPacketType;
};

typedef union MIV3_PACKET_HEADER_UNION
{
	struct MIV3_PACKET_HEADER_STRUCT s;
	uint8_t auc[5];
} MIV3_PACKET_HEADER_T;



/* This is a complete sync packet. */
struct __attribute__((__packed__)) MIV3_PACKET_SYNC_STRUCT
{
	MIV3_PACKET_HEADER_T tHeader;
	uint8_t  aucMacic[4];
	uint16_t  usVersionMinor;
	uint16_t  usVersionMajor;
	uint8_t   ucChipType;
	uint16_t  usMaximumPacketSize;
	uint8_t   ucCrcHi;
	uint8_t   ucCrcLo;
};

typedef union MIV3_PACKET_SYNC_UNION
{
	struct MIV3_PACKET_SYNC_STRUCT s;
	uint8_t auc[18];
} MIV3_PACKET_SYNC_T;



/* This is a complete acknowledge packet. */
struct __attribute__((__packed__)) MIV3_PACKET_ACK_STRUCT
{
	MIV3_PACKET_HEADER_T tHeader;
	uint8_t  ucCrcHi;
	uint8_t  ucCrcLo;
};

typedef union MIV3_PACKET_ACK_UNION
{
	struct MIV3_PACKET_ACK_STRUCT s;
	uint8_t auc[7];
} MIV3_PACKET_ACK_T;



/* This is a complete status packet. */
struct __attribute__((__packed__)) MIV3_PACKET_STATUS_STRUCT
{
	MIV3_PACKET_HEADER_T tHeader;
	uint8_t  ucStatus;
	uint8_t  ucCrcHi;
	uint8_t  ucCrcLo;
};

typedef union MIV3_PACKET_STATUS_UNION
{
	struct MIV3_PACKET_STATUS_STRUCT s;
	uint8_t auc[8];
} MIV3_PACKET_STATUS_T;



/* This is a complete status packet. */
struct __attribute__((__packed__)) MIV3_PACKET_CANCEL_CALL_STRUCT
{
	MIV3_PACKET_HEADER_T tHeader;
	uint8_t  ucData;
	uint8_t  ucCrcHi;
	uint8_t  ucCrcLo;
};

typedef union MIV3_PACKET_CANCEL_CALL_UNION
{
	struct MIV3_PACKET_CANCEL_CALL_STRUCT s;
	uint8_t auc[8];
} MIV3_PACKET_CANCEL_CALL_T;



/* This is a complete read packet. */
struct __attribute__((__packed__)) MIV3_PACKET_COMMAND_READ_DATA_STRUCT
{
	MIV3_PACKET_HEADER_T tHeader;
	uint16_t usDataSize;
	uint32_t ulAddress;
	uint8_t  ucCrcHi;
	uint8_t  ucCrcLo;
};

typedef union MIV3_PACKET_COMMAND_READ_DATA_UNION
{
	struct MIV3_PACKET_COMMAND_READ_DATA_STRUCT s;
	uint8_t auc[13];
} MIV3_PACKET_COMMAND_READ_DATA_T;



/* This is the start of a write packet. */
struct __attribute__((__packed__)) MIV3_PACKET_COMMAND_WRITE_DATA_HEADER_STRUCT
{
	MIV3_PACKET_HEADER_T tHeader;
	uint16_t usDataSize;
	uint32_t ulAddress;
};

typedef union MIV3_PACKET_COMMAND_WRITE_DATA_HEADER_UNION
{
	struct MIV3_PACKET_COMMAND_WRITE_DATA_HEADER_STRUCT s;
	uint8_t auc[11];
} MIV3_PACKET_COMMAND_WRITE_DATA_HEADER_T;



/* This is a complete call package. */
struct __attribute__((__packed__)) MIV3_PACKET_COMMAND_CALL_STRUCT
{
	MIV3_PACKET_HEADER_T tHeader;
	uint32_t ulAddress;
	uint32_t ulR0;
};

typedef union MIV3_PACKET_COMMAND_CALL_UNION
{
	struct MIV3_PACKET_COMMAND_CALL_STRUCT s;
	uint8_t auc[13];
} MIV3_PACKET_COMMAND_CALL_T;

#endif  /* !defined(SWIG) */


/*-----------------------------------*/

class romloader_eth_provider;

/*-----------------------------------*/

class romloader_eth : public romloader
{
public:
	romloader_eth(const char *pcName, const char *pcTyp, romloader_eth_provider *ptProvider, const char *pcServerName);
	~romloader_eth(void);

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
	/* This is the maximum size for a packet buffer in bytes.
	 * NOTE: This has nothing to do with the maximum packet size
	 * for a hboot backet.
	 */
	static const size_t sizMaxPacketSizeHost = 4096;
	size_t m_sizMaxPacketSizeClient;

	void hexdump(const uint8_t *pucData, uint32_t ulSize);
	romloader::TRANSPORTSTATUS_T send_packet(MIV3_PACKET_HEADER_T *ptPacket, size_t sizData);
	romloader::TRANSPORTSTATUS_T receive_packet(void);
	TRANSPORTSTATUS_T send_sync_command(void);
	bool synchronize(void);
	TRANSPORTSTATUS_T send_ack(unsigned char ucSequenceToAck);
	TRANSPORTSTATUS_T execute_command(MIV3_PACKET_HEADER_T *ptPacket, size_t sizPacket);
	TRANSPORTSTATUS_T read_data(uint32_t ulNetxAddress, MONITOR_ACCESSSIZE_T tAccessSize, uint16_t sizDataInBytes);
	TRANSPORTSTATUS_T write_data(uint32_t ulNetxAddress, MONITOR_ACCESSSIZE_T tAccessSize, const void *pvData, uint16_t sizDataInBytes);

	bool m_fIsConnected;
	romloader_eth_device_platform *m_ptEthDev;

	uint8_t m_ucMonitorSequence;

	size_t m_sizPacketInputBuffer;
	uint8_t m_aucPacketInputBuffer[sizMaxPacketSizeHost];
	uint8_t aucTxBuffer[sizMaxPacketSizeHost];
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


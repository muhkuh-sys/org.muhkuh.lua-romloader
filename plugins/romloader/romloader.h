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

#include <stdint.h>

#include "../muhkuh_plugin_interface.h"
#include "../muhkuh_static_assert.h"
#include "machine_interface/netx/src/monitor_commands.h"
#include "romloader_def.h"


#ifndef __ROMLOADER_H__
#define __ROMLOADER_H__


/*-----------------------------------*/


/* Do not use these structures in Swig. */
#if !defined(SWIG)

/* Endianness macros translating 16 and 32bit from the host to the netX byte
 * order. Depend on GLIBC for now.
 * NOTE: This can be done with CMAKE too: https://cmake.org/cmake/help/v3.5/module/TestBigEndian.html
 */
#if defined(__GLIBC__)
/* GLIBC provides "htole*" and "le*toh" macros. */
#       include <endian.h>
#       define HTONETX16(a) (htole16(a))
#       define HTONETX32(a) (htole32(a))
#       define NETXTOH16(a) (htole16(a))
#       define NETXTOH32(a) (htole32(a))
#elif defined(_WIN32)
#	include <winsock2.h>
#       include <sys/param.h>
#       if BYTE_ORDER == LITTLE_ENDIAN
#               define HTONETX16(a) (a)
#               define HTONETX32(a) (a)
#               define NETXTOH16(a) (a)
#               define NETXTOH32(a) (a)
#       elif BYTE_ORDER == BIG_ENDIAN
#               define HTONETX16(a) __builtin_bswap16(a)
#               define HTONETX32(a) __builtin_bswap32(a)
#               define NETXTOH16(a) __builtin_bswap16(a)
#               define NETXTOH32(a) __builtin_bswap32(a)
#       else
#               error "Unknown endianness."
#       endif
#else
#       error "Unknown system. Add a way to detect the endianness here or use CMake."
#endif


/* NOTE: Use "pragma pack" instead of "attribute packed" as the latter does not work on MinGW.
 *       See here for details: https://sourceforge.net/p/mingw-w64/bugs/588/
 */
#pragma pack(push, 1)

/* This is a packet header up to the packet type. */
struct MIV3_PACKET_HEADER_STRUCT
{
	uint8_t  ucStreamStart;
	uint16_t usDataSize;
	uint8_t  ucSequenceNumber;
	uint8_t  ucPacketType;
};
MUHKUH_STATIC_ASSERT( sizeof(struct MIV3_PACKET_HEADER_STRUCT)==5, "Packing of MIV3_PACKET_HEADER_STRUCT does not work.");

typedef union MIV3_PACKET_HEADER_UNION
{
	struct MIV3_PACKET_HEADER_STRUCT s;
	uint8_t auc[5];
} MIV3_PACKET_HEADER_T;
MUHKUH_STATIC_ASSERT( sizeof(MIV3_PACKET_HEADER_T)==5, "Packing of MIV3_PACKET_HEADER_T does not work.");



/* This is a complete sync packet. */
struct MIV3_PACKET_SYNC_STRUCT
{
	MIV3_PACKET_HEADER_T tHeader;
	uint8_t  aucMagic[4];
	uint16_t  usVersionMinor;
	uint16_t  usVersionMajor;
	uint8_t   ucChipType;
	uint16_t  usMaximumPacketSize;
	uint8_t   ucCrcHi;
	uint8_t   ucCrcLo;
};
MUHKUH_STATIC_ASSERT( sizeof(struct MIV3_PACKET_SYNC_STRUCT)==18, "Packing of MIV3_PACKET_SYNC_STRUCT does not work.");

typedef union MIV3_PACKET_SYNC_UNION
{
	struct MIV3_PACKET_SYNC_STRUCT s;
	uint8_t auc[18];
} MIV3_PACKET_SYNC_T;
MUHKUH_STATIC_ASSERT( sizeof(MIV3_PACKET_SYNC_T)==18, "Packing of MIV3_PACKET_SYNC_T does not work.");



/* This is a complete acknowledge packet. */
struct MIV3_PACKET_ACK_STRUCT
{
	MIV3_PACKET_HEADER_T tHeader;
	uint8_t  ucCrcHi;
	uint8_t  ucCrcLo;
};
MUHKUH_STATIC_ASSERT( sizeof(struct MIV3_PACKET_ACK_STRUCT)==7, "Packing of MIV3_PACKET_ACK_STRUCT does not work.");

typedef union MIV3_PACKET_ACK_UNION
{
	struct MIV3_PACKET_ACK_STRUCT s;
	uint8_t auc[7];
} MIV3_PACKET_ACK_T;
MUHKUH_STATIC_ASSERT( sizeof(MIV3_PACKET_ACK_T)==7, "Packing of MIV3_PACKET_ACK_T does not work.");



/* This is a complete status packet. */
struct MIV3_PACKET_STATUS_STRUCT
{
	MIV3_PACKET_HEADER_T tHeader;
	uint8_t  ucStatus;
	uint8_t  ucCrcHi;
	uint8_t  ucCrcLo;
};
MUHKUH_STATIC_ASSERT( sizeof(struct MIV3_PACKET_STATUS_STRUCT)==8, "Packing of MIV3_PACKET_STATUS_STRUCT does not work.");

typedef union MIV3_PACKET_STATUS_UNION
{
	struct MIV3_PACKET_STATUS_STRUCT s;
	uint8_t auc[8];
} MIV3_PACKET_STATUS_T;
MUHKUH_STATIC_ASSERT( sizeof(MIV3_PACKET_STATUS_T)==8, "Packing of MIV3_PACKET_STATUS_T does not work.");



/* This is a complete status packet. */
struct MIV3_PACKET_CANCEL_CALL_STRUCT
{
	MIV3_PACKET_HEADER_T tHeader;
	uint8_t  ucData;
	uint8_t  ucCrcHi;
	uint8_t  ucCrcLo;
};
MUHKUH_STATIC_ASSERT( sizeof(struct MIV3_PACKET_CANCEL_CALL_STRUCT)==8, "Packing of MIV3_PACKET_CANCEL_CALL_STRUCT does not work.");

typedef union MIV3_PACKET_CANCEL_CALL_UNION
{
	struct MIV3_PACKET_CANCEL_CALL_STRUCT s;
	uint8_t auc[8];
} MIV3_PACKET_CANCEL_CALL_T;
MUHKUH_STATIC_ASSERT( sizeof(MIV3_PACKET_CANCEL_CALL_T)==8, "Packing of MIV3_PACKET_CANCEL_CALL_T does not work.");



/* This is a complete read packet. */
struct MIV3_PACKET_COMMAND_READ_DATA_STRUCT
{
	MIV3_PACKET_HEADER_T tHeader;
	uint16_t usDataSize;
	uint32_t ulAddress;
	uint8_t  ucCrcHi;
	uint8_t  ucCrcLo;
};
MUHKUH_STATIC_ASSERT( sizeof(struct MIV3_PACKET_COMMAND_READ_DATA_STRUCT)==13, "Packing of MIV3_PACKET_COMMAND_READ_DATA_STRUCT does not work.");

typedef union MIV3_PACKET_COMMAND_READ_DATA_UNION
{
	struct MIV3_PACKET_COMMAND_READ_DATA_STRUCT s;
	uint8_t auc[13];
} MIV3_PACKET_COMMAND_READ_DATA_T;
MUHKUH_STATIC_ASSERT( sizeof(MIV3_PACKET_COMMAND_READ_DATA_T)==13, "Packing of MIV3_PACKET_COMMAND_READ_DATA_T does not work.");



/* This is the start of a write packet. */
struct MIV3_PACKET_COMMAND_WRITE_DATA_HEADER_STRUCT
{
	MIV3_PACKET_HEADER_T tHeader;
	uint16_t usDataSize;
	uint32_t ulAddress;
};
MUHKUH_STATIC_ASSERT( sizeof(struct MIV3_PACKET_COMMAND_WRITE_DATA_HEADER_STRUCT)==11, "Packing of MIV3_PACKET_COMMAND_WRITE_DATA_HEADER_STRUCT does not work.");

typedef union MIV3_PACKET_COMMAND_WRITE_DATA_HEADER_UNION
{
	struct MIV3_PACKET_COMMAND_WRITE_DATA_HEADER_STRUCT s;
	uint8_t auc[11];
} MIV3_PACKET_COMMAND_WRITE_DATA_HEADER_T;
MUHKUH_STATIC_ASSERT( sizeof(MIV3_PACKET_COMMAND_WRITE_DATA_HEADER_T)==11, "Packing of MIV3_PACKET_COMMAND_WRITE_DATA_HEADER_T does not work.");



/* This is a complete call package. */
struct MIV3_PACKET_COMMAND_CALL_STRUCT
{
	MIV3_PACKET_HEADER_T tHeader;
	uint32_t ulAddress;
	uint32_t ulR0;
	uint8_t  ucCrcHi;
	uint8_t  ucCrcLo;
};
MUHKUH_STATIC_ASSERT( sizeof(struct MIV3_PACKET_COMMAND_CALL_STRUCT)==15, "Packing of MIV3_PACKET_COMMAND_CALL_STRUCT does not work.");

typedef union MIV3_PACKET_COMMAND_CALL_UNION
{
	struct MIV3_PACKET_COMMAND_CALL_STRUCT s;
	uint8_t auc[15];
} MIV3_PACKET_COMMAND_CALL_T;
MUHKUH_STATIC_ASSERT( sizeof(MIV3_PACKET_COMMAND_CALL_T)==15, "Packing of MIV3_PACKET_COMMAND_CALL_T does not work.");

#pragma pack(pop)


#endif  /* !defined(SWIG) */


/*-----------------------------------*/

class MUHKUH_EXPORT romloader_read_functinoid
{
public:
	romloader_read_functinoid(void) {}

	virtual uint32_t read_data32(uint32_t ulAddress) = 0;
};


class MUHKUH_EXPORT romloader : public muhkuh_plugin
{
protected:
	romloader(const char *pcName, const char *pcTyp, muhkuh_plugin_provider *ptProvider);
	romloader(const char *pcName, const char *pcTyp, const char *pcLocation, muhkuh_plugin_provider *ptProvider);
	~romloader(void);

public:
/* *** LUA interface start *** */
	/* Read a byte (8bit) from the netX to the PC. */
	virtual uint8_t read_data08(lua_State *ptClientData, uint32_t ulNetxAddress);
	/* Read a word (16bit) from the netX to the PC. */
	virtual uint16_t read_data16(lua_State *ptClientData, uint32_t ulNetxAddress);
	/* read a long (32bit) from the netX to the PC. */
	virtual uint32_t read_data32(lua_State *ptClientData, uint32_t ulNetxAddress);
	/* read a byte array from the netX to the PC. */
	virtual void read_image(uint32_t ulNetxAddress, uint32_t ulSize, char **ppcBUFFER_OUT, size_t *psizBUFFER_OUT, SWIGLUA_REF tLuaFn, long lCallbackUserData);

	/* Write a byte (8bit) from the PC to the netX. */
	virtual void write_data08(lua_State *ptClientData, uint32_t ulNetxAddress, uint8_t ucData);
	/* Write a word (16bit) from the PC to the netX. */
	virtual void write_data16(lua_State *ptClientData, uint32_t ulNetxAddress, uint16_t usData);
	/* Write a long (32bit) from the PC to the netX. */
	virtual void write_data32(lua_State *ptClientData, uint32_t ulNetxAddress, uint32_t ulData);
	/* Write a byte array from the PC to the netX. */
	virtual void write_image(uint32_t ulNetxAddress, const char *pcBUFFER_IN, size_t sizBUFFER_IN, SWIGLUA_REF tLuaFn, long lCallbackUserData);

	/* Call a routine on the netX. */
	virtual void call(uint32_t ulNetxAddress, uint32_t ulParameterR0, SWIGLUA_REF tLuaFn, long lCallbackUserData);

	/* Get the chip type. */
	virtual ROMLOADER_CHIPTYP GetChiptyp(void) const;
	virtual const char *GetChiptypName(ROMLOADER_CHIPTYP tChiptyp) const;

	/* Wrapper functions for compatibility with old function names. */
	virtual ROMLOADER_CHIPTYP get_chiptyp(void) const;
	virtual const char *get_chiptyp_name(ROMLOADER_CHIPTYP tChiptyp) const;
	virtual unsigned int get_romcode(void) const;
	virtual const char *get_romcode_name(unsigned int tRomcode) const;

/* *** LUA interface end *** */

#if !defined(SWIG)
	typedef enum
	{
		TRANSPORTSTATUS_OK                        = 0,
		TRANSPORTSTATUS_TIMEOUT                   = 1,
		TRANSPORTSTATUS_PACKET_TOO_LARGE          = 2,
		TRANSPORTSTATUS_RECEIVE_FAILED            = 3,
		TRANSPORTSTATUS_SEND_FAILED               = 4,
		TRANSPORTSTATUS_FAILED_TO_SYNC            = 5,
		TRANSPORTSTATUS_CRC_MISMATCH              = 6,
		TRANSPORTSTATUS_MISSING_USERDATA          = 7,
		TRANSPORTSTATUS_COMMAND_EXECUTION_FAILED  = 8,
		TRANSPORTSTATUS_SEQUENCE_MISMATCH         = 9,
		TRANSPORTSTATUS_NOT_CONNECTED             = 10,
		TRANSPORTSTATUS_NETX_ERROR                = 11,  /* The netX returned a status != OK. */
		TRANSPORTSTATUS_PACKET_TOO_SMALL          = 12,  /* The packet does not have enough data for 1 byte of user data. */
		TRANSPORTSTATUS_INVALID_PACKET_SIZE       = 13,  /* The packet size does not match the expected size for the packet type. */
		TRANSPORTSTATUS_UNEXPECTED_PACKET_SIZE    = 14,  /* The packet size does not match the number of requested bytes (usually a "read_data" response to a read command). */
		TRANSPORTSTATUS_UNEXPECTED_PACKET_TYP     = 15   /* The netX sent an unexpected answer. */
	} TRANSPORTSTATUS_T;

	const char *get_error_message(TRANSPORTSTATUS_T tStatus);

	/* This is only for debug messages. */
	static void hexdump(const uint8_t *pucData, uint32_t ulSize);
#endif

#if !defined(SWIG)
protected:
	/* Do not include this into the SWIG bindings. */
	typedef struct
	{
		uint32_t ulResetVector;
		uint32_t ulVersionAddress;
		uint32_t ulVersionValue;
		uint32_t ulCheckAddress;
		uint32_t ulCheckMask;
		uint32_t ulCheckCmpValue;
		ROMLOADER_CHIPTYP tChiptyp;
		const char *pcChiptypName;
	} ROMLOADER_RESET_ID_T;

	virtual TRANSPORTSTATUS_T send_raw_packet(const void *pvPacket, size_t sizPacket) = 0;
	virtual TRANSPORTSTATUS_T receive_packet(void) = 0;

	virtual bool synchronize(void);
	virtual TRANSPORTSTATUS_T send_packet(MIV3_PACKET_HEADER_T *ptPacket, size_t sizData);
	virtual TRANSPORTSTATUS_T execute_command(MIV3_PACKET_HEADER_T *ptPacket, size_t sizPacket);

	virtual TRANSPORTSTATUS_T send_ack(unsigned char ucSequenceToAck);
	virtual TRANSPORTSTATUS_T read_data(uint32_t ulNetxAddress, MONITOR_ACCESSSIZE_T tAccessSize, uint16_t sizDataInBytes);
	virtual TRANSPORTSTATUS_T write_data(uint32_t ulNetxAddress, MONITOR_ACCESSSIZE_T tAccessSize, const void *pvData, uint16_t sizDataInBytes);

	bool detect_chiptyp(romloader_read_functinoid *ptFn);
	uint16_t crc16(uint16_t usCrc, uint8_t ucData);
	bool callback_long(SWIGLUA_REF *ptLuaFn, long lProgressData, long lCallbackUserData);
	bool callback_string(SWIGLUA_REF *ptLuaFn, const char *pcProgressData, size_t sizProgressData, long lCallbackUserData);

	ROMLOADER_CHIPTYP m_tChiptyp;

	/* This is the maximum size for a packet buffer in bytes.
	 * NOTE: This has nothing to do with the maximum packet size
	 * for a HBOOT packet.
	 */
	static const size_t m_sizMaxPacketSizeHost = 4096;
	size_t m_sizMaxPacketSizeClient;

	uint8_t m_ucMonitorSequence;

	size_t m_sizPacketInputBuffer;
	uint8_t m_aucPacketInputBuffer[m_sizMaxPacketSizeHost];
	uint8_t aucTxBuffer[m_sizMaxPacketSizeHost];

private:

	bool callback_common(SWIGLUA_REF *ptLuaFn, long lCallbackUserData, int iOldTopOfStack);

	static const ROMLOADER_RESET_ID_T atResIds[14];
#endif  /* !defined(SWIG) */
};

/*-----------------------------------*/

#endif  /* __ROMLOADER_H__ */


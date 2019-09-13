/***************************************************************************
 *   Copyright (C) 2012 by Christoph Thelen                                *
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


#include "romloader_uart_read_functinoid.h"
#include "../machine_interface/netx/src/monitor_commands.h"
#include "romloader_uart_device.h"


#ifndef __ROMLOADER_UART_READ_FUNCTIONOID_MI1_H__
#define __ROMLOADER_UART_READ_FUNCTIONOID_MI1_H__


class romloader_uart_read_functinoid_mi1 : public romloader_uart_read_functinoid
{
public:
	romloader_uart_read_functinoid_mi1(romloader_uart_device *ptDevice, char *pcPortName);
	bool read_data32(uint32_t ulAddress, uint32_t *pulValue);
	int update_device(ROMLOADER_CHIPTYP tChiptyp);

private:
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


	static const size_t sizMaxPacketSizeMI1 = 256;

	size_t m_sizPacketRingBufferHead;
	size_t m_sizPacketRingBufferFill;
	uint8_t m_aucPacketRingBuffer[sizMaxPacketSizeMI1];

	size_t m_sizPacketInputBuffer;
	uint8_t m_aucPacketInputBuffer[sizMaxPacketSizeMI1];


	void packet_ringbuffer_init(void);
	UARTSTATUS_T packet_ringbuffer_fill(size_t sizRequestedFillLevel);
	uint8_t packet_ringbuffer_get(void);
	int packet_ringbuffer_peek(size_t sizOffset);

	UARTSTATUS_T execute_command(const uint8_t *aucCommand, size_t sizCommand);
	UARTSTATUS_T send_packet(const uint8_t *pucData, size_t sizData);
	UARTSTATUS_T receive_packet(void);

	bool write_image(uint32_t ulNetxAddress, const uint8_t *pucBUFFER_IN, size_t sizBUFFER_IN);
	bool call(uint32_t ulNetxAddress, uint32_t ulParameterR0);
	
	void hexdump(const uint8_t *pucData, uint32_t ulSize);

	romloader_uart_device *m_ptUartDev;
	char *m_pcPortName;
};


#endif  /* __ROMLOADER_UART_READ_FUNCTIONOID_MI1_H__ */


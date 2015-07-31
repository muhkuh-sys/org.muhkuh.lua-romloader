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
#include "romloader_uart_device.h"


#ifndef __ROMLOADER_UART_READ_FUNCTINOID_ABOOT_H__
#define __ROMLOADER_UART_READ_FUNCTINOID_ABOOT_H__


class romloader_uart_read_functinoid_aboot : public romloader_uart_read_functinoid
{
public:
	romloader_uart_read_functinoid_aboot(romloader_uart_device *ptDevice, char *pcPortName);
	uint32_t read_data32(uint32_t ulAddress);
	int update_device(ROMLOADER_CHIPTYP tChiptyp);

private:
	bool legacy_read_v1(uint32_t ulAddress, uint32_t *pulValue);
	void hexdump(const uint8_t *pucData, uint32_t ulSize);

	bool netx50_load_code(const uint8_t *pucNetxCode, size_t sizNetxCode);
	bool netx500_load_code(const uint8_t *pucNetxCode, size_t sizNetxCode, uint32_t ulLoadAddress);
	bool netx50_start_code(void);
	bool netx500_start_code(uint32_t ulExecuteAddress);


	romloader_uart_device *m_ptDevice;
	char *m_pcPortName;
};


#endif  /* __ROMLOADER_UART_READ_FUNCTINOID_ABOOT_H__ */


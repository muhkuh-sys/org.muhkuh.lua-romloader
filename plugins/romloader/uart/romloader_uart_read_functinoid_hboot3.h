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
#include "romloader_uart_main.h"


#ifndef __ROMLOADER_UART_READ_FUNCTINOID_HBOOT3_H__
#define __ROMLOADER_UART_READ_FUNCTINOID_HBOOT3_H__

typedef enum READ_RESULT_ENUM
{
	READ_RESULT_OK = 0,
	READ_RESULT_ERROR = 1,
	READ_RESULT_FORBIDDEN = 2
} READ_RESULT_T;

class romloader_uart_read_functinoid_hboot3 : public romloader_uart_read_functinoid
{
public:
	romloader_uart_read_functinoid_hboot3(romloader_uart_device *ptDevice, char *pcPortName);
	bool read_data32(uint32_t ulAddress, uint32_t *pulValue);
	READ_RESULT_T is_read_allowed();
	READ_RESULT_T read_data32_check_secure(uint32_t ulAddress, uint32_t *pulValue);
	CONSOLE_MODE_T detect_console_mode(void);
	bool send_vers_command(uint32_t *pulVersionVal1, uint32_t *pulVersionVal2);
	int update_device(ROMLOADER_CHIPTYP tChiptyp);

private:
	void hexdump(const uint8_t *pucData, uint32_t ulSize);
	bool read_binary_file(const char* pucFilename, const uint8_t **ppucData, size_t *p_sizData);
	bool get_netx90_mi_image(const uint8_t **ppucData, size_t *p_sizData);
	bool netx90_load_usip(const uint8_t *pucNetxImage, size_t sizNetxImage);
	bool netx90_start_hboot_image(const uint8_t *pucNetxImage, size_t sizNetxImage);
	bool skip_mi_magic(void);
	bool netx90_reset(void);
	romloader_uart_device *m_ptDevice;
	char *m_pcPortName;
};


#endif  /* __ROMLOADER_UART_READ_FUNCTINOID_HBOOT3_H__ */


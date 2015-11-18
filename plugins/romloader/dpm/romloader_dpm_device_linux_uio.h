/***************************************************************************
 *   Copyright (C) 2015 by Christoph Thelen                                *
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

#ifndef __ROMLOADER_DPM_DEVICE_LINUX_UIO_H__
#define __ROMLOADER_DPM_DEVICE_LINUX_UIO_H__


#include "romloader_dpm_device.h"


#include <stddef.h>


class romloader_dpm_device_linux_uio : public romloader_dpm_device
{
public:
	romloader_dpm_device_linux_uio(unsigned int uiUioDevice);
	romloader_dpm_device_linux_uio(const char *pcDeviceName);
	virtual ~romloader_dpm_device_linux_uio(void);

	int Open(void);
	void Close(void);

	virtual int dpm_read08(uint32_t ulDpmAddress, uint8_t *pucValue);
	virtual int dpm_read16(uint32_t ulDpmAddress, uint16_t *pusValue);
	virtual int dpm_read32(uint32_t ulDpmAddress, uint32_t *pulValue);
	virtual int dpm_read_area(uint32_t ulDpmAddress, uint8_t *pucData, size_t sizData);

	virtual int dpm_write08(uint32_t ulDpmAddress, uint8_t ucValue);
	virtual int dpm_write16(uint32_t ulDpmAddress, uint16_t usValue);
	virtual int dpm_write32(uint32_t ulDpmAddress, uint32_t ulValue);
	virtual int dpm_write_area(uint32_t ulDpmAddress, const uint8_t *pucData, size_t sizData);

	static size_t ScanForDevices(char ***pppcPortNames);

	static int IsMyDeviceName(const char *pcDeviceName);

protected:

private:
	typedef struct NETX_UIO_TYP_STRUCT
	{
		const char *pcName;
		unsigned int uiBar;
		ptrdiff_t ulBarSize;
	} NETX_UIO_TYP_T;

	typedef union VPTR_UNION
	{
		uint8_t *puc;
		uint16_t *pus;
		uint32_t *pul;
		void *pv;
	} VPTR_T;

	unsigned int m_uiUioDevice;    /* The number of the UIO device. */
	unsigned int m_uiUioBar;       /* The bar number of the UIO device. */
	char *m_pcUioName;             /* The name of the UIO device. */
	ptrdiff_t m_ulUioAddress;
	size_t m_sizUioMemory;
	VPTR_T m_pvUioMappedMemory;

	void cut_off_newlines(char *pcBuffer);

	int uio_get_name(void);
	int uio_get_size(void);
	int uio_get_addr(void);
	int uio_probe(void);
	int uio_map_dpm(void);

	static size_t scan_devices(char **ppcDevices, size_t sizDevicesMax);

	static const NETX_UIO_TYP_T acKnownNetxUioTypes[1];

	static const char *s_pcDeviceNamePattern;
};


#endif  /* __ROMLOADER_UART_DEVICE_LINUX_UIO_H__ */


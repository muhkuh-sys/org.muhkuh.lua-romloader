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

#ifndef __ROMLOADER_DPM_DEVICE_H__
#define __ROMLOADER_DPM_DEVICE_H__


#include "../romloader.h"


class romloader_dpm_device
{
public:
	romloader_dpm_device(void);
	virtual ~romloader_dpm_device(void);

	virtual int Open(void) = 0;
	virtual void Close(void) = 0;

	virtual int dpm_read08(uint32_t ulDpmAddress, uint8_t *pucValue) = 0;
	virtual int dpm_read16(uint32_t ulDpmAddress, uint16_t *pusValue) = 0;
	virtual int dpm_read32(uint32_t ulDpmAddress, uint32_t *pulValue) = 0;
	virtual int dpm_read_area(uint32_t ulDpmAddress, uint8_t *pucData, size_t sizData) = 0;

	virtual int dpm_write08(uint32_t ulDpmAddress, uint8_t ucValue) = 0;
	virtual int dpm_write16(uint32_t ulDpmAddress, uint16_t usValue) = 0;
	virtual int dpm_write32(uint32_t ulDpmAddress, uint32_t ulValue) = 0;
	virtual int dpm_write_area(uint32_t ulDpmAddress, const uint8_t *pucData, size_t sizData) = 0;

	ROMLOADER_CHIPTYP get_chiptyp(void);

protected:
	typedef enum NETX_DPM_SIZE_ENUM
	{
		NETX_DPM_SIZE_08,
		NETX_DPM_SIZE_16,
		NETX_DPM_SIZE_32
	} NETX_DPM_SIZE_T;

	typedef struct DPM_DETECT_ELEMENT_STRUCT
	{
		size_t sizOffset;
		uint32_t ulMask;
		uint32_t ulExpectedValue;
	} DPM_DETECT_ELEMENT_T;

	typedef struct DPM_DETECT_LIST_STRUCT
	{
		const char *pcName;
		ROMLOADER_CHIPTYP tChipTyp;
		const DPM_DETECT_ELEMENT_T *ptDetectList;
		size_t sizDetectList;
	} DPM_DETECT_LIST_T;

	ROMLOADER_CHIPTYP m_tChipTyp;

	int detect(void);

private:
	static const DPM_DETECT_ELEMENT_T atDpmDetectElements_netx56[2];
	static const DPM_DETECT_ELEMENT_T atDpmDetectElements_netx4000_RELAXED[2];
	static const DPM_DETECT_LIST_T atDpmDetectList[2];
};


#endif  /* __ROMLOADER_UART_DEVICE_H__ */


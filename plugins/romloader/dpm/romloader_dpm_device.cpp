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


#include <stdio.h>
#include <string.h>

#include "romloader_dpm_device.h"
#include "romloader_dpm_memorylayout.h"


#if defined(__GNUC__) || defined(__MINGW32__) || defined(__MINGW64__)
#       include <unistd.h>
#       define SLEEP_MS(ms) usleep(ms*1000)
#endif


/* Convert a 32bit value from little endianess to bis endianess. */
#define LITTLE16_TO_BIGEND16_UINT32(a) (((a&0x000000ffU)<<8U)|((a&0x0000ff00U)>>8U)|((a&0x00ff0000U)<<8U)|((a&0xff000000U)>>8U))


const romloader_dpm_device::DPM_DETECT_ELEMENT_T romloader_dpm_device::atDpmDetectElements_netx56[2] =
{
	{
		.sizOffset = offsetof(NETX_DPM_MEMORYLAYOUT_T,uReg) + offsetof(DPM_REGISTER_NETX56_T,tCtrl) + offsetof(NX56_DPM_AREA_T,ulDpm_netx_version),
		.ulMask = NETX56_NETX_VERSION_MASK,
		.ulExpectedValue = NETX56_NETX_VERSION_STEP
	},
	{
		.sizOffset = offsetof(NETX_DPM_MEMORYLAYOUT_T,uReg) + offsetof(DPM_REGISTER_NETX56_T,tCtrl) + offsetof(NX56_DPM_AREA_T,ulDpm_netx_version_bigend16),
		.ulMask = LITTLE16_TO_BIGEND16_UINT32(NETX56_NETX_VERSION_MASK),
		.ulExpectedValue = LITTLE16_TO_BIGEND16_UINT32(NETX56_NETX_VERSION_STEP)
	},
};


//const romloader_dpm_device::DPM_DETECT_ELEMENT_T romloader_dpm_device::atDpmDetectElements_netx4000_RELAXED[2] =
//{
//	{
//		.sizOffset = offsetof(NETX_DPM_MEMORYLAYOUT_T,uReg) + offsetof(DPM_REGISTER_NETX4000_RELAXED_T,tCtrl) + offsetof(NX4000_DPM_AREA_T,ulDpm_netx_version),
//		.ulMask = NETX4000_RELAXED_NETX_VERSION_MASK,
//		.ulExpectedValue = NETX4000_RELAXED_NETX_VERSION_STEP
//	},
//	{
//		.sizOffset = offsetof(NETX_DPM_MEMORYLAYOUT_T,uReg) + offsetof(DPM_REGISTER_NETX4000_RELAXED_T,tCtrl) + offsetof(NX4000_DPM_AREA_T,ulDpm_netx_version_bigend16),
//		.ulMask = LITTLE16_TO_BIGEND16_UINT32(NETX4000_RELAXED_NETX_VERSION_MASK),
//		.ulExpectedValue = LITTLE16_TO_BIGEND16_UINT32(NETX4000_RELAXED_NETX_VERSION_STEP)
//	},
//};

const romloader_dpm_device::DPM_DETECT_LIST_T romloader_dpm_device::atDpmDetectList[2] =
{
	{
		.pcName = "netX56",
		.tChipTyp = ROMLOADER_CHIPTYP_NETX56,
		.ptDetectList = atDpmDetectElements_netx56,
		.sizDetectList = sizeof(atDpmDetectElements_netx56)/sizeof(atDpmDetectElements_netx56[0])
	},

//	{
//		.pcName = "netX4000_RELAXED",
//		.tChipTyp = ROMLOADER_CHIPTYP_NETX4000_RELAXED,
//		.ptDetectList = atDpmDetectElements_netx4000_RELAXED,
//		.sizDetectList = sizeof(atDpmDetectElements_netx4000_RELAXED)/sizeof(atDpmDetectElements_netx4000_RELAXED[0])
//	}
};


romloader_dpm_device::romloader_dpm_device(void)
 : m_tChipTyp(ROMLOADER_CHIPTYP_UNKNOWN)
{
}



romloader_dpm_device::~romloader_dpm_device(void)
{
}



ROMLOADER_CHIPTYP romloader_dpm_device::get_chiptyp(void)
{
	return m_tChipTyp;
}



int romloader_dpm_device::detect(void)
{
	ROMLOADER_CHIPTYP tChipTyp;
	const romloader_dpm_device::DPM_DETECT_LIST_T *ptListCnt;
	const romloader_dpm_device::DPM_DETECT_LIST_T *ptListEnd;
	const romloader_dpm_device::DPM_DETECT_ELEMENT_T *ptElementCnt;
	const romloader_dpm_device::DPM_DETECT_ELEMENT_T *ptElementEnd;
	uint32_t ulValue;
	int iAllMatchesOk;
	int iResult;


	/* No chip detected yet. */
	tChipTyp = ROMLOADER_CHIPTYP_UNKNOWN;

	/* Be optimistic. */
	iResult = 0;

	printf("__/Detect/___________________________________________________________________\n");
	/* Loop over all entries in the detect list. */
	ptListCnt = this->atDpmDetectList;
	ptListEnd = this->atDpmDetectList + (sizeof(this->atDpmDetectList)/sizeof(this->atDpmDetectList[0]));
	while( ptListCnt<ptListEnd )
	{
		printf("Looking for '%s'...\n", ptListCnt->pcName);

		/* Loop over all elements in the detect list. */
		iAllMatchesOk = (1==1);

		ptElementCnt = ptListCnt->ptDetectList;
		ptElementEnd = ptListCnt->ptDetectList + ptListCnt->sizDetectList;
		while( ptElementCnt<ptElementEnd )
		{
			/* Read the value from the offset. */
			printf("  Is DPM[0x%04zx] & 0x%08x == 0x%08x ?", ptElementCnt->sizOffset, ptElementCnt->ulMask, ptElementCnt->ulExpectedValue);
			iResult = this->dpm_read32(ptElementCnt->sizOffset, &ulValue);
			if( iResult!=0 )
			{
				printf(" -> Failed to read data!\n");
				iAllMatchesOk = 0;
				break;
			}
			else
			{
				ulValue &= ptElementCnt->ulMask;
				if( ulValue==ptElementCnt->ulExpectedValue )
				{
					printf(" -> Yes\n");
				}
				else
				{
					printf(" -> No (0x%08x)\n", ulValue);
					iAllMatchesOk = 0;
					break;
				}
			}
			++ptElementCnt;
		}

		if( iResult!=0 )
		{
			break;
		}
		else if( iAllMatchesOk!=0 )
		{
			/* Found a match! */
			tChipTyp = ptListCnt->tChipTyp;
			printf("Found chip typ %d.\n", tChipTyp);
			break;
		}

		++ptListCnt;
	}
	printf("_____________________________________________________________________________\n\n");

	/* No chip detected -> error. */
	if( tChipTyp==ROMLOADER_CHIPTYP_UNKNOWN )
	{
		iResult = -1;
	}
	/* In case of a general error reset the detected chip type to "unknown". */
	else if( iResult!=0 )
	{
		tChipTyp = ROMLOADER_CHIPTYP_UNKNOWN;
	}

	m_tChipTyp = tChipTyp;

	return iResult;
}

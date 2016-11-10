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


#include <stdio.h>

#include "romloader.h"


romloader::romloader(const char *pcName, const char *pcTyp, muhkuh_plugin_provider *ptProvider)
 : muhkuh_plugin(pcName, pcTyp, ptProvider)
 , m_tChiptyp(ROMLOADER_CHIPTYP_UNKNOWN)
{
/*	printf("%s(%p): created in romloader\n", m_pcName, this); */
}


romloader::romloader(const char *pcName, const char *pcTyp, const char *pcLocation, muhkuh_plugin_provider *ptProvider)
 : muhkuh_plugin(pcName, pcTyp, pcLocation, ptProvider)
 , m_tChiptyp(ROMLOADER_CHIPTYP_UNKNOWN)
{
/*	printf("%s(%p): created in romloader\n", m_pcName, this); */
}


romloader::~romloader(void)
{
/*	printf("%s(%p): deleted in romloader\n", m_pcName, this); */
}


ROMLOADER_CHIPTYP romloader::GetChiptyp(void) const
{
	return m_tChiptyp;
}



const char *romloader::GetChiptypName(ROMLOADER_CHIPTYP tChiptyp) const
{
	const char *pcChiptyp;
	const ROMLOADER_RESET_ID_T *ptCnt, *ptEnd;


	// init chip name with unknown name
	pcChiptyp = "unknown chip";

	// loop over all known romcodes and search the romcode typ
	ptCnt = atResIds;
	ptEnd = ptCnt + (sizeof(atResIds)/sizeof(atResIds[0]));
	while( ptCnt<ptEnd )
	{
		if( ptCnt->tChiptyp==tChiptyp )
		{
			pcChiptyp = ptCnt->pcChiptypName;
			break;
		}
		++ptCnt;
	}

	return pcChiptyp;
}


// wrapper functions for compatibility with old function names
ROMLOADER_CHIPTYP  romloader::get_chiptyp(void) const                             {return GetChiptyp();}
unsigned int       romloader::get_romcode(void) const                             {return 0;}
const char        *romloader::get_chiptyp_name(ROMLOADER_CHIPTYP tChiptyp) const  {return GetChiptypName(tChiptyp);}
const char        *romloader::get_romcode_name(unsigned int tRomcode) const       {return "Unknown";}

bool romloader::detect_chiptyp(romloader_read_functinoid *ptFn)
{
	uint32_t ulResetVector;
	const ROMLOADER_RESET_ID_T *ptRstCnt, *ptRstEnd;
	uint32_t ulVersionAddr;
	uint32_t ulVersion;
	bool fResult;
	ROMLOADER_CHIPTYP tChiptyp;
	//const char *pcChiptypName;


	tChiptyp = ROMLOADER_CHIPTYP_UNKNOWN;

	// read the reset vector at 0x00000000
	ulResetVector = ptFn->read_data32(0);
	printf("%s(%p): reset vector: 0x%08X\n", m_pcName, this, ulResetVector);

	// match the reset vector to all known chipfamilies
	ptRstCnt = atResIds;
	ptRstEnd = ptRstCnt + (sizeof(atResIds)/sizeof(atResIds[0]));
	ulVersionAddr = 0xffffffff;
	while( ptRstCnt<ptRstEnd )
	{
		if( ptRstCnt->ulResetVector==ulResetVector )
		{
			ulVersionAddr = ptRstCnt->ulVersionAddress;
			// read version address
			ulVersion = ptFn->read_data32(ulVersionAddr);
			printf("%s(%p): version value: 0x%08X\n", m_pcName, this, ulVersion);
			if( ptRstCnt->ulVersionValue==ulVersion )
			{
				// found chip!
				tChiptyp = ptRstCnt->tChiptyp;
				printf("%s(%p): found chip %s.\n", m_pcName, this, ptRstCnt->pcChiptypName);
				break;
			}
		}
		++ptRstCnt;
	}

	/* Found something? */
	fResult = ( tChiptyp!=ROMLOADER_CHIPTYP_UNKNOWN );

	if( fResult==true )
	{
		/* Accept new chiptype and romcode. */
		m_tChiptyp = tChiptyp;
//
//		pcChiptypName = GetChiptypName(tChiptyp);
//		printf("%s(%p): found chip %s.\n", m_pcName, this, pcChiptypName);
	}

	return fResult;
}


const romloader::ROMLOADER_RESET_ID_T romloader::atResIds[8] =
{
	{
		0xea080001,
		0x00200008,
		0x00001000,
		ROMLOADER_CHIPTYP_NETX500,
		"netX500"
	},

	{
		0xea080002,
		0x00200008,
		0x00003002,
		ROMLOADER_CHIPTYP_NETX100,
		"netX100"
	},

	{
		0xeac83ffc,
		0x08200008,
		0x00002001,
		ROMLOADER_CHIPTYP_NETX50,
		"netX50"
	},

	{
		0xeafdfffa,
		0x08070008,
		0x00005003,
		ROMLOADER_CHIPTYP_NETX10,
		"netX10"
	},

	{
		0xeafbfffa,
		0x080f0008,
		0x00006003,
		ROMLOADER_CHIPTYP_NETX56,
		"netX51/52 Step A"
	},

	{
		0xeafbfffa,
		0x080f0008,
		0x00106003,
		ROMLOADER_CHIPTYP_NETX56B,
		"netX51/52 Step B"
	},

	{
		0xe59ff00c,
		0x04100020,
		0x00108004,
		ROMLOADER_CHIPTYP_NETX4000RELAXED,
		"netX4000"
	},

	{
		0x2009fff0,
		0x00005110,
		0x1f13933b,
		ROMLOADER_CHIPTYP_NETX90MPW,
		"netX90MPW"
	}
};


unsigned int romloader::crc16(uint16_t usCrc, uint8_t ucData)
{
	usCrc  = (usCrc >> 8) | ((usCrc & 0xff) << 8);
	usCrc ^= ucData;
	usCrc ^= (usCrc & 0xff) >> 4;
	usCrc ^= (usCrc & 0x0f) << 12;
	usCrc ^= ((usCrc & 0xff) << 4) << 1;

	return usCrc;
}


bool romloader::callback_long(SWIGLUA_REF *ptLuaFn, long lProgressData, long lCallbackUserData)
{
	bool fStillRunning;
	int iOldTopOfStack;


	// default value
	fStillRunning = false;

	// check lua state and callback tag
	if( ptLuaFn->L!=NULL && ptLuaFn->ref!=LUA_NOREF && ptLuaFn->ref!=LUA_REFNIL )
	{
		// get the current stack position
		iOldTopOfStack = lua_gettop(ptLuaFn->L);
		lua_rawgeti(ptLuaFn->L, LUA_REGISTRYINDEX, ptLuaFn->ref);
		// push the arguments on the stack
		lua_pushnumber(ptLuaFn->L, lProgressData);
		fStillRunning = callback_common(ptLuaFn, lCallbackUserData, iOldTopOfStack);
	}

	return fStillRunning;
}


bool romloader::callback_string(SWIGLUA_REF *ptLuaFn, const char *pcProgressData, size_t sizProgressData, long lCallbackUserData)
{
	bool fStillRunning;
	int iOldTopOfStack;


	// default value
	fStillRunning = false;

	// check lua state and callback tag
	if( ptLuaFn->L!=NULL && ptLuaFn->ref!=LUA_NOREF && ptLuaFn->ref!=LUA_REFNIL )
	{
		// get the current stack position
		iOldTopOfStack = lua_gettop(ptLuaFn->L);
		lua_rawgeti(ptLuaFn->L, LUA_REGISTRYINDEX, ptLuaFn->ref);
		// push the arguments on the stack
		lua_pushlstring(ptLuaFn->L, pcProgressData, sizProgressData);
		fStillRunning = callback_common(ptLuaFn, lCallbackUserData, iOldTopOfStack);
	}

	return fStillRunning;
}


bool romloader::callback_common(SWIGLUA_REF *ptLuaFn, long lCallbackUserData, int iOldTopOfStack)
{
	bool fStillRunning;
	int iResult;
	double dResult;
	int iLuaType;
	const char *pcErrMsg;
	const char *pcErrDetails;


	// check lua state and callback tag
	if( ptLuaFn->L!=NULL && ptLuaFn->ref!=LUA_NOREF && ptLuaFn->ref!=LUA_REFNIL )
	{
		lua_pushnumber(ptLuaFn->L, lCallbackUserData);
		// call the function
		iResult = lua_pcall(ptLuaFn->L, 2, 1, 0);
		if( iResult!=0 )
		{
			switch( iResult )
			{
			case LUA_ERRRUN:
				pcErrMsg = "runtime error";
				break;
			case LUA_ERRMEM:
				pcErrMsg = "memory allocation error";
				break;
			default:
				pcErrMsg = "unknown errorcode";
				break;
			}
			pcErrDetails = lua_tostring(ptLuaFn->L, -1);
			MUHKUH_PLUGIN_ERROR(ptLuaFn->L, "callback function failed: %s (%d): %s", pcErrMsg, iResult, pcErrDetails);
			fStillRunning = false;
		}
		else
		{
			// get the function's return value
			iLuaType = lua_type(ptLuaFn->L, -1);
			if( iLuaType!=LUA_TNUMBER && iLuaType!=LUA_TBOOLEAN )
			{
				MUHKUH_PLUGIN_ERROR(ptLuaFn->L, "callback function returned a non-boolean type: %d", iLuaType);
				fStillRunning = false;
			}
			else
			{
				if( iLuaType==LUA_TNUMBER )
				{
					dResult = lua_tonumber(ptLuaFn->L, -1);
					fStillRunning = (dResult!=0);
				}
				else
				{
					iResult = lua_toboolean(ptLuaFn->L, -1);
					fStillRunning = (iResult!=0);
				}
			}
		}
		// return old stack top
		lua_settop(ptLuaFn->L, iOldTopOfStack);
	}
	else
	{
		// no callback function -> keep running
		fStillRunning = true;
	}

	return fStillRunning;
}


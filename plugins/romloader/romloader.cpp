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
 , m_tRomcode(ROMLOADER_ROMCODE_UNKNOWN)
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


ROMLOADER_ROMCODE romloader::GetRomcode(void) const
{
	return m_tRomcode;
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




const char *romloader::GetRomcodeName(ROMLOADER_ROMCODE tRomcode) const
{
	const char *pcRomcode;
	const ROMLOADER_RESET_ID_T *ptCnt, *ptEnd;


	// init romcode name with unknown name
	pcRomcode = "unknown romcode";

	// loop over all known romcodes and search the romcode typ
	ptCnt = atResIds;
	ptEnd = ptCnt + (sizeof(atResIds)/sizeof(atResIds[0]));
	while( ptCnt<ptEnd )
	{
		if( ptCnt->tRomcode==tRomcode )
		{
			pcRomcode = ptCnt->pcRomcodeName;
			break;
		}
		++ptCnt;
	}

	return pcRomcode;
}

// wrapper functions for compatibility with old function names
ROMLOADER_CHIPTYP romloader::get_chiptyp(void) const						{return GetChiptyp();}
ROMLOADER_ROMCODE romloader::get_romcode(void) const						{return GetRomcode();}
const char	*romloader::get_chiptyp_name(ROMLOADER_CHIPTYP tChiptyp) const	{return GetChiptypName(tChiptyp);}
const char *romloader::get_romcode_name(ROMLOADER_ROMCODE tRomcode) const	{return GetRomcodeName(tRomcode);}

bool romloader::detect_chiptyp(lua_State *ptClientData)
{
	unsigned long ulResetVector;
	const ROMLOADER_RESET_ID_T *ptRstCnt, *ptRstEnd;
	unsigned long ulVersionAddr;
	unsigned long ulVersion;
	bool fResult;
	ROMLOADER_CHIPTYP tChiptyp;
	ROMLOADER_ROMCODE tRomcode;
	const char *pcChiptypName;
	const char *pcRomcodeName;


	tChiptyp = ROMLOADER_CHIPTYP_UNKNOWN;
	tRomcode = ROMLOADER_ROMCODE_UNKNOWN;

	// read the reset vector at 0x00000000
	ulResetVector = read_data32(ptClientData, 0);
	printf("%s(%p): reset vector: 0x%08lX\n", m_pcName, this, ulResetVector);

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
			ulVersion = read_data32(ptClientData, ulVersionAddr);
			printf("%s(%p): version value: 0x%08lX\n", m_pcName, this, ulVersion);
			if( ptRstCnt->ulVersionValue==ulVersion )
			{
				// found chip!
				tChiptyp = ptRstCnt->tChiptyp;
				tRomcode = ptRstCnt->tRomcode;
				break;
			}
		}
		++ptRstCnt;
	}

	// found something?
	fResult = ( tChiptyp!=ROMLOADER_CHIPTYP_UNKNOWN && tRomcode!=ROMLOADER_ROMCODE_UNKNOWN );

	if( fResult!=true )
	{
		MUHKUH_PLUGIN_ERROR(ptClientData, "failed to detect the type of the connected chip!");
	}
	else
	{
		/* Accept new chiptype and romcode. */
		m_tChiptyp = tChiptyp;
		m_tRomcode = tRomcode;

		pcChiptypName = GetChiptypName(tChiptyp);
		pcRomcodeName = GetRomcodeName(tRomcode);
		printf("%s(%p): found chip %s with romcode %s\n", m_pcName, this, pcChiptypName, pcRomcodeName);
	}

	return fResult;
}


const romloader::ROMLOADER_RESET_ID_T romloader::atResIds[5] =
{
	{
		0xea080001,
		0x00200008,
		0x00001000,
		ROMLOADER_CHIPTYP_NETX500,
		"netX500",
		ROMLOADER_ROMCODE_ABOOT,
		"ABoot"
	},

	{
		0xea080002,
		0x00200008,
		0x00003002,
		ROMLOADER_CHIPTYP_NETX100,
		"netX100",
		ROMLOADER_ROMCODE_ABOOT,
		"ABoot"
	},

	{
		0xeac83ffc,
		0x08200008,
		0x00002001,
		ROMLOADER_CHIPTYP_NETX50,
		"netX50",
		ROMLOADER_ROMCODE_HBOOT,
		"HBoot"
	},

	{
		0xeafdfffa,
		0x08070008,
		0x00005003,
		ROMLOADER_CHIPTYP_NETX10,
		"netX10",
		ROMLOADER_ROMCODE_HBOOT,
		"HBoot"
	},

	{
		0xeafbfffa,
		0x080f0008,
		0x00006003,
		ROMLOADER_CHIPTYP_NETX56,
		"netX56",
		ROMLOADER_ROMCODE_HBOOT2,
		"HBoot"
	}

};


unsigned int romloader::crc16(unsigned short usCrc, unsigned char ucData)
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


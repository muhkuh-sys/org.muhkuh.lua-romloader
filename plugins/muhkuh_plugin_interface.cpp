/***************************************************************************
 *   Copyright (C) 2009 by Christoph Thelen                                *
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

#include "muhkuh_plugin_interface.h"


/*-----------------------------------*/


#if !defined(LUA_VERSION_NUM) || LUA_VERSION_NUM < 501
#       define WRAPPER_LUA_RAWLEN lua_strlen
#elif LUA_VERSION_NUM == 501
#       define WRAPPER_LUA_RAWLEN lua_objlen
#else
#       define WRAPPER_LUA_RAWLEN lua_rawlen
#endif



muhkuh_plugin::muhkuh_plugin(const char *pcName, const char *pcTyp, muhkuh_plugin_provider *ptProvider)
 : m_pcName(NULL)
 , m_pcTyp(NULL)
 , m_pcLocation(NULL)
 , m_ptProvider(ptProvider)
 , m_fIsConnected(false)
{
	// clone name and typ
	m_pcName = clone_string(pcName, SIZ_MAX_MUHKUH_PLUGIN_STRING);
	m_pcTyp = clone_string(pcTyp, SIZ_MAX_MUHKUH_PLUGIN_STRING);

	printf("%s(%p): created\n", m_pcName, this);
}


muhkuh_plugin::muhkuh_plugin(const char *pcName, const char *pcTyp, const char *pcLocation, muhkuh_plugin_provider *ptProvider)
 : m_pcName(NULL)
 , m_pcTyp(NULL)
 , m_pcLocation(NULL)
 , m_ptProvider(ptProvider)
 , m_fIsConnected(false)
{
	/* Clone name, typ and location. */
	m_pcName = clone_string(pcName, SIZ_MAX_MUHKUH_PLUGIN_STRING);
	m_pcTyp = clone_string(pcTyp, SIZ_MAX_MUHKUH_PLUGIN_STRING);
	m_pcLocation = clone_string(pcLocation, SIZ_MAX_MUHKUH_PLUGIN_STRING);
	
	printf("%s(%p): created\n", m_pcName, this);
}


muhkuh_plugin::~muhkuh_plugin(void)
{
	bool fOk;


	if( m_ptProvider!=NULL )
	{
		fOk = m_ptProvider->ReleaseInterface(this);
		if( fOk!=true )
		{
			printf("%s(%p): failed to release interface!\n", m_pcName, this);
		}
	}

	printf("%s(%p): deleted\n", m_pcName, this);

	if( m_pcName!=NULL )
	{
		delete[] m_pcName;
	}
	if( m_pcTyp!=NULL )
	{
		delete[] m_pcTyp;
	}
	delete[] m_pcLocation;
}


// returns the connection state of the device
bool muhkuh_plugin::IsConnected(void) const
{
	printf("%s(%p): is_connected() = %s\n", m_pcName, this, (m_fIsConnected?"true":"false"));

	return m_fIsConnected;
}


// returns the device name
const char *muhkuh_plugin::GetName(void) const
{
	return m_pcName;
}


// returns the device typ
const char *muhkuh_plugin::GetTyp(void) const
{
	return m_pcTyp;
}


/* Returns the USB Location ID (only WinUSB). */
const char *muhkuh_plugin::GetLocation(void) const
{
	return m_pcLocation;
}


// wrapper functions for compatibility with old function names
void		muhkuh_plugin::connect(lua_State *ptClientData)		{return Connect(ptClientData);}
void		muhkuh_plugin::disconnect(lua_State *ptClientData)	{return Disconnect(ptClientData);}
bool		muhkuh_plugin::is_connected(void) const				{return IsConnected();}
const char *muhkuh_plugin::get_name(void) const					{return GetName();}
const char *muhkuh_plugin::get_typ(void) const					{return GetTyp();}



char *muhkuh_plugin::clone_string(const char *pcStr, size_t sizMax)
{
	char *pcClone;
	size_t sizStr;


	pcClone = NULL;
	if( pcStr!=NULL )
	{
		sizStr = strlen(pcStr);
		if( sizStr>sizMax-1 )
		{
			sizStr = sizMax-1;
		}
		pcClone = new char[sizStr+1];
		memcpy(pcClone, pcStr, sizStr);
		pcClone[sizStr] = 0;
	}

	return pcClone;
}


/*-----------------------------------*/


muhkuh_plugin_reference::muhkuh_plugin_reference(void)
 : m_fIsValid(false)
 , m_pcName(NULL)
 , m_pcTyp(NULL)
 , m_pcLocation(NULL)
 , m_ptProvider(NULL)
 , m_fIsUsed(false)
{
}


muhkuh_plugin_reference::muhkuh_plugin_reference(const char *pcName, const char *pcTyp, const char *pcLocation, bool fIsUsed, muhkuh_plugin_provider *ptProvider)
 : m_fIsValid(true)
 , m_pcName(NULL)
 , m_pcTyp(NULL)
 , m_pcLocation(NULL)
 , m_ptProvider(ptProvider)
 , m_fIsUsed(fIsUsed)
{
	m_pcName = clone_string(pcName, SIZ_MAX_MUHKUH_PLUGIN_STRING);
	m_pcTyp = clone_string(pcTyp, SIZ_MAX_MUHKUH_PLUGIN_STRING);
	m_pcLocation = clone_string(pcLocation, SIZ_MAX_MUHKUH_PLUGIN_STRING);
}


muhkuh_plugin_reference::muhkuh_plugin_reference(const char *pcName, const char *pcTyp, bool fIsUsed, muhkuh_plugin_provider *ptProvider)
 : m_fIsValid(true)
 , m_pcName(NULL)
 , m_pcTyp(NULL)
 , m_pcLocation(NULL)
 , m_ptProvider(ptProvider)
 , m_fIsUsed(fIsUsed)
{
	m_pcName = clone_string(pcName, SIZ_MAX_MUHKUH_PLUGIN_STRING);
	m_pcTyp = clone_string(pcTyp, SIZ_MAX_MUHKUH_PLUGIN_STRING);
}


muhkuh_plugin_reference::muhkuh_plugin_reference(const muhkuh_plugin_reference *ptCloneMe)
 : m_fIsValid(true)
 , m_pcName(NULL)
 , m_pcTyp(NULL)
 , m_pcLocation(NULL)
 , m_ptProvider(ptCloneMe->m_ptProvider)
 , m_fIsUsed(ptCloneMe->m_fIsUsed)
{
	m_pcName = clone_string(ptCloneMe->m_pcName, SIZ_MAX_MUHKUH_PLUGIN_STRING);
	m_pcTyp = clone_string(ptCloneMe->m_pcTyp, SIZ_MAX_MUHKUH_PLUGIN_STRING);
	m_pcLocation = clone_string(ptCloneMe->m_pcLocation, SIZ_MAX_MUHKUH_PLUGIN_STRING);
}


muhkuh_plugin_reference::~muhkuh_plugin_reference(void)
{
	delete[] m_pcName;
	delete[] m_pcTyp;
	delete[] m_pcLocation;
}


bool muhkuh_plugin_reference::IsValid(void) const
{
	return m_fIsValid;
}


const char *muhkuh_plugin_reference::GetName(void) const
{
	return m_pcName;
}


const char *muhkuh_plugin_reference::GetTyp(void) const
{
	return m_pcTyp;
}


const char *muhkuh_plugin_reference::GetLocation(void) const
{
	return m_pcLocation;
}


bool muhkuh_plugin_reference::IsUsed(void) const
{
	return m_fIsUsed;
}


muhkuh_plugin *muhkuh_plugin_reference::Create(void) const
{
	muhkuh_plugin *ptPlugin;


	ptPlugin = NULL;

	if( m_ptProvider!=NULL )
	{
		ptPlugin = m_ptProvider->ClaimInterface(this);
	}

	return ptPlugin;
}


swig_type_info *muhkuh_plugin_reference::GetTypeInfo(void) const
{
	swig_type_info *ptTypeInfo;


	ptTypeInfo = NULL;

	if( m_ptProvider!=NULL )
	{
		ptTypeInfo = m_ptProvider->GetTypeInfo();
	}

	return ptTypeInfo;
}


char *muhkuh_plugin_reference::clone_string(const char *pcStr, size_t sizMax)
{
	char *pcClone;
	size_t sizStr;


	pcClone = NULL;
	if( pcStr!=NULL )
	{
		sizStr = strlen(pcStr);
		if( sizStr>sizMax-1 )
		{
			sizStr = sizMax-1;
		}
		pcClone = new char[sizStr+1];
		memcpy(pcClone, pcStr, sizStr);
		pcClone[sizStr] = 0;
	}

	return pcClone;
}


/*-----------------------------------*/


muhkuh_plugin_provider::muhkuh_plugin_provider(const char *pcPluginId)
{
	// init the plugin description with default values
	m_pcPluginName = NULL;
	m_pcPluginId = NULL;
	m_tVersion.uiVersionMajor = 0;
	m_tVersion.uiVersionMinor = 0;
	m_tVersion.uiVersionSub = 0;

	// copy the id
	m_pcPluginId = clone_string(pcPluginId, SIZ_MAX_MUHKUH_PLUGIN_STRING);

	/* init the lua type info */
	m_ptPluginTypeInfo = NULL;
}


muhkuh_plugin_provider::~muhkuh_plugin_provider(void)
{
	if( m_pcPluginName!=NULL )
	{
		delete[] m_pcPluginName;
	}
	if( m_pcPluginId!=NULL )
	{
		delete[] m_pcPluginId;
	}
}


const char *muhkuh_plugin_provider::GetName(void) const
{
	return m_pcPluginName;
}


const char *muhkuh_plugin_provider::GetID(void) const
{
	return m_pcPluginId;
}


const muhkuh_plugin_version *muhkuh_plugin_provider::GetVersion(void) const
{
	return &m_tVersion;
}


swig_type_info *muhkuh_plugin_provider::GetTypeInfo(void) const
{
	return m_ptPluginTypeInfo;
}


char *muhkuh_plugin_provider::clone_string(const char *pcStr, size_t sizMax)
{
	char *pcClone;
	size_t sizStr;


	pcClone = NULL;
	if( pcStr!=NULL )
	{
		sizStr = strlen(pcStr);
		if( sizStr>sizMax-1 )
		{
			sizStr = sizMax-1;
		}
		pcClone = new char[sizStr+1];
		memcpy(pcClone, pcStr, sizStr);
		pcClone[sizStr] = 0;
	}

	return pcClone;
}


void muhkuh_plugin_provider::add_reference_to_table(lua_State *ptLuaState, muhkuh_plugin_reference *ptReference)
{
	size_t sizTable;


	/* get the size of the table */
	/* NOTE: lua_rawlen is new in LUA5.2, but swig provides a
	 * compatibility wrapper in swigluarun.h .
	 */
	sizTable = WRAPPER_LUA_RAWLEN(ptLuaState, 2);
	/* create a new pointer object with the special type */
	SWIG_NewPointerObj(ptLuaState, ptReference, m_ptReferenceTypeInfo, 1);
	/* add the pointer object to the table */
	lua_rawseti(ptLuaState, 2, sizTable+1);
}


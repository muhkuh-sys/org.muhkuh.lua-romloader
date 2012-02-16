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


#include <string.h>

#ifndef __MUHKUH_PLUGIN_INTERFACE__
#define __MUHKUH_PLUGIN_INTERFACE__

/*-----------------------------------*/

#if defined(_WINDOWS)
#       define MUHKUH_EXPORT __declspec(dllexport)
#else
#       define MUHKUH_EXPORT
#endif

/*-----------------------------------*/

#ifndef SWIGRUNTIME
#include <swigluarun.h>

/* swigluarun does not include the lua specific defines. Add them here. */
typedef struct
{
	lua_State* L; /* the state */
	int ref;      /* a ref in the lua global index */
} SWIGLUA_REF;
#endif

/*-----------------------------------*/

#define MUHKUH_PLUGIN_PUSH_ERROR(L,...) { lua_pushfstring(L,__VA_ARGS__); }
#define MUHKUH_PLUGIN_EXIT_ERROR(L) { lua_error(L); }
#define MUHKUH_PLUGIN_ERROR(L,...) { lua_pushfstring(L,__VA_ARGS__); lua_error(L); }

#define SIZ_MAX_MUHKUH_PLUGIN_STRING 256

/*-----------------------------------*/


class muhkuh_plugin_provider;
class muhkuh_plugin_reference;
class muhkuh_plugin;


/*-----------------------------------*/


typedef struct
{
	unsigned int uiVersionMajor;
	unsigned int uiVersionMinor;
	unsigned int uiVersionSub;
} muhkuh_plugin_version;


/*-----------------------------------*/


class MUHKUH_EXPORT muhkuh_plugin
{
public:
	muhkuh_plugin(const char *pcName, const char *pcTyp, muhkuh_plugin_provider *ptProvider);
	~muhkuh_plugin(void);

	// open the connection to the device
	virtual void Connect(lua_State *ptClientData) = 0;
	// close the connection to the device
	virtual void Disconnect(lua_State *ptClientData) = 0;
	// returns the connection state of the device
	virtual bool IsConnected(void) const;

	// returns the device name
	virtual const char *GetName(void) const;
	// returns the device typ
	virtual const char *GetTyp(void) const;

	// wrapper functions for compatibility with old function names
	virtual void connect(lua_State *ptClientData);
	virtual void disconnect(lua_State *ptClientData);
	virtual bool is_connected(void) const;
	virtual const char *get_name(void) const;
	virtual const char *get_typ(void) const;


protected:
	char *clone_string(const char *pcStr, size_t sizMax);


	char *m_pcName;
	char *m_pcTyp;

	muhkuh_plugin_provider *m_ptProvider;

	bool m_fIsConnected;
};


/*-----------------------------------*/


class MUHKUH_EXPORT muhkuh_plugin_provider
{
public:
	muhkuh_plugin_provider(const char *pcPluginId);
	~muhkuh_plugin_provider(void);

	const char *GetName(void) const;
	const char *GetID(void) const;
	const muhkuh_plugin_version *GetVersion(void) const;
	swig_type_info *GetTypeInfo(void) const;

	virtual int DetectInterfaces(lua_State *ptLuaStateForTableAccess) = 0;
	virtual muhkuh_plugin *ClaimInterface(const muhkuh_plugin_reference *ptReference) = 0;
	virtual bool ReleaseInterface(muhkuh_plugin *ptPlugin) = 0;

protected:
	char *clone_string(const char *pcStr, size_t sizMax);

	void add_reference_to_table(lua_State *ptLuaState, muhkuh_plugin_reference *ptReference);

	const char *m_pcPluginName;
	const char *m_pcPluginId;
	muhkuh_plugin_version m_tVersion;
	swig_type_info *m_ptPluginTypeInfo;
	swig_type_info *m_ptReferenceTypeInfo;
};


/*-----------------------------------*/


class MUHKUH_EXPORT muhkuh_plugin_reference
{
public:
	muhkuh_plugin_reference(void);
	muhkuh_plugin_reference(const char *pcName, const char *pcTyp, bool fIsUsed, muhkuh_plugin_provider *ptProvider);
	muhkuh_plugin_reference(const muhkuh_plugin_reference *ptCloneMe);

	bool IsValid(void) const;
	const char *GetName(void) const;
	const char *GetTyp(void) const;
	bool IsUsed(void) const;

	muhkuh_plugin *Create(void) const;
	swig_type_info *GetTypeInfo(void) const;

protected:
	bool m_fIsValid;
	char *m_pcName;
	char *m_pcTyp;
	muhkuh_plugin_provider *m_ptProvider;
	bool m_fIsUsed;

private:
	char *clone_string(const char *pcStr, size_t sizMax);
};


/*-----------------------------------*/


#endif  /* __MUHKUH_PLUGIN_INTERFACE__ */


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


#include <vector>

#include <wx/wx.h>
#include <wx/dynlib.h>

#include <wxlua/include/wxlua.h>
#include <wxluasocket/include/wxldserv.h>
#include <plugins/_luaif/muhkuh_wxlua_bindings.h>
extern "C"
{
	#include <lualib.h>
}

#ifndef __MUHKUH_PLUGIN_INTERFACE__
#define __MUHKUH_PLUGIN_INTERFACE__

/*-----------------------------------*/

class muhkuh_plugin_instance : public wxObject
{
public:
	muhkuh_plugin_instance(void)
	 : fIsValid(false)
	 , strName(wxEmptyString)
	 , strTyp(wxEmptyString)
	 , fIsUsed(false)
	 , strLuaCreateFn(wxEmptyString)
	 , pvHandle(NULL)
	{
	}

	muhkuh_plugin_instance(wxString _strName, wxString _strTyp, bool _fIsUsed, wxString _strLuaCreateFn, void *_pvHandle)
	 : fIsValid(true)
	 , strName(_strName)
	 , strTyp(_strTyp)
	 , fIsUsed(_fIsUsed)
	 , strLuaCreateFn(_strLuaCreateFn)
	 , pvHandle(_pvHandle)
	{
	}

	muhkuh_plugin_instance(const muhkuh_plugin_instance *ptCloneMe)
	 : fIsValid(true)
	 , strName(ptCloneMe->strName)
	 , strTyp(ptCloneMe->strTyp)
	 , fIsUsed(ptCloneMe->fIsUsed)
	 , strLuaCreateFn(ptCloneMe->strLuaCreateFn)
	 , pvHandle(ptCloneMe->pvHandle)
	{
	}

	bool IsValid(void)
	{
		return fIsValid;
	}

	wxString GetName(void)
	{
		return strName;
	}

	wxString GetTyp(void)
	{
		return strTyp;
	}

	bool IsUsed(void)
	{
		return fIsUsed;
	}

	wxString GetLuaCreateFn(void)
	{
		return strLuaCreateFn;
	}

	void *GetHandle(void)
	{
		return pvHandle;
	}

	bool fIsValid;
	wxString strName;
	wxString strTyp;
	bool fIsUsed;
	wxString strLuaCreateFn;
	void *pvHandle;
};

/*-----------------------------------*/

typedef struct
{
	unsigned int	uiVersionMajor;
	unsigned int	uiVersionMinor;
	unsigned int	uiVersionSub;
} muhkuh_plugin_version;

typedef struct
{
	const char		*pcPluginName;
	const char		*pcPluginId;
	muhkuh_plugin_version	tVersion;
} muhkuh_plugin_desc;

/*-----------------------------------*/

/*
  This is the required plugin interface which has to be present in every
  plugin. All function symbols are resolved when the plugin is opened.
*/
typedef int (*muhkuh_plugin_fn_init)(wxLog *ptLogTarget);
typedef int (*muhkuh_plugin_fn_init_lua)(wxLuaState *ptLuaState);
typedef int (*muhkuh_plugin_fn_leave)(void);
typedef const muhkuh_plugin_desc *(*muhkuh_plugin_fn_get_desc)(void);
typedef int (*muhkuh_plugin_fn_detect_interfaces)(std::vector<muhkuh_plugin_instance*> *pvInterfaceList);

/*
  NOTE: The function 'muhkuh_plugin_fn_close_instance' is not part of the
  required plugin interface. The function's symbol is not resolved in the
  plugin open function. It is a suggestion how to close a plugin instance.

  The idea is to set this function pointer in the plugin instance class at
  the creation time. It points to the plugin's internal 'close_instance'
  function and is called when the plugin instance class is destroyed.
  Please take a look at the 'romloader_baka' plugin for an example.
*/
typedef void (*muhkuh_plugin_fn_close_instance)(void *pvHandle);

/*-----------------------------------*/

typedef struct
{
	muhkuh_plugin_fn_init			fn_init;
	muhkuh_plugin_fn_init_lua		fn_init_lua;
	muhkuh_plugin_fn_leave			fn_leave;
	muhkuh_plugin_fn_get_desc		fn_get_desc;
	muhkuh_plugin_fn_detect_interfaces	fn_detect_interfaces;
} muhkuh_plugin_function_interface_t;

typedef struct
{
	union
	{
		muhkuh_plugin_function_interface_t tFn;
		void *apv[sizeof(muhkuh_plugin_function_interface_t)/sizeof(void*)];
	} uSym;
	wxDllType tHandle;
} muhkuh_plugin_interface;

/*-----------------------------------*/

#endif	/* __MUHKUH_PLUGIN_INTERFACE__ */


// ---------------------------------------------------------------------------
// muhkuh_lua.h - headers and tags for wxLua binding
//
// This file was generated by genwxbind.lua 
// Any changes made to this file may be lost when file is regenerated
// ---------------------------------------------------------------------------

#ifndef __HOOK_WXLUA_muhkuh_lua_H__
#define __HOOK_WXLUA_muhkuh_lua_H__


#include "wxlua/include/wxlstate.h"
#include "wxlua/include/wxlbind.h"

// ---------------------------------------------------------------------------
// Check if the version of binding generator used to create this is older than
//   the current version of the bindings.
//   See 'bindings/genwxbind.lua' and 'modules/wxlua/include/wxldefs.h'
#if WXLUA_BINDING_VERSION > 17
#   error "The WXLUA_BINDING_VERSION in the bindings is too old, regenerate bindings."
#endif //WXLUA_BINDING_VERSION > 17
// ---------------------------------------------------------------------------

// binding class
class wxLuaBinding_muhkuh_lua : public wxLuaBinding
{
public:
    wxLuaBinding_muhkuh_lua();
    virtual wxLuaBinding* Clone() const { return new wxLuaBinding_muhkuh_lua; }

protected:
    virtual void PreRegister(const wxLuaState& wxlState, bool registerTypes, int luaTable);
    virtual void PostRegister(const wxLuaState& wxlState, bool registerTypes, int luaTable);

    DECLARE_DYNAMIC_CLASS(wxLuaBinding_muhkuh_lua)
};


// bind wxLuaBinding_muhkuh_lua to a single wxLuaState
extern WXLUA_NO_DLLIMPEXP bool wxLuaBinding_muhkuh_lua_bind(const wxLuaState& wxlState);
// initialize wxLuaBinding_muhkuh_lua for all wxLuaStates
extern WXLUA_NO_DLLIMPEXP bool wxLuaBinding_muhkuh_lua_init();

// ---------------------------------------------------------------------------
// Includes
// ---------------------------------------------------------------------------

#include "application/muhkuh_mainFrame.h"


// ---------------------------------------------------------------------------
// Lua Tag Method Values and Tables for each Class
// ---------------------------------------------------------------------------

extern WXLUA_NO_DLLIMPEXP_DATA(int) s_wxluatag_muhkuh_binfile;
extern WXLUA_NO_DLLIMPEXP wxLuaBindMethod muhkuh_binfile_methods[];
extern WXLUA_NO_DLLIMPEXP_DATA(int) muhkuh_binfile_methodCount;
extern WXLUA_NO_DLLIMPEXP_DATA(int) s_wxluatag_muhkuh_plugin_instance;
extern WXLUA_NO_DLLIMPEXP wxLuaBindMethod muhkuh_plugin_instance_methods[];
extern WXLUA_NO_DLLIMPEXP_DATA(int) muhkuh_plugin_instance_methodCount;



// ---------------------------------------------------------------------------
// Encapsulation Declarations - need to be public for other bindings.
// ---------------------------------------------------------------------------


#endif // __HOOK_WXLUA_muhkuh_lua_H__


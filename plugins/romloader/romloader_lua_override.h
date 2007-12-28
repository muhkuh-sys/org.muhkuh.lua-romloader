
%override wxLua_romloader_read_image
static int LUACALL wxLua_romloader_read_image(lua_State *L)
{
    int iLuaCallbackTag;
    wxLuaState wxlState(L);
    wxString returns;
    // voidptr_long vplCallbackUserData
    long vplCallbackUserData = (long)wxlua_getnumbertype(L, 5);
    // LuaFunction fnCallback
    if( lua_isfunction(L, 4) )
    {
        // push function to top of stack
        lua_pushvalue(L, 4);
        // ref function and pop it from stack
        iLuaCallbackTag = luaL_ref(L, LUA_REGISTRYINDEX);
    }
    else
    {
        // no callback function provided
        wxlState.wxlua_Error(wxString::Format(_("wxLua: Expected lua function(long item1, long item2, long data) for parameter %d, but got '%s'."),
                                        4, wxlState.lua_TypeNameIndex(4).c_str()));
    }
    // unsigned long ulSize
    unsigned long ulSize = (long)wxlua_getnumbertype(L, 3);
    // unsigned long ulNetxAddress
    unsigned long ulNetxAddress = (long)wxlua_getnumbertype(L, 2);
    // get this
    romloader * self = (romloader *)wxlState.GetUserDataType(1, s_wxluatag_romloader);
    // call read_image
    returns = (self->read_image(ulNetxAddress, ulSize, L, iLuaCallbackTag, (void*)vplCallbackUserData));

    // remove ref to function
    luaL_unref(L, LUA_REGISTRYINDEX, iLuaCallbackTag);

    // push the result string
    wxlState.lua_PushLString(returns,returns.Len());

    return 1;
}
%end


%override wxLua_romloader_write_image
static int LUACALL wxLua_romloader_write_image(lua_State *L)
{
    int iLuaCallbackTag;
    wxLuaState wxlState(L);
    // voidptr_long vplCallbackUserData
    long vplCallbackUserData = (long)wxlua_getnumbertype(L, 5);
    // LuaFunction fnCallback
    if( lua_isfunction(L, 4) )
    {
        // push function to top of stack
        lua_pushvalue(L, 4);
        // ref function and pop it from stack
        iLuaCallbackTag = luaL_ref(L, LUA_REGISTRYINDEX);
    }
    else
    {
        // no callback function provided
        wxlState.wxlua_Error(wxString::Format(_("wxLua: Expected lua function(long item1, long item2, long data) for parameter %d, but got '%s'."),
                                        4, wxlState.lua_TypeNameIndex(4).c_str()));
    }
    // wxString strData
    wxString strData;
    size_t sizLen;
    const char *pcBuf;
    pcBuf = lua_tolstring(L, 3, &sizLen);
    if( sizLen==0 )
    {
        strData = wxEmptyString;
    }
    else
    {
        strData = wxString::From8BitData(pcBuf, sizLen);
    }
    // unsigned long ulNetxAddress
    unsigned long ulNetxAddress = (long)wxlua_getnumbertype(L, 2);
    // get this
    romloader * self = (romloader *)wxlState.GetUserDataType(1, s_wxluatag_romloader);
    // call write_image
    self->write_image(ulNetxAddress, strData, L, iLuaCallbackTag, (void*)vplCallbackUserData);

    // remove ref to function
    luaL_unref(L, LUA_REGISTRYINDEX, iLuaCallbackTag);

    return 0;
}
%end


%override wxLua_romloader_call
static int LUACALL wxLua_romloader_call(lua_State *L)
{
    int iLuaCallbackTag;
    wxLuaState wxlState(L);
    // voidptr_long vplCallbackUserData
    long vplCallbackUserData = (long)wxlua_getnumbertype(L, 5);
    // LuaFunction fnCallback
    if( lua_isfunction(L, 4) )
    {
        // push function to top of stack
        lua_pushvalue(L, 4);
        // ref function and pop it from stack
        iLuaCallbackTag = luaL_ref(L, LUA_REGISTRYINDEX);
    }
    else
    {
        // no callback function provided
        wxlState.wxlua_Error(wxString::Format(_("wxLua: Expected lua function(long item1, long item2, long data) for parameter %d, but got '%s'."),
                                        4, wxlState.lua_TypeNameIndex(4).c_str()));
    }
    // unsigned long ulParameterR0
    unsigned long ulParameterR0 = (long)wxlua_getnumbertype(L, 3);
    // unsigned long ulNetxAddress
    unsigned long ulNetxAddress = (long)wxlua_getnumbertype(L, 2);
    // get this
    romloader * self = (romloader *)wxlState.GetUserDataType(1, s_wxluatag_romloader);
    // call call
    self->call(ulNetxAddress, ulParameterR0, L, iLuaCallbackTag, (void*)vplCallbackUserData);

    return 0;
}


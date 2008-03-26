
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
        wxlua_argerror(L, 4, wxT("a 'function'"));
    }
    // unsigned long ulSize
    double dSize = wxlua_getnumbertype(L, 3);
    // unsigned long ulNetxAddress
    double dNetxAddress = wxlua_getnumbertype(L, 2);
    // get this
    romloader * self = (romloader *)wxluaT_getuserdatatype(L, 1, wxluatype_romloader);
    // call read_image
    returns = (self->read_image(dNetxAddress, dSize, L, iLuaCallbackTag, (void*)vplCallbackUserData));

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
        wxlua_argerror(L, 4, wxT("a 'function'"));
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
    double dNetxAddress = wxlua_getnumbertype(L, 2);
    // get this
    romloader * self = (romloader *)wxluaT_getuserdatatype(L, 1, wxluatype_romloader);
    // call write_image
    self->write_image(dNetxAddress, strData, L, iLuaCallbackTag, (void*)vplCallbackUserData);

    // remove ref to function
    luaL_unref(L, LUA_REGISTRYINDEX, iLuaCallbackTag);

    return 0;
}
%end


%override wxLua_romloader_call
static int LUACALL wxLua_romloader_call(lua_State *L)
{
    int iLuaCallbackTag;
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
        wxlua_argerror(L, 4, wxT("a 'function'"));
    }
    // unsigned long ulParameterR0
    double dParameterR0 = wxlua_getnumbertype(L, 3);
    // unsigned long ulNetxAddress
    double dNetxAddress = wxlua_getnumbertype(L, 2);
    // get this
    romloader * self = (romloader *)wxluaT_getuserdatatype(L, 1, wxluatype_romloader);
    // call call
    self->call(dNetxAddress, dParameterR0, L, iLuaCallbackTag, (void*)vplCallbackUserData);

    // remove ref to function
    luaL_unref(L, LUA_REGISTRYINDEX, iLuaCallbackTag);

    return 0;
}


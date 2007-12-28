
%override wxLua_function_load
static int LUACALL wxLua_function_load(lua_State *L)
{
    wxLuaState wxlState(L);
    wxString returns;
    // wxString strFileName
    wxString strFileName = wxlState.GetwxStringType(1);
    // call load
    returns = (load(strFileName));
    // push the result string
    wxlState.lua_PushLString(returns, returns.Len());

    return 1;
}


%override wxLua_romloader_write_image
static int LUACALL wxLua_romloader_write_image(lua_State *L)
{
    wxLuaState wxlState(L);
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
    self->write_image(ulNetxAddress, strData);

    return 0;
}
%end

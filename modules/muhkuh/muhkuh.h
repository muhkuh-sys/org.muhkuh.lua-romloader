
#include <wx/wx.h>
#include <wx/url.h>

extern "C"
{
	#include "lua.h"
	#include "lauxlib.h"
	#include "lualib.h"
}


void muhkuh_internal_init(void);

const char *muhkuh_lua_GetUrlErrorString(wxURLError tUrlError);
const char *muhkuh_lua_GetLuaErrorString(int iLuaError);
const char *muhkuh_lua_GetLuaTypeString(int iLuaType);
void muhkuh_lua_GetErrorInfo(lua_State *L, int iStatus, int iTop, wxString *pstrErrorMsg, int *piLineNum);

void load(lua_State *ptLuaState, char *pcUrl, char **ppcBUFFER_OUT, size_t *psizBUFFER_OUT);
void include(lua_State *ptLuaState, char *pcUrl, char *pcChunkName);

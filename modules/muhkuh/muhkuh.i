%module muhkuh

%include muhkuh_typemaps.i

%{
	#include "muhkuh.h"
	#include "muhkuh_capture_std.h"
%}


%init %{
	muhkuh_internal_init();
%}


#ifdef SWIGLUA
/* This Lua code is executed on the first "require" operation for this
 * module. It adds some pure lua functions to the module.
 */
/*
%luacode
{
function muhkuh.TestHasFinished()
	-- the panel is the only available starting point
	if __MUHKUH_PANEL then
		-- move up until there is no more parent
		local tWin
		local tParent = __MUHKUH_PANEL
		repeat
			tWin = tParent
			tParent = tWin:GetParent()
		until not tParent
		-- this must be the root window, close it
		tWin:Close()
	end
end
}
*/
#endif

void load(lua_State *ptLuaState, char *pcUrl, char **ppcBUFFER_OUT, size_t *psizBUFFER_OUT);
void include(lua_State *ptLuaState, char *pcUrl, char *pcChunkName);

class capture_std
{
public:
	capture_std(long lMyId, long lEvtHandlerId);
	~capture_std(void);

	int run(const char *pcCommand, lua_State *ptLuaStateForTableAccess);
};


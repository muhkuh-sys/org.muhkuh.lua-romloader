%module muhkuh

%include muhkuh_typemaps.i

%{
	#include "muhkuh.h"
%}

void load(lua_State *ptLuaState, char *pcUrl, char **ppcBUFFER_OUT, size_t *psizBUFFER_OUT);
void include(lua_State *ptLuaState, char *pcUrl, char *pcChunkName);

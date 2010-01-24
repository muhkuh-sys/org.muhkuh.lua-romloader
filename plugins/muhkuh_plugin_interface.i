%module muhkuh_plugin_interface

/* This interface is Lua specific. It will definitely not work with other
 * scripting languages. Refuse to continue if the output is not Lua.
 */
#ifndef SWIGLUA
	#error "This module is Lua specific. It will not work with other scripting languages!"
#endif

%include "lua_fnptr.i"

/* Include the header file in the Lua wrapper.
 */
%header %{
	#include "muhkuh_plugin_interface.h"
%}

%include "muhkuh_typemaps.i"
%include "muhkuh_plugin_interface.h"

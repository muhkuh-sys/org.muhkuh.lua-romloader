%module romloader

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
	#include "romloader.h"
%}

%include "../muhkuh_typemaps.i"

/* import interfaces */
%import "../muhkuh_plugin_interface.i"

%include "romloader.h"

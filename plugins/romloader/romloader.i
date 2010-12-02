%module romloader

#ifdef SWIGLUA
%include "lua_fnptr.i"
#endif

/* Include the header file in the Lua wrapper.
 */
%header %{
	#include "romloader.h"
%}

%include muhkuh_typemaps.i

/* import interfaces */
%include "../muhkuh_plugin_interface.h"
%include "romloader.h"

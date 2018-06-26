%module romloader

%include <stdint.i>

#ifdef SWIGLUA
%include "lua_fnptr.i"
#endif

/* Include the header file in the Lua wrapper.
 */
%header %{
	#include "romloader.h"
%}

%include plugins/muhkuh_typemaps.i

/* import interfaces */
%include "../muhkuh_plugin_interface.h"
%include "romloader.h"
%include "romloader_def.h"

%include "version_information.i"

%module romloader_usb

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
	#include "romloader_usb_main.h"
%}

/* This Lua code is executed on the first "require" operation for this
 * module. It adds the plugin provider to the global list
 * "__MUHKUH_PLUGINS".
 */
%luacode
{
	if not _G.__MUHKUH_PLUGINS then
		_G.__MUHKUH_PLUGINS = {}
	end
	table.insert(_G.__MUHKUH_PLUGINS, romloader_usb.romloader_usb_provider())
}

%include "../../muhkuh_typemaps.i"

/* The plugin provider's functions "ClaimInterface" and "ReleaseInterface"
 * are only used from the C++ side. "ClaimInterface" is called from the
 * plugin reference's "Create" function and "ReleaseInterface" is called from
 * the plugin's destructor.
 */
%ignore romloader_usb_provider::ClaimInterface;
%ignore romloader_usb_provider::ReleaseInterface;

/* The plugin reference's function "Create" allocates a new plugin object on
 * the heap and passes it to the Lua interpreter. Now Lua "owns" the plugin
 * object. This means it must delete the object once it is garbage collected.
 */
%newobject romloader_usb_reference::Create;


/* TODO: split the interface in multiple files. */
%include "../../muhkuh_plugin_interface.h"
%include "../romloader.h"
%include "romloader_usb_main.h"

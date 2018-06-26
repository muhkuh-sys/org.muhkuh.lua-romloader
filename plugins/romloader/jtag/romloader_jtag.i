%module romloader_jtag

%include <stdint.i>

#ifdef SWIGLUA
%include "lua_fnptr.i"
#endif

/* Include the header file in the Lua wrapper.
 */
%header %{
	#include "romloader_jtag_main.h"
%}

#ifdef SWIGLUA
/* This Lua code is executed on the first "require" operation for this
 * module. It adds the plugin provider to the global list
 * "__MUHKUH_PLUGINS".
 */
%luacode
{
	require("romloader")
	if not _G.__MUHKUH_PLUGINS then
		_G.__MUHKUH_PLUGINS = {}
	end
	table.insert(_G.__MUHKUH_PLUGINS, romloader_jtag.romloader_jtag_provider())
}
#endif

%include plugins/muhkuh_typemaps.i

/* The plugin provider's functions "ClaimInterface" and "ReleaseInterface"
 * are only used from the C++ side. "ClaimInterface" is called from the
 * plugin reference's "Create" function and "ReleaseInterface" is called from
 * the plugin's destructor.
 */
%ignore romloader_jtag_provider::ClaimInterface;
%ignore romloader_jtag_provider::ReleaseInterface;

/* The plugin reference's function "Create" allocates a new plugin object on
 * the heap and passes it to the Lua interpreter. Now Lua "owns" the plugin
 * object. This means it must delete the object once it is garbage collected.
 */
%newobject romloader_jtag_reference::Create;


/* import interfaces */
%include "../../muhkuh_plugin_interface.h"
%include "../romloader.h"
%include "../romloader_def.h"
%include "romloader_jtag_main.h"

%include "version_information.i"

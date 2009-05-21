%module romloader_baka

/* This interface is lua specific. It will definitely not work with other
 * scripting languages. Refuse to continue if the output is not lua.
 */
#ifndef SWIGLUA
	#error "This module is lua specific. It will not work with other scripting languages!"
#endif

/* Include the header file in the lua wrapper.
 */
%header %{
	#include "romloader_baka_main.h"
%}

/* This lua code is executed on the first "require" operation for this
 * module. It adds the plugin provider to the global list
 * "__MUHKUH_PLUGINS".
 */
%luacode
{
	if not _G.__MUHKUH_PLUGINS then
		_G.__MUHKUH_PLUGINS = {}
	end
	table.insert(_G.__MUHKUH_PLUGINS, romloader_baka.romloader_baka_provider())
}

/* This typemap passes the lua type of the plugin and the plugin reference to
 * the provider's constructor.
 */
%typemap(in, numinputs=0) (swig_type_info *pt_romloader_baka_type_info, swig_type_info *pt_romloader_baka_reference_type_info)
%{
	$1 = SWIGTYPE_p_romloader_baka;
	$2 = SWIGTYPE_p_romloader_baka_reference;
%}

/* This typemap checks the first argument after the class. It must be a
 * table. It passes the lua state to the function. This allows the function
 * to add elements to the table without the overhead of creating and deleting
 * a C array.
 */
%typemap(in) lua_State *tLuaStateForTableAccess
%{
	if( lua_istable(L,2)!=1 )
	{
		SWIG_fail_arg("Create",2,"table");
	}
	$1 = L;
%}

/* This typemap converts the output of the plugin reference's "Create"
 * function from the general "muhkuh_plugin" type to the type of this
 * interface. It transfers the ownership of the object to lua (this is the
 * last parameter in the call to "SWIG_NewPointerObj").
 */
%typemap(out) muhkuh_plugin *
%{
	SWIG_NewPointerObj(L,result,((muhkuh_plugin_reference const *)arg1)->GetTypeInfo(),1); SWIG_arg++;
%}

/* The plugin provider's functions "ClaimInterface" and "ReleaseInterface"
 * are only used from the C++ side. "ClaimInterface" is called from the
 * plugin reference's "Create" function and "ReleaseInterface" is called from
 * the plugin's destructor.
 */
%ignore romloader_baka_provider::ClaimInterface;
%ignore romloader_baka_provider::ReleaseInterface;

/* The plugin reference's function "Create" allocates a new plugin object on
 * the heap and passes it to the lua interpreter. Now lua "owns" the plugin
 * object. This means it must delete the object once it is garbage collected.
 */
%newobject romloader_baka_reference::Create;

/* TODO: split the interface in multiple files. */
%include "../../muhkuh_plugin_interface.h"
%include "../romloader.h"
%include "romloader_baka_main.h"

%module romloader_baka

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
	#include "romloader_baka_main.h"
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
	table.insert(_G.__MUHKUH_PLUGINS, romloader_baka.romloader_baka_provider())
}

/* This typemap adds "SWIGTYPE_" to the name of the input parameter to
 * construct the swig typename. The parameter name must match the definition
 * in the wrapper.
 */
%typemap(in, numinputs=0) swig_type_info *
%{
	$1 = SWIGTYPE_$1_name;
%}

/* This typemap expects a table as input and replaces it with the Lua state.
 * This allows the function to add elements to the table without the overhead
 * of creating and deleting a C array.
 */
%typemap(in,checkfn="lua_istable") lua_State *ptLuaStateForTableAccess
%{
	$1 = L;
%}

/* This typemap passes the Lua state to the function. This allows the function
 * to call functions of the Swig Runtime API and the Lua C API.
 */
%typemap(in, numinputs=0) lua_State *
%{
	$1 = L;
%}

/* This typemap converts the output of the plugin reference's "Create"
 * function from the general "muhkuh_plugin" type to the type of this
 * interface. It transfers the ownership of the object to Lua (this is the
 * last parameter in the call to "SWIG_NewPointerObj").
 */
%typemap(out) muhkuh_plugin *
%{
	SWIG_NewPointerObj(L,result,((muhkuh_plugin_reference const *)arg1)->GetTypeInfo(),1); SWIG_arg++;
%}


%typemap(in, numinputs=0) (char **ppcOutputData, unsigned long *pulOutputData)
%{
	char *pcOutputData;
	unsigned long ulOutputData;
	$1 = &pcOutputData;
	$2 = &ulOutputData;
%}

%typemap(argout) (char **ppcOutputData, unsigned long *pulOutputData)
%{
	if( pcOutputData!=NULL && ulOutputData!=0 )
	{
		lua_pushlstring(L, pcOutputData, ulOutputData);
		delete[] pcOutputData;
	}
	else
	{
		lua_pushnil(L);
	}
	++SWIG_arg;
%}

%typemap(in) (const char *pcInputData, unsigned long ulInputData)
{
	size_t sizInputData;
	$1 = (char*)lua_tolstring(L, $argnum, &sizInputData);
	$2 = (unsigned long)sizInputData;
}

/* The plugin provider's functions "ClaimInterface" and "ReleaseInterface"
 * are only used from the C++ side. "ClaimInterface" is called from the
 * plugin reference's "Create" function and "ReleaseInterface" is called from
 * the plugin's destructor.
 */
%ignore romloader_baka_provider::ClaimInterface;
%ignore romloader_baka_provider::ReleaseInterface;

/* The plugin reference's function "Create" allocates a new plugin object on
 * the heap and passes it to the Lua interpreter. Now Lua "owns" the plugin
 * object. This means it must delete the object once it is garbage collected.
 */
%newobject romloader_baka_reference::Create;


/* TODO: split the interface in multiple files. */
%include "../../muhkuh_plugin_interface.h"
%include "../romloader.h"
%include "romloader_baka_main.h"

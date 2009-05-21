%module romloader_baka

/* TODO: get rid of vectors and use plain lua tables */
%include "std_vector.i"

/* Include the header file in the lua wrapper.
 */
%{
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

/* This typemap passes the lua type of the plugin to the provider's
 * constructor.
 */
%typemap(in, numinputs=0) swig_type_info *pt_romloader_baka_type_info
%{
	$1 = SWIGTYPE_p_romloader_baka;
%}

/* This typemap converts the output of the plugin reference's "Create"
 * function from the general "muhkuh_plugin" type to the 
 */
%typemap(out) muhkuh_plugin *
%{
	SWIG_NewPointerObj(L,result,((muhkuh_plugin_reference const *)arg1)->GetTypeInfo(),0); SWIG_arg++;
%}

/* The plugin provider's functions "ClaimInterface" and "ReleaseInterface"
 * are 
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

/* TODO: get rid of vectors and use plain lua tables */
namespace std
{
   %template(PluginVector) vector<muhkuh_plugin_reference*>;
}

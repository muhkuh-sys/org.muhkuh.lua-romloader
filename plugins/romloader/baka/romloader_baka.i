%module romloader_baka

%include "std_vector.i"

%{
	#include "romloader_baka_main.h"
%}

%luacode
{
	if not _G.__MUHKUH_PLUGINS then
		_G.__MUHKUH_PLUGINS = {}
	end
	table.insert(_G.__MUHKUH_PLUGINS, romloader_baka.romloader_baka_provider())
}

%typemap(in, numinputs=0) swig_type_info *ptPluginTypeInfo
%{
	$1 = SWIGTYPE_p_romloader_baka;
%}

%typemap(out) muhkuh_plugin *
%{
	SWIG_NewPointerObj(L,result,((muhkuh_plugin_reference const *)arg1)->GetTypeInfo(),0); SWIG_arg++;
%}


%ignore romloader_baka_provider::ClaimInterface;
%ignore romloader_baka_provider::ReleaseInterface;

%newobject romloader_baka_reference::Create;

%include "../../muhkuh_plugin_interface.h"
%include "../romloader.h"
%include "romloader_baka_main.h"

namespace std
{
   %template(PluginVector) vector<muhkuh_plugin_reference*>;
}

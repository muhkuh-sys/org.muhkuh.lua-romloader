%module romloader_baka

%include "std_vector.i"

%{
	#include "romloader_baka_main.h"
%}


%newobject romloader_baka_reference::ClaimInterface;
%delobject romloader_baka_reference::ReleaseInterface;

%include "../../muhkuh_plugin_interface.h"
%include "../romloader.h"
%include "romloader_baka_main.h"

namespace std
{
   %template(PluginVector) vector<muhkuh_plugin_reference*>;
}

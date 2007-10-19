
%include "application/muhkuh_lua_interface.h"


%class %noclassinfo muhkuh_plugin_instance
	bool IsValid()
	wxString GetName()
	wxString GetTyp()
	bool IsUsed()
	wxString GetLuaCreateFn()
	voidptr_long GetHandle()
%endclass


%function void TestHasFinished()

%function wxString load(wxString strFileName)
%function void include(wxString strFileName, wxString strChunkName)

%function void setLogMarker()
%function wxString getMarkedLog()

%function void ScanPlugins(wxString strPattern)
%function %gc muhkuh_plugin_instance *GetNextPlugin()


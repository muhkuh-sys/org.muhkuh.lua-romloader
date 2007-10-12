
%include "application/muhkuh_mainFrame.h"


%class %noclassinfo muhkuh_binfile
	wxString GetFileName()
	bool IsOk()
	wxString GetErrorMessage()
	wxString GetText()
	int GetSize()
%endclass


%class %noclassinfo muhkuh_plugin_instance
	bool IsValid()
	wxString GetName()
	wxString GetTyp()
	bool IsUsed()
	wxString GetLuaCreateFn()
	voidptr_long GetHandle()
%endclass


%function void TestHasFinished()

%function %gc muhkuh_binfile *LoadBinFile(wxString strFileName)
%function void include(wxString strFileName, wxString strChunkName)


%function void ScanPlugins(wxString strPattern)
%function %gc muhkuh_plugin_instance *GetNextPlugin()


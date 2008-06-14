
%include "application/muhkuh_lua_interface.h"


%class %noclassinfo muhkuh_plugin_instance
	bool IsValid()
	wxString GetName()
	wxString GetTyp()
	bool IsUsed()
	wxString GetLuaCreateFn()
	voidptr_long GetHandle()
%endclass


%class %noclassinfo muhkuh_wrap_xml
	wxString testDescription_getName()
	wxString testDescription_getVersion()
	unsigned int testDescription_getTestCnt()
	bool testDescription_setTest(unsigned int uiTestIdx)

	wxString test_getName()
	wxString test_getVersion()
%endclass


%function void TestHasFinished()

%function wxString load(wxString strFileName)
%function void include(wxString strFileName)

%function void setLogMarker()
%function wxString getMarkedLog()

%function void ScanPlugins(wxString strPattern)
%function %gc muhkuh_plugin_instance *GetNextPlugin()


%function muhkuh_wrap_xml *GetSelectedTest()


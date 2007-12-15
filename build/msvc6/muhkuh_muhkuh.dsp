# Microsoft Developer Studio Project File - Name="muhkuh_muhkuh" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=muhkuh - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "muhkuh_muhkuh.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "muhkuh_muhkuh.mak" CFG="muhkuh - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "muhkuh - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "muhkuh - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "muhkuh - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\..\bin"
# PROP BASE Intermediate_Dir "vcmsw_dll\muhkuh"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\bin"
# PROP Intermediate_Dir "vcmsw_dll\muhkuh"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MD /W1 /GR /EHsc /I "$(WXWIN)\lib\vc_dll\msw" /I "$(WXWIN)\include" /O2 /I "..\..\modules" /I ".\..\.." /I "..\..\..\modules\lua\include" /I "..\..\application" /Fd..\..\bin\muhkuh.pdb /D "WIN32" /D "WXUSINGDLL" /D "__WXMSW__" /D "_WINDOWS" /c
# ADD CPP /nologo /FD /MD /W1 /GR /EHsc /I "$(WXWIN)\lib\vc_dll\msw" /I "$(WXWIN)\include" /O2 /I "..\..\modules" /I ".\..\.." /I "..\..\..\modules\lua\include" /I "..\..\application" /Fd..\..\bin\muhkuh.pdb /D "WIN32" /D "WXUSINGDLL" /D "__WXMSW__" /D "_WINDOWS" /c
# ADD BASE MTL /nologo /D "WIN32" /D "WXUSINGDLL" /D "__WXMSW__" /D "_WINDOWS" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "WXUSINGDLL" /D "__WXMSW__" /D "_WINDOWS" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "WXUSINGDLL" /d "__WXMSW__" /i "$(WXWIN)\lib\vc_dll\msw" /i "$(WXWIN)\include" /i "..\..\modules" /i ".\..\.." /i "..\..\..\modules\lua\include" /d "_WINDOWS" /i ..\..\application
# ADD RSC /l 0x409 /d "WXUSINGDLL" /d "__WXMSW__" /i "$(WXWIN)\lib\vc_dll\msw" /i "$(WXWIN)\include" /i "..\..\modules" /i ".\..\.." /i "..\..\..\modules\lua\include" /d "_WINDOWS" /i ..\..\application
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxlua_msw28_wxluasocket.lib wxlua_msw28_wxluadebug.lib wxlua_msw28_wxbindxrc.lib wxlua_msw28_wxbindhtml.lib wxlua_msw28_wxbindadv.lib wxlua_msw28_wxbindaui.lib wxlua_msw28_wxbindnet.lib wxlua_msw28_wxbindxml.lib wxlua_msw28_wxbindcore.lib wxlua_msw28_wxbindbase.lib wxlua_msw28_wxlua.lib lua5.1.lib wxmsw28_xrc.lib wxmsw28_html.lib wxmsw28_adv.lib wxmsw28_aui.lib wxbase28_net.lib wxbase28_xml.lib wxmsw28_core.lib wxbase28.lib wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib oleacc.lib /nologo /machine:i386 /out:"..\..\bin\muhkuh.exe" /libpath:"$(WXWIN)\lib\vc_dll" /libpath:"..\..\lib\vc_dll" /libpath:"..\..\..\modules\lua\lib" /subsystem:windows /pdb:"..\..\bin\muhkuh.pdb"
# ADD LINK32 wxlua_msw28_wxluasocket.lib wxlua_msw28_wxluadebug.lib wxlua_msw28_wxbindxrc.lib wxlua_msw28_wxbindhtml.lib wxlua_msw28_wxbindadv.lib wxlua_msw28_wxbindaui.lib wxlua_msw28_wxbindnet.lib wxlua_msw28_wxbindxml.lib wxlua_msw28_wxbindcore.lib wxlua_msw28_wxbindbase.lib wxlua_msw28_wxlua.lib lua5.1.lib wxmsw28_xrc.lib wxmsw28_html.lib wxmsw28_adv.lib wxmsw28_aui.lib wxbase28_net.lib wxbase28_xml.lib wxmsw28_core.lib wxbase28.lib wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib oleacc.lib /nologo /machine:i386 /out:"..\..\bin\muhkuh.exe" /libpath:"$(WXWIN)\lib\vc_dll" /libpath:"..\..\lib\vc_dll" /libpath:"..\..\..\modules\lua\lib" /subsystem:windows /pdb:"..\..\bin\muhkuh.pdb"

!ELSEIF  "$(CFG)" == "muhkuh - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\bin"
# PROP BASE Intermediate_Dir "vcmswd_dll\muhkuh"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\bin"
# PROP Intermediate_Dir "vcmswd_dll\muhkuh"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MDd /W1 /GR /EHsc /I "$(WXWIN)\lib\vc_dll\mswd" /I "$(WXWIN)\include" /Od /I "..\..\modules" /I ".\..\.." /I "..\..\..\modules\lua\include" /I "..\..\application" /Zi /Gm /GZ /Fd..\..\bin\muhkuh.pdb /D "WIN32" /D "WXUSINGDLL" /D "__WXDEBUG__" /D "__WXMSW__" /D "_WINDOWS" /D "_DEBUG" /c
# ADD CPP /nologo /FD /MDd /W1 /GR /EHsc /I "$(WXWIN)\lib\vc_dll\mswd" /I "$(WXWIN)\include" /Od /I "..\..\modules" /I ".\..\.." /I "..\..\..\modules\lua\include" /I "..\..\application" /Zi /Gm /GZ /Fd..\..\bin\muhkuh.pdb /D "WIN32" /D "WXUSINGDLL" /D "__WXDEBUG__" /D "__WXMSW__" /D "_WINDOWS" /D "_DEBUG" /c
# ADD BASE MTL /nologo /D "WIN32" /D "WXUSINGDLL" /D "__WXDEBUG__" /D "__WXMSW__" /D "_WINDOWS" /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "WXUSINGDLL" /D "__WXDEBUG__" /D "__WXMSW__" /D "_WINDOWS" /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "WXUSINGDLL" /d "__WXDEBUG__" /d "__WXMSW__" /i "$(WXWIN)\lib\vc_dll\mswd" /i "$(WXWIN)\include" /i "..\..\modules" /i ".\..\.." /i "..\..\..\modules\lua\include" /d "_WINDOWS" /i "..\..\application" /d _DEBUG
# ADD RSC /l 0x409 /d "WXUSINGDLL" /d "__WXDEBUG__" /d "__WXMSW__" /i "$(WXWIN)\lib\vc_dll\mswd" /i "$(WXWIN)\include" /i "..\..\modules" /i ".\..\.." /i "..\..\..\modules\lua\include" /d "_WINDOWS" /i "..\..\application" /d _DEBUG
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 wxlua_msw28d_wxluasocket.lib wxlua_msw28d_wxluadebug.lib wxlua_msw28d_wxbindxrc.lib wxlua_msw28d_wxbindhtml.lib wxlua_msw28d_wxbindadv.lib wxlua_msw28d_wxbindaui.lib wxlua_msw28d_wxbindnet.lib wxlua_msw28d_wxbindxml.lib wxlua_msw28d_wxbindcore.lib wxlua_msw28d_wxbindbase.lib wxlua_msw28d_wxlua.lib lua5.1.lib wxmsw28d_xrc.lib wxmsw28d_html.lib wxmsw28d_adv.lib wxmsw28d_aui.lib wxbase28d_net.lib wxbase28d_xml.lib wxmsw28d_core.lib wxbase28d.lib wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib oleacc.lib /nologo /machine:i386 /out:"..\..\bin\muhkuh.exe" /libpath:"$(WXWIN)\lib\vc_dll" /libpath:"..\..\lib\vc_dll" /libpath:"..\..\..\modules\lua\lib" /subsystem:windows /debug /pdb:"..\..\bin\muhkuh.pdb"
# ADD LINK32 wxlua_msw28d_wxluasocket.lib wxlua_msw28d_wxluadebug.lib wxlua_msw28d_wxbindxrc.lib wxlua_msw28d_wxbindhtml.lib wxlua_msw28d_wxbindadv.lib wxlua_msw28d_wxbindaui.lib wxlua_msw28d_wxbindnet.lib wxlua_msw28d_wxbindxml.lib wxlua_msw28d_wxbindcore.lib wxlua_msw28d_wxbindbase.lib wxlua_msw28d_wxlua.lib lua5.1.lib wxmsw28d_xrc.lib wxmsw28d_html.lib wxmsw28d_adv.lib wxmsw28d_aui.lib wxbase28d_net.lib wxbase28d_xml.lib wxmsw28d_core.lib wxbase28d.lib wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib odbc32.lib oleacc.lib /nologo /machine:i386 /out:"..\..\bin\muhkuh.exe" /libpath:"$(WXWIN)\lib\vc_dll" /libpath:"..\..\lib\vc_dll" /libpath:"..\..\..\modules\lua\lib" /subsystem:windows /debug /pdb:"..\..\bin\muhkuh.pdb"

!ENDIF

# Begin Target

# Name "muhkuh - Win32 Release"
# Name "muhkuh - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=../..\plugins\_luaif\bit_lua.cpp
# End Source File
# Begin Source File

SOURCE=../..\plugins\_luaif\bit_lua_bind.cpp
# End Source File
# Begin Source File

SOURCE=../..\application\bit_lua_interface.cpp
# End Source File
# Begin Source File

SOURCE=../..\application\growbuffer.cpp
# End Source File
# Begin Source File

SOURCE=../..\application\license.cpp
# End Source File
# Begin Source File

SOURCE=../..\application\muhkuh.rc
# End Source File
# Begin Source File

SOURCE=../..\application\muhkuh_aboutDialog.cpp
# End Source File
# Begin Source File

SOURCE=../..\application\muhkuh_app.cpp
# End Source File
# Begin Source File

SOURCE=../..\application\muhkuh_configDialog.cpp
# End Source File
# Begin Source File

SOURCE=../..\application\muhkuh_config_reposEntryDialog.cpp
# End Source File
# Begin Source File

SOURCE=../..\plugins\_luaif\muhkuh_lua.cpp
# End Source File
# Begin Source File

SOURCE=../..\plugins\_luaif\muhkuh_lua_bind.cpp
# End Source File
# Begin Source File

SOURCE=../..\application\muhkuh_mainFrame.cpp
# End Source File
# Begin Source File

SOURCE=../..\application\muhkuh_plugin.cpp
# End Source File
# Begin Source File

SOURCE=../..\application\muhkuh_plugin_manager.cpp
# End Source File
# Begin Source File

SOURCE=../..\application\muhkuh_repository.cpp
# End Source File
# Begin Source File

SOURCE=../..\application\muhkuh_repository_manager.cpp
# End Source File
# Begin Source File

SOURCE=../..\application\muhkuh_wrap_xml.cpp
# End Source File
# Begin Source File

SOURCE=../..\application\readFsFile.cpp
# End Source File
# End Group
# End Target
# End Project


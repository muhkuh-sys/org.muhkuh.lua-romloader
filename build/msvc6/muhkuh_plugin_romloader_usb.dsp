# Microsoft Developer Studio Project File - Name="muhkuh_plugin_romloader_usb" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=plugin_romloader_usb - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "muhkuh_plugin_romloader_usb.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "muhkuh_plugin_romloader_usb.mak" CFG="plugin_romloader_usb - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "plugin_romloader_usb - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "plugin_romloader_usb - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "plugin_romloader_usb - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\..\bin"
# PROP BASE Intermediate_Dir "vcmsw_dll\plugin_romloader_usb"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\bin"
# PROP Intermediate_Dir "vcmsw_dll\plugin_romloader_usb"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MD /W1 /GR /EHsc /I "$(WXWIN)\lib\vc_dll\msw" /I "$(WXWIN)\include" /O2 /I "..\..\modules" /I ".\..\.." /I "..\..\..\modules\lua\include" /Fd..\..\bin\bootloader_usb.pdb /I "..\..\plugins\romloader\usb" /I "..\..\plugins\romloader\usb\win\include" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "WXUSINGDLL" /D "__WXMSW__" /c
# ADD CPP /nologo /FD /MD /W1 /GR /EHsc /I "$(WXWIN)\lib\vc_dll\msw" /I "$(WXWIN)\include" /O2 /I "..\..\modules" /I ".\..\.." /I "..\..\..\modules\lua\include" /Fd..\..\bin\bootloader_usb.pdb /I "..\..\plugins\romloader\usb" /I "..\..\plugins\romloader\usb\win\include" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "WXUSINGDLL" /D "__WXMSW__" /c
# ADD BASE MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "WXUSINGDLL" /D "__WXMSW__" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "WXUSINGDLL" /D "__WXMSW__" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "WXUSINGDLL" /d "__WXMSW__" /i "$(WXWIN)\lib\vc_dll\msw" /i "$(WXWIN)\include" /i "..\..\modules" /i ".\..\.." /i "..\..\..\modules\lua\include" /i "..\..\plugins\romloader\usb" /i ..\..\plugins\romloader\usb\win\include
# ADD RSC /l 0x409 /d "WXUSINGDLL" /d "__WXMSW__" /i "$(WXWIN)\lib\vc_dll\msw" /i "$(WXWIN)\include" /i "..\..\modules" /i ".\..\.." /i "..\..\..\modules\lua\include" /i "..\..\plugins\romloader\usb" /i ..\..\plugins\romloader\usb\win\include
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 libusb.lib setupapi.lib wxlua_msw28_wxluasocket.lib wxlua_msw28_wxluadebug.lib wxlua_msw28_wxbindxrc.lib wxlua_msw28_wxbindhtml.lib wxlua_msw28_wxbindadv.lib wxlua_msw28_wxbindaui.lib wxlua_msw28_wxbindnet.lib wxlua_msw28_wxbindxml.lib wxlua_msw28_wxbindcore.lib wxlua_msw28_wxbindbase.lib wxlua_msw28_wxlua.lib lua5.1.lib wxmsw28_xrc.lib wxmsw28_html.lib wxmsw28_adv.lib wxmsw28_aui.lib wxbase28_net.lib wxbase28_xml.lib wxmsw28_core.lib wxbase28.lib /nologo /dll /machine:i386 /out:"..\..\bin\bootloader_usb.dll" /pdb:"..\..\bin\bootloader_usb.pdb" /libpath:"..\..\plugins\romloader\usb\win\lib\msvc"
# ADD LINK32 libusb.lib setupapi.lib wxlua_msw28_wxluasocket.lib wxlua_msw28_wxluadebug.lib wxlua_msw28_wxbindxrc.lib wxlua_msw28_wxbindhtml.lib wxlua_msw28_wxbindadv.lib wxlua_msw28_wxbindaui.lib wxlua_msw28_wxbindnet.lib wxlua_msw28_wxbindxml.lib wxlua_msw28_wxbindcore.lib wxlua_msw28_wxbindbase.lib wxlua_msw28_wxlua.lib lua5.1.lib wxmsw28_xrc.lib wxmsw28_html.lib wxmsw28_adv.lib wxmsw28_aui.lib wxbase28_net.lib wxbase28_xml.lib wxmsw28_core.lib wxbase28.lib /nologo /dll /machine:i386 /out:"..\..\bin\bootloader_usb.dll" /pdb:"..\..\bin\bootloader_usb.pdb" /libpath:"..\..\plugins\romloader\usb\win\lib\msvc"

!ELSEIF  "$(CFG)" == "plugin_romloader_usb - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\..\bin"
# PROP BASE Intermediate_Dir "vcmswd_dll\plugin_romloader_usb"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\bin"
# PROP Intermediate_Dir "vcmswd_dll\plugin_romloader_usb"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MDd /W1 /GR /EHsc /I "$(WXWIN)\lib\vc_dll\mswd" /I "$(WXWIN)\include" /Od /I "..\..\modules" /I ".\..\.." /I "..\..\..\modules\lua\include" /Zi /Gm /GZ /Fd..\..\bin\bootloader_usb.pdb /I "..\..\plugins\romloader\usb" /I "..\..\plugins\romloader\usb\win\include" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "WXUSINGDLL" /D "__WXDEBUG__" /D "__WXMSW__" /D "_DEBUG" /c
# ADD CPP /nologo /FD /MDd /W1 /GR /EHsc /I "$(WXWIN)\lib\vc_dll\mswd" /I "$(WXWIN)\include" /Od /I "..\..\modules" /I ".\..\.." /I "..\..\..\modules\lua\include" /Zi /Gm /GZ /Fd..\..\bin\bootloader_usb.pdb /I "..\..\plugins\romloader\usb" /I "..\..\plugins\romloader\usb\win\include" /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "WXUSINGDLL" /D "__WXDEBUG__" /D "__WXMSW__" /D "_DEBUG" /c
# ADD BASE MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "WXUSINGDLL" /D "__WXDEBUG__" /D "__WXMSW__" /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_USRDLL" /D "DLL_EXPORTS" /D "WXUSINGDLL" /D "__WXDEBUG__" /D "__WXMSW__" /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "WXUSINGDLL" /d "__WXDEBUG__" /d "__WXMSW__" /i "$(WXWIN)\lib\vc_dll\mswd" /i "$(WXWIN)\include" /i "..\..\modules" /i ".\..\.." /i "..\..\..\modules\lua\include" /d "_DEBUG" /i "..\..\plugins\romloader\usb" /i ..\..\plugins\romloader\usb\win\include
# ADD RSC /l 0x409 /d "WXUSINGDLL" /d "__WXDEBUG__" /d "__WXMSW__" /i "$(WXWIN)\lib\vc_dll\mswd" /i "$(WXWIN)\include" /i "..\..\modules" /i ".\..\.." /i "..\..\..\modules\lua\include" /d "_DEBUG" /i "..\..\plugins\romloader\usb" /i ..\..\plugins\romloader\usb\win\include
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 libusb.lib setupapi.lib wxlua_msw28d_wxluasocket.lib wxlua_msw28d_wxluadebug.lib wxlua_msw28d_wxbindxrc.lib wxlua_msw28d_wxbindhtml.lib wxlua_msw28d_wxbindadv.lib wxlua_msw28d_wxbindaui.lib wxlua_msw28d_wxbindnet.lib wxlua_msw28d_wxbindxml.lib wxlua_msw28d_wxbindcore.lib wxlua_msw28d_wxbindbase.lib wxlua_msw28d_wxlua.lib lua5.1.lib wxmsw28d_xrc.lib wxmsw28d_html.lib wxmsw28d_adv.lib wxmsw28d_aui.lib wxbase28d_net.lib wxbase28d_xml.lib wxmsw28d_core.lib wxbase28d.lib /nologo /dll /machine:i386 /out:"..\..\bin\bootloader_usb.dll" /debug /pdb:"..\..\bin\bootloader_usb.pdb" /libpath:"..\..\plugins\romloader\usb\win\lib\msvc"
# ADD LINK32 libusb.lib setupapi.lib wxlua_msw28d_wxluasocket.lib wxlua_msw28d_wxluadebug.lib wxlua_msw28d_wxbindxrc.lib wxlua_msw28d_wxbindhtml.lib wxlua_msw28d_wxbindadv.lib wxlua_msw28d_wxbindaui.lib wxlua_msw28d_wxbindnet.lib wxlua_msw28d_wxbindxml.lib wxlua_msw28d_wxbindcore.lib wxlua_msw28d_wxbindbase.lib wxlua_msw28d_wxlua.lib lua5.1.lib wxmsw28d_xrc.lib wxmsw28d_html.lib wxmsw28d_adv.lib wxmsw28d_aui.lib wxbase28d_net.lib wxbase28d_xml.lib wxmsw28d_core.lib wxbase28d.lib /nologo /dll /machine:i386 /out:"..\..\bin\bootloader_usb.dll" /debug /pdb:"..\..\bin\bootloader_usb.pdb" /libpath:"..\..\plugins\romloader\usb\win\lib\msvc"

!ENDIF

# Begin Target

# Name "plugin_romloader_usb - Win32 Release"
# Name "plugin_romloader_usb - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=../..\plugins\romloader\romloader.cpp
# End Source File
# Begin Source File

SOURCE=../..\plugins\romloader\usb\romloader_usb_io.cpp
# End Source File
# Begin Source File

SOURCE=../..\plugins\romloader\usb\_luaif\romloader_usb_lua.cpp
# End Source File
# Begin Source File

SOURCE=../..\plugins\romloader\usb\_luaif\romloader_usb_lua_bind.cpp
# End Source File
# Begin Source File

SOURCE=../..\plugins\romloader\usb\romloader_usb_main.cpp
# End Source File
# End Group
# End Target
# End Project


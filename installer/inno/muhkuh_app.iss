
[Components]
Name: muhkuh;                   Description: "Muhkuh base application";  Types: full compact custom; Flags: fixed


[Files]
Source: "bin\muhkuh.exe";                     DestDir: "{app}\application"; Components: muhkuh
Source: "bin\muhkuh_tips.txt";                DestDir: "{app}\application"; Components: muhkuh
Source: "icons\custom\muhkuh_uninstall.ico";  DestDir: "{app}\application"; Components: muhkuh
; system dlls
Source: "bin\msvcr71.dll" ;              DestDir: "{app}\application"; Components: muhkuh
Source: "bin\msvcp71.dll";               DestDir: "{app}\application"; Components: muhkuh
; the wxwidgets dlls
Source: "bin\wxbase28_net_vc.dll" ;      DestDir: "{app}\application"; Components: muhkuh
Source: "bin\wxbase28_odbc_vc.dll";      DestDir: "{app}\application"; Components: muhkuh
Source: "bin\wxbase28_vc.dll";           DestDir: "{app}\application"; Components: muhkuh
Source: "bin\wxbase28_xml_vc.dll";       DestDir: "{app}\application"; Components: muhkuh
Source: "bin\wxmsw28_adv_vc.dll";        DestDir: "{app}\application"; Components: muhkuh
Source: "bin\wxmsw28_aui_vc.dll";        DestDir: "{app}\application"; Components: muhkuh
Source: "bin\wxmsw28_core_vc.dll";       DestDir: "{app}\application"; Components: muhkuh
Source: "bin\wxmsw28_dbgrid_vc.dll";     DestDir: "{app}\application"; Components: muhkuh
Source: "bin\wxmsw28_gl_vc.dll";         DestDir: "{app}\application"; Components: muhkuh
Source: "bin\wxmsw28_html_vc.dll";       DestDir: "{app}\application"; Components: muhkuh
Source: "bin\wxmsw28_media_vc.dll";      DestDir: "{app}\application"; Components: muhkuh
Source: "bin\wxmsw28_qa_vc.dll";         DestDir: "{app}\application"; Components: muhkuh
Source: "bin\wxmsw28_richtext_vc.dll";   DestDir: "{app}\application"; Components: muhkuh
Source: "bin\wxmsw28_stc_vc_custom.dll"; DestDir: "{app}\application"; Components: muhkuh
Source: "bin\wxmsw28_vc.dll";            DestDir: "{app}\application"; Components: muhkuh
Source: "bin\wxmsw28_xrc_vc.dll";        DestDir: "{app}\application"; Components: muhkuh
; the wxLua dlls
Source: "bin\lua5.1.dll";                  DestDir: "{app}\application"; Components: muhkuh
Source: "bin\wxlua_msw28_wxbindadv.dll";   DestDir: "{app}\application"; Components: muhkuh
Source: "bin\wxlua_msw28_wxbindaui.dll";   DestDir: "{app}\application"; Components: muhkuh
Source: "bin\wxlua_msw28_wxbindbase.dll";  DestDir: "{app}\application"; Components: muhkuh
Source: "bin\wxlua_msw28_wxbindcore.dll";  DestDir: "{app}\application"; Components: muhkuh
Source: "bin\wxlua_msw28_wxbindgl.dll";    DestDir: "{app}\application"; Components: muhkuh
Source: "bin\wxlua_msw28_wxbindhtml.dll";  DestDir: "{app}\application"; Components: muhkuh
Source: "bin\wxlua_msw28_wxbindmedia.dll"; DestDir: "{app}\application"; Components: muhkuh
Source: "bin\wxlua_msw28_wxbindnet.dll";   DestDir: "{app}\application"; Components: muhkuh
Source: "bin\wxlua_msw28_wxbindstc.dll";   DestDir: "{app}\application"; Components: muhkuh
Source: "bin\wxlua_msw28_wxbindxml.dll";   DestDir: "{app}\application"; Components: muhkuh
Source: "bin\wxlua_msw28_wxbindxrc.dll";   DestDir: "{app}\application"; Components: muhkuh
Source: "bin\wxlua_msw28_wxlua.dll";       DestDir: "{app}\application"; Components: muhkuh
Source: "bin\wxlua_msw28_wxluadebug.dll";  DestDir: "{app}\application"; Components: muhkuh
Source: "bin\wxlua_msw28_wxluasocket.dll"; DestDir: "{app}\application"; Components: muhkuh
; the docs
Source: "docs\gpl-2.0.txt"; DestDir: "{app}\docs"; Components: muhkuh
Source: "changelog.txt";    DestDir: "{app}\docs"; Components: muhkuh


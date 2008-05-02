
[Tasks]
Name: startmenu;       Description: "Create icons in Start menu";  GroupDescription: "Additional icons:"; Components: muhkuh
Name: desktopicon;     Description: "Create a &desktop icon";      GroupDescription: "Additional icons:"; Components: muhkuh
Name: quicklaunchicon; Description: "Create a &Quick Launch icon"; GroupDescription: "Additional icons:"; Components: muhkuh


[Icons]
Name: {group}\Muhkuh;  Filename: {app}\application\muhkuh.exe;  WorkingDir: {app}\application;  Components: muhkuh; Tasks: startmenu
Name: {group}\Uninstall;  Filename: {uninstallexe};  IconFilename: {app}\application\muhkuh_uninstall.ico;  Components: muhkuh; Tasks: startmenu

Name: {userdesktop}\Muhkuh;  Filename: {app}\application\muhkuh.exe;  WorkingDir: {app}\application;  Components: muhkuh; Tasks: desktopicon
Name: {userappdata}\Microsoft\Internet Explorer\Quick Launch\Muhkuh;  Filename: {app}\application\muhkuh.exe;  WorkingDir: {app}\application;  Tasks: quicklaunchicon


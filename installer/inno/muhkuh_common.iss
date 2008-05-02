
[Setup]
AppName={#AppName}
AppVersion={#AppVersion}
AppVerName={#AppVerName}
OutputBaseFilename={#InstallerName}
VersionInfoTextVersion={#AppVersion}
VersionInfoVersion={#AppVersion}

DefaultDirName={pf}\{#AppName}
DefaultGroupName={#AppName}
Compression=lzma/max
SolidCompression=yes

SourceDir={#SourceDir}
; write installer exe to SourceDir
OutputDir=.

AllowNoIcons=yes
;ChangesAssociations=yes
InfoBeforeFile=changelog.txt
LicenseFile=docs\gpl-2.0.txt

; misc info
AppPublisher=Muhkuh team
AppPublisherURL=http://muhkuh.sourceforge.net/
AppSupportURL=http://www.sourceforge.net/projects/muhkuh
AppUpdatesURL=http://www.sourceforge.net/projects/muhkuh
VersionInfoCopyright=Copyright (C) 2008, Muhkuh team
AppCopyright=Copyright (C) 2008, Muhkuh team

; icon stuff
SetupIconFile=icons\custom\muhkuh.ico
;WizardImageFile=icons\custom\muhkuh.ico


[Types]
Name: "full";    Description: "Full installation"
Name: "compact"; Description: "Only install the base system without plugins"
Name: "custom";  Description: "Custom installation";                          Flags: iscustom


[Messages]
BeveledLabel=Muhkuh


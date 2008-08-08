
[Setup]
AppName={#AppName}
AppVersion={#AppVersion}
AppVerName={#AppVerName}
OutputBaseFilename={#InstallerName}
VersionInfoTextVersion={#AppVersion}
VersionInfoVersion={#AppVersion}

Compression=lzma/max
SolidCompression=yes

SourceDir={#SourceDir}
OutputDir={#OutputDir}

AllowNoIcons=yes
ChangesAssociations=yes
LicenseFile=docs\gpl-2.0.txt


[Types]
Name: "full";    Description: "Full installation"
Name: "compact"; Description: "Only install the base system without plugins"
Name: "custom";  Description: "Custom installation";                          Flags: iscustom



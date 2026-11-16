; MSHV-IC8TEM - Windows installer (Inno Setup 6)
; ISCC /DAppVersion=2.76.7-ic8tem.2 /DSrcDir=<folder with MSHV-IC8TEM.exe> /DArch=x64|x86|arm64 /DOutputName=<name> mshv-ic8tem.iss
;
; Installs per user (no administrator rights) in %LOCALAPPDATA%\Programs\MSHV-IC8TEM,
; so MSHV can keep its settings and logs next to the program, as in the zip version.
; Existing settings/logs are never overwritten by an update and never removed by uninstall.

#ifndef AppVersion
  #define AppVersion "dev"
#endif
#ifndef Arch
  #define Arch "x64"
#endif

[Setup]
AppId={{760ED237-B6DF-446D-961A-7B9D00FAE8A0}
AppName=MSHV-IC8TEM
AppVersion={#AppVersion}
AppVerName=MSHV-IC8TEM {#AppVersion}
AppPublisher=IC8TEM
AppPublisherURL=https://github.com/ic8tem/MSHV-IC8TEM
AppSupportURL=https://github.com/ic8tem/MSHV-IC8TEM/issues
AppUpdatesURL=https://github.com/ic8tem/MSHV-IC8TEM/releases
AppComments=Modified version of MSHV 2.76.7 by LZ2HV (GPLv3)
DefaultDirName={userpf}\MSHV-IC8TEM
DefaultGroupName=MSHV-IC8TEM
DisableProgramGroupPage=yes
PrivilegesRequired=lowest
LicenseFile=..\..\COPYING.txt
SetupIconFile=..\..\src\pic\ms_ico.ico
UninstallDisplayIcon={app}\MSHV-IC8TEM.exe
UninstallDisplayName=MSHV-IC8TEM {#AppVersion}
OutputBaseFilename={#OutputName}
Compression=lzma2/max
SolidCompression=yes
WizardStyle=modern
CloseApplications=yes
#if Arch == "x64"
ArchitecturesAllowed=x64compatible
MinVersion=6.1sp1
ArchitecturesInstallIn64BitMode=x64compatible
#elif Arch == "arm64"
ArchitecturesAllowed=arm64
ArchitecturesInstallIn64BitMode=arm64
#endif

[Languages]
Name: "italian"; MessagesFile: "compiler:Languages\Italian.isl"
Name: "english"; MessagesFile: "compiler:Default.isl"

[Tasks]
Name: "desktopicon"; Description: "{cm:CreateDesktopIcon}"; GroupDescription: "{cm:AdditionalIcons}"

[Files]
; program, Qt and documents: always updated
Source: "{#SrcDir}\*"; DestDir: "{app}"; Excludes: "\settings,\log,\RxWavs,\ExportLog,\AllTxtMonthly,\Screenshots,.gitkeep"; Flags: recursesubdirs createallsubdirs ignoreversion
; user data seeds: installed only the first time, kept on update and on uninstall
Source: "{#SrcDir}\settings\*"; DestDir: "{app}\settings"; Flags: recursesubdirs createallsubdirs onlyifdoesntexist uninsneveruninstall skipifsourcedoesntexist

[Dirs]
Name: "{app}\settings"; Flags: uninsneveruninstall
Name: "{app}\log"; Flags: uninsneveruninstall
Name: "{app}\RxWavs"; Flags: uninsneveruninstall
Name: "{app}\ExportLog"; Flags: uninsneveruninstall
Name: "{app}\AllTxtMonthly"; Flags: uninsneveruninstall
Name: "{app}\Screenshots"; Flags: uninsneveruninstall

[Icons]
Name: "{group}\MSHV-IC8TEM"; Filename: "{app}\MSHV-IC8TEM.exe"; WorkingDir: "{app}"
Name: "{group}\{cm:UninstallProgram,MSHV-IC8TEM}"; Filename: "{uninstallexe}"
Name: "{userdesktop}\MSHV-IC8TEM"; Filename: "{app}\MSHV-IC8TEM.exe"; WorkingDir: "{app}"; Tasks: desktopicon

[Run]
Filename: "{app}\MSHV-IC8TEM.exe"; Description: "{cm:LaunchProgram,MSHV-IC8TEM}"; WorkingDir: "{app}"; Flags: nowait postinstall skipifsilent
; automatic update started by MSHV (setup.exe /SILENT /AUTOUPDATE=1): start MSHV again
Filename: "{app}\MSHV-IC8TEM.exe"; WorkingDir: "{app}"; Flags: nowait; Check: IsAutoUpdate

[Code]
function IsAutoUpdate(): Boolean;
begin
  Result := ExpandConstant('{param:AUTOUPDATE|0}') = '1';
end;

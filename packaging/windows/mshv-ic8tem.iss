; MSHV-IC8TEM - Windows installer (Inno Setup 6)
; ISCC /DAppVersion=2.76.7-ic8tem.2 /DSrcDir=<folder with MSHV-IC8TEM.exe> /DArch=x64|x86|arm64 /DOutputName=<name> mshv-ic8tem.iss
;
; Installs per user (no administrator rights) in C:\MSHV-IC8TEM-NEW (from 2.76.7-ic8tem.20;
; before: %LOCALAPPDATA%\Programs\MSHV-IC8TEM), so MSHV can keep its settings and logs next to the
; program, as in the zip version. Settings and logs of the previous folder are copied once.
; Existing settings/logs are never overwritten by an update and never removed by uninstall.
;
; More installations side by side (for example one per radio, each in its own folder): the automatic
; update of MSHV passes /DIR=<its folder> and /AUTOUPDATE=2, so every installation updates itself
; (from 2.76.7-ic8tem.31); shortcuts of a folder other than the default one carry the folder name.

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
DefaultDirName={sd}\MSHV-IC8TEM-NEW
UsePreviousAppDir=no
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
; automatic update: the shortcuts are left as they are
Name: "{group}\{code:IconName}"; Filename: "{app}\MSHV-IC8TEM.exe"; WorkingDir: "{app}"; Check: not IsAutoUpdate
Name: "{group}\{code:UninstallIconName}"; Filename: "{uninstallexe}"; Check: not IsAutoUpdate
Name: "{userdesktop}\{code:IconName}"; Filename: "{app}\MSHV-IC8TEM.exe"; WorkingDir: "{app}"; Tasks: desktopicon; Check: not IsAutoUpdate

[Run]
Filename: "{app}\MSHV-IC8TEM.exe"; Description: "{cm:LaunchProgram,MSHV-IC8TEM}"; WorkingDir: "{app}"; Flags: nowait postinstall skipifsilent
; automatic update started by MSHV up to 2.76.7-ic8tem.30 (setup.exe /SILENT /AUTOUPDATE=1): start MSHV again
; (from ic8tem.31 MSHV passes /AUTOUPDATE=2 and starts itself again with its own command line)
Filename: "{app}\MSHV-IC8TEM.exe"; WorkingDir: "{app}"; Flags: nowait; Check: IsAutoUpdateRestart

[Code]
var
  OldDir: String;
  FreshDir: Boolean;

{ /AUTOUPDATE=1 (MSHV up to ic8tem.30) or /AUTOUPDATE=2 (MSHV from ic8tem.31) }
function IsAutoUpdate(): Boolean;
begin
  Result := ExpandConstant('{param:AUTOUPDATE|0}') <> '0';
end;

function IsAutoUpdateRestart(): Boolean;
begin
  Result := ExpandConstant('{param:AUTOUPDATE|0}') = '1';
end;

{ shortcut name: "MSHV-IC8TEM" in the default folder, "MSHV-IC8TEM (<folder>)" elsewhere, so a second
  installation does not take the shortcuts of the first one }
function IconName(Param: String): String;
var
  Dir, Name: String;
begin
  Dir := RemoveBackslashUnlessRoot(ExpandConstant('{app}'));
  if CompareText(Dir, ExpandConstant('{sd}\MSHV-IC8TEM-NEW')) = 0 then
    Result := 'MSHV-IC8TEM'
  else
  begin
    Name := ExtractFileName(Dir);
    if Name = '' then Name := Copy(Dir, 1, 1);
    Result := 'MSHV-IC8TEM (' + Name + ')';
  end;
end;

function UninstallIconName(Param: String): String;
begin
  Result := FmtMessage(CustomMessage('UninstallProgram'), [IconName('')]);
end;

{ previous installation folder (before C:\MSHV-IC8TEM-NEW) }
function InitializeSetup(): Boolean;
var
  s: String;
begin
  OldDir := '';
  if RegQueryStringValue(HKCU, 'Software\Microsoft\Windows\CurrentVersion\Uninstall\{760ED237-B6DF-446D-961A-7B9D00FAE8A0}_is1', 'InstallLocation', s) then
    OldDir := RemoveBackslashUnlessRoot(s);
  if (OldDir = '') and DirExists(ExpandConstant('{userpf}\MSHV-IC8TEM')) then
    OldDir := ExpandConstant('{userpf}\MSHV-IC8TEM');
  Result := True;
end;

{ copies a folder tree; existing files in the destination are kept }
procedure CopyTree(const Src, Dst: String);
var
  F: TFindRec;
begin
  if not DirExists(Src) then exit;
  ForceDirectories(Dst);
  if FindFirst(Src + '\*', F) then
  begin
    try
      repeat
        if (F.Name <> '.') and (F.Name <> '..') then
        begin
          if (F.Attributes and FILE_ATTRIBUTE_DIRECTORY) <> 0 then
            CopyTree(Src + '\' + F.Name, Dst + '\' + F.Name)
          else if not FileExists(Dst + '\' + F.Name) then
            FileCopy(Src + '\' + F.Name, Dst + '\' + F.Name, True);
        end;
      until not FindNext(F);
    finally
      FindClose(F);
    end;
  end;
end;

{ settings and logs of the previous folder move to the new one (copied, the old folder is left as it is) }
procedure CurStepChanged(CurStep: TSetupStep);
var
  NewDir, Marker: String;
begin
  { only into a new folder (ic8tem.31): with two installations the registered folder can be the other
    one, and an installation that already has its settings must never get those of another }
  if CurStep = ssInstall then
  begin
    FreshDir := not FileExists(ExpandConstant('{app}\settings\ms_settings'));
    exit;
  end;
  if CurStep <> ssPostInstall then exit;
  if IsAutoUpdate() or not FreshDir then exit;
  NewDir := ExpandConstant('{app}');
  if (OldDir = '') or (CompareText(OldDir, NewDir) = 0) or not DirExists(OldDir) then exit;
  Marker := NewDir + '\settings\.migrated-from-old-folder';
  if FileExists(Marker) then exit;
  { the seed files installed now are replaced by the user's own ones }
  if DirExists(OldDir + '\settings') then
    DelTree(NewDir + '\settings', True, True, True);
  CopyTree(OldDir + '\settings', NewDir + '\settings');
  CopyTree(OldDir + '\log', NewDir + '\log');
  CopyTree(OldDir + '\RxWavs', NewDir + '\RxWavs');
  CopyTree(OldDir + '\ExportLog', NewDir + '\ExportLog');
  CopyTree(OldDir + '\AllTxtMonthly', NewDir + '\AllTxtMonthly');
  CopyTree(OldDir + '\Screenshots', NewDir + '\Screenshots');
  ForceDirectories(NewDir + '\settings');
  SaveStringToFile(Marker, 'settings and logs copied from ' + OldDir, False);
end;

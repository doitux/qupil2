; SPDX-FileCopyrightText: 2006-2026 Felix Hammer
; SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
;
; Windows installer (Inno Setup 6), built in CI from the deployed folder (cmake --install):
;   ISCC /DSourceDir=<dist> /DAppExe=bin\Qupil.exe "/DAppVersion=2.0.0 (nightly-abc1234)"
;        /DFileVersion=2.0.0.123 /DOutputDir=<package> /DOutputName=Qupil-Windows-x64-Setup-... qupil.iss
;
; Always the same AppId: a newer installer finds the installed Qupil and updates it in place
; (same folder, same start menu entry, one entry under "Installed apps") instead of putting
; a second copy next to it. Without admin rights it installs for the current user
; (%LOCALAPPDATA%\Programs\Qupil); an update keeps the choice made the first time.
; Qupil's data (%APPDATA%\Qupil) is never touched, also not when uninstalling.
; Saved as UTF-8 with BOM (umlauts in the messages).

#ifndef SourceDir
  #error SourceDir (the deployed Qupil folder) is missing
#endif
#ifndef AppExe
  #define AppExe "bin\Qupil.exe"
#endif
#ifndef AppVersion
  #define AppVersion "2.0.0"
#endif
#ifndef FileVersion
  #define FileVersion "2.0.0.0"
#endif
#ifndef OutputDir
  #define OutputDir "."
#endif
#ifndef OutputName
  #define OutputName "Qupil-Windows-x64-Setup"
#endif

; Never change: it is how an update recognises an installed Qupil
#define AppGuid "70C98A36-44FC-4D45-9D00-3B795E3A8E87"
#define UninstallKey "Software\Microsoft\Windows\CurrentVersion\Uninstall\{" + AppGuid + "}_is1"
#define AppExeDir ExtractFileDir(AppExe)

[Setup]
AppId={{{#AppGuid}}
AppName=Qupil
AppVersion={#AppVersion}
AppVerName=Qupil {#AppVersion}
AppPublisher=Felix Hammer
AppPublisherURL=https://qupil.de
AppSupportURL=https://qupil.de
AppUpdatesURL=https://github.com/doitux/qupil2/releases
AppCopyright=© 2006–2026 Felix Hammer
VersionInfoVersion={#FileVersion}
VersionInfoProductName=Qupil
VersionInfoDescription=Qupil Setup
DefaultDirName={autopf}\Qupil
DefaultGroupName=Qupil
DisableProgramGroupPage=yes
; An update installs into the folder of the installed Qupil without asking
DisableDirPage=auto
PrivilegesRequired=lowest
PrivilegesRequiredOverridesAllowed=dialog commandline
ArchitecturesAllowed=x64compatible
ArchitecturesInstallIn64BitMode=x64compatible
MinVersion=10.0
WizardStyle=modern
SetupIconFile=qupil.ico
UninstallDisplayIcon={app}\{#AppExe}
UninstallDisplayName=Qupil
OutputDir={#OutputDir}
OutputBaseFilename={#OutputName}
Compression=lzma2/max
SolidCompression=yes
; A running Qupil is closed for the update (it asks first)
CloseApplications=yes
RestartApplications=no

[Languages]
Name: "de"; MessagesFile: "compiler:Languages\German.isl"
Name: "en"; MessagesFile: "compiler:Default.isl"

[CustomMessages]
de.UpdateTitle=Bereit zur Aktualisierung
de.UpdateDescription=Qupil wird auf die neue Version gebracht.
de.UpdateReady=Qupil %1 ist installiert und wird auf %2 aktualisiert.%n%nIhre Daten (Schüler, Notizen, Einstellungen) bleiben erhalten.
de.UpdateButton=&Aktualisieren
en.UpdateTitle=Ready to update
en.UpdateDescription=Qupil is brought up to the new version.
en.UpdateReady=Qupil %1 is installed and will be updated to %2.%n%nYour data (students, notes, settings) stays as it is.
en.UpdateButton=&Update

[Tasks]
Name: "desktopicon"; Description: "{cm:CreateDesktopIcon}"; GroupDescription: "{cm:AdditionalIcons}"; Flags: unchecked

; Before copying: the folders and files of the new build replace those of the old one
; completely, so nothing left over from an older Qt or plugin stays behind
[InstallDelete]
#define FindHandle
#define FindResult
#sub DeleteOldEntry
  #define EntryName FindGetFileName(FindHandle)
  #if EntryName != "." && EntryName != ".."
Type: filesandordirs; Name: "{app}\{#EntryName}"
  #endif
#endsub
#for {FindHandle = FindResult = FindFirst(SourceDir + "\*", faAnyFile); FindResult; FindResult = FindNext(FindHandle)} DeleteOldEntry
#if FindHandle
  #expr FindClose(FindHandle)
#endif

[Files]
Source: "{#SourceDir}\*"; DestDir: "{app}"; Flags: ignoreversion recursesubdirs createallsubdirs

[Icons]
Name: "{autoprograms}\Qupil"; Filename: "{app}\{#AppExe}"; WorkingDir: "{app}\{#AppExeDir}"
Name: "{autodesktop}\Qupil"; Filename: "{app}\{#AppExe}"; WorkingDir: "{app}\{#AppExeDir}"; Tasks: desktopicon

[Run]
Filename: "{app}\{#AppExe}"; Description: "{cm:LaunchProgram,Qupil}"; Flags: nowait postinstall skipifsilent

[Code]
var
  InstalledVersion: String;

// The version of an installed Qupil (for the current user or for all users), or ''
function FindInstalledVersion(): String;
begin
  Result := '';
  if RegQueryStringValue(HKCU, '{#UninstallKey}', 'DisplayVersion', Result) then
    exit;
  if RegQueryStringValue(HKLM64, '{#UninstallKey}', 'DisplayVersion', Result) then
    exit;
  if RegQueryStringValue(HKLM32, '{#UninstallKey}', 'DisplayVersion', Result) then
    exit;
  Result := '';
end;

function InitializeSetup(): Boolean;
begin
  InstalledVersion := FindInstalledVersion();
  Result := True;
end;

procedure CurPageChanged(CurPageID: Integer);
begin
  // An update: say so instead of "ready to install"
  if (InstalledVersion <> '') and (CurPageID = wpReady) then
  begin
    WizardForm.PageNameLabel.Caption := CustomMessage('UpdateTitle');
    WizardForm.PageDescriptionLabel.Caption := CustomMessage('UpdateDescription');
    WizardForm.ReadyLabel.Caption := FmtMessage(CustomMessage('UpdateReady'), [InstalledVersion, '{#AppVersion}']);
    WizardForm.NextButton.Caption := CustomMessage('UpdateButton');
  end;
end;

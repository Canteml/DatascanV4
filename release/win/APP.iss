#define MyAppName "DatascanV2"
#define MyAppVersion "1.0.6-BETA-FR"
#define MyAppPublisher "Roboplanet"
#define MyAppURL "http://www.roboplanet.fr"
#define MyAppExeName "DatascanV2.exe"
 
[Setup]
PrivilegesRequired=admin
AppId={{18A43CCC-3F2B-4F4B-8247-4EEFA600A762}
AppName={#MyAppName}
AppVersion={#MyAppVersion}
AppPublisher={#MyAppPublisher}
AppPublisherURL={#MyAppURL}
AppSupportURL={#MyAppURL}
AppUpdatesURL={#MyAppURL}
DefaultDirName={autopf}\{#MyAppPublisher}\{#MyAppName}
DefaultGroupName={#MyAppName}
OutputBaseFilename={#MyAppName}_{#MyAppVersion}
Compression=lzma
SolidCompression=yes
  
[Tasks]
Name: "desktopicon"; Description: "{cm:CreateDesktopIcon}"; GroupDescription: "{cm:AdditionalIcons}";
Name: "quicklaunchicon"; Description: "{cm:CreateQuickLaunchIcon}"; GroupDescription: "{cm:AdditionalIcons}";
 
[Files]
Source: "app\bin\*"; DestDir: "{app}\bin"; Flags: ignoreversion recursesubdirs
Source: "app\res\*"; DestDir: "{app}\res"; Flags: ignoreversion recursesubdirs
Source: "tmp\banner.bmp"; Flags: dontcopy
 
[Icons]               
Name: "{group}\{#MyAppName}"; Filename: "{app}\bin\{#MyAppExeName}"; WorkingDir: "{app}"
Name: "{userdesktop}\{#MyAppName}"; Filename: "{app}\bin\{#MyAppExeName}"; WorkingDir: "{app}"; Tasks: desktopicon               

[Code]
procedure InitializeWizard();
var
  BitmapImage: TBitmapImage;
begin
  ExtractTemporaryFile('banner.bmp');
  BitmapImage := TBitmapImage.Create(WizardForm);
  BitmapImage.Parent := WizardForm.MainPanel;
  BitmapImage.Height := WizardForm.MainPanel.Height;
  BitmapImage.Anchors := [akLeft, akTop, akRight, akBottom];
  BitmapImage.Stretch := True;
  BitmapImage.AutoSize := True;
  BitmapImage.Left := 20;
  BitmapImage.Top := 5;

  BitmapImage.Bitmap.LoadFromFile(ExpandConstant('{tmp}\banner.bmp'));

  WizardForm.WizardSmallBitmapImage.Visible := False;
  WizardForm.PageDescriptionLabel.Visible := False;
  WizardForm.PageNameLabel.Visible := False;
end;               

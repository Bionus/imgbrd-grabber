#define MyAppName "Grabber"
#define MyAppVersion "5.5.2"
#define MyAppPublisher "Bionus"
#define MyAppURL "https://github.com/Bionus/imgbrd-grabber"
#define MyAppExeName "Grabber.exe"

[Setup]
; NOTE: The value of AppId uniquely identifies this application.
; Do not use the same AppId value in installers for other applications.
; (To generate a new GUID, click Tools | Generate GUID inside the IDE.)
AppId={{8C007AE6-3F7D-41CC-AB7C-75C08C276EC8}
AppName={#MyAppName}
AppVersion={#MyAppVersion}
;AppVerName={#MyAppName} {#MyAppVersion}
AppPublisher={#MyAppPublisher}
AppPublisherURL={#MyAppURL}
AppSupportURL={#MyAppURL}
AppUpdatesURL={#MyAppURL}
DefaultDirName={pf}\{#MyAppName}
DefaultGroupName={#MyAppName}
AllowNoIcons=yes
LicenseFile=Apache license 2.0.txt
OutputDir=.
OutputBaseFilename=Grabber_{#MyAppVersion}
SetupIconFile=resources\images\icon.ico
UninstallDisplayName="{#MyAppName} {#MyAppVersion}"
UninstallDisplayIcon={app}\icon.ico
WizardSmallImageFile=resources\images\icon.bmp
Compression=lzma
SolidCompression=yes
ChangesAssociations=yes
DisableDirPage=no

[CustomMessages]
en.IGL=Imageboard-Grabber Links
fr.IGL=Liens Imageboard-Grabber
vcredist2015_title=Visual C++ 2015 Redistributable
vcredist2015_title_x64=Visual C++ 2015 64-Bit Redistributable
fr.vcredist2015_size=12.8 Mo
fr.vcredist2015_size_x64=13.9 Mo

[Registry]
Root: HKCR; Subkey: ".igl"; ValueType: string; ValueName: ""; ValueData: "Imageboard-Grabber"; Flags: uninsdeletevalue
Root: HKCR; Subkey: "Imageboard-Grabber"; ValueType: string; ValueName: ""; ValueData: "{cm:IGL}"; Flags: uninsdeletekey
Root: HKCR; Subkey: "Imageboard-Grabber\DefaultIcon"; ValueType: string; ValueName: ""; ValueData: "{app}\{#MyAppExeName},0"
Root: HKCR; Subkey: "Imageboard-Grabber\shell\open\command"; ValueType: string; ValueName: ""; ValueData: """{app}\{#MyAppExeName}"" ""%1"""

[Languages]
Name: "en"; MessagesFile: "compiler:Default.isl"
Name: "de"; MessagesFile: "compiler:Languages\German.isl"
Name: "fr"; MessagesFile: "compiler:Languages\French.isl"
Name: "it"; MessagesFile: "compiler:Languages\Italian.isl"
Name: "nl"; MessagesFile: "compiler:Languages\Dutch.isl"
Name: "pl"; MessagesFile: "compiler:Languages\Polish.isl"

#include "scripts\products.iss"
#include "scripts\products\msiproduct.iss"
#include "scripts\products\vcredist2015.iss"

[Tasks]
Name: "desktopicon"; Description: "{cm:CreateDesktopIcon}"; GroupDescription: "{cm:AdditionalIcons}"; Flags: unchecked
Name: "quicklaunchicon"; Description: "{cm:CreateQuickLaunchIcon}"; GroupDescription: "{cm:AdditionalIcons}"; Flags: unchecked; OnlyBelowVersion: 0,6.1

[Files]
Source: "resources\images\icon.ico";            DestDir: "{app}"; Flags: ignoreversion
Source: "..\release\CDR.exe";                   DestDir: "{app}"; Flags: ignoreversion
Source: "..\build\CrashReporter\CrashReporter.exe"; DestDir: "{app}"; Flags: ignoreversion
Source: "..\build\cli\Grabber-cli.exe";         DestDir: "{app}"; Flags: ignoreversion; DestName: "Grabber.com"
Source: "..\build\gui\Grabber.exe";             DestDir: "{app}"; Flags: ignoreversion;
Source: "..\release\words.txt";                 DestDir: "{app}";
Source: "..\release\libeay32.dll";              DestDir: "{app}"; Flags: ignoreversion
Source: "..\release\libEGL.dll";                DestDir: "{app}"; Flags: ignoreversion
Source: "..\release\libGLESv2.dll";             DestDir: "{app}"; Flags: ignoreversion
Source: "..\release\libmysql.dll";              DestDir: "{app}"; Flags: ignoreversion
Source: "..\release\libssl32.dll";              DestDir: "{app}"; Flags: ignoreversion
Source: "..\release\qscintilla2.dll";           DestDir: "{app}"; Flags: ignoreversion
Source: "..\release\Qt5Core.dll";               DestDir: "{app}"; Flags: ignoreversion
Source: "..\release\Qt5Gui.dll";                DestDir: "{app}"; Flags: ignoreversion
Source: "..\release\Qt5Multimedia.dll";         DestDir: "{app}"; Flags: ignoreversion
Source: "..\release\Qt5Network.dll";            DestDir: "{app}"; Flags: ignoreversion
Source: "..\release\Qt5OpenGL.dll";             DestDir: "{app}"; Flags: ignoreversion
Source: "..\release\Qt5PrintSupport.dll";       DestDir: "{app}"; Flags: ignoreversion
Source: "..\release\Qt5Qml.dll";                DestDir: "{app}"; Flags: ignoreversion
Source: "..\release\Qt5Sql.dll";                DestDir: "{app}"; Flags: ignoreversion
Source: "..\release\Qt5Widgets.dll";            DestDir: "{app}"; Flags: ignoreversion
Source: "..\release\Qt5Xml.dll";                DestDir: "{app}"; Flags: ignoreversion
Source: "..\release\Qt5WinExtras.dll";          DestDir: "{app}"; Flags: ignoreversion
Source: "..\release\ssleay32.dll";              DestDir: "{app}"; Flags: ignoreversion
Source: "..\release\crashreporter\ChineseSimplified.qm"; DestDir: "{app}\crashreporter"; Flags: ignoreversion
Source: "..\release\crashreporter\English.qm";  DestDir: "{app}\crashreporter"; Flags: ignoreversion
Source: "..\release\crashreporter\French.qm";   DestDir: "{app}\crashreporter"; Flags: ignoreversion
Source: "..\release\crashreporter\Russian.qm";  DestDir: "{app}\crashreporter"; Flags: ignoreversion
Source: "..\release\crashreporter\Spanish.qm";  DestDir: "{app}\crashreporter"; Flags: ignoreversion
Source: "..\release\imageformats\qdds.dll";     DestDir: "{app}\imageformats"; Flags: ignoreversion
Source: "..\release\imageformats\qgif.dll";     DestDir: "{app}\imageformats"; Flags: ignoreversion
Source: "..\release\imageformats\qicns.dll";    DestDir: "{app}\imageformats"; Flags: ignoreversion
Source: "..\release\imageformats\qico.dll";     DestDir: "{app}\imageformats"; Flags: ignoreversion
Source: "..\release\imageformats\qjpeg.dll";    DestDir: "{app}\imageformats"; Flags: ignoreversion
Source: "..\release\imageformats\qsvg.dll";     DestDir: "{app}\imageformats"; Flags: ignoreversion
Source: "..\release\imageformats\qtga.dll";     DestDir: "{app}\imageformats"; Flags: ignoreversion
Source: "..\release\imageformats\qtiff.dll";    DestDir: "{app}\imageformats"; Flags: ignoreversion
Source: "..\release\imageformats\qwbmp.dll";    DestDir: "{app}\imageformats"; Flags: ignoreversion
Source: "..\release\imageformats\qwebp.dll";    DestDir: "{app}\imageformats"; Flags: ignoreversion
Source: "..\release\languages\ChineseSimplified.qm"; DestDir: "{app}\languages"; Flags: ignoreversion
Source: "..\release\languages\English.qm";      DestDir: "{app}\languages"; Flags: ignoreversion
Source: "..\release\languages\French.qm";       DestDir: "{app}\languages"; Flags: ignoreversion
Source: "..\release\languages\Russian.qm";      DestDir: "{app}\languages"; Flags: ignoreversion
Source: "..\release\languages\Spanish.qm";      DestDir: "{app}\languages"; Flags: ignoreversion
Source: "..\release\languages\languages.ini";   DestDir: "{app}\languages"; Flags: ignoreversion
Source: "..\release\languages\qt\English.qm";      DestDir: "{app}\languages\qt"; Flags: ignoreversion
Source: "..\release\languages\qt\French.qm";       DestDir: "{app}\languages\qt"; Flags: ignoreversion
Source: "..\release\languages\qt\Russian.qm";      DestDir: "{app}\languages\qt"; Flags: ignoreversion
Source: "..\release\platforms\qminimal.dll";    DestDir: "{app}\platforms"; Flags: ignoreversion
Source: "..\release\platforms\qoffscreen.dll";  DestDir: "{app}\platforms"; Flags: ignoreversion
Source: "..\release\platforms\qwindows.dll";    DestDir: "{app}\platforms"; Flags: ignoreversion
Source: "..\release\sqldrivers\qsqlite.dll";    DestDir: "{app}\sqldrivers"; Flags: ignoreversion
Source: "..\release\sqldrivers\qsqlmysql.dll";  DestDir: "{app}\sqldrivers"; Flags: ignoreversion
Source: "..\release\sqldrivers\qsqlodbc.dll";   DestDir: "{app}\sqldrivers"; Flags: ignoreversion
Source: "..\release\sqldrivers\qsqlpsql.dll";   DestDir: "{app}\sqldrivers"; Flags: ignoreversion
Source: "..\release\sites\Anime pictures\icon.png";     DestDir: "{localappdata}\Bionus\Grabber\sites\Anime pictures"; Flags: ignoreversion
Source: "..\release\sites\Anime pictures\model.xml";    DestDir: "{localappdata}\Bionus\Grabber\sites\Anime pictures"; Flags: ignoreversion
Source: "..\release\sites\Anime pictures\sites.txt";    DestDir: "{localappdata}\Bionus\Grabber\sites\Anime pictures"; Flags: onlyifdoesntexist
Source: "..\release\sites\Anime pictures\anime-pictures.net\defaults.ini"; DestDir: "{localappdata}\Bionus\Grabber\sites\Anime pictures\anime-pictures.net"; Flags: ignoreversion
Source: "..\release\sites\Booru-on-rails\icon.png";     DestDir: "{localappdata}\Bionus\Grabber\sites\Booru-on-rails"; Flags: ignoreversion
Source: "..\release\sites\Booru-on-rails\model.xml";    DestDir: "{localappdata}\Bionus\Grabber\sites\Booru-on-rails"; Flags: ignoreversion
Source: "..\release\sites\Booru-on-rails\sites.txt";    DestDir: "{localappdata}\Bionus\Grabber\sites\Booru-on-rails"; Flags: onlyifdoesntexist
Source: "..\release\sites\Danbooru\icon.png";           DestDir: "{localappdata}\Bionus\Grabber\sites\Danbooru"; Flags: ignoreversion
Source: "..\release\sites\Danbooru\model.xml";          DestDir: "{localappdata}\Bionus\Grabber\sites\Danbooru"; Flags: ignoreversion
Source: "..\release\sites\Danbooru\sites.txt";          DestDir: "{localappdata}\Bionus\Grabber\sites\Danbooru"; Flags: onlyifdoesntexist
Source: "..\release\sites\Danbooru\behoimi.org\defaults.ini";              DestDir: "{localappdata}\Bionus\Grabber\sites\Danbooru\behoimi.org"; Flags: ignoreversion
Source: "..\release\sites\Danbooru\behoimi.org\tag-types.txt";             DestDir: "{localappdata}\Bionus\Grabber\sites\Danbooru\behoimi.org"; Flags: onlyifdoesntexist
Source: "..\release\sites\Danbooru\e621.net\defaults.ini";                 DestDir: "{localappdata}\Bionus\Grabber\sites\Danbooru\e621.net"; Flags: ignoreversion
Source: "..\release\sites\Danbooru\e621.net\tag-types.txt";                DestDir: "{localappdata}\Bionus\Grabber\sites\Danbooru\e621.net"; Flags: onlyifdoesntexist
Source: "..\release\sites\Sankaku\icon.png";            DestDir: "{localappdata}\Bionus\Grabber\sites\Sankaku"; Flags: ignoreversion
Source: "..\release\sites\Sankaku\model.xml";           DestDir: "{localappdata}\Bionus\Grabber\sites\Sankaku"; Flags: ignoreversion
Source: "..\release\sites\Sankaku\sites.txt";           DestDir: "{localappdata}\Bionus\Grabber\sites\Sankaku"; Flags: onlyifdoesntexist
Source: "..\release\sites\Sankaku\chan.sankakucomplex.com\defaults.ini";  DestDir: "{localappdata}\Bionus\Grabber\sites\Sankaku\chan.sankakucomplex.com"; Flags: ignoreversion
Source: "..\release\sites\Sankaku\idol.sankakucomplex.com\defaults.ini";  DestDir: "{localappdata}\Bionus\Grabber\sites\Sankaku\idol.sankakucomplex.com"; Flags: ignoreversion
Source: "..\release\sites\Danbooru (2.0)\icon.png";     DestDir: "{localappdata}\Bionus\Grabber\sites\Danbooru (2.0)"; Flags: ignoreversion
Source: "..\release\sites\Danbooru (2.0)\model.xml";    DestDir: "{localappdata}\Bionus\Grabber\sites\Danbooru (2.0)"; Flags: ignoreversion
Source: "..\release\sites\Danbooru (2.0)\sites.txt";    DestDir: "{localappdata}\Bionus\Grabber\sites\Danbooru (2.0)"; Flags: onlyifdoesntexist
Source: "..\release\sites\Danbooru (2.0)\danbooru.donmai.us\defaults.ini";              DestDir: "{localappdata}\Bionus\Grabber\sites\Danbooru (2.0)\danbooru.donmai.us"; Flags: ignoreversion
Source: "..\release\sites\Danbooru (2.0)\danbooru.donmai.us\tag-types.txt";             DestDir: "{localappdata}\Bionus\Grabber\sites\Danbooru (2.0)\danbooru.donmai.us"; Flags: onlyifdoesntexist
Source: "..\release\sites\Gelbooru (0.1)\icon.png";     DestDir: "{localappdata}\Bionus\Grabber\sites\Gelbooru (0.1)"; Flags: ignoreversion
Source: "..\release\sites\Gelbooru (0.1)\model.xml";    DestDir: "{localappdata}\Bionus\Grabber\sites\Gelbooru (0.1)"; Flags: ignoreversion
Source: "..\release\sites\Gelbooru (0.1)\sites.txt";    DestDir: "{localappdata}\Bionus\Grabber\sites\Gelbooru (0.1)"; Flags: onlyifdoesntexist
Source: "..\release\sites\Gelbooru (0.2)\icon.png";     DestDir: "{localappdata}\Bionus\Grabber\sites\Gelbooru (0.2)"; Flags: ignoreversion
Source: "..\release\sites\Gelbooru (0.2)\model.xml";    DestDir: "{localappdata}\Bionus\Grabber\sites\Gelbooru (0.2)"; Flags: ignoreversion
Source: "..\release\sites\Gelbooru (0.2)\sites.txt";    DestDir: "{localappdata}\Bionus\Grabber\sites\Gelbooru (0.2)"; Flags: onlyifdoesntexist
Source: "..\release\sites\Gelbooru (0.2)\gelbooru.com\defaults.ini"; DestDir: "{localappdata}\Bionus\Grabber\sites\Gelbooru (0.2)\gelbooru.com"; Flags: ignoreversion
Source: "..\release\sites\Moebooru\icon.png";  DestDir: "{localappdata}\Bionus\Grabber\sites\Moebooru"; Flags: ignoreversion
Source: "..\release\sites\Moebooru\model.xml"; DestDir: "{localappdata}\Bionus\Grabber\sites\Moebooru"; Flags: ignoreversion
Source: "..\release\sites\Moebooru\sites.txt"; DestDir: "{localappdata}\Bionus\Grabber\sites\Moebooru"; Flags: onlyifdoesntexist
Source: "..\release\sites\Shimmie\icon.png";   DestDir: "{localappdata}\Bionus\Grabber\sites\Shimmie"; Flags: ignoreversion
Source: "..\release\sites\Shimmie\model.xml";  DestDir: "{localappdata}\Bionus\Grabber\sites\Shimmie"; Flags: ignoreversion
Source: "..\release\sites\Shimmie\sites.txt";  DestDir: "{localappdata}\Bionus\Grabber\sites\Shimmie"; Flags: onlyifdoesntexist
Source: "..\release\sites\Zerochan\icon.png";  DestDir: "{localappdata}\Bionus\Grabber\sites\Zerochan"; Flags: ignoreversion
Source: "..\release\sites\Zerochan\model.xml"; DestDir: "{localappdata}\Bionus\Grabber\sites\Zerochan"; Flags: ignoreversion
Source: "..\release\sites\Zerochan\sites.txt"; DestDir: "{localappdata}\Bionus\Grabber\sites\Zerochan"; Flags: onlyifdoesntexist
Source: "..\release\sites\Zerochan\www.zerochan.net\defaults.ini"; DestDir: "{localappdata}\Bionus\Grabber\sites\Zerochan\www.zerochan.net"; Flags: ignoreversion
Source: "..\release\themes\Default\*"; DestDir: "{localappdata}\Bionus\Grabber\themes\Default"; Flags: onlyifdoesntexist recursesubdirs
Source: "..\release\themes\QDarkStyleSheet\*"; DestDir: "{localappdata}\Bionus\Grabber\themes\QDarkStyleSheet"; Flags: onlyifdoesntexist recursesubdirs
Source: "..\release\webservices\*.ico"; DestDir: "{localappdata}\Bionus\Grabber\webservices"; Flags: onlyifdoesntexist

[InstallDelete]
Type: filesandordirs; Name: "{app}\languages\updater"
Type: filesandordirs; Name: "{app}\sites"
Type: files; Name: "{app}\Updater.exe"
Type: files; Name: "{app}\VERSION"
Type: files; Name: "{app}\MD5"
Type: files; Name: "{app}\libgcc_s_dw2-1.dll"
Type: files; Name: "{app}\QtCore4.dll"
Type: files; Name: "{app}\QtGui4.dll"
Type: files; Name: "{app}\QtNetwork4.dll"
Type: files; Name: "{app}\QtScript4.dll"
Type: files; Name: "{app}\QtSql4.dll"
Type: files; Name: "{app}\QtXml4.dll"
Type: files; Name: "{app}\imageformats\qgif4.dll"
Type: files; Name: "{app}\imageformats\qico4.dll"
Type: files; Name: "{app}\imageformats\qjpeg4.dll"
Type: files; Name: "{app}\imageformats\qmng4.dll"
Type: files; Name: "{app}\imageformats\qsvg4.dll"
Type: files; Name: "{app}\imageformats\qtga4.dll"
Type: files; Name: "{app}\imageformats\qtiff4.dll"
Type: files; Name: "{app}\sqldrivers\qsqlmysql4.dll"

[UninstallDelete]
Type: filesandordirs; Name: "{%UserProfile}\Grabber"

[Icons]
Name: "{group}\{#MyAppName}"; Filename: "{app}\{#MyAppExeName}"
Name: "{group}\{cm:UninstallProgram,{#MyAppName}}"; Filename: "{uninstallexe}"
Name: "{commondesktop}\{#MyAppName}"; Filename: "{app}\{#MyAppExeName}"; Tasks: desktopicon
Name: "{userappdata}\Microsoft\Internet Explorer\Quick Launch\{#MyAppName}"; Filename: "{app}\{#MyAppExeName}"; Tasks: quicklaunchicon

[Run]
Filename: "{app}\{#MyAppExeName}"; Description: "{cm:LaunchProgram,{#StringChange(MyAppName, "&", "&&")}}"; Flags: nowait postinstall skipifsilent

[Code]
function InitializeSetup(): Boolean;
begin
  vcredist2015();
  Result := true;
end;

procedure CurStepChanged(CurStep: TSetupStep);
begin
  if CurStep = ssPostInstall then
  begin
    Log('Post install');
    SaveStringToFile(ExpandConstant('{localappdata}') + '\Bionus\Grabber\innosetup.ini', '[general]' + #13#10 + 'language=' + ExpandConstant('{language}') + #13#10, False);
  end;
end;

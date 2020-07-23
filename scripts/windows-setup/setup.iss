#define MyAppName "Grabber"
#define MyAppPublisher "Bionus"
#define MyAppURL "https://github.com/Bionus/imgbrd-grabber"
#define MyAppExeName "Grabber.exe"

#define RootDir "..\.."
#define BuildDir RootDir + "\build"
#define SrcDir RootDir + "\src"

#ifndef QtDir
# define QtDir "C:\Qt\Qt5.10.1\5.10.1\msvc2015\bin"
#endif
#define QtPlugins QtDir + "\..\plugins"
#define QtLib QtDir + "\..\lib"

#ifndef OpenSSLDir
# define OpenSSLDir "C:\bin\OpenSSL-Win32-1.1.1d"
#endif

#ifndef MySQLDir
# define MySQLDir "C:\Program Files (x86)\MySQL\MySQL Server 5.7\lib"
#endif

#ifndef MyAppVersion
# define MyAppVersion "7.3.1"
#endif

#ifndef QtApngDll
# define QtApngDll QtPlugins + "\imageformats\qapng.dll"
#endif

#define PlatformNamePrefix ""
#ifdef PlatformName
# define PlatformNamePrefix "_" + PlatformName
#endif


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
LicenseFile={#RootDir}\LICENSE
OutputDir=.
OutputBaseFilename=Grabber_{#MyAppVersion}{#PlatformNamePrefix}
SetupIconFile=resources\images\icon.ico
UninstallDisplayName="{#MyAppName} {#MyAppVersion}"
UninstallDisplayIcon={app}\icon.ico
WizardSmallImageFile=resources\images\icon.bmp
Compression=lzma
SolidCompression=yes
ChangesAssociations=yes
DisableDirPage=no
PrivilegesRequired=admin
ArchitecturesAllowed=x86 x64 ia64
ArchitecturesInstallIn64BitMode=x64 ia64

; downloading and installing dependencies will only work if the memo/ready page is enabled (default and current behaviour)
DisableReadyPage=no
DisableReadyMemo=no


[CustomMessages]
en.IGL=Imageboard-Grabber Links
fr.IGL=Liens Imageboard-Grabber
DependenciesDir=MyProgramDependencies
WindowsServicePack=Windows %1 Service Pack %2

[Registry]
Root: HKCR; Subkey: ".igl"; ValueType: string; ValueName: ""; ValueData: "Imageboard-Grabber"; Flags: uninsdeletevalue
Root: HKCR; Subkey: "Imageboard-Grabber"; ValueType: string; ValueName: ""; ValueData: "{cm:IGL}"; Flags: uninsdeletekey
Root: HKCR; Subkey: "Imageboard-Grabber\DefaultIcon"; ValueType: string; ValueName: ""; ValueData: "{app}\{#MyAppExeName},0"
Root: HKCR; Subkey: "Imageboard-Grabber\shell\open\command"; ValueType: string; ValueName: ""; ValueData: """{app}\{#MyAppExeName}"" ""%1"""

; Languages
#include "scripts\lang\english.iss"
#include "scripts\lang\german.iss"
#include "scripts\lang\french.iss"
#include "scripts\lang\italian.iss"
#include "scripts\lang\dutch.iss"

#ifdef UNICODE
#include "scripts\lang\chinese.iss"
#include "scripts\lang\polish.iss"
#include "scripts\lang\russian.iss"
#include "scripts\lang\japanese.iss"
#endif

#include "scripts\products.iss"
#include "scripts\products\stringversion.iss"
#include "scripts\products\winversion.iss"
#include "scripts\products\fileversion.iss"
#include "scripts\products\msiproduct.iss"
#include "scripts\products\vcredist2015.iss"
#include "scripts\products\vcredist2017.iss"

[Tasks]
Name: "desktopicon"; Description: "{cm:CreateDesktopIcon}"; GroupDescription: "{cm:AdditionalIcons}"; Flags: unchecked
Name: "quicklaunchicon"; Description: "{cm:CreateQuickLaunchIcon}"; GroupDescription: "{cm:AdditionalIcons}"; Flags: unchecked; OnlyBelowVersion: 0,6.1

[Files]
Source: "resources\images\icon.ico";            DestDir: "{app}"; Flags: ignoreversion
Source: "{#SrcDir}\dist\windows\CDR.exe";       DestDir: "{app}"; Flags: ignoreversion
Source: "{#BuildDir}\CrashReporter\CrashReporter.exe"; DestDir: "{app}"; Flags: ignoreversion
Source: "{#BuildDir}\cli\Grabber-cli.exe";      DestDir: "{app}"; Flags: ignoreversion; DestName: "Grabber.com"
Source: "{#BuildDir}\gui\Grabber.exe";          DestDir: "{app}"; Flags: ignoreversion;
Source: "{#SrcDir}\dist\common\words.txt";      DestDir: "{app}";
Source: "{#OpenSSLDir}\libcrypto-1_1.dll";      DestDir: "{app}"; Flags: ignoreversion skipifsourcedoesntexist
Source: "{#OpenSSLDir}\libcrypto-1_1-x64.dll";  DestDir: "{app}"; Flags: ignoreversion skipifsourcedoesntexist
Source: "{#OpenSSLDir}\libssl-1_1.dll";         DestDir: "{app}"; Flags: ignoreversion skipifsourcedoesntexist
Source: "{#OpenSSLDir}\libssl-1_1-x64.dll";     DestDir: "{app}"; Flags: ignoreversion skipifsourcedoesntexist
Source: "{#QtDir}\libEGL.dll";                  DestDir: "{app}"; Flags: ignoreversion
Source: "{#QtDir}\libGLESv2.dll";               DestDir: "{app}"; Flags: ignoreversion
Source: "{#MySQLDir}\libmysql.dll";             DestDir: "{app}"; Flags: ignoreversion
Source: "{#QtLib}\qscintilla2_qt5.dll";         DestDir: "{app}"; Flags: ignoreversion
Source: "{#QtDir}\Qt5Concurrent.dll";           DestDir: "{app}"; Flags: ignoreversion
Source: "{#QtDir}\Qt5Core.dll";                 DestDir: "{app}"; Flags: ignoreversion
Source: "{#QtDir}\Qt5Gui.dll";                  DestDir: "{app}"; Flags: ignoreversion
Source: "{#QtDir}\Qt5Multimedia.dll";           DestDir: "{app}"; Flags: ignoreversion
Source: "{#QtDir}\Qt5Network.dll";              DestDir: "{app}"; Flags: ignoreversion
Source: "{#QtDir}\Qt5OpenGL.dll";               DestDir: "{app}"; Flags: ignoreversion
Source: "{#QtDir}\Qt5PrintSupport.dll";         DestDir: "{app}"; Flags: ignoreversion
Source: "{#QtDir}\Qt5Qml.dll";                  DestDir: "{app}"; Flags: ignoreversion
Source: "{#QtDir}\Qt5Sql.dll";                  DestDir: "{app}"; Flags: ignoreversion
Source: "{#QtDir}\Qt5Widgets.dll";              DestDir: "{app}"; Flags: ignoreversion
Source: "{#QtDir}\Qt5Xml.dll";                  DestDir: "{app}"; Flags: ignoreversion
Source: "{#QtDir}\Qt5WinExtras.dll";            DestDir: "{app}"; Flags: ignoreversion
Source: "{#BuildDir}\crash-reporter\languages\ChineseSimplified.qm"; DestDir: "{app}\crashreporter"; Flags: ignoreversion
Source: "{#BuildDir}\crash-reporter\languages\English.qm";  DestDir: "{app}\crashreporter"; Flags: ignoreversion
Source: "{#BuildDir}\crash-reporter\languages\French.qm";   DestDir: "{app}\crashreporter"; Flags: ignoreversion
Source: "{#BuildDir}\crash-reporter\languages\German.qm";   DestDir: "{app}\crashreporter"; Flags: ignoreversion
Source: "{#BuildDir}\crash-reporter\languages\Polish.qm";   DestDir: "{app}\crashreporter"; Flags: ignoreversion
Source: "{#BuildDir}\crash-reporter\languages\Russian.qm";  DestDir: "{app}\crashreporter"; Flags: ignoreversion
Source: "{#BuildDir}\crash-reporter\languages\Spanish.qm";  DestDir: "{app}\crashreporter"; Flags: ignoreversion
Source: "{#QtApngDll}";                         DestDir: "{app}\imageformats"; Flags: ignoreversion
Source: "{#QtPlugins}\imageformats\qdds.dll";   DestDir: "{app}\imageformats"; Flags: ignoreversion skipifsourcedoesntexist
Source: "{#QtPlugins}\imageformats\qgif.dll";   DestDir: "{app}\imageformats"; Flags: ignoreversion
Source: "{#QtPlugins}\imageformats\qicns.dll";  DestDir: "{app}\imageformats"; Flags: ignoreversion
Source: "{#QtPlugins}\imageformats\qico.dll";   DestDir: "{app}\imageformats"; Flags: ignoreversion
Source: "{#QtPlugins}\imageformats\qjpeg.dll";  DestDir: "{app}\imageformats"; Flags: ignoreversion
Source: "{#QtPlugins}\imageformats\qsvg.dll";   DestDir: "{app}\imageformats"; Flags: ignoreversion
Source: "{#QtPlugins}\imageformats\qtga.dll";   DestDir: "{app}\imageformats"; Flags: ignoreversion
Source: "{#QtPlugins}\imageformats\qtiff.dll";  DestDir: "{app}\imageformats"; Flags: ignoreversion
Source: "{#QtPlugins}\imageformats\qwbmp.dll";  DestDir: "{app}\imageformats"; Flags: ignoreversion
Source: "{#QtPlugins}\imageformats\qwebp.dll";  DestDir: "{app}\imageformats"; Flags: ignoreversion
Source: "{#BuildDir}\languages\ChineseSimplified.qm"; DestDir: "{app}\languages"; Flags: ignoreversion
Source: "{#BuildDir}\languages\English.qm";     DestDir: "{app}\languages"; Flags: ignoreversion
Source: "{#BuildDir}\languages\French.qm";      DestDir: "{app}\languages"; Flags: ignoreversion
Source: "{#BuildDir}\languages\German.qm";      DestDir: "{app}\languages"; Flags: ignoreversion
Source: "{#BuildDir}\languages\Polish.qm";      DestDir: "{app}\languages"; Flags: ignoreversion
Source: "{#BuildDir}\languages\Russian.qm";     DestDir: "{app}\languages"; Flags: ignoreversion
Source: "{#BuildDir}\languages\Spanish.qm";     DestDir: "{app}\languages"; Flags: ignoreversion
Source: "{#SrcDir}\dist\common\languages\languages.ini";  DestDir: "{app}\languages"; Flags: ignoreversion
Source: "{#QtDir}\..\translations\qtbase_en.qm";   DestDir: "{app}\languages\qt"; DestName: "English.qm"; Flags: ignoreversion
Source: "{#QtDir}\..\translations\qtbase_fr.qm";   DestDir: "{app}\languages\qt"; DestName: "French.qm";  Flags: ignoreversion
Source: "{#QtDir}\..\translations\qtbase_ru.qm";   DestDir: "{app}\languages\qt"; DestName: "Russian.qm"; Flags: ignoreversion
Source: "{#QtPlugins}\platforms\qminimal.dll";     DestDir: "{app}\platforms"; Flags: ignoreversion
Source: "{#QtPlugins}\platforms\qoffscreen.dll";   DestDir: "{app}\platforms"; Flags: ignoreversion
Source: "{#QtPlugins}\platforms\qwindows.dll";     DestDir: "{app}\platforms"; Flags: ignoreversion
Source: "{#QtPlugins}\sqldrivers\qsqlite.dll";     DestDir: "{app}\sqldrivers"; Flags: ignoreversion
Source: "{#QtPlugins}\sqldrivers\qsqlmysql.dll";   DestDir: "{app}\sqldrivers"; Flags: ignoreversion
Source: "{#QtPlugins}\sqldrivers\qsqlodbc.dll";    DestDir: "{app}\sqldrivers"; Flags: ignoreversion
Source: "{#QtPlugins}\sqldrivers\qsqlpsql.dll";    DestDir: "{app}\sqldrivers"; Flags: ignoreversion
Source: "{#QtPlugins}\styles\qwindowsvistastyle.dll";  DestDir: "{app}\styles"; Flags: ignoreversion
Source: "{#SrcDir}\sites\helper.js";                   DestDir: "{localappdata}\Bionus\Grabber\sites"; Flags: ignoreversion
Source: "{#SrcDir}\sites\Anime pictures\icon.png";     DestDir: "{localappdata}\Bionus\Grabber\sites\Anime pictures"; Flags: ignoreversion
Source: "{#SrcDir}\sites\Anime pictures\model.js";     DestDir: "{localappdata}\Bionus\Grabber\sites\Anime pictures"; Flags: ignoreversion
Source: "{#SrcDir}\sites\Anime pictures\sites.txt";    DestDir: "{localappdata}\Bionus\Grabber\sites\Anime pictures"; Flags: onlyifdoesntexist
Source: "{#SrcDir}\sites\Anime pictures\anime-pictures.net\defaults.ini";  DestDir: "{localappdata}\Bionus\Grabber\sites\Anime pictures\anime-pictures.net"; Flags: ignoreversion
Source: "{#SrcDir}\sites\Anime pictures\anime-pictures.net\tag-types.txt"; DestDir: "{localappdata}\Bionus\Grabber\sites\Anime pictures\anime-pictures.net"; Flags: onlyifdoesntexist
Source: "{#SrcDir}\sites\Philomena\icon.png";     DestDir: "{localappdata}\Bionus\Grabber\sites\Philomena"; Flags: ignoreversion
Source: "{#SrcDir}\sites\Philomena\model.js";     DestDir: "{localappdata}\Bionus\Grabber\sites\Philomena"; Flags: ignoreversion
Source: "{#SrcDir}\sites\Philomena\sites.txt";    DestDir: "{localappdata}\Bionus\Grabber\sites\Philomena"; Flags: onlyifdoesntexist
Source: "{#SrcDir}\sites\Philomena\derpibooru.org\defaults.ini"; DestDir: "{localappdata}\Bionus\Grabber\sites\Philomena\derpibooru.org"; Flags: ignoreversion
Source: "{#SrcDir}\sites\Danbooru\icon.png";           DestDir: "{localappdata}\Bionus\Grabber\sites\Danbooru"; Flags: ignoreversion
Source: "{#SrcDir}\sites\Danbooru\model.js";           DestDir: "{localappdata}\Bionus\Grabber\sites\Danbooru"; Flags: ignoreversion
Source: "{#SrcDir}\sites\Danbooru\sites.txt";          DestDir: "{localappdata}\Bionus\Grabber\sites\Danbooru"; Flags: onlyifdoesntexist
Source: "{#SrcDir}\sites\Danbooru\behoimi.org\defaults.ini";              DestDir: "{localappdata}\Bionus\Grabber\sites\Danbooru\behoimi.org"; Flags: ignoreversion
Source: "{#SrcDir}\sites\Danbooru\behoimi.org\tag-types.txt";             DestDir: "{localappdata}\Bionus\Grabber\sites\Danbooru\behoimi.org"; Flags: onlyifdoesntexist
Source: "{#SrcDir}\sites\Danbooru (2.0)\icon.png";     DestDir: "{localappdata}\Bionus\Grabber\sites\Danbooru (2.0)"; Flags: ignoreversion
Source: "{#SrcDir}\sites\Danbooru (2.0)\model.js";     DestDir: "{localappdata}\Bionus\Grabber\sites\Danbooru (2.0)"; Flags: ignoreversion
Source: "{#SrcDir}\sites\Danbooru (2.0)\sites.txt";    DestDir: "{localappdata}\Bionus\Grabber\sites\Danbooru (2.0)"; Flags: onlyifdoesntexist
Source: "{#SrcDir}\sites\Danbooru (2.0)\danbooru.donmai.us\defaults.ini";              DestDir: "{localappdata}\Bionus\Grabber\sites\Danbooru (2.0)\danbooru.donmai.us"; Flags: ignoreversion
Source: "{#SrcDir}\sites\Danbooru (2.0)\danbooru.donmai.us\tag-types.txt";             DestDir: "{localappdata}\Bionus\Grabber\sites\Danbooru (2.0)\danbooru.donmai.us"; Flags: onlyifdoesntexist
Source: "{#SrcDir}\sites\E621\icon.png";     DestDir: "{localappdata}\Bionus\Grabber\sites\E621"; Flags: ignoreversion
Source: "{#SrcDir}\sites\E621\model.js";     DestDir: "{localappdata}\Bionus\Grabber\sites\E621"; Flags: ignoreversion
Source: "{#SrcDir}\sites\E621\sites.txt";    DestDir: "{localappdata}\Bionus\Grabber\sites\E621"; Flags: onlyifdoesntexist
Source: "{#SrcDir}\sites\E621\e621.net\defaults.ini";              DestDir: "{localappdata}\Bionus\Grabber\sites\E621\e621.net"; Flags: ignoreversion
Source: "{#SrcDir}\sites\E621\e621.net\tag-types.txt";             DestDir: "{localappdata}\Bionus\Grabber\sites\E621\e621.net"; Flags: onlyifdoesntexist
Source: "{#SrcDir}\sites\E-Hentai\icon.png";     DestDir: "{localappdata}\Bionus\Grabber\sites\E-Hentai"; Flags: ignoreversion
Source: "{#SrcDir}\sites\E-Hentai\model.js";     DestDir: "{localappdata}\Bionus\Grabber\sites\E-Hentai"; Flags: ignoreversion
Source: "{#SrcDir}\sites\E-Hentai\sites.txt";    DestDir: "{localappdata}\Bionus\Grabber\sites\E-Hentai"; Flags: onlyifdoesntexist
Source: "{#SrcDir}\sites\E-Hentai\e-hentai.org\defaults.ini"; DestDir: "{localappdata}\Bionus\Grabber\sites\E-Hentai\e-hentai.org"; Flags: ignoreversion
Source: "{#SrcDir}\sites\E-Hentai\exhentai.org\defaults.ini"; DestDir: "{localappdata}\Bionus\Grabber\sites\E-Hentai\exhentai.org"; Flags: ignoreversion
Source: "{#SrcDir}\sites\FurAffinity\icon.png";     DestDir: "{localappdata}\Bionus\Grabber\sites\FurAffinity"; Flags: ignoreversion
Source: "{#SrcDir}\sites\FurAffinity\model.js";     DestDir: "{localappdata}\Bionus\Grabber\sites\FurAffinity"; Flags: ignoreversion
Source: "{#SrcDir}\sites\FurAffinity\sites.txt";    DestDir: "{localappdata}\Bionus\Grabber\sites\FurAffinity"; Flags: onlyifdoesntexist
Source: "{#SrcDir}\sites\FurAffinity\www.furaffinity.net\defaults.ini"; DestDir: "{localappdata}\Bionus\Grabber\sites\FurAffinity\www.furaffinity.net"; Flags: ignoreversion
Source: "{#SrcDir}\sites\Gelbooru (0.1)\icon.png";     DestDir: "{localappdata}\Bionus\Grabber\sites\Gelbooru (0.1)"; Flags: ignoreversion
Source: "{#SrcDir}\sites\Gelbooru (0.1)\model.js";     DestDir: "{localappdata}\Bionus\Grabber\sites\Gelbooru (0.1)"; Flags: ignoreversion
Source: "{#SrcDir}\sites\Gelbooru (0.1)\sites.txt";    DestDir: "{localappdata}\Bionus\Grabber\sites\Gelbooru (0.1)"; Flags: onlyifdoesntexist
Source: "{#SrcDir}\sites\Gelbooru (0.2)\icon.png";     DestDir: "{localappdata}\Bionus\Grabber\sites\Gelbooru (0.2)"; Flags: ignoreversion
Source: "{#SrcDir}\sites\Gelbooru (0.2)\model.js";     DestDir: "{localappdata}\Bionus\Grabber\sites\Gelbooru (0.2)"; Flags: ignoreversion
Source: "{#SrcDir}\sites\Gelbooru (0.2)\sites.txt";    DestDir: "{localappdata}\Bionus\Grabber\sites\Gelbooru (0.2)"; Flags: onlyifdoesntexist
Source: "{#SrcDir}\sites\Gelbooru (0.2)\gelbooru.com\defaults.ini"; DestDir: "{localappdata}\Bionus\Grabber\sites\Gelbooru (0.2)\gelbooru.com"; Flags: ignoreversion
Source: "{#SrcDir}\sites\Moebooru\icon.png";  DestDir: "{localappdata}\Bionus\Grabber\sites\Moebooru"; Flags: ignoreversion
Source: "{#SrcDir}\sites\Moebooru\model.js";  DestDir: "{localappdata}\Bionus\Grabber\sites\Moebooru"; Flags: ignoreversion
Source: "{#SrcDir}\sites\Moebooru\sites.txt"; DestDir: "{localappdata}\Bionus\Grabber\sites\Moebooru"; Flags: onlyifdoesntexist
Source: "{#SrcDir}\sites\NHentai\icon.png";   DestDir: "{localappdata}\Bionus\Grabber\sites\NHentai"; Flags: ignoreversion
Source: "{#SrcDir}\sites\NHentai\model.js";   DestDir: "{localappdata}\Bionus\Grabber\sites\NHentai"; Flags: ignoreversion
Source: "{#SrcDir}\sites\NHentai\sites.txt";  DestDir: "{localappdata}\Bionus\Grabber\sites\NHentai"; Flags: onlyifdoesntexist
Source: "{#SrcDir}\sites\NHentai\nhentai.net\defaults.ini"; DestDir: "{localappdata}\Bionus\Grabber\sites\NHentai\nhentai.net"; Flags: ignoreversion
Source: "{#SrcDir}\sites\Pixiv\icon.png";     DestDir: "{localappdata}\Bionus\Grabber\sites\Pixiv"; Flags: ignoreversion
Source: "{#SrcDir}\sites\Pixiv\model.js";     DestDir: "{localappdata}\Bionus\Grabber\sites\Pixiv"; Flags: ignoreversion
Source: "{#SrcDir}\sites\Pixiv\sites.txt";    DestDir: "{localappdata}\Bionus\Grabber\sites\Pixiv"; Flags: onlyifdoesntexist
Source: "{#SrcDir}\sites\Pixiv\www.pixiv.net\defaults.ini"; DestDir: "{localappdata}\Bionus\Grabber\sites\Pixiv\www.pixiv.net"; Flags: ignoreversion
Source: "{#SrcDir}\sites\Shimmie\icon.png";   DestDir: "{localappdata}\Bionus\Grabber\sites\Shimmie"; Flags: ignoreversion
Source: "{#SrcDir}\sites\Shimmie\model.js";   DestDir: "{localappdata}\Bionus\Grabber\sites\Shimmie"; Flags: ignoreversion
Source: "{#SrcDir}\sites\Shimmie\sites.txt";  DestDir: "{localappdata}\Bionus\Grabber\sites\Shimmie"; Flags: onlyifdoesntexist
Source: "{#SrcDir}\sites\Twitter\icon.png";   DestDir: "{localappdata}\Bionus\Grabber\sites\Twitter"; Flags: ignoreversion
Source: "{#SrcDir}\sites\Twitter\model.js";   DestDir: "{localappdata}\Bionus\Grabber\sites\Twitter"; Flags: ignoreversion
Source: "{#SrcDir}\sites\Twitter\sites.txt";  DestDir: "{localappdata}\Bionus\Grabber\sites\Twitter"; Flags: onlyifdoesntexist
Source: "{#SrcDir}\sites\Twitter\api.twitter.com\defaults.ini"; DestDir: "{localappdata}\Bionus\Grabber\sites\Twitter\api.twitter.com"; Flags: ignoreversion
Source: "{#SrcDir}\sites\Zerochan\icon.png";  DestDir: "{localappdata}\Bionus\Grabber\sites\Zerochan"; Flags: ignoreversion
Source: "{#SrcDir}\sites\Zerochan\model.js";  DestDir: "{localappdata}\Bionus\Grabber\sites\Zerochan"; Flags: ignoreversion
Source: "{#SrcDir}\sites\Zerochan\sites.txt"; DestDir: "{localappdata}\Bionus\Grabber\sites\Zerochan"; Flags: onlyifdoesntexist
Source: "{#SrcDir}\sites\Zerochan\www.zerochan.net\defaults.ini"; DestDir: "{localappdata}\Bionus\Grabber\sites\Zerochan\www.zerochan.net"; Flags: ignoreversion
Source: "{#SrcDir}\dist\common\themes\Default\*"; DestDir: "{localappdata}\Bionus\Grabber\themes\Default"; Flags: onlyifdoesntexist recursesubdirs
Source: "{#SrcDir}\dist\common\themes\QDarkStyleSheet\*"; DestDir: "{localappdata}\Bionus\Grabber\themes\QDarkStyleSheet"; Flags: onlyifdoesntexist recursesubdirs
Source: "{#SrcDir}\dist\common\webservices\*.ico"; DestDir: "{localappdata}\Bionus\Grabber\webservices"; Flags: onlyifdoesntexist

[InstallDelete]
Type: filesandordirs; Name: "{app}\languages\updater"
Type: filesandordirs; Name: "{app}\sites"
Type: files; Name: "{app}\Updater.exe"
Type: files; Name: "{app}\VERSION"
Type: files; Name: "{app}\MD5"
Type: files; Name: "{app}\libgcc_s_dw2-1.dll"
Type: files; Name: "{app}\libeay32.dll"
Type: files; Name: "{app}\libssl32.dll"
Type: files; Name: "{app}\ssleay32.dll"
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
  initwinversion();

  vcredist2015('14');
  vcredist2017('14');

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

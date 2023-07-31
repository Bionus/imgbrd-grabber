#define MyAppName "Grabber"
#define MyAppPublisher "Bionus"
#define MyAppURL "https://github.com/Bionus/imgbrd-grabber"
#define MyAppExeName "Grabber.exe"

#define RootDir "..\.."
#define BuildDir RootDir + "\build"
#define SrcDir RootDir + "\src"

#ifndef QtDir
# define QtDir "C:\Qt\Qt6.5.0\6.5.0\msvc2019_64\bin"
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
# define MyAppVersion "7.11.2"
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

; Dependencies
#define UseMsiProductCheck
#define UseVC2015To2019
#include "scripts\dependencies.iss"

[Languages]
Name: en; MessagesFile: "compiler:Default.isl"
Name: nl; MessagesFile: "compiler:Languages\Dutch.isl"
Name: fr; MessagesFile: "compiler:Languages\French.isl"
Name: de; MessagesFile: "compiler:Languages\German.isl"
Name: ru; MessagesFile: "compiler:Languages\Russian.isl"
Name: es; MessagesFile: "compiler:Languages\Spanish.isl"

[CustomMessages]
en.IGL=Imageboard-Grabber Links
fr.IGL=Liens Imageboard-Grabber

[Registry]
Root: HKCR; Subkey: ".igl"; ValueType: string; ValueName: ""; ValueData: "Imageboard-Grabber"; Flags: uninsdeletevalue
Root: HKCR; Subkey: "Imageboard-Grabber"; ValueType: string; ValueName: ""; ValueData: "{cm:IGL}"; Flags: uninsdeletekey
Root: HKCR; Subkey: "Imageboard-Grabber\DefaultIcon"; ValueType: string; ValueName: ""; ValueData: "{app}\{#MyAppExeName},0"
Root: HKCR; Subkey: "Imageboard-Grabber\shell\open\command"; ValueType: string; ValueName: ""; ValueData: """{app}\{#MyAppExeName}"" ""%1"""

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
Source: "{#OpenSSLDir}\libcrypto-3.dll";        DestDir: "{app}"; Flags: ignoreversion skipifsourcedoesntexist
Source: "{#OpenSSLDir}\libcrypto-3-x64.dll";    DestDir: "{app}"; Flags: ignoreversion skipifsourcedoesntexist
Source: "{#OpenSSLDir}\libssl-3.dll";           DestDir: "{app}"; Flags: ignoreversion skipifsourcedoesntexist
Source: "{#OpenSSLDir}\libssl-3-x64.dll";       DestDir: "{app}"; Flags: ignoreversion skipifsourcedoesntexist
Source: "{#QtDir}\D3Dcompiler_47.dll";          DestDir: "{app}"; Flags: ignoreversion
Source: "{#QtDir}\opengl32sw.dll";              DestDir: "{app}"; Flags: ignoreversion
Source: "{#MySQLDir}\libmysql.dll";             DestDir: "{app}"; Flags: ignoreversion
Source: "{#QtLib}\qscintilla2_qt6.dll";         DestDir: "{app}"; Flags: ignoreversion
Source: "{#QtDir}\Qt6Concurrent.dll";           DestDir: "{app}"; Flags: ignoreversion
Source: "{#QtDir}\Qt6Core.dll";                 DestDir: "{app}"; Flags: ignoreversion
Source: "{#QtDir}\Qt6Gui.dll";                  DestDir: "{app}"; Flags: ignoreversion
Source: "{#QtDir}\Qt6Multimedia.dll";           DestDir: "{app}"; Flags: ignoreversion
Source: "{#QtDir}\Qt6MultimediaWidgets.dll";    DestDir: "{app}"; Flags: ignoreversion
Source: "{#QtDir}\Qt6Network.dll";              DestDir: "{app}"; Flags: ignoreversion
Source: "{#QtDir}\Qt6NetworkAuth.dll";          DestDir: "{app}"; Flags: ignoreversion
Source: "{#QtDir}\Qt6OpenGL.dll";               DestDir: "{app}"; Flags: ignoreversion
Source: "{#QtDir}\Qt6PrintSupport.dll";         DestDir: "{app}"; Flags: ignoreversion
Source: "{#QtDir}\Qt6Qml.dll";                  DestDir: "{app}"; Flags: ignoreversion
Source: "{#QtDir}\Qt6Sql.dll";                  DestDir: "{app}"; Flags: ignoreversion
Source: "{#QtDir}\Qt6Svg.dll";                  DestDir: "{app}"; Flags: ignoreversion
Source: "{#QtDir}\Qt6Widgets.dll";              DestDir: "{app}"; Flags: ignoreversion
Source: "{#QtDir}\Qt6Xml.dll";                  DestDir: "{app}"; Flags: ignoreversion
Source: "{#BuildDir}\crash-reporter\languages\ChineseSimplified.qm"; DestDir: "{app}\crashreporter"; Flags: ignoreversion
Source: "{#BuildDir}\crash-reporter\languages\English.qm";  DestDir: "{app}\crashreporter"; Flags: ignoreversion
Source: "{#BuildDir}\crash-reporter\languages\French.qm";   DestDir: "{app}\crashreporter"; Flags: ignoreversion
Source: "{#BuildDir}\crash-reporter\languages\German.qm";   DestDir: "{app}\crashreporter"; Flags: ignoreversion
Source: "{#BuildDir}\crash-reporter\languages\Japanese.qm"; DestDir: "{app}\crashreporter"; Flags: ignoreversion
Source: "{#BuildDir}\crash-reporter\languages\Korean.qm";   DestDir: "{app}\crashreporter"; Flags: ignoreversion
Source: "{#BuildDir}\crash-reporter\languages\Polish.qm";   DestDir: "{app}\crashreporter"; Flags: ignoreversion
Source: "{#BuildDir}\crash-reporter\languages\PortugueseBrazilian.qm"; DestDir: "{app}\crashreporter"; Flags: ignoreversion
Source: "{#BuildDir}\crash-reporter\languages\Russian.qm";  DestDir: "{app}\crashreporter"; Flags: ignoreversion
Source: "{#BuildDir}\crash-reporter\languages\Spanish.qm";  DestDir: "{app}\crashreporter"; Flags: ignoreversion
Source: "{#QtPlugins}\imageformats\qgif.dll";   DestDir: "{app}\imageformats"; Flags: ignoreversion
Source: "{#QtPlugins}\imageformats\qicns.dll";  DestDir: "{app}\imageformats"; Flags: ignoreversion
Source: "{#QtPlugins}\imageformats\qico.dll";   DestDir: "{app}\imageformats"; Flags: ignoreversion
Source: "{#QtPlugins}\imageformats\qjpeg.dll";  DestDir: "{app}\imageformats"; Flags: ignoreversion
Source: "{#QtPlugins}\imageformats\qsvg.dll";   DestDir: "{app}\imageformats"; Flags: ignoreversion
Source: "{#QtPlugins}\imageformats\qtga.dll";   DestDir: "{app}\imageformats"; Flags: ignoreversion
Source: "{#QtPlugins}\imageformats\qtiff.dll";  DestDir: "{app}\imageformats"; Flags: ignoreversion
Source: "{#QtPlugins}\imageformats\qwbmp.dll";  DestDir: "{app}\imageformats"; Flags: ignoreversion
Source: "{#QtPlugins}\imageformats\qwebp.dll";  DestDir: "{app}\imageformats"; Flags: ignoreversion
Source: "{#QtPlugins}\tls\qschannelbackend.dll";DestDir: "{app}\tls"; Flags: ignoreversion
Source: "{#QtPlugins}\tls\qopensslbackend.dll"; DestDir: "{app}\tls"; Flags: ignoreversion
Source: "{#QtPlugins}\tls\qcertonlybackend.dll";DestDir: "{app}\tls"; Flags: ignoreversion
Source: "{#BuildDir}\languages\ChineseSimplified.qm"; DestDir: "{app}\languages"; Flags: ignoreversion
Source: "{#BuildDir}\languages\English.qm";     DestDir: "{app}\languages"; Flags: ignoreversion
Source: "{#BuildDir}\languages\French.qm";      DestDir: "{app}\languages"; Flags: ignoreversion
Source: "{#BuildDir}\languages\German.qm";      DestDir: "{app}\languages"; Flags: ignoreversion
Source: "{#BuildDir}\languages\Japanese.qm";    DestDir: "{app}\languages"; Flags: ignoreversion
Source: "{#BuildDir}\languages\Korean.qm";      DestDir: "{app}\languages"; Flags: ignoreversion
Source: "{#BuildDir}\languages\Polish.qm";      DestDir: "{app}\languages"; Flags: ignoreversion
Source: "{#BuildDir}\languages\PortugueseBrazilian.qm"; DestDir: "{app}\languages"; Flags: ignoreversion
Source: "{#BuildDir}\languages\Russian.qm";     DestDir: "{app}\languages"; Flags: ignoreversion
Source: "{#BuildDir}\languages\Spanish.qm";     DestDir: "{app}\languages"; Flags: ignoreversion
Source: "{#SrcDir}\dist\common\languages\languages.ini";  DestDir: "{app}\languages"; Flags: ignoreversion
Source: "{#QtDir}\..\translations\qtbase_en.qm";   DestDir: "{app}\languages\qt"; DestName: "English.qm"; Flags: ignoreversion
Source: "{#QtDir}\..\translations\qtbase_fr.qm";   DestDir: "{app}\languages\qt"; DestName: "French.qm";  Flags: ignoreversion
Source: "{#QtDir}\..\translations\qtbase_ru.qm";   DestDir: "{app}\languages\qt"; DestName: "Russian.qm"; Flags: ignoreversion
Source: "{#QtPlugins}\multimedia\windowsmediaplugin.dll"; DestDir: "{app}\multimedia"; Flags: ignoreversion
Source: "{#QtPlugins}\platforms\qwindows.dll";     DestDir: "{app}\platforms"; Flags: ignoreversion
Source: "{#QtPlugins}\sqldrivers\qsqlite.dll";     DestDir: "{app}\sqldrivers"; Flags: ignoreversion
Source: "{#QtPlugins}\sqldrivers\qsqlmysql.dll";   DestDir: "{app}\sqldrivers"; Flags: ignoreversion
Source: "{#QtPlugins}\sqldrivers\qsqlodbc.dll";    DestDir: "{app}\sqldrivers"; Flags: ignoreversion
Source: "{#QtPlugins}\sqldrivers\qsqlpsql.dll";    DestDir: "{app}\sqldrivers"; Flags: ignoreversion
Source: "{#QtPlugins}\styles\qwindowsvistastyle.dll";  DestDir: "{app}\styles"; Flags: ignoreversion
Source: "{#SrcDir}\sites\helper.js";                   DestDir: "{app}\sites"; Flags: ignoreversion
Source: "{#SrcDir}\sites\Anime pictures\icon.png";     DestDir: "{app}\sites\Anime pictures"; Flags: ignoreversion
Source: "{#SrcDir}\sites\Anime pictures\model.js";     DestDir: "{app}\sites\Anime pictures"; Flags: ignoreversion
Source: "{#SrcDir}\sites\Anime pictures\sites.txt";    DestDir: "{app}\sites\Anime pictures"; Flags: ignoreversion
Source: "{#SrcDir}\sites\Anime pictures\anime-pictures.net\defaults.ini";  DestDir: "{app}\sites\Anime pictures\anime-pictures.net"; Flags: ignoreversion
Source: "{#SrcDir}\sites\Anime pictures\anime-pictures.net\tag-types.txt"; DestDir: "{app}\sites\Anime pictures\anime-pictures.net"; Flags: ignoreversion
Source: "{#SrcDir}\sites\ArtStation\icon.png";      DestDir: "{app}\sites\ArtStation"; Flags: ignoreversion
Source: "{#SrcDir}\sites\ArtStation\model.js";      DestDir: "{app}\sites\ArtStation"; Flags: ignoreversion
Source: "{#SrcDir}\sites\ArtStation\sites.txt";     DestDir: "{app}\sites\ArtStation"; Flags: ignoreversion
Source: "{#SrcDir}\sites\ArtStation\supported.txt"; DestDir: "{app}\sites\ArtStation"; Flags: ignoreversion
Source: "{#SrcDir}\sites\ArtStation\www.artstation.com\defaults.ini"; DestDir: "{app}\sites\ArtStation\www.artstation.com"; Flags: ignoreversion
Source: "{#SrcDir}\sites\Booru.io\icon.png";      DestDir: "{app}\sites\Booru.io"; Flags: ignoreversion
Source: "{#SrcDir}\sites\Booru.io\model.js";      DestDir: "{app}\sites\Booru.io"; Flags: ignoreversion
Source: "{#SrcDir}\sites\Booru.io\sites.txt";     DestDir: "{app}\sites\Booru.io"; Flags: ignoreversion
Source: "{#SrcDir}\sites\Booru.io\supported.txt"; DestDir: "{app}\sites\Booru.io"; Flags: ignoreversion
Source: "{#SrcDir}\sites\Booru.io\booru.io\defaults.ini"; DestDir: "{app}\sites\Booru.io\booru.io"; Flags: ignoreversion
Source: "{#SrcDir}\sites\Danbooru\icon.png";           DestDir: "{app}\sites\Danbooru"; Flags: ignoreversion
Source: "{#SrcDir}\sites\Danbooru\model.js";           DestDir: "{app}\sites\Danbooru"; Flags: ignoreversion
Source: "{#SrcDir}\sites\Danbooru\sites.txt";          DestDir: "{app}\sites\Danbooru"; Flags: ignoreversion
Source: "{#SrcDir}\sites\Danbooru\behoimi.org\defaults.ini";              DestDir: "{app}\sites\Danbooru\behoimi.org"; Flags: ignoreversion
Source: "{#SrcDir}\sites\Danbooru\behoimi.org\tag-types.txt";             DestDir: "{app}\sites\Danbooru\behoimi.org"; Flags: ignoreversion
Source: "{#SrcDir}\sites\Danbooru (2.0)\icon.png";     DestDir: "{app}\sites\Danbooru (2.0)"; Flags: ignoreversion
Source: "{#SrcDir}\sites\Danbooru (2.0)\model.js";     DestDir: "{app}\sites\Danbooru (2.0)"; Flags: ignoreversion
Source: "{#SrcDir}\sites\Danbooru (2.0)\sites.txt";    DestDir: "{app}\sites\Danbooru (2.0)"; Flags: ignoreversion
Source: "{#SrcDir}\sites\Danbooru (2.0)\danbooru.donmai.us\defaults.ini";              DestDir: "{app}\sites\Danbooru (2.0)\danbooru.donmai.us"; Flags: ignoreversion
Source: "{#SrcDir}\sites\Danbooru (2.0)\danbooru.donmai.us\tag-types.txt";             DestDir: "{app}\sites\Danbooru (2.0)\danbooru.donmai.us"; Flags: ignoreversion
Source: "{#SrcDir}\sites\DeviantArt\icon.png";           DestDir: "{app}\sites\DeviantArt"; Flags: ignoreversion
Source: "{#SrcDir}\sites\DeviantArt\model.js";           DestDir: "{app}\sites\DeviantArt"; Flags: ignoreversion
Source: "{#SrcDir}\sites\DeviantArt\sites.txt";          DestDir: "{app}\sites\DeviantArt"; Flags: ignoreversion
Source: "{#SrcDir}\sites\DeviantArt\www.deviantart.com\defaults.ini";              DestDir: "{app}\sites\DeviantArt\www.deviantart.com"; Flags: ignoreversion
Source: "{#SrcDir}\sites\E621\icon.png";     DestDir: "{app}\sites\E621"; Flags: ignoreversion
Source: "{#SrcDir}\sites\E621\model.js";     DestDir: "{app}\sites\E621"; Flags: ignoreversion
Source: "{#SrcDir}\sites\E621\sites.txt";    DestDir: "{app}\sites\E621"; Flags: ignoreversion
Source: "{#SrcDir}\sites\E621\e621.net\defaults.ini";              DestDir: "{app}\sites\E621\e621.net"; Flags: ignoreversion
Source: "{#SrcDir}\sites\E621\e621.net\tag-types.txt";             DestDir: "{app}\sites\E621\e621.net"; Flags: ignoreversion
Source: "{#SrcDir}\sites\E-Hentai\icon.png";     DestDir: "{app}\sites\E-Hentai"; Flags: ignoreversion
Source: "{#SrcDir}\sites\E-Hentai\model.js";     DestDir: "{app}\sites\E-Hentai"; Flags: ignoreversion
Source: "{#SrcDir}\sites\E-Hentai\sites.txt";    DestDir: "{app}\sites\E-Hentai"; Flags: ignoreversion
Source: "{#SrcDir}\sites\E-Hentai\e-hentai.org\defaults.ini"; DestDir: "{app}\sites\E-Hentai\e-hentai.org"; Flags: ignoreversion
Source: "{#SrcDir}\sites\E-Hentai\exhentai.org\defaults.ini"; DestDir: "{app}\sites\E-Hentai\exhentai.org"; Flags: ignoreversion
Source: "{#SrcDir}\sites\E-Shuushuu\icon.png";     DestDir: "{app}\sites\E-Shuushuu"; Flags: ignoreversion
Source: "{#SrcDir}\sites\E-Shuushuu\model.js";     DestDir: "{app}\sites\E-Shuushuu"; Flags: ignoreversion
Source: "{#SrcDir}\sites\E-Shuushuu\sites.txt";    DestDir: "{app}\sites\E-Shuushuu"; Flags: ignoreversion
Source: "{#SrcDir}\sites\E-Shuushuu\e-shuushuu.net\defaults.ini";  DestDir: "{app}\sites\E-Shuushuu\e-shuushuu.net"; Flags: ignoreversion
Source: "{#SrcDir}\sites\E-Shuushuu\e-shuushuu.net\tag-types.txt"; DestDir: "{app}\sites\E-Shuushuu\e-shuushuu.net"; Flags: ignoreversion
Source: "{#SrcDir}\sites\FurAffinity\icon.png";     DestDir: "{app}\sites\FurAffinity"; Flags: ignoreversion
Source: "{#SrcDir}\sites\FurAffinity\model.js";     DestDir: "{app}\sites\FurAffinity"; Flags: ignoreversion
Source: "{#SrcDir}\sites\FurAffinity\sites.txt";    DestDir: "{app}\sites\FurAffinity"; Flags: ignoreversion
Source: "{#SrcDir}\sites\FurAffinity\www.furaffinity.net\defaults.ini"; DestDir: "{app}\sites\FurAffinity\www.furaffinity.net"; Flags: ignoreversion
Source: "{#SrcDir}\sites\Gelbooru (0.1)\icon.png";     DestDir: "{app}\sites\Gelbooru (0.1)"; Flags: ignoreversion
Source: "{#SrcDir}\sites\Gelbooru (0.1)\model.js";     DestDir: "{app}\sites\Gelbooru (0.1)"; Flags: ignoreversion
Source: "{#SrcDir}\sites\Gelbooru (0.1)\sites.txt";    DestDir: "{app}\sites\Gelbooru (0.1)"; Flags: ignoreversion
Source: "{#SrcDir}\sites\Gelbooru (0.2)\icon.png";     DestDir: "{app}\sites\Gelbooru (0.2)"; Flags: ignoreversion
Source: "{#SrcDir}\sites\Gelbooru (0.2)\model.js";     DestDir: "{app}\sites\Gelbooru (0.2)"; Flags: ignoreversion
Source: "{#SrcDir}\sites\Gelbooru (0.2)\sites.txt";    DestDir: "{app}\sites\Gelbooru (0.2)"; Flags: ignoreversion
Source: "{#SrcDir}\sites\Gelbooru (0.2)\gelbooru.com\defaults.ini"; DestDir: "{app}\sites\Gelbooru (0.2)\gelbooru.com"; Flags: ignoreversion
Source: "{#SrcDir}\sites\Kemono\icon.png";      DestDir: "{app}\sites\Kemono"; Flags: ignoreversion
Source: "{#SrcDir}\sites\Kemono\model.js";      DestDir: "{app}\sites\Kemono"; Flags: ignoreversion
Source: "{#SrcDir}\sites\Kemono\sites.txt";     DestDir: "{app}\sites\Kemono"; Flags: ignoreversion
Source: "{#SrcDir}\sites\Kemono\supported.txt"; DestDir: "{app}\sites\Kemono"; Flags: ignoreversion
Source: "{#SrcDir}\sites\Mastodon\icon.png";  DestDir: "{app}\sites\Mastodon"; Flags: ignoreversion
Source: "{#SrcDir}\sites\Mastodon\model.js";  DestDir: "{app}\sites\Mastodon"; Flags: ignoreversion
Source: "{#SrcDir}\sites\Mastodon\sites.txt"; DestDir: "{app}\sites\Mastodon"; Flags: ignoreversion
Source: "{#SrcDir}\sites\Moebooru\icon.png";  DestDir: "{app}\sites\Moebooru"; Flags: ignoreversion
Source: "{#SrcDir}\sites\Moebooru\model.js";  DestDir: "{app}\sites\Moebooru"; Flags: ignoreversion
Source: "{#SrcDir}\sites\Moebooru\sites.txt"; DestDir: "{app}\sites\Moebooru"; Flags: ignoreversion
Source: "{#SrcDir}\sites\Newgrounds Art\icon.png";      DestDir: "{app}\sites\Newgrounds Art"; Flags: ignoreversion
Source: "{#SrcDir}\sites\Newgrounds Art\model.js";      DestDir: "{app}\sites\Newgrounds Art"; Flags: ignoreversion
Source: "{#SrcDir}\sites\Newgrounds Art\sites.txt";     DestDir: "{app}\sites\Newgrounds Art"; Flags: ignoreversion
Source: "{#SrcDir}\sites\Newgrounds Art\supported.txt"; DestDir: "{app}\sites\Newgrounds Art"; Flags: ignoreversion
Source: "{#SrcDir}\sites\NHentai\icon.png";   DestDir: "{app}\sites\NHentai"; Flags: ignoreversion
Source: "{#SrcDir}\sites\NHentai\model.js";   DestDir: "{app}\sites\NHentai"; Flags: ignoreversion
Source: "{#SrcDir}\sites\NHentai\sites.txt";  DestDir: "{app}\sites\NHentai"; Flags: ignoreversion
Source: "{#SrcDir}\sites\NHentai\nhentai.net\defaults.ini"; DestDir: "{app}\sites\NHentai\nhentai.net"; Flags: ignoreversion
Source: "{#SrcDir}\sites\Nijie\icon.png";   DestDir: "{app}\sites\Nijie"; Flags: ignoreversion
Source: "{#SrcDir}\sites\Nijie\model.js";   DestDir: "{app}\sites\Nijie"; Flags: ignoreversion
Source: "{#SrcDir}\sites\Nijie\sites.txt";  DestDir: "{app}\sites\Nijie"; Flags: ignoreversion
Source: "{#SrcDir}\sites\Nijie\nijie.info\defaults.ini"; DestDir: "{app}\sites\Nijie\nijie.info"; Flags: ignoreversion
Source: "{#SrcDir}\sites\Philomena\icon.png";     DestDir: "{app}\sites\Philomena"; Flags: ignoreversion
Source: "{#SrcDir}\sites\Philomena\model.js";     DestDir: "{app}\sites\Philomena"; Flags: ignoreversion
Source: "{#SrcDir}\sites\Philomena\sites.txt";    DestDir: "{app}\sites\Philomena"; Flags: ignoreversion
Source: "{#SrcDir}\sites\Philomena\derpibooru.org\defaults.ini"; DestDir: "{app}\sites\Philomena\derpibooru.org"; Flags: ignoreversion
Source: "{#SrcDir}\sites\Pixiv\icon.png";     DestDir: "{app}\sites\Pixiv"; Flags: ignoreversion
Source: "{#SrcDir}\sites\Pixiv\model.js";     DestDir: "{app}\sites\Pixiv"; Flags: ignoreversion
Source: "{#SrcDir}\sites\Pixiv\sites.txt";    DestDir: "{app}\sites\Pixiv"; Flags: ignoreversion
Source: "{#SrcDir}\sites\Pixiv\www.pixiv.net\defaults.ini"; DestDir: "{app}\sites\Pixiv\www.pixiv.net"; Flags: ignoreversion
Source: "{#SrcDir}\sites\Reddit\icon.png";    DestDir: "{app}\sites\Reddit"; Flags: ignoreversion
Source: "{#SrcDir}\sites\Reddit\model.js";    DestDir: "{app}\sites\Reddit"; Flags: ignoreversion
Source: "{#SrcDir}\sites\Reddit\sites.txt";   DestDir: "{app}\sites\Reddit"; Flags: ignoreversion
Source: "{#SrcDir}\sites\Reddit\www.reddit.com\defaults.ini";              DestDir: "{app}\sites\Reddit\www.reddit.com"; Flags: ignoreversion
Source: "{#SrcDir}\sites\Rule34.us Gelbooru\icon.png";      DestDir: "{app}\sites\Rule34.us Gelbooru"; Flags: ignoreversion
Source: "{#SrcDir}\sites\Rule34.us Gelbooru\model.js";      DestDir: "{app}\sites\Rule34.us Gelbooru"; Flags: ignoreversion
Source: "{#SrcDir}\sites\Rule34.us Gelbooru\sites.txt";     DestDir: "{app}\sites\Rule34.us Gelbooru"; Flags: ignoreversion
Source: "{#SrcDir}\sites\Shimmie\icon.png";   DestDir: "{app}\sites\Shimmie"; Flags: ignoreversion
Source: "{#SrcDir}\sites\Shimmie\model.js";   DestDir: "{app}\sites\Shimmie"; Flags: ignoreversion
Source: "{#SrcDir}\sites\Shimmie\sites.txt";  DestDir: "{app}\sites\Shimmie"; Flags: ignoreversion
Source: "{#SrcDir}\sites\Szurubooru\icon.png";   DestDir: "{app}\sites\Szurubooru"; Flags: ignoreversion
Source: "{#SrcDir}\sites\Szurubooru\model.js";   DestDir: "{app}\sites\Szurubooru"; Flags: ignoreversion
Source: "{#SrcDir}\sites\Szurubooru\sites.txt";  DestDir: "{app}\sites\Szurubooru"; Flags: ignoreversion
Source: "{#SrcDir}\sites\Twitter\icon.png";   DestDir: "{app}\sites\Twitter"; Flags: ignoreversion
Source: "{#SrcDir}\sites\Twitter\model.js";   DestDir: "{app}\sites\Twitter"; Flags: ignoreversion
Source: "{#SrcDir}\sites\Twitter\sites.txt";  DestDir: "{app}\sites\Twitter"; Flags: ignoreversion
Source: "{#SrcDir}\sites\Twitter\api.twitter.com\defaults.ini"; DestDir: "{app}\sites\Twitter\api.twitter.com"; Flags: ignoreversion
Source: "{#SrcDir}\sites\WallHaven\icon.png";  DestDir: "{app}\sites\WallHaven"; Flags: ignoreversion
Source: "{#SrcDir}\sites\WallHaven\model.js";  DestDir: "{app}\sites\WallHaven"; Flags: ignoreversion
Source: "{#SrcDir}\sites\WallHaven\sites.txt"; DestDir: "{app}\sites\WallHaven"; Flags: ignoreversion
Source: "{#SrcDir}\sites\WallHaven\wallhaven.cc\defaults.ini"; DestDir: "{app}\sites\WallHaven\wallhaven.cc"; Flags: ignoreversion
Source: "{#SrcDir}\sites\Zerochan\icon.png";  DestDir: "{app}\sites\Zerochan"; Flags: ignoreversion
Source: "{#SrcDir}\sites\Zerochan\model.js";  DestDir: "{app}\sites\Zerochan"; Flags: ignoreversion
Source: "{#SrcDir}\sites\Zerochan\sites.txt"; DestDir: "{app}\sites\Zerochan"; Flags: ignoreversion
Source: "{#SrcDir}\sites\Zerochan\www.zerochan.net\defaults.ini"; DestDir: "{app}\sites\Zerochan\www.zerochan.net"; Flags: ignoreversion
Source: "{#SrcDir}\dist\common\themes\Default\*"; DestDir: "{localappdata}\Bionus\Grabber\themes\Default"; Flags: recursesubdirs
Source: "{#SrcDir}\dist\common\themes\QDarkStyleSheet\*"; DestDir: "{localappdata}\Bionus\Grabber\themes\QDarkStyleSheet"; Flags: recursesubdirs
Source: "{#SrcDir}\dist\common\webservices\*.ico"; DestDir: "{localappdata}\Bionus\Grabber\webservices"

[InstallDelete]
Type: files; Name: "{app}\Updater.exe"
Type: files; Name: "{app}\VERSION"
Type: files; Name: "{app}\MD5"
Type: files; Name: "{app}\libgcc_s_dw2-1.dll"
Type: files; Name: "{app}\libeay32.dll"
Type: files; Name: "{app}\libssl32.dll"
Type: files; Name: "{app}\ssleay32.dll"
; OpenSSL 1.1 DLLs (Qt 6.5 moved to OpenSSL 3)
Type: files; Name: "{app}\libcrypto-1_1.dll"
Type: files; Name: "{app}\libcrypto-1_1-x64.dll"
Type: files; Name: "{app}\libssl-1_1.dll"
Type: files; Name: "{app}\libssl-1_1-x64.dll"
; Qt5 DLLs
Type: files; Name: "{app}\qscintilla2_qt5.dll"
Type: files; Name: "{app}\libEGL.dll"
Type: files; Name: "{app}\libGLESv2.dll"
Type: files; Name: "{app}\Qt5Concurrent.dll"
Type: files; Name: "{app}\Qt5Core.dll"
Type: files; Name: "{app}\Qt5Gui.dll"
Type: files; Name: "{app}\Qt5Multimedia.dll"
Type: files; Name: "{app}\Qt5MultimediaWidgets.dll"
Type: files; Name: "{app}\Qt5Network.dll"
Type: files; Name: "{app}\Qt5NetworkAuth.dll"
Type: files; Name: "{app}\Qt5OpenGL.dll"
Type: files; Name: "{app}\Qt5PrintSupport.dll"
Type: files; Name: "{app}\Qt5Qml.dll"
Type: files; Name: "{app}\Qt5Sql.dll"
Type: files; Name: "{app}\Qt5Svg.dll"
Type: files; Name: "{app}\Qt5Widgets.dll"
Type: files; Name: "{app}\Qt5WinExtras.dll"
Type: files; Name: "{app}\Qt5Xml.dll"
Type: files; Name: "{app}\imageformats\qdds.dll"
; Qt4 DLLs
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
; Media engines
Type: files; Name: "{app}\mediaservice\dsengine.dll"
Type: files; Name: "{app}\mediaservice\wmfengine.dll"
; Legacy sites files (now moved to "{app}\sites")
Type: files; Name: "{localappdata}\Bionus\Grabber\sites\helper.js"
Type: files; Name: "{localappdata}\Bionus\Grabber\sites\Anime pictures\icon.png"
Type: files; Name: "{localappdata}\Bionus\Grabber\sites\Anime pictures\model.js"
Type: files; Name: "{localappdata}\Bionus\Grabber\sites\Anime pictures\anime-pictures.net\defaults.ini"
Type: files; Name: "{localappdata}\Bionus\Grabber\sites\Danbooru\icon.png"
Type: files; Name: "{localappdata}\Bionus\Grabber\sites\Danbooru\model.js"
Type: files; Name: "{localappdata}\Bionus\Grabber\sites\Danbooru\behoimi.org\defaults.ini"
Type: files; Name: "{localappdata}\Bionus\Grabber\sites\Danbooru (2.0)\icon.png"
Type: files; Name: "{localappdata}\Bionus\Grabber\sites\Danbooru (2.0)\model.js"
Type: files; Name: "{localappdata}\Bionus\Grabber\sites\Danbooru (2.0)\danbooru.donmai.us\defaults.ini"
Type: files; Name: "{localappdata}\Bionus\Grabber\sites\DeviantArt\icon.png"
Type: files; Name: "{localappdata}\Bionus\Grabber\sites\DeviantArt\model.js"
Type: files; Name: "{localappdata}\Bionus\Grabber\sites\DeviantArt\www.deviantart.com\defaults.ini"
Type: files; Name: "{localappdata}\Bionus\Grabber\sites\E621\icon.png"
Type: files; Name: "{localappdata}\Bionus\Grabber\sites\E621\model.js"
Type: files; Name: "{localappdata}\Bionus\Grabber\sites\E621\e621.net\defaults.ini"
Type: files; Name: "{localappdata}\Bionus\Grabber\sites\E-Hentai\icon.png"
Type: files; Name: "{localappdata}\Bionus\Grabber\sites\E-Hentai\model.js"
Type: files; Name: "{localappdata}\Bionus\Grabber\sites\E-Hentai\e-hentai.org\defaults.ini"
Type: files; Name: "{localappdata}\Bionus\Grabber\sites\E-Hentai\exhentai.org\defaults.ini"
Type: files; Name: "{localappdata}\Bionus\Grabber\sites\E-Shuushuu\icon.png"
Type: files; Name: "{localappdata}\Bionus\Grabber\sites\E-Shuushuu\model.js"
Type: files; Name: "{localappdata}\Bionus\Grabber\sites\E-Shuushuu\e-shuushuu.net\defaults.ini"
Type: files; Name: "{localappdata}\Bionus\Grabber\sites\FurAffinity\icon.png"
Type: files; Name: "{localappdata}\Bionus\Grabber\sites\FurAffinity\model.js"
Type: files; Name: "{localappdata}\Bionus\Grabber\sites\FurAffinity\www.furaffinity.net\defaults.ini"
Type: files; Name: "{localappdata}\Bionus\Grabber\sites\Gelbooru (0.1)\icon.png"
Type: files; Name: "{localappdata}\Bionus\Grabber\sites\Gelbooru (0.1)\model.js"
Type: files; Name: "{localappdata}\Bionus\Grabber\sites\Gelbooru (0.2)\icon.png"
Type: files; Name: "{localappdata}\Bionus\Grabber\sites\Gelbooru (0.2)\model.js"
Type: files; Name: "{localappdata}\Bionus\Grabber\sites\Gelbooru (0.2)\gelbooru.com\defaults.ini"
Type: files; Name: "{localappdata}\Bionus\Grabber\sites\Moebooru\icon.png"
Type: files; Name: "{localappdata}\Bionus\Grabber\sites\Moebooru\model.js"
Type: files; Name: "{localappdata}\Bionus\Grabber\sites\NHentai\icon.png"
Type: files; Name: "{localappdata}\Bionus\Grabber\sites\NHentai\model.js"
Type: files; Name: "{localappdata}\Bionus\Grabber\sites\NHentai\nhentai.net\defaults.ini"
Type: files; Name: "{localappdata}\Bionus\Grabber\sites\Nijie\icon.png"
Type: files; Name: "{localappdata}\Bionus\Grabber\sites\Nijie\model.js"
Type: files; Name: "{localappdata}\Bionus\Grabber\sites\Nijie\nijie.info\defaults.ini"
Type: files; Name: "{localappdata}\Bionus\Grabber\sites\Philomena\icon.png"
Type: files; Name: "{localappdata}\Bionus\Grabber\sites\Philomena\model.js"
Type: files; Name: "{localappdata}\Bionus\Grabber\sites\Philomena\derpibooru.org\defaults.ini"
Type: files; Name: "{localappdata}\Bionus\Grabber\sites\Pixiv\icon.png"
Type: files; Name: "{localappdata}\Bionus\Grabber\sites\Pixiv\model.js"
Type: files; Name: "{localappdata}\Bionus\Grabber\sites\Pixiv\www.pixiv.net\defaults.ini"
Type: files; Name: "{localappdata}\Bionus\Grabber\sites\Reddit\icon.png"
Type: files; Name: "{localappdata}\Bionus\Grabber\sites\Reddit\model.js"
Type: files; Name: "{localappdata}\Bionus\Grabber\sites\Reddit\www.reddit.com\defaults.ini"
Type: files; Name: "{localappdata}\Bionus\Grabber\sites\Shimmie\icon.png"
Type: files; Name: "{localappdata}\Bionus\Grabber\sites\Shimmie\model.js"
Type: files; Name: "{localappdata}\Bionus\Grabber\sites\Twitter\icon.png"
Type: files; Name: "{localappdata}\Bionus\Grabber\sites\Twitter\model.js"
Type: files; Name: "{localappdata}\Bionus\Grabber\sites\Twitter\api.twitter.com\defaults.ini"
Type: files; Name: "{localappdata}\Bionus\Grabber\sites\WallHaven\icon.png"
Type: files; Name: "{localappdata}\Bionus\Grabber\sites\WallHaven\model.js"
Type: files; Name: "{localappdata}\Bionus\Grabber\sites\WallHaven\wallhaven.cc\defaults.ini"
Type: files; Name: "{localappdata}\Bionus\Grabber\sites\Zerochan\icon.png"
Type: files; Name: "{localappdata}\Bionus\Grabber\sites\Zerochan\model.js"
Type: files; Name: "{localappdata}\Bionus\Grabber\sites\Zerochan\www.zerochan.net\defaults.ini"

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
procedure CurStepChanged(CurStep: TSetupStep);
begin
  if CurStep = ssPostInstall then
  begin
    Log('Post install');
    SaveStringToFile(ExpandConstant('{localappdata}') + '\Bionus\Grabber\innosetup.ini', '[general]' + #13#10 + 'language=' + ExpandConstant('{language}') + #13#10, False);
  end;
end;

{ Pop-up to ask to delete AppData settings }
procedure CurUninstallStepChanged(CurUninstallStep: TUninstallStep);
begin
  if CurUninstallStep = usUninstall then begin
    if MsgBox('Do you also want to delete your settings directory?', mbConfirmation, MB_YESNO) = IDYES
    then begin
      Log('Deleting settings directory');
      if DelTree(ExpandConstant('{localappdata}\Bionus\Grabber'), True, True, True) then begin
        Log('Deleted settings directory');
      end else begin
        MsgBox('Error deleting settings directory', mbError, MB_OK);
      end;
    end;
  end;
end;

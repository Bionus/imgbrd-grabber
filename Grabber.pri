PDIR = $${_PRO_FILE_PWD_}/..

# Options
CONFIG += use_ssl
Release {
	CONFIG += use_breakpad
}

# Share build artifacts between projects without cluttering
CONFIG(debug, debug|release) {
	DESTDIR = build/debug
}
CONFIG(release, debug|release) {
	DESTDIR = build/release
}
OBJECTS_DIR = $$DESTDIR/obj
MOC_DIR     = $$DESTDIR/moc
RCC_DIR     = $$DESTDIR/qrc
UI_DIR      = $$DESTDIR/ui

# TODO: move to gui
QT += multimedia widgets multimediawidgets

# Travis settings
@
T = $$(TRAVIS)
!isEmpty(T) {
	CONFIG -= use_breakpad use_qscintilla
}
@

# Global
APP_VERSION = \\\"4.9.0\\\"

# General
TEMPLATE = app
DEPENDPATH += . .. $${PDIR}/languages $${PDIR}/src $${PDIR}/vendor
INCLUDEPATH += . .. $${PDIR}/src $${PDIR}/vendor
DEFINES += VERSION=$$APP_VERSION
QT += core network xml sql script

# Windows specials
win32 {
	QT += winextras
}

# Additionnal
CONFIG += plugin c++11
RESOURCES += $${PDIR}/resources.qrc
RC_FILE = ../icon.rc
CODECFORTR = UTF-8
TRANSLATIONS += $${PDIR}/languages/English.ts $${PDIR}/languages/Français.ts $${PDIR}/languages/Russian.ts

# Target WinXP
Release:win32 {
	QMAKE_LFLAGS_WINDOWS = /SUBSYSTEM:WINDOWS,5.01
	QMAKE_LFLAGS_CONSOLE = /SUBSYSTEM:CONSOLE,5.01
	DEFINES += _ATL_XP_TARGETING
	QMAKE_CFLAGS += /D _USING_V110_SDK71
	QMAKE_CXXFLAGS += /D _USING_V110_SDK71
	LIBS *= -L"%ProgramFiles(x86)%/Microsoft SDKs/Windows/7.1A/Lib"
	INCLUDEPATH += "%ProgramFiles(x86)%/Microsoft SDKs/Windows/7.1A/Include"
}

# SSL
use_ssl {
	win32 {
		LIBS += -L"C:/bin/OpenSSL-Win32/lib" -llibeay32
		INCLUDEPATH += C:/bin/OpenSSL-Win32/include
	}
	unix {
		PKGCONFIG += openssl
	}
}

# Google-Breakpad
use_breakpad {
	DEFINES += USE_BREAKPAD=1
	win32 {
		QMAKE_LFLAGS_RELEASE = /INCREMENTAL:NO /DEBUG
		QMAKE_CFLAGS_RELEASE = -O2 -MD -zi
		BREAKPAD = D:/bin/google-breakpad
		Debug:LIBS   += $${BREAKPAD}/src/client/windows/Debug/lib/common.lib \
						$${BREAKPAD}/src/client/windows/Debug/lib/crash_generation_client.lib \
						$${BREAKPAD}/src/client/windows/Debug/lib/exception_handler.lib
		Release:LIBS += $${BREAKPAD}/src/client/windows/Release/lib/common.lib \
						$${BREAKPAD}/src/client/windows/Release/lib/crash_generation_client.lib \
						$${BREAKPAD}/src/client/windows/Release/lib/exception_handler.lib
	}
	unix {
		QMAKE_CXXFLAGS += -fpermissive
		BREAKPAD = ~/Programmation/google-breakpad
		LIBS += $${BREAKPAD}/src/client/linux/libbreakpad_client.a
	}
	INCLUDEPATH += $${BREAKPAD}/src
}

OTHER_FILES += \
	$${PDIR}/Grabber.pri \
	$${PDIR}/icon.rc \
	$${PDIR}/.gitignore

DISTFILES += \
	$${PDIR}/README.md \
	$${PDIR}/LICENSE \
	$${PDIR}/NOTICE

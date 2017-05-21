PDIR = $${PWD}

# Options
CONFIG += use_ssl

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

# Global
APP_VERSION = \\\"5.3.1\\\"
APP_PREFIX = \\\"$$(PREFIX)\\\"

# General
TEMPLATE = app
QT += core network xml sql script

# Defines
DEFINES += VERSION=$$APP_VERSION
DEFINES += PREFIX=$$APP_PREFIX
DEFINES += PROJECT_WEBSITE_URL=\\\"https://bionus.github.io/imgbrd-grabber/\\\"
DEFINES += PROJECT_GITHUB_URL=\\\"https://github.com/Bionus/imgbrd-grabber\\\"

# Additionnal
CONFIG += plugin c++11
RESOURCES += $${PWD}/resources/resources.qrc
RC_FILE = ../resources/icon.rc
CODECFORTR = UTF-8
TRANSLATIONS += $${PWD}/languages/English.ts \
				$${PWD}/languages/French.ts \
				$${PWD}/languages/Russian.ts \
				$${PWD}/languages/ChineseSimplified.ts \
				$${PWD}/languages/Spanish.ts \
				$${PWD}/languages/YourLanguage.ts

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

# Code coverage
@
T = $$(TRAVIS)
!isEmpty(T) {
	unix:!macx {
		QMAKE_CXXFLAGS -= -O2
		QMAKE_CXXFLAGS_RELEASE -= -O2

		LIBS += -lgcov
		QMAKE_CXXFLAGS += -g -fprofile-arcs -ftest-coverage -O0 --coverage
		QMAKE_LFLAGS += -g -fprofile-arcs -ftest-coverage  -O0 --coverage
	}
}
@

OTHER_FILES += \
	$${PWD}/Grabber.pri \
	$${PWD}/icon.rc \
	$${PWD}/.gitignore

DISTFILES += \
	$${PWD}/README.md \
	$${PWD}/LICENSE \
	$${PWD}/NOTICE

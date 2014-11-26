# Options
CONFIG += use_qscintilla
#CONFIG += use_breakpad
#CONFIG += use_cli

# Global
TARGET = Grabber
APP_VERSION = \\\"4.0.0a\\\"

# General
TEMPLATE = app
DEPENDPATH += . debug includes languages release source ui
INCLUDEPATH += . includes source
DEFINES += VERSION=$$APP_VERSION
QT += network xml sql script designer multimedia

# Additionnal
CONFIG += plugin
RESOURCES += resources.qrc
RC_FILE = icon.rc
CODECFORTR = UTF-8
TRANSLATIONS += languages/English.ts languages/Français.ts languages/Russian.ts

# Command Line Interface
use_cli {
	DEFINES += USE_CLI=1
	CONFIG += console
}

# QScintilla
use_qscintilla {
	DEFINES += USE_QSCINTILLA=1
	LIBS += -lqscintilla2
}

# Google-Breakpad
use_breakpad {
	DEFINES += USE_BREAKPAD=1
	win32 {
		QMAKE_LFLAGS_RELEASE = /INCREMENTAL:NO /DEBUG
		QMAKE_CFLAGS_RELEASE = -O2 -MD -zi
		BREAKPAD = D:/Programmation/C++/Qt/google-breakpad
		Debug:LIBS		= $${BREAKPAD}/src/client/windows/Debug/lib/common.lib \
						  $${BREAKPAD}/src/client/windows/Debug/lib/crash_generation_client.lib \
						  $${BREAKPAD}/src/client/windows/Debug/lib/exception_handler.lib
		Release:LIBS	+= $${BREAKPAD}/src/client/windows/Release/lib/common.lib \
						   $${BREAKPAD}/src/client/windows/Release/lib/crash_generation_client.lib \
						   $${BREAKPAD}/src/client/windows/Release/lib/exception_handler.lib
	}
	unix {
		QMAKE_CXXFLAGS += -fpermissive
		BREAKPAD = /home/spark/Documents/Programmation/C++/Qt/google-breakpad
		LIBS += $${BREAKPAD}/src/client/linux/libbreakpad_client.a
	}
	INCLUDEPATH += $${BREAKPAD}/src
}

# Input
HEADERS += includes/*.h
FORMS += ui/*.ui
SOURCES += source/*.cpp

OTHER_FILES += \
    icon.rc \
    VERSION \
    LICENSE \
    README.md

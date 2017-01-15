# General
TARGET = Grabber
DEFINES += GUI=1
DEPENDPATH += ui
QT += widgets multimedia
Release {
	CONFIG += use_breakpad use_qscintilla
}

# Travis settings
@
T = $$(TRAVIS)
!isEmpty(T) {
	CONFIG -= use_breakpad use_qscintilla
}
@

# Include common config
!include(../Grabber.pri) {
	error("Could not find the common configuration file!")
}

# Include library
!include(../lib/lib.pri) {
	error("Could not find lib dependency configuration file!")
}

# Windows specials
win32 {
	QT += winextras
}

# QScintilla
use_qscintilla {
	DEFINES += USE_QSCINTILLA=1
	unix:!macx{
		exists( $(QTDIR)/lib/libqscintilla2-qt5*) {
			LIBS += -lqscintilla2-qt5
		}
		exists($(QTDIR)/lib/libqt5scintilla2*){
			LIBS += -lqt5scintilla2
		}
		exists(/usr/lib/libqt5scintilla2*){
			LIBS += -lqt5scintilla2
		}
		exists(/usr/lib64/libqt5scintilla2*){
			LIBS += -lqt5scintilla2
		}
		!contains(LIBS, "^-l*.scintilla2.*$"){
			message("libqscintilla2-qt5/libqt5scintilla2 not found disabling qscintilla support")
			DEFINES -=USE_QSCINTILLA=1
		}
	} else {
		LIBS += -lqscintilla2
	}
}

# Input
INCLUDEPATH += $${PDIR} $${PWD}/src
HEADERS += $${PDIR}/vendor/*.h \
	$${PWD}/src/*.h \
	$${PWD}/src/batch/*.h \
	$${PWD}/src/settings/*.h \
	$${PWD}/src/sources/*.h \
	$${PWD}/src/tabs/*.h \
	$${PWD}/src/ui/*.h \
	$${PWD}/src/updater/*.h \
	$${PWD}/src/utils/rename-existing/*.h \
	$${PWD}/src/utils/blacklist-fix/*.h \
	$${PWD}/src/utils/empty-dirs-fix/*.h \
	$${PWD}/src/utils/md5-fix/*.h \
	$${PWD}/src/viewer/*.h
SOURCES += $${PDIR}/vendor/*.cpp \
	$${PWD}/src/main/main.cpp \
	$${PWD}/src/*.cpp \
	$${PWD}/src/batch/*.cpp \
	$${PWD}/src/settings/*.cpp \
	$${PWD}/src/sources/*.cpp \
	$${PWD}/src/tabs/*.cpp \
	$${PWD}/src/ui/*.cpp \
	$${PWD}/src/updater/*.cpp \
	$${PWD}/src/utils/rename-existing/*.cpp \
	$${PWD}/src/utils/blacklist-fix/*.cpp \
	$${PWD}/src/utils/empty-dirs-fix/*.cpp \
	$${PWD}/src/utils/md5-fix/*.cpp \
	$${PWD}/src/viewer/*.cpp
FORMS += $${PWD}/src/*.ui \
	$${PWD}/src/batch/*.ui \
	$${PWD}/src/settings/*.ui \
	$${PWD}/src/sources/*.ui \
	$${PWD}/src/tabs/*.ui \
	$${PWD}/src/updater/*.ui \
	$${PWD}/src/utils/rename-existing/*.ui \
	$${PWD}/src/utils/blacklist-fix/*.ui \
	$${PWD}/src/utils/empty-dirs-fix/*.ui \
	$${PWD}/src/utils/md5-fix/*.ui \
	$${PWD}/src/viewer/*.ui

# Breakpad files
use_breakpad {
	HEADERS += $${PWD}/src/crashhandler/*.h
	SOURCES += $${PWD}/src/crashhandler/*.cpp
}

# Linux install script
unix:!macx{
	isEmpty(PREFIX){
		PREFIX = /usr/local
	}
	target.path = $$PREFIX/bin
	INSTALLS += target
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

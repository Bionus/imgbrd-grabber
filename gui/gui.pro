TARGET = Grabber

# GUI
Release {
	CONFIG += use_qscintilla
}
DEPENDPATH += ui

# Include common config
!include(../Grabber.pri) {
	error("Could not find the common configuration file!")
}

# Include library
!include(../lib/depends.pri) {
	error("Could not find lib dependency configuration file!")
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
INCLUDEPATH += $${PDIR} $${PDIR}/gui/src
HEADERS += $${PDIR}/vendor/*.h \
	$${PDIR}/gui/src/*.h \
	$${PDIR}/gui/src/batch/*.h \
	$${PDIR}/gui/src/settings/*.h \
	$${PDIR}/gui/src/sources/*.h \
	$${PDIR}/gui/src/tabs/*.h \
	$${PDIR}/gui/src/ui/*.h \
	$${PDIR}/gui/src/updater/*.h \
	$${PDIR}/gui/src/utils/rename-existing/*.h \
	$${PDIR}/gui/src/utils/blacklist-fix/*.h \
	$${PDIR}/gui/src/utils/empty-dirs-fix/*.h \
	$${PDIR}/gui/src/utils/md5-fix/*.h \
	$${PDIR}/gui/src/viewer/*.h
SOURCES += $${PDIR}/vendor/*.cpp \
	$${PDIR}/gui/src/main/main.cpp \
	$${PDIR}/gui/src/*.cpp \
	$${PDIR}/gui/src/batch/*.cpp \
	$${PDIR}/gui/src/settings/*.cpp \
	$${PDIR}/gui/src/sources/*.cpp \
	$${PDIR}/gui/src/tabs/*.cpp \
	$${PDIR}/gui/src/ui/*.cpp \
	$${PDIR}/gui/src/updater/*.cpp \
	$${PDIR}/gui/src/utils/rename-existing/*.cpp \
	$${PDIR}/gui/src/utils/blacklist-fix/*.cpp \
	$${PDIR}/gui/src/utils/empty-dirs-fix/*.cpp \
	$${PDIR}/gui/src/utils/md5-fix/*.cpp \
	$${PDIR}/gui/src/viewer/*.cpp
FORMS += $${PDIR}/gui/src/*.ui \
	$${PDIR}/gui/src/batch/*.ui \
	$${PDIR}/gui/src/settings/*.ui \
	$${PDIR}/gui/src/sources/*.ui \
	$${PDIR}/gui/src/tabs/*.ui \
	$${PDIR}/gui/src/updater/*.ui \
	$${PDIR}/gui/src/utils/rename-existing/*.ui \
	$${PDIR}/gui/src/utils/blacklist-fix/*.ui \
	$${PDIR}/gui/src/utils/empty-dirs-fix/*.ui \
	$${PDIR}/gui/src/utils/md5-fix/*.ui \
	$${PDIR}/gui/src/viewer/*.ui

# Breakpad files
use_breakpad {
	HEADERS += $${PDIR}/gui/src/crashhandler/*.h
	SOURCES += $${PDIR}/gui/src/crashhandler/*.cpp
}

# Linux install script
unix:!macx{
	isEmpty(PREFIX){
		PREFIX = /usr/local
	}
	target.path = $$PREFIX/bin
	INSTALLS += target
}

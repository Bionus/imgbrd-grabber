TARGET = Cli

# Command Line Interface
DEFINES += USE_CLI=1
CONFIG += console

# Include common config
!include(../Grabber.pri) {
	error("Could not find the common configuration file!")
}

# Include library
DEPENDPATH += $${PDIR}/lib
INCLUDEPATH += $${PDIR}/lib/src
CONFIG(release, debug|release) {
	LIBS = -L$${PDIR}/lib/build/release/ -llib
}
else {
	LIBS = -L$${PDIR}/lib/build/debug/ -llib
}

# TODO: remove these dependencies
QT += multimedia widgets
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
use_qscintilla {
	DEFINES += USE_QSCINTILLA=0
	LIBS -= -lqscintilla2
}
# end TODO

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

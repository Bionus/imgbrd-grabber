TARGET = Cli

# Command Line Interface
DEFINES += USE_CLI=1
CONFIG += console

# Include common config
!include(../Grabber.pri) {
	error("Could not find the common configuration file!")
}

# TODO: remove these dependencies
QT += multimedia widgets
FORMS += $${PDIR}/src/*.ui \
	$${PDIR}/src/batch/*.ui \
	$${PDIR}/src/settings/*.ui \
	$${PDIR}/src/sources/*.ui \
	$${PDIR}/src/tabs/*.ui \
	$${PDIR}/src/utils/rename-existing/*.ui \
	$${PDIR}/src/utils/blacklist-fix/*.ui \
	$${PDIR}/src/utils/empty-dirs-fix/*.ui \
	$${PDIR}/src/utils/md5-fix/*.ui \
	$${PDIR}/src/viewer/*.ui
use_qscintilla {
	DEFINES += USE_QSCINTILLA=0
	LIBS -= -lqscintilla2
}
# end TODO

# Input
HEADERS += $${PDIR}/vendor/*.h \
	$${PDIR}/src/*.h \
	$${PDIR}/src/batch/*.h \
	$${PDIR}/src/downloader/*.h \
	$${PDIR}/src/models/*.h \
	$${PDIR}/src/settings/*.h \
	$${PDIR}/src/sources/*.h \
	$${PDIR}/src/tabs/*.h \
	$${PDIR}/src/ui/*.h \
	$${PDIR}/src/utils/rename-existing/*.h \
	$${PDIR}/src/utils/blacklist-fix/*.h \
	$${PDIR}/src/utils/empty-dirs-fix/*.h \
	$${PDIR}/src/utils/md5-fix/*.h \
	$${PDIR}/src/viewer/*.h
SOURCES += $${PDIR}/vendor/*.cpp \
	$${PDIR}/src/main/main.cpp \
	$${PDIR}/src/*.cpp \
	$${PDIR}/src/batch/*.cpp \
	$${PDIR}/src/downloader/*.cpp \
	$${PDIR}/src/models/*.cpp \
	$${PDIR}/src/settings/*.cpp \
	$${PDIR}/src/sources/*.cpp \
	$${PDIR}/src/tabs/*.cpp \
	$${PDIR}/src/ui/*.cpp \
	$${PDIR}/src/utils/rename-existing/*.cpp \
	$${PDIR}/src/utils/blacklist-fix/*.cpp \
	$${PDIR}/src/utils/empty-dirs-fix/*.cpp \
	$${PDIR}/src/utils/md5-fix/*.cpp \
	$${PDIR}/src/viewer/*.cpp

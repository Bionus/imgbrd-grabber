# Include common config
!include(../Grabber.pri) {
	error("Could not find the common configuration file!")
}

# Library project
TEMPLATE = lib
TARGET = lib
CONFIG += staticlib

# Input
INCLUDEPATH += $${PDIR} $${PDIR}/lib/src
HEADERS += $${PDIR}/vendor/*.h \
	$${PDIR}/lib/src/*.h \
	$${PDIR}/lib/src/commands/*.h \
	$${PDIR}/lib/src/downloader/*.h \
	$${PDIR}/lib/src/models/*.h \
	$${PDIR}/lib/src/updater/*.h
SOURCES += $${PDIR}/vendor/*.cpp \
	$${PDIR}/lib/src/*.cpp \
	$${PDIR}/lib/src/commands/*.cpp \
	$${PDIR}/lib/src/downloader/*.cpp \
	$${PDIR}/lib/src/models/*.cpp \
	$${PDIR}/lib/src/updater/*.cpp

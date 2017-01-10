# Include common config
!include(../Grabber.pri) {
	error("Could not find the common configuration file!")
}

# Library project
TEMPLATE = lib
TARGET = lib
CONFIG += staticlib

# Input
INCLUDEPATH += $${PDIR}/lib/src
HEADERS += $${PDIR}/vendor/*.h \
#	$${PDIR}/lib/src/*.h \
	$${PDIR}/lib/src/models/*.h
SOURCES += $${PDIR}/vendor/*.cpp \
#	$${PDIR}/lib/src/*.cpp \
	$${PDIR}/lib/src/models/*.cpp

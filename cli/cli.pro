# General
TARGET = Cli
CONFIG += console
DEFINES += CLI=1

# Include common config
!include(../Grabber.pri) {
	error("Could not find the common configuration file!")
}

# Include library
!include(../lib/depends.pri) {
	error("Could not find lib dependency configuration file!")
}

# Input
INCLUDEPATH += $${PDIR} $${PWD}/src
HEADERS += $${PDIR}/vendor/*.h
SOURCES += $${PDIR}/vendor/*.cpp \
	$${PWD}/src/*.cpp

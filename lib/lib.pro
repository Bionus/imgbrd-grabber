# Include common config
!include(../Grabber.pri) {
	error("Could not find the common configuration file!")
}

# General
TEMPLATE = lib
TARGET = lib
CONFIG += staticlib

# Input
INCLUDEPATH += $${PWD}/src
HEADERS += $${PWD}/src/*.h \
	$${PWD}/src/commands/*.h \
	$${PWD}/src/downloader/*.h \
	$${PWD}/src/models/*.h \
	$${PWD}/src/updater/*.h \
	$${PWD}/src/vendor/*.h
SOURCES += $${PWD}/src/*.cpp \
	$${PWD}/src/commands/*.cpp \
	$${PWD}/src/downloader/*.cpp \
	$${PWD}/src/models/*.cpp \
	$${PWD}/src/updater/*.cpp \
	$${PWD}/src/vendor/*.cpp

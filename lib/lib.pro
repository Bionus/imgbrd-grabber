PRO = $${_PRO_FILE_PWD_}

# Include common config
!include(../Grabber.pri) {
	error("Could not find the common configuration file!")
}

# Library project
TEMPLATE = lib
TARGET = lib
CONFIG += staticlib

# Input
INCLUDEPATH += $${PDIR} $${PRO}/src
HEADERS += $${PDIR}/vendor/*.h \
	$${PRO}/src/*.h \
	$${PRO}/src/commands/*.h \
	$${PRO}/src/downloader/*.h \
	$${PRO}/src/models/*.h \
	$${PRO}/src/updater/*.h
SOURCES += $${PDIR}/vendor/*.cpp \
	$${PRO}/src/*.cpp \
	$${PRO}/src/commands/*.cpp \
	$${PRO}/src/downloader/*.cpp \
	$${PRO}/src/models/*.cpp \
	$${PRO}/src/updater/*.cpp

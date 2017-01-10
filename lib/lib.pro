# Include common config
!include(../Grabber.pri) {
	error("Could not find the common configuration file!")
}

# Code coverage
@
T = $$(TRAVIS)
!isEmpty(T) {
	unix:!macx {
		QMAKE_CXXFLAGS -= -O2
		QMAKE_CXXFLAGS_RELEASE -= -O2

		LIBS += -lgcov
		QMAKE_CXXFLAGS += -g -fprofile-arcs -ftest-coverage -O0
		QMAKE_LFLAGS += -g -fprofile-arcs -ftest-coverage  -O0
	}
}
@

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
	$${PDIR}/lib/src/models/*.h
SOURCES += $${PDIR}/vendor/*.cpp \
	$${PDIR}/lib/src/*.cpp \
	$${PDIR}/lib/src/commands/*.cpp \
	$${PDIR}/lib/src/downloader/*.cpp \
	$${PDIR}/lib/src/models/*.cpp

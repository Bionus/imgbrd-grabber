include(../gui/gui.pro)

# Test configuration
QT       += testlib
TARGET    = tests
CONFIG   += console testcase
CONFIG   -= app_bundle

TEMPLATE  = app

INCLUDEPATH += . .. $${PDIR}/tests
DEFINES     += SRCDIR=\\\"$$PWD/\\\"

# Remove original main
SOURCES -= $${PDIR}/src/main/main.cpp

SOURCES += $${PDIR}/tests/*.cpp \
	$${PDIR}/tests/models/*.cpp
HEADERS += $${PDIR}/tests/models/*.h

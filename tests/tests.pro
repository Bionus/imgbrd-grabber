# Include common config
!include(../Grabber.pri) {
	error("Could not find the common configuration file!")
}

# Include library
!include(../lib/lib.pri) {
	error("Could not find lib dependency configuration file!")
}

# Test configuration
QT      += testlib
TARGET   = tests
CONFIG  += console testcase
CONFIG  -= app_bundle
TEMPLATE = app
DEFINES += TEST=1
target.files = 
INSTALLS += target

# Input
INCLUDEPATH += $${PWD}/src/ $${PDIR}/lib/src
SOURCES += $${PWD}/src/test-suite.cpp \
	$${PWD}/src/main.cpp \
	$${PWD}/src/functions-test.cpp \
	$${PWD}/src/models/*.cpp \
	$${PWD}/src/updater/*.cpp \
	$${PWD}/src/integration/*.cpp
HEADERS += $${PWD}/src/*.h \
	$${PWD}/src/models/*.h \
	$${PWD}/src/updater/*.h \
	$${PWD}/src/integration/*.h

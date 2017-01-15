include(../gui/gui.pro)

# Disable unnecessary stuff
CONFIG  -= use_breakpad use_qscintilla

# Test configuration
QT      += testlib
TARGET   = tests
CONFIG  += console testcase
CONFIG  -= app_bundle
TEMPLATE = app

INCLUDEPATH += . .. $${PWD}
DEFINES     += TEST=1

# Code coverage
@
T = $$(TRAVIS)
!isEmpty(T) {
	unix:!macx {
		QMAKE_CXXFLAGS -= -O2
		QMAKE_CXXFLAGS_RELEASE -= -O2

		LIBS += -lgcov
		QMAKE_CXXFLAGS += -g -fprofile-arcs -ftest-coverage -O0 --coverage
		QMAKE_LFLAGS += -g -fprofile-arcs -ftest-coverage  -O0 --coverage
	}
}
@

# Remove original main
SOURCES -= $${PDIR}/gui/src/main/main.cpp

SOURCES += $${PWD}/test-suite.cpp \
	$${PWD}/main.cpp \
	$${PWD}/functions-test.cpp \
	$${PWD}/models/*.cpp \
	$${PWD}/updater/*.cpp \
	$${PWD}/integration/*.cpp
HEADERS += $${PWD}/*.h \
	$${PWD}/models/*.h \
	$${PWD}/updater/*.h \
	$${PWD}/integration/*.h

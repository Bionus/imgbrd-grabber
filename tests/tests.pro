include(../gui/gui.pro)

# Disable unnecessary stuff
CONFIG -= use_breakpad use_qscintilla

# Test configuration
QT       += testlib
TARGET    = tests
CONFIG   += console testcase
CONFIG   -= app_bundle

TEMPLATE  = app

INCLUDEPATH += . .. $${PDIR}/tests
DEFINES     += SRCDIR=\\\"$$PWD/\\\"
DEFINES     += TEST=1

# Code coverage
@
T = $$(TRAVIS)
!isEmpty(T) {
    QMAKE_CXXFLAGS -= -O2
    QMAKE_CXXFLAGS_RELEASE -= -O2

    LIBS += -lgcov
    QMAKE_CXXFLAGS += -g -fprofile-arcs -ftest-coverage -O0
    QMAKE_LFLAGS += -g -fprofile-arcs -ftest-coverage  -O0
}
@

# Remove original main
SOURCES -= $${PDIR}/src/main/main.cpp

SOURCES += $${PDIR}/tests/*.cpp \
    $${PDIR}/tests/models/*.cpp \
    $${PDIR}/tests/integration/*.cpp
HEADERS += $${PDIR}/tests/*.h \
    $${PDIR}/tests/models/*.h \
    $${PDIR}/tests/integration/*.h

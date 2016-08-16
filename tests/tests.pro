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

# Remove original main
SOURCES -= $${PDIR}/src/main/main.cpp

SOURCES += $${PDIR}/tests/*.cpp \
    $${PDIR}/tests/models/*.cpp \
    $${PDIR}/tests/integration/*.cpp
HEADERS += $${PDIR}/tests/*.h \
    $${PDIR}/tests/models/*.h \
    $${PDIR}/tests/integration/*.h

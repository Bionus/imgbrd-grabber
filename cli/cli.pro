TARGET = Cli

# Command Line Interface
DEFINES += USE_CLI=1
CONFIG += console

# Include common config
!include(../Grabber.pri) {
    error("Could not find the common configuration file!")
}

# TODO: remove these dependencies
QT += multimedia widgets
FORMS += $${PDIR}/ui/*.ui
use_qscintilla {
    DEFINES += USE_QSCINTILLA=0
    LIBS -= -lqscintilla2
}
# end TODO

# Input
HEADERS += $${PDIR}/includes/*.h $${PDIR}/vendor/*.h
SOURCES += $${PDIR}/source/*.cpp $${PDIR}/vendor/*.cpp

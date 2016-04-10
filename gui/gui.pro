TARGET = Grabber

# GUI
QT += multimedia widgets
CONFIG += use_qscintilla
DEPENDPATH += ui

# Include common config
!include(../Grabber.pri) {
    error("Could not find the common configuration file!")
}

# QScintilla
use_qscintilla {
	DEFINES += USE_QSCINTILLA=1
	LIBS += -lqscintilla2
}

# Input
HEADERS += $${PDIR}/includes/*.h $${PDIR}/vendor/*.h
FORMS += $${PDIR}/ui/*.ui
SOURCES += $${PDIR}/source/*.cpp $${PDIR}/vendor/*.cpp

TARGET = Grabber

# GUI
CONFIG += use_qscintilla
DEPENDPATH += ui

# Include common config
!include(../Grabber.pri) {
    error("Could not find the common configuration file!")
}

# QScintilla
use_qscintilla {
	DEFINES += USE_QSCINTILLA=1
	unix:!macx{
		exists( $(QTDIR)/lib/libqscintilla2-qt5*) {
			LIBS += -lqscintilla2-qt5
		}
		exists($(QTDIR)/lib/libqt5scintilla2*){
			LIBS += -lqt5scintilla2
		}
		exists(/usr/lib/libqt5scintilla2*){
			LIBS += -lqt5scintilla2
		}
		exists(/usr/lib64/libqt5scintilla2*){
			LIBS += -lqt5scintilla2
		}
		!contains(LIBS, "^-l*.scintilla2.*$"){
			message("libqscintilla2-qt5/libqt5scintilla2 not found disabling qscintilla support")
			DEFINES -=USE_QSCINTILLA=1
		}
	} else {
		LIBS += -lqscintilla2
	}
}

# Input
HEADERS += $${PDIR}/includes/*.h \
    $${PDIR}/vendor/*.h \
    $${PDIR}/source/rename-existing/*.h
SOURCES += $${PDIR}/source/*.cpp \
    $${PDIR}/vendor/*.cpp \
    $${PDIR}/source/rename-existing/*.cpp
FORMS += $${PDIR}/ui/*.ui \
    ../source/rename-existing/*.ui

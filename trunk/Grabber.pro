######################################################################
# Automatically generated by qmake (2.01a) dim. 10. avr. 16:55:35 2011
######################################################################

TEMPLATE = app
TARGET =
DEPENDPATH += . includes languages source
INCLUDEPATH += . includes languages source

# Input
HEADERS += includes/addgroupwindow.h \
	includes/adduniquewindow.h \
	includes/advancedWindow.h \
	includes/functions.h \
	includes/mainWindow.h \
	includes/optionsWindow.h \
	includes/QAffiche.h \
	includes/QBouton.h \
	includes/textedit.h \
	includes/zoomWindow.h \
	includes/json.h \
    includes/favoritewindow.h
SOURCES += source/addgroupwindow.cpp \
	source/adduniquewindow.cpp \
	source/advancedWindow.cpp \
	source/functions.cpp \
	source/main.cpp \
	source/mainWindow.cpp \
	source/optionsWindow.cpp \
	source/QAffiche.cpp \
	source/QBouton.cpp \
	source/textedit.cpp \
	source/zoomWindow.cpp \
	source/json.cpp \
    source/favoritewindow.cpp
RESOURCES += resources.qrc
RC_FILE = icon.rc
TRANSLATIONS += languages/English.ts languages/Fran�ais.ts
QT += network xml

FORMS += \
    ui/favoriteWindow.ui

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
	includes/functions.h \
	includes/mainwindow.h \
	includes/qaffiche.h \
	includes/qbouton.h \
	includes/textedit.h \
	includes/zoomwindow.h \
	includes/json.h \
	includes/favoritewindow.h \
	includes/detailswindow.h \
	includes/startwindow.h \
	includes/optionswindow.h \
	includes/batchwindow.h \
	includes/sourceswindow.h \
	includes/sitewindow.h \
	includes/page.h \
	includes/image.h \
	includes/aboutwindow.h \
	includes/tag.h \
	includes/imagethread.h \
	includes/searchtab.h \
    includes/customwindow.h \
    includes/blacklistfix.h \
    includes/blacklistfix2.h \
    includes/blacklistfix3.h \
    includes/searchwindow.h
SOURCES += source/addgroupwindow.cpp \
	source/adduniquewindow.cpp \
	source/functions.cpp \
	source/main.cpp \
	source/mainwindow.cpp \
	source/qaffiche.cpp \
	source/qbouton.cpp \
	source/textedit.cpp \
	source/zoomwindow.cpp \
	source/json.cpp \
	source/favoritewindow.cpp \
	source/detailswindow.cpp \
	source/startwindow.cpp \
	source/optionswindow.cpp \
	source/batchwindow.cpp \
	source/sourceswindow.cpp \
	source/sitewindow.cpp \
	source/page.cpp \
	source/image.cpp \
	source/aboutwindow.cpp \
	source/tag.cpp \
	source/imagethread.cpp \
	source/searchtab.cpp \
    source/customwindow.cpp \
    source/blacklistfix.cpp \
    source/blacklistfix2.cpp \
    source/blacklistfix3.cpp \
    source/searchwindow.cpp
RESOURCES += resources.qrc
RC_FILE = icon.rc
TRANSLATIONS += languages/English.ts languages/Fran�ais.ts
QT += network xml
CONFIG += designer plugin

FORMS += \
	ui/favoritewindow.ui \
	ui/zoomwindow.ui \
	ui/detailswindow.ui \
	ui/startwindow.ui \
	ui/optionswindow.ui \
	ui/mainwindow.ui \
	ui/batchwindow.ui \
	ui/sourceswindow.ui \
	ui/sitewindow.ui \
    ui/aboutwindow.ui \
    ui/searchtab.ui \
    ui/customwindow.ui \
    ui/blacklistfix.ui \
    ui/blacklistfix2.ui \
    ui/blacklistfix3.ui \
    ui/searchwindow.ui




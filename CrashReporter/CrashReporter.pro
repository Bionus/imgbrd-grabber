#-------------------------------------------------
#
# Project created by QtCreator 2012-12-01T23:25:49
#
#-------------------------------------------------

CONFIG += qt
QT       += core gui widgets designer

TARGET = CrashReporter
TEMPLATE = app

SOURCES += main.cpp\
        mainwindow.cpp

HEADERS  += mainwindow.h

FORMS    += mainwindow.ui

RESOURCES += resources.qrc
RC_FILE = icon.rc
TRANSLATIONS += languages/English.ts languages/Français.ts

# Basic config
CONFIG   += qt
QT       += core gui widgets
TARGET    = CrashReporter
TEMPLATE  = app

# Source files
SOURCES += main.cpp \
		   mainwindow.cpp
HEADERS	+= mainwindow.h
FORMS	+= mainwindow.ui

# Resources
RESOURCES    += resources/resources.qrc
RC_FILE       = resources/icon.rc
TRANSLATIONS += languages/English.ts \
				languages/French.ts \
				languages/Russian.ts \
				languages/YourLanguage.ts

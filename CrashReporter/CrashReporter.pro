# Basic config
CONFIG   += qt
QT       += core gui widgets
TARGET    = CrashReporter
TEMPLATE  = app

# Defines
DEFINES += PROJECT_GITHUB_URL=\\\"https://github.com/Bionus/imgbrd-grabber\\\"

# Source files
SOURCES += main.cpp \
		   mainwindow.cpp
HEADERS	+= mainwindow.h
FORMS	+= mainwindow.ui

# Resources
RESOURCES    += resources.qrc
RC_FILE       = icon.rc
TRANSLATIONS += languages/English.ts \
				languages/French.ts \
				languages/ChineseSimplified.ts \
				languages/Russian.ts \
				languages/Spanish.ts \
				languages/YourLanguage.ts

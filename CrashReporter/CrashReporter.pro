# Basic config
CONFIG     += qt
QT         += core gui widgets designer
TARGET      = CrashReporter
TEMPLATE    = app

# Source files
SOURCES    += main.cpp mainwindow.cpp
HEADERS    += mainwindow.h
FORMS      += mainwindow.ui

# Resources
RESOURCES      += resources.qrc
RC_FILE         = icon.rc
TRANSLATIONS   += languages/English.ts \
                  languages/Français.ts

# Multiple-project project
TEMPLATE = subdirs
CONFIG  += ordered

# List of subprojects
SUBDIRS  = lib
SUBDIRS += gui
SUBDIRS += tests
SUBDIRS += CrashReporter
gui.depends = lib
tests.depends = lib

# CLI project for Windows
win32 {
	SUBDIRS += cli
	cli.depends = lib
}

# Setup script for Linux
unix:!macx{
	isEmpty(PREFIX){
		PREFIX = /usr/local
	}

	config.path = $$PREFIX/share/Grabber
	config.extra = touch release/settings.ini
	config.files = release/sites release/words.txt release/settings.ini

	languages.path = $$PREFIX/share/Grabber
	languages.files = release/languages

	desktop.path = $$PREFIX/share/applications
	desktop.files += release/Grabber.desktop

	icon.path = $$PREFIX/share/pixmaps
	icon.extra = install -D -m644 resources/icon.png $(INSTALL_ROOT)/$$PREFIX/share/pixmaps/Grabber.png

	INSTALLS += desktop icon config languages
}

TEMPLATE = subdirs
CONFIG += ordered

SUBDIRS = gui
win32 {
    SUBDIRS += cli
}
SUBDIRS += tests

unix:!macx{
	isEmpty(PREFIX){
		PREFIX = /usr/local
	}
	config.path = $$PREFIX/share/Grabber/example
	config.extra = touch release/settings.ini
	config.files = release/languages release/sites release/words.txt release/settings.ini

	desktop.path = $$PREFIX/share/applications/Grabber
	desktop.files += release/Grabber.desktop
	icon.path = $$PREFIX/share/icons/128x128/apps
	icon.extra = cp icon.png Grabber.png
	icon.files += Grabber.png
	INSTALLS += desktop icon config
}

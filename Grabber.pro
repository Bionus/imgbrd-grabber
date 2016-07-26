TEMPLATE = subdirs
CONFIG += ordered

SUBDIRS = gui
win32 {
    SUBDIRS += cli
}
SUBDIRS += tests

unix:!macx{
	target.path = /usr/local/bin
	target.files += gui/Grabber
	config.path = /usr/local/Grabber/example
	config.files = release/languages release/sites release/words.txt
	config.extra = touch /usr/local/Grabber/example/settings.ini
	desktop.path = /usr/share/applications/Grabber
	desktop.files += release/Grabber.desktop
	icon.path = /usr/share/icons/128x128/apps
	icon.extra = cp icon.png Grabber.png
	icon.files += Grabber.png
	INSTALLS += target desktop icon config
}

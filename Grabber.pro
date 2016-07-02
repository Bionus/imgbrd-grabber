TEMPLATE = subdirs
CONFIG += ordered

SUBDIRS = gui
win32 {
    SUBDIRS += cli
}
SUBDIRS += tests

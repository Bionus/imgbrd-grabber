DEPENDPATH += $${PWD}
INCLUDEPATH += $${PWD}/src

CONFIG(release, debug|release) {
	LIBS = -L$${PWD}/build/release/ -llib
	win32: PRE_TARGETDEPS += $${PWD}/build/release/lib.lib
	unix:  PRE_TARGETDEPS += $${PWD}/build/release/liblib.a
}
else {
	LIBS = -L$${PWD}/build/debug/ -llib
	win32: PRE_TARGETDEPS += $${PWD}/build/debug/lib.lib
	unix:  PRE_TARGETDEPS += $${PWD}/build/debug/liblib.a
}

DEPENDPATH += $${PDIR}/lib
INCLUDEPATH += $${PDIR}/lib/src

CONFIG(release, debug|release) {
	LIBS = -L$${PDIR}/lib/build/release/ -llib
	win32: PRE_TARGETDEPS += $${PDIR}/lib/build/release/lib.lib
	unix:  PRE_TARGETDEPS += $${PDIR}/lib/build/release/liblib.a
}
else {
	LIBS = -L$${PDIR}/lib/build/debug/ -llib
	win32: PRE_TARGETDEPS += $${PDIR}/lib/build/debug/lib.lib
	unix:  PRE_TARGETDEPS += $${PDIR}/lib/build/debug/liblib.a
}
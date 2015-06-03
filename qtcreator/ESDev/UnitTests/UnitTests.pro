#-------------------------------------------------
#
# Project created by QtCreator 2015-06-03T02:44:53
#
#-------------------------------------------------

include(../ESDev.pri)

QT       += widgets network opengl sql testlib


macx {
	DESTDIR = ../../../executable
#	CONFIG(release, debug|release):
#QMAKE_POST_LINK = macdeployqt $${TARGET}.app -verbose=3
}


win32 {
	DESTDIR = ../../../executable
}

linux {
	DESTDIR = ../../../executable
}



TARGET = ESTkUnitTests
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += \
    main.cpp
DEFINES += SRCDIR=\\\"$$PWD/\\\"

HEADERS += \
    test_unittest.h \
    tst_unitteststest.h

#-------------------------------------------------
#
# Project created by QtCreator 2014-12-21T07:10:54
# DONT FUCKING USE -msse IN LUA LIBS ON WINDOWS!!!!111
#-------------------------------------------------
#include(../../../qtcreator/ESDev/ESDev.pri)
QT       -= core gui

TARGET = Lua5_2
TEMPLATE = lib
CONFIG += staticlib

QMAKE_CFLAGS += -Wno-deprecated-declarations

DEFINES += ELYSIAN_LUA_MODS

INCLUDEPATH += ../../include \
    ../../include/lua \
    ../../../libGyro/include

unix {
    target.path = /usr/lib
    INSTALLS += target
}


SOURCES += \
    ../../source/lapi.c \
    ../../source/lauxlib.c \
    ../../source/lbaselib.c \
    ../../source/lbitlib.c \
    ../../source/lcode.c \
    ../../source/lcorolib.c \
    ../../source/lctype.c \
    ../../source/ldblib.c \
    ../../source/ldebug.c \
    ../../source/ldo.c \
    ../../source/ldump.c \
    ../../source/lfunc.c \
    ../../source/lgc.c \
    ../../source/linit.c \
    ../../source/liolib.c \
    ../../source/llex.c \
    ../../source/lmathlib.c \
    ../../source/lmem.c \
    ../../source/loadlib.c \
    ../../source/lobject.c \
    ../../source/lopcodes.c \
    ../../source/loslib.c \
    ../../source/lparser.c \
    ../../source/lstate.c \
    ../../source/lstring.c \
    ../../source/lstrlib.c \
    ../../source/ltable.c \
    ../../source/ltablib.c \
    ../../source/ltm.c \
    ../../source/lua.c \
    ../../source/lundump.c \
    ../../source/lvm.c \
    ../../source/lzio.c

HEADERS += \
    ../../include/lua/lapi.h \
    ../../include/lua/lauxlib.h \
    ../../include/lua/lcode.h \
    ../../include/lua/lctype.h \
    ../../include/lua/ldebug.h \
    ../../include/lua/ldo.h \
    ../../include/lua/lfunc.h \
    ../../include/lua/lgc.h \
    ../../include/lua/llex.h \
    ../../include/lua/llimits.h \
    ../../include/lua/lmem.h \
    ../../include/lua/lobject.h \
    ../../include/lua/lopcodes.h \
    ../../include/lua/lparser.h \
    ../../include/lua/lstate.h \
    ../../include/lua/lstring.h \
    ../../include/lua/ltable.h \
    ../../include/lua/ltm.h \
    ../../include/lua/lua.h \
    ../../include/lua/lua.hpp \
    ../../include/lua/luaconf.h \
    ../../include/lua/lualib.h \
    ../../include/lua/lundump.h \
    ../../include/lua/lvm.h \
    ../../include/lua/lzio.h \
    ../../../libGyro/source/ios/gyviewcontroller.h

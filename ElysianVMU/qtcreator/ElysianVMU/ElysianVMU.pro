include(../../../qtcreator/ESDev/ESDev.pri)
TEMPLATE = app
TARGET = ElysianVMU
QT += gui opengl

win32 {
    DESTDIR = ../../../executable
}

linux {
    DESTDIR = ../../../executable
}

QMAKE_CFLAGS += -std=c99

macx {
    #DESTDIR = ../../executable
    #    CONFIG(release, debug|release):
    release: QMAKE_POST_LINK = "sh ./macdeployvmu.sh"
}

ICON = resource/images/vmu_icon.icns

SOURCES += ../../console/main.cpp \
    ../../lib/source/gyro_vmu_cpu.c \
    ../../lib/source/gyro_vmu_bios.c \
    ../../lib/source/gyro_vmu_device.c \
    ../../lib/source/gyro_vmu_instr.c \
    ../../lib/source/gyro_vmu_memory.c \
    ../../lib/source/gyro_vmu_display.c \
    ../../lib/source/gyro_vmu_timers.c \
    ../../lib/source/gyro_vmu_osc.c \
    gyro_vmu_main_window.cpp \
    ../../lib/source/gyro_vmu_gamepad.c \
    ../../lib/source/gyro_vmu_bullshit.c \
    ../../lib/source/gyro_vmu_buzzer.c \
    ../../lib/source/gyro_vmu_flash.c \
    ../../lib/source/gyro_vmu_vms.c \
    ../../lib/source/gyro_vmu_vmi.c \
    ../../lib/source/gyro_vmu_serial.c \
    ../../lib/source/gyro_vmu_battery.c \
    ../../lib/source/gyro_vmu_util.c \
    gyro_vmu_tcp_dialog.cpp \
    ../../lib/source/gyro_vmu_version.c


HEADERS += \
    ../../lib/source/gyro_vmu_sfr.h \
    ../../lib/source/gyro_vmu_instr.h \
    ../../lib/source/gyro_vmu_isr.h \
    ../../lib/source/gyro_vmu_device.h \
    ../../lib/source/gyro_vmu_bios.h \
    ../../lib/source/gyro_vmu_cpu.h \
    ../../lib/source/gyro_vmu_display.h \
    ../../lib/source/gyro_vmu_gamepad.h \
    ../../lib/source/gyro_vmu_memory.h \
    ../../lib/source/gyro_vmu_timers.h \
    ../../lib/source/gyro_vmu_osc.h \
    gyro_vmu_main_window.hpp \
    ../../lib/source/gyro_vmu_buzzer.h \
    ../../lib/source/gyro_vmu_flash.h \
    ../../lib/source/gyro_vmu_vms.h \
    ../../lib/source/gyro_vmu_vmi.h \
    ../../lib/source/gyro_vmu_serial.h \
    ../../lib/source/gyro_vmu_battery.h \
    ../../lib/source/gyro_vmu_util.h \
    gyro_vmu_tcp_dialog.h \
    ../../lib/source/gyro_vmu_version.h

INCLUDEPATH += ../../../libGyro/include \
    ../../lib/source

DEPENDPATH += ../../../libGyro/include \
    ../../lib/source \
    ../../lib/include

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../../../libGyro/qtcreator/libGyro/release/ -llibGyro
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../../../libGyro/qtcreator/libGyro/debug/ -llibGyro
else:unix: LIBS += -L$$OUT_PWD/../../../libGyro/qtcreator/libGyro/ -llibGyro

INCLUDEPATH += $$PWD/../../../libGyro/qtcreator/libGyro
DEPENDPATH += $$PWD/../../../libGyro/qtcreator/libGyro

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../../libGyro/qtcreator/libGyro/release/liblibGyro.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../../libGyro/qtcreator/libGyro/debug/liblibGyro.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../../libGyro/qtcreator/libGyro/release/libGyro.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../../libGyro/qtcreator/libGyro/debug/libGyro.lib
else:unix: PRE_TARGETDEPS += $$OUT_PWD/../../../libGyro/qtcreator/libGyro/liblibGyro.a

FORMS += \
    gyro_vmu_main_window.ui \
    gyro_vmu_tcp_dialog.ui


macx {
  LIBS +=  -L../../../libGyro/lib/osx \
        -framework OpenGL \
        -framework OpenAL \
        -lz \
        -F../../../ESGamma/lib/osx \
        -L../../../ESGamma/lib/osx \
        -F../../../libGyro/lib/osx \
        -framework SDL2 \
        -framework SDL2_net \
        -framework Foundation \
        -framework Cocoa \
        -lobjc \
        -logg \
        -lvorbis \
        -lvorbisenc \
        -lvorbisfile \
        -L../../../ESGamma/lib/osx
}


win32 {
    QMAKE_CFLAGS += -msse -std=c99
    INCLUDEPATH += ../../../libGyro/lib/win32
    LIBS += -L../../../lib/win32 \
        -L../../../libGyro/lib/win32 \
        -lDbgHelp \
        -lglew32 \
        -lglu32 \
        -lopengl32 \
        -lopenal32.dll \
        -lz \
        -lz.dll \
        -lSDL2 \
        -lSDL2Main \
        -lSDL2.dll \
        -lSDL2_net \
        -lSDL2_net.dll \
        -logg \
        -lvorbis \
        -lvorbis.dll \
        -lvorbisenc \
        -lvorbisenc.dll \
        -lvorbisfile \
        -limagehlp \
        -lvorbisfile.dll

}

linux {
    LIBS += \
        -lSDL2 \
        -lGL \
        -lopenal \
        -lGLU \
        -lGLEW \
        -lz \
        -logg \
        -lvorbis \
        -lvorbisenc \
        -lvorbisfile \
        -lX11 \
        -lpthread \
        -lrt
}


DISTFILES +=

RESOURCES += \
    resource/resource.qrc



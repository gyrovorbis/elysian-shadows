include(../../../qtcreator/ESDev/ESDev.pri)
vmu {
    TEMPLATE = app
}
vmu_child_app {
    TEMPLATE = lib
    CONFIG += staticlib
}
TARGET = ElysianVMU
QT += gui opengl

!raspberrypi : !ios {
    QT += serialport
}


!vmu_child_app {
    win32 {
        DESTDIR = ../../../executable
    }

    linux {
        DESTDIR = ../../../executable
    }
}

QMAKE_CFLAGS += -std=c99

vmu {
    macx {
        #DESTDIR = ../../executable
        #    CONFIG(release, debug|release):
        release: QMAKE_POST_LINK = "sh ./macdeployvmu.sh"
    }
}

ICON = resource/images/vmu_icon.icns

SOURCES += \
    ../../lib/source/gyro_vmu_cpu.c \
    ../../lib/source/gyro_vmu_bios.c \
    ../../lib/source/gyro_vmu_device.c \
    ../../lib/source/gyro_vmu_instr.c \
    ../../lib/source/gyro_vmu_memory.c \
    ../../lib/source/gyro_vmu_display.c \
    ../../lib/source/gyro_vmu_timers.c \
    ../../lib/source/gyro_vmu_osc.c \
    ../../lib/source/gyro_vmu_gamepad.c \
    ../../lib/source/gyro_vmu_buzzer.c \
    ../../lib/source/gyro_vmu_flash.c \
    ../../lib/source/gyro_vmu_vms.c \
    ../../lib/source/gyro_vmu_vmi.c \
    ../../lib/source/gyro_vmu_battery.c \
    ../../lib/source/gyro_vmu_util.c \
    ../../lib/source/gyro_vmu_version.c \
    ../../lib/source/gyro_vmu_disassembler.c \
    ../../lib/source/gyro_vmu_tcp.c \
    ../../lib/source/gyro_vmu_serial.c \
    ../../lib/source/gyro_vmu_port1.c \
    ../../lib/source/gyro_vmu_port7.c \
    ../../lib/source/gyro_vmu_lcd.c \
    gyro_vmu_main_window.cpp \
    gyro_vmu_tcp_dialog.cpp \
    gif/qgifimage.cpp \
    gif/dgif_lib.c \
    gif/egif_lib.c \
    gif/gif_err.c \
    gif/gif_font.c \
    gif/gif_hash.c \
    gif/gifalloc.c \
    gif/quantize.c

!vmu_child_app {
    SOURCES += ../../console/main.cpp
}


!raspberrypi {
    SOURCES += gyro_vmu_com_dialog.cpp
}


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
    ../../lib/source/gyro_vmu_buzzer.h \
    ../../lib/source/gyro_vmu_flash.h \
    ../../lib/source/gyro_vmu_vms.h \
    ../../lib/source/gyro_vmu_vmi.h \
    ../../lib/source/gyro_vmu_battery.h \
    ../../lib/source/gyro_vmu_util.h \
    ../../lib/source/gyro_vmu_version.h \
    ../../lib/source/gyro_vmu_disassembler.h \
    ../../lib/source/gyro_vmu_tcp.h \
    ../../lib/source/gyro_vmu_serial.h \
    ../../lib/source/gyro_vmu_port1.h \
    ../../lib/source/gyro_vmu_port7.h \
    ../../lib/source/gyro_vmu_lcd.h \
    gyro_vmu_main_window.hpp \
    gyro_vmu_tcp_dialog.hpp \
    gif/gif_hash.h \
    gif/gif_lib.h \
    gif/gif_lib_private.h \
    gif/qgifglobal.h \
    gif/qgifimage.h \
    gif/qgifimage_p.h


!raspberrypi {
    HEADERS += gyro_vmu_com_dialog.hpp
}

INCLUDEPATH += ../../../libGyro/include \
    ../../lib/source \
    gif

DEPENDPATH += ../../../libGyro/include \
    ../../lib/source \
    ../../lib/include \
    gif

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
    gyro_vmu_tcp_dialog.ui \
    gyro_vmu_com_dialog.ui


macx {
  LIBS +=  -L../../../libGyro/lib/osx \
        -framework Metal \
        -framework OpenGL \
        -framework OpenAL \
        -lz \
        -F../../../ESGamma/lib/osx \
        -L../../../ESGamma/lib/osx \
        -F../../../libGyro/lib/osx \
        -framework SDL2 \
        -framework SDL2_net
vulkan {
        LIBS += -framework MoltenVK
}

    LIBS += -framework Foundation \
        -framework Cocoa \
        -framework Quartz \
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

raspberrypi {
    LIBS += \
        -lSDL2 \
        -lSDL2_net \
        -lGLESv2 \
        -lEGL \
        -lopenal \
        -lz \
        -logg \
        -lvorbis \
        -lvorbisenc \
        -lvorbisfile \
        -lpthread \
        -lrt

} else : linux {
    LIBS += \
        -lSDL2 \
        -lSDL2_net \
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



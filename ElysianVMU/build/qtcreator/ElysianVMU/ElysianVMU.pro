#include(../../../qtcreator/ESDev/ESDev.pri)

isEmpty(LIBGYRO_PATH) {
    LIBGYRO_PATH = ../../evmu-core/libGyro
    LIBGYRO_BUILD_PATH  = $${LIBGYRO_PATH}/qtcreator/libGyro
}

CONFIG += app_bundle
#TEMPLATE = subdirs
#SUBDIRS +=  $${LIBGYRO_BUILD_PATH}


vmu_child_app {
    DEFINES+=VMU_CHILD_APP
    TEMPLATE = lib
    CONFIG += staticlib
    CONFIG -= app_bundle
    INCLUDEPATH += $${GLOBAL_INCLUDE_PATH}
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

SOURCE_ROOT = ../../../app/qt

!vmu_child_app {
macx {
    #DESTDIR = ../../executable
    #    CONFIG(release, debug|release):
    release: QMAKE_POST_LINK = "sh $${SOURCE_ROOT}/macdeployvmu.sh"
}
}



ICON = $${SOURCE_ROOT}/resource/images/vmu_icon.icns

SOURCES += \
    $${SOURCE_ROOT}/source/gyro_vmu_main_window.cpp \
    $${SOURCE_ROOT}/source/gyro_vmu_tcp_dialog.cpp \
    $${SOURCE_ROOT}/gif/qgifimage.cpp \
    $${SOURCE_ROOT}/gif/dgif_lib.c \
    $${SOURCE_ROOT}/gif/egif_lib.c \
    $${SOURCE_ROOT}/gif/gif_err.c \
    $${SOURCE_ROOT}/gif/gif_font.c \
    $${SOURCE_ROOT}/gif/gif_hash.c \
    $${SOURCE_ROOT}/gif/gifalloc.c \
    $${SOURCE_ROOT}/gif/quantize.c \
    $${SOURCE_ROOT}/source/gyro_vmu_buzzer_dialog.cpp \
    ../../../evmu-core/source/gyro_vmu_battery.c \
    ../../../evmu-core/source/gyro_vmu_bios.c \
    ../../../evmu-core/source/gyro_vmu_buzzer.c \
    ../../../evmu-core/source/gyro_vmu_cpu.c \
    ../../../evmu-core/source/gyro_vmu_device.c \
    ../../../evmu-core/source/gyro_vmu_disassembler.c \
    ../../../evmu-core/source/gyro_vmu_display.c \
    ../../../evmu-core/source/gyro_vmu_flash.c \
    ../../../evmu-core/source/gyro_vmu_gamepad.c \
    ../../../evmu-core/source/gyro_vmu_instr.c \
    ../../../evmu-core/source/gyro_vmu_lcd.c \
    ../../../evmu-core/source/gyro_vmu_maple.c \
    ../../../evmu-core/source/gyro_vmu_memory.c \
    ../../../evmu-core/source/gyro_vmu_osc.c \
    ../../../evmu-core/source/gyro_vmu_port1.c \
    ../../../evmu-core/source/gyro_vmu_port7.c \
    ../../../evmu-core/source/gyro_vmu_serial.c \
    ../../../evmu-core/source/gyro_vmu_tcp.c \
    ../../../evmu-core/source/gyro_vmu_timers.c \
    ../../../evmu-core/source/gyro_vmu_util.c \
    ../../../evmu-core/source/gyro_vmu_version.c \
    ../../../evmu-core/source/gyro_vmu_vmi.c \
    ../../../evmu-core/source/gyro_vmu_vms.c \
    ../../../app/qt/source/gyro_vmu_address_space_model.cpp

!vmu_child_app {
    SOURCES += $${SOURCE_ROOT}/source/main.cpp
}


!raspberrypi {
    SOURCES += $${SOURCE_ROOT}/source/gyro_vmu_com_dialog.cpp
}


HEADERS += \
    $${SOURCE_ROOT}/include/gyro_vmu_main_window.hpp \
    $${SOURCE_ROOT}/include/gyro_vmu_tcp_dialog.hpp \
    $${SOURCE_ROOT}/gif/gif_hash.h \
    $${SOURCE_ROOT}/gif/gif_lib.h \
    $${SOURCE_ROOT}/gif/gif_lib_private.h \
    $${SOURCE_ROOT}/gif/qgifglobal.h \
    $${SOURCE_ROOT}/gif/qgifimage.h \
    $${SOURCE_ROOT}/gif/qgifimage_p.h \
    $${SOURCE_ROOT}/include/gyro_vmu_buzzer_dialog.hpp \
    ../../../app/qt/include/gyro_vmu_address_space_model.hpp \
    ../../../evmu-core/include/gyro_vmu_adapter.h \
    ../../../evmu-core/include/gyro_vmu_battery.h \
    ../../../evmu-core/include/gyro_vmu_bios.h \
    ../../../evmu-core/include/gyro_vmu_buzzer.h \
    ../../../evmu-core/include/gyro_vmu_cpu.h \
    ../../../evmu-core/include/gyro_vmu_device.h \
    ../../../evmu-core/include/gyro_vmu_disassembler.h \
    ../../../evmu-core/include/gyro_vmu_display.h \
    ../../../evmu-core/include/gyro_vmu_flash.h \
    ../../../evmu-core/include/gyro_vmu_gamepad.h \
    ../../../evmu-core/include/gyro_vmu_instr.h \
    ../../../evmu-core/include/gyro_vmu_isr.h \
    ../../../evmu-core/include/gyro_vmu_lcd.h \
    ../../../evmu-core/include/gyro_vmu_maple.h \
    ../../../evmu-core/include/gyro_vmu_memory.h \
    ../../../evmu-core/include/gyro_vmu_osc.h \
    ../../../evmu-core/include/gyro_vmu_port1.h \
    ../../../evmu-core/include/gyro_vmu_port7.h \
    ../../../evmu-core/include/gyro_vmu_serial.h \
    ../../../evmu-core/include/gyro_vmu_sfr.h \
    ../../../evmu-core/include/gyro_vmu_tcp.h \
    ../../../evmu-core/include/gyro_vmu_timers.h \
    ../../../evmu-core/include/gyro_vmu_util.h \
    ../../../evmu-core/include/gyro_vmu_version.h \
    ../../../evmu-core/include/gyro_vmu_vmi.h \
    ../../../evmu-core/include/gyro_vmu_vms.h


!raspberrypi {
    HEADERS += $${SOURCE_ROOT}/include/gyro_vmu_com_dialog.hpp
}

INCLUDEPATH += $${LIBGYRO_PATH}/include \
    $${LIBGYRO_PATH}/api \
    $${SOURCE_ROOT}/include \
    ../../lib/source \
    $${SOURCE_ROOT}/gif \
    ../../../evmu-core/include \
    gif

DEPENDPATH += $${LIBGYRO_PATH}/include \
    ../../lib/source \
    ../../lib/include \
    gif

FORMS += \
    $${SOURCE_ROOT}/ui/gyro_vmu_main_window.ui \
    $${SOURCE_ROOT}/ui/gyro_vmu_tcp_dialog.ui \
    $${SOURCE_ROOT}/ui/gyro_vmu_com_dialog.ui \
    $${SOURCE_ROOT}/ui/gyro_vmu_buzzer_dialog.ui


LIBGYRO_BUILD_PATH = -L$${LIBGYRO_PATH}

win32:CONFIG(release, debug|release): LIBS += -L$${LIBGYRO_BUILD_PATH}/qtcreator/libGyro/release/ -llibGyro
else:win32:CONFIG(debug, debug|release): LIBS += -L$${LIBGYRO_BUILD_PATH}/qtcreator/libGyro/debug/ -llibGyro
else:unix:!vmu_child_app: LIBS += -L$${LIBGYRO_BUILD_PATH}/qtcreator/libGyro/ -llibGyro

INCLUDEPATH += $${LIBGYRO_BUILD_PATH}/libGyro/qtcreator/libGyro
DEPENDPATH += $${LIBGYRO_BUILD_PATH}/libGyro/qtcreator/libGyro

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $${LIBGYRO_BUILD_PATH}/qtcreator/libGyro/release/liblibGyro.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $${LIBGYRO_BUILD_PATH}/qtcreator/libGyro/debug/liblibGyro.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $${LIBGYRO_BUILD_PATH}/qtcreator/libGyro/release/libGyro.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $${LIBGYRO_BUILD_PATH}/qtcreator/libGyro/debug/libGyro.lib
else:unix:!vmu_child_app: PRE_TARGETDEPS += $${LIBGYRO_BUILD_PATH}/qtcreator/libGyro/liblibGyro.a

!vmu_child_app {

macx {
  LIBS +=  -L$${LIBGYRO_PATH}/lib/osx \
        -framework Metal \
        -framework OpenGL \
        -framework OpenAL \
        -lz \
        -F../../../ESGamma/lib/osx \
        -L../../../ESGamma/lib/osx \
        -F$${LIBGYRO_PATH}/lib/osx \
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


}

win32 {
    QMAKE_CFLAGS += -msse -std=c99
    INCLUDEPATH += $${LIBGYRO_PATH}//lib/win32
    LIBS += -L../../../lib/win32 \
        -L$${LIBGYRO_PATH}/lib/win32 \
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


!vmu_child_app {
RESOURCES += \
    $${SOURCE_ROOT}/resource/evmu.qrc
}


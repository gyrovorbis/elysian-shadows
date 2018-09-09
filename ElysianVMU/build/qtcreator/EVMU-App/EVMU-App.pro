#include(../../../qtcreator/ESDev/ESDev.pri)

#!isEmpty(CMD_LINE_ARGS_PRI_FILE) {
 #   include($${CMD_LINE_ARGS_PRI_FILE})
#}

isEmpty(LIBGYRO_PATH) {
    include(../ElysianVMU/ElysianVMU.pri)
    LIBGYRO_PATH = ../../../evmu-core/libGyro
    LIBGYRO_BUILD_PATH  = $${LIBGYRO_PATH}/qtcreator/libGyro
}

LIBGYRO_PATH        = $$relative_path($$LIBGYRO_PATH)
LIBGYRO_BUILD_PATH  = $$relative_path($$LIBGYRO_BUILD_PATH)

!vmu_child_app: CONFIG += app_bundle
#TEMPLATE = subdirs
#SUBDIRS +=  $${LIBGYRO_BUILD_PATH}

vmu_child_app {
    DEFINES+=VMU_CHILD_APP
    TEMPLATE = lib
    CONFIG += staticlib
    CONFIG -= app_bundle
}

INCLUDEPATH += ../../../app/qt \
    ../../../app/qt/include/ElysianVMU \
    ../../../evmu-core/include \
    ../../../evmu-core/include/evmu-core \
    $${LIBGYRO_PATH}/api \
    $${LIBGYRO_PATH}/api/libGyro

TARGET = ElysianVMU
QT += gui opengl

!raspberrypi : !ios {
    QT += serialport
}


!vmu_child_app {
    win32 {
        DESTDIR = ../executable
    }

    linux {
        DESTDIR = ../executable
    }
}

SOURCE_ROOT = ../../../app/qt

!vmu_child_app {
macx {
    #DESTDIR = ../../executable
    #    CONFIG(release, debug|release):
    release: QMAKE_POST_LINK = "sh ./macdeployvmu.sh $${LIBGYRO_PATH}"
}
}



ICON = $${SOURCE_ROOT}/resource/images/vmu_icon.icns

SOURCES += \
    ../../../app/qt/source/gyro_vmu_main_window.cpp \
    ../../../app/qt/source/gyro_vmu_tcp_dialog.cpp \
    ../../../app/qt/source/gyro_vmu_buzzer_dialog.cpp \
    ../../../app/qt/source/gyro_vmu_address_space_model.cpp \
    ../../../app/qt/gif/qgifimage.cpp \
    ../../../app/qt/gif/dgif_lib.c \
    ../../../app/qt/gif/egif_lib.c \
    ../../../app/qt/gif/gif_err.c \
    ../../../app/qt/gif/gif_font.c \
    ../../../app/qt/gif/gif_hash.c \
    ../../../app/qt/gif/gifalloc.c \
    ../../../app/qt/gif/quantize.c

!vmu_child_app {
    SOURCES += ../../../app/qt/source/main.cpp

}


!raspberrypi {
    SOURCES += ../../../app/qt/source/gyro_vmu_com_dialog.cpp
}


HEADERS += \
    $${SOURCE_ROOT}/gif/gif_hash.h \
    $${SOURCE_ROOT}/gif/gif_lib.h \
    $${SOURCE_ROOT}/gif/gif_lib_private.h \
    $${SOURCE_ROOT}/gif/qgifglobal.h \
    $${SOURCE_ROOT}/gif/qgifimage.h \
    $${SOURCE_ROOT}/gif/qgifimage_p.h \
    $${SOURCE_ROOT}/include/ElysianVMU/gyro_vmu_buzzer_dialog.hpp \
    $${SOURCE_ROOT}/include/ElysianVMU/gyro_vmu_address_space_model.hpp \
    $${SOURCE_ROOT}/include/ElysianVMU/gyro_vmu_main_window.hpp \
    $${SOURCE_ROOT}/include/ElysianVMU/gyro_vmu_tcp_dialog.hpp

!raspberrypi {

    HEADERS += $${SOURCE_ROOT}/include/ElysianVMU/gyro_vmu_com_dialog.hpp

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


EVMU_CORE_PATH      = ../../../evmu-core
EVMU_CORE_BUILD_PATH = $${EVMU_CORE_PATH}/build/qtcreator/EVMU-Core

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/$${EVMU_CORE_BUILD_PATH}/release/ -levmu-core
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/$${EVMU_CORE_BUILD_PATH}/debug/ -levmu-core
else:unix:!vmu_child_app: LIBS += -L$$OUT_PWD/$${EVMU_CORE_BUILD_PATH} -levmu-core

INCLUDEPATH += $$PWD/$${EVMU_CORE_PATH}
DEPENDPATH += $$PWD/$${EVMU_CORE_PATH}

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/$${EVMU_CORE_BUILD_PATH}/release/libevmu-core.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/$${EVMU_CORE_BUILD_PATH}/debug/libevmu-core.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/$${EVMU_CORE_BUILD_PATH}/release/evmu-core.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/$${EVMU_CORE_BUILD_PATH}/debug/evmu-core.lib
else:unix:!vmu_child_app: PRE_TARGETDEPS += $$OUT_PWD/$${EVMU_CORE_BUILD_PATH}/libevmu-core.a


LIBGYRO_BUILD_PATH = $${LIBGYRO_PATH}

win32:CONFIG(release, debug|release): LIBS += -L$${LIBGYRO_BUILD_PATH}/qtcreator/libGyro/release/ -llibGyro
else:win32:CONFIG(debug, debug|release): LIBS += -L$${LIBGYRO_BUILD_PATH}/qtcreator/libGyro/debug/ -llibGyro
else:unix:!vmu_child_app: LIBS += -L$${LIBGYRO_BUILD_PATH}/qtcreator/libGyro/ -llibGyro

INCLUDEPATH += $${LIBGYRO_BUILD_PATH}/libGyro/qtcreator/libGyro
DEPENDPATH += $${LIBGYRO_BUILD_PATH}/libGyro/qtcreator/libGyro

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $${LIBGYRO_BUILD_PATH}/qtcreator/libGyro/release/liblibGyro.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $${LIBGYRO_BUILD_PATH}/qtcreator/libGyro/debug/liblibGyro.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $${LIBGYRO_BUILD_PATH}/qtcreator/libGyro/release/libGyro.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS +=$${LIBGYRO_BUILD_PATH}/qtcreator/libGyro/debug/libGyro.lib
else:unix:!vmu_child_app: PRE_TARGETDEPS += $${LIBGYRO_BUILD_PATH}/qtcreator/libGyro/liblibGyro.a

!vmu_child_app {

macx {
  LIBS +=  -L$${LIBGYRO_PATH}/lib/osx \
        -framework Metal \
        -framework OpenGL \
        -framework OpenAL \
        -lz \
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
    INCLUDEPATH += $${LIBGYRO_PATH}/lib/win32
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


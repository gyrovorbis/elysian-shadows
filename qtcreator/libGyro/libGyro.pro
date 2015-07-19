include(../../../qtcreator/ESDev/ESDev.pri)
TEMPLATE = lib
QT += opengl
CONFIG += staticlib console
CONFIG -= app_bundle
QMAKE_CFLAGS += -std=c99
!ios {
    QMAKE_CXXFLAGS += -Wno-c++11-extensions -Wno-c++11-long-long
}

INCLUDEPATH += ../../include \
                ../../../Spine/include

#==========PLATFORM-SPECIFIC BULLSHIT==========

win32 {s
    INCLUDEPATH +=  ../../lib/win32 \
                    ../../lib/win32/mingw
    SOURCES +=	../../source/x86/turfs_windows.c
}

macx {
    INCLUDEPATH += ../../lib/osx
    INCLUDEPATH += ../../lib/osx/SDL.framework/Headers
    SOURCES +=	../../source/x86/turfs_osx.c
}

linux {
    LIBS += \
        -ldynamic
    SOURCES +=	../../source/x86/turfs_unix.c
}

ios {

    INCLUDEPATH += ../../lib/ios/Ogg.framework/Headers \
        ../../lib/ios/Vorbis.framework/Headers
}


#==========/PLATFORM-SPECIFIC BULLSHIT==========

DEPENDPATH += ../../include \
    ../../source/glal/shaders \
    ../../source/glal \
    ../../include/glal

!ios {
    DEPENDPATH += ../../include/x86
} else {
    DEPENDPATH += ../../include/ios
}

OTHER_FILES += \
    ../../source/glal/shaders/fixedFunctionTextured.vsh \
    ../../source/glal/shaders/fixedFunctionTextured.fsh \
    ../../source/glal/shaders/fixedFunctionUntextured.vsh \
    ../../source/glal/shaders/fixedFunctionUntextured.fsh \
    ../../source/glal/shaders/vertexLightingDirectional.fsh \
    ../../source/glal/shaders/vertexLightingDirectional.vsh \
    ../../source/glal/shaders/vertexLighting.fsh \
    ../../source/glal/shaders/vertexLighting.vsh \
    ../../source/glal/shaders/pixelLighting.fsh \
    ../../source/glal/shaders/pixelLighting.vsh \
    ../../source/glal/shaders/shadowMap.vsh \
    ../../source/glal/shaders/shadowMap.fsh

HEADERS += \
    ../../include/gyro.h \
    ../../include/gyro_wav_api.h \
    ../../include/gyro_video_api.h \
    ../../include/gyro_timer_api.h \
    ../../include/gyro_texture_api.h \
    ../../include/gyro_system_api.h \
    ../../include/gyro_shader_api.h \
    ../../include/gyro_platform.h \
    ../../include/gyro_ogg_api.h \
    ../../include/gyro_matrix_api.h \
    ../../include/gyro_math_api.h \
    ../../include/gyro_input_api.h \
    ../../include/gyro_file_api.h \
    ../../include/gyro_defines.h \
    ../../include/gyro_audio_api.h \
    ../../include/common/gyro_wav_common.h \
    ../../include/common/gyro_texture_common.h \
    ../../include/common/gyro_system_common.h \
    ../../include/common/gyro_matrix_stack.h \
    ../../include/kinetic/gyro_kinetic.h \
    ../../include/kinetic/gyro_kinetic_static.h \
    ../../include/kinetic/gyro_kinetic_physics.h \
    ../../include/kinetic/gyro_kinetic_kinetic.h \
    ../../include/kinetic/gyro_kinetic_contact.h \
    ../../include/kinetic/gyro_kinetic_collision.h \
    ../../include/kinetic/gyro_kinetic_collider.h \
    ../../source/common/gyro_asset_bank.h \
    ../../include/glal/gyro_audio_glal.inl \
    ../../include/glal/gyro_texture_glal.inl \
    ../../include/common/gyro_video_common.h \
    ../../source/common/gyro_system_callbacks.h \
    ../../source/common/internal.h \
    ../../source/glal/tinycthread.h \
    ../../include/glal/gyro_shaders_glal.h \
    ../../include/glal/gyro_video_glal.h \
    ../../include/glal/gyro_gl.h \
    ../../include/glal/gyro_al.h \
    ../../include/glal/gyro_al.h \
    ../../include/common/turfs.h \
    ../../include/glal/gyro_file_glal.inl


!ios {
    HEADERS += ../../include/x86/gyro_input_x86.inl \
    ../../include/x86/gyro_math_sse_x86.inl \
    ../../include/x86/gyro_matrix_sse_x86.inl \
    ../../include/x86/gyro_sdl_x86.inl \
    ../../include/x86/gyro_video_x86.inl
} else {
    HEADERS += ../../source/ios/GYAppDelegate.h \
        ../../source/ios/GYView.h \
        ../../source/ios/GYViewController.h \
        ../../include/ios/gyro_input_ios.inl \
        ../../include/ios/gyro_video_ios.inl \
        ../../include/arm/gyro_matrix_arm_neon.inl
}


SOURCES += \
    ../../source/common/turfs_common.c \
    ../../source/common/gyro_file.c \
    ../../source/glal/gyro_lighting_glal.c \
    ../../source/glal/gyro_shaders_glal.c \
    ../../source/glal/gyro_video_glal.c \
    ../../source/kinetic/gyro_kinetic_collision.c \
    ../../source/kinetic/gyro_kinetic_physics.c \
    ../../source/common/gyro_asset_bank.c \
    ../../source/common/gyro_matrix_stack.c \
    ../../source/common/gyro_shader.c \
    ../../source/common/gyro_system.c \
    ../../source/common/gyro_texture.c \
    ../../source/common/gyro_wav.c \
    ../../source/glal/gyro_audio_glal.c \
    ../../source/glal/gyro_wav_glal.c \
    ../../source/glal/tinycthread.c \
    ../../source/glal/gyro_ogg_glal.c \
    ../../source/common/turfs_file_stdio.c \
    ../../source/x86/gyro_matrix_x86.c \


!ios {
    SOURCES += \
        ../../source/x86/gyro_system_glfw_x86.c \
        ../../source/qt/gyro_texture_qt.cpp \
        ../../source/qt/gyro_debug_qt.cpp \
        ../../source/qt/gyro_input_qt.cpp \
        ../../source/qt/gyro_timer_qt.cpp \
        ../../source/qt/gyro_file_qt.cpp
} else {

    OBJECTIVE_SOURCES += \
        ../../source/ios/main.mm \
        ../../source/ios/gyro_texture_ios.mm \
        ../../source/ios/gyro_debug_ios.mm \
        ../../source/ios/gyro_timer_ios.mm \
        ../../source/ios/turfs_ios.mm \
        ../../source/ios/gyro_system_ios.mm \
        ../../source/ios/gyro_video_ios.mm \
        ../../source/ios/GYAppDelegate.mm \
        ../../source/ios/GYView.mm \
        ../../source/ios/GYViewController.mm
}

TEMPLATE = lib
CONFIG += staticlib
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt
DEFINES += GYRO_PLATFORM=GYRO_PLATFORM_x86

INCLUDEPATH += ../../include \
				../../source/x86/png

win32 {
	INCLUDEPATH +=  ../../lib/win32
        QMAKE_CFLAGS += -msse -std=c99
        DEFINES += WIN32
}

macx {
    INCLUDEPATH += ../../lib/osx
    INCLUDEPATH += ../../lib/osx/SDL.framework/Headers
}

linux {
    QMAKE_CFLAGS += -msse

    LIBS += \
        -ldynamic
}

DEPENDPATH += ../../include \
	../../include/x86/png \
	../../source/glal/shaders \
	../../source/glal \
	../../include/x86 \
	../../include/glal

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
    ../../include/glal/gyro_glal.h \
    ../../include/kinetic/gyro_kinetic.h \
    ../../include/kinetic/gyro_kinetic_static.h \
    ../../include/kinetic/gyro_kinetic_physics.h \
    ../../include/kinetic/gyro_kinetic_kinetic.h \
    ../../include/kinetic/gyro_kinetic_contact.h \
    ../../include/kinetic/gyro_kinetic_collision.h \
    ../../include/kinetic/gyro_kinetic_collider.h \
    ../../source/x86/png/pngstruct.h \
    ../../source/x86/png/pngpriv.h \
    ../../source/x86/png/pnglibconf.h \
    ../../source/x86/png/pnginfo.h \
    ../../source/x86/png/pngdebug.h \
    ../../source/x86/png/pngconf.h \
    ../../source/x86/png/png.h \
    ../../source/glal/gyro_video_glal.h \
    ../../source/glal/gyro_shaders_glal.h \
    ../../source/common/gyro_asset_bank.h \
    ../../include/x86/gyro_input_x86.inl \
    ../../include/x86/gyro_math_sse_x86.inl \
    ../../include/x86/gyro_matrix_sse_x86.inl \
    ../../include/x86/gyro_sdl_x86.inl \
    ../../include/x86/gyro_video_x86.inl \
    ../../include/glal/gyro_audio_glal.inl \
    ../../include/glal/gyro_texture_glal.inl \
    ../../include/common/gyro_video_common.h

SOURCES += \
    ../../source/x86/png/pngwutil.c \
    ../../source/x86/png/pngwtran.c \
    ../../source/x86/png/pngwrite.c \
    ../../source/x86/png/pngwio.c \
    ../../source/x86/png/pngtrans.c \
    ../../source/x86/png/pngset.c \
    ../../source/x86/png/pngrutil.c \
    ../../source/x86/png/pngrtran.c \
    ../../source/x86/png/pngrio.c \
    ../../source/x86/png/pngread.c \
    ../../source/x86/png/pngpread.c \
    ../../source/x86/png/pngmem.c \
    ../../source/x86/png/pngget.c \
    ../../source/x86/png/pngerror.c \
    ../../source/x86/png/png.c \
    ../../source/x86/gyro_video_x86.c \
    ../../source/x86/gyro_timer_x86.c \
    ../../source/x86/gyro_texture_x86.c \
    ../../source/x86/gyro_system_x86.c \
    ../../source/x86/gyro_matrix_x86.c \
    ../../source/x86/gyro_input_x86.c \
    ../../source/kinetic/gyro_kinetic_physics.c \
    ../../source/kinetic/gyro_kinetic_collision.c \
    ../../source/glal/gyro_wav_glal.c \
    ../../source/glal/gyro_video_glal.c \
    ../../source/glal/gyro_shaders_glal.c \
    ../../source/glal/gyro_ogg_glal.c \
    ../../source/glal/gyro_audio_glal.c \
    ../../source/common/gyro_wav.c \
    ../../source/common/gyro_texture.c \
    ../../source/common/gyro_system.c \
    ../../source/common/gyro_shader.c \
    ../../source/common/gyro_matrix_stack.c \
    ../../source/common/gyro_asset_bank.c \
    ../../source/glal/gyro_lighting_glal.c


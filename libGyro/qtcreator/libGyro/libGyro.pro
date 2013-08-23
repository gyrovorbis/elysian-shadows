TEMPLATE = lib
CONFIG += staticlib
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt
DEFINES += GYRO_PLATFORM=GYRO_PLATFORM_x86

INCLUDEPATH += ../../include \
    ../../lib/win32 \
    ../../include/x86/png

DEPENDPATH += ../../include \
    ../../lib/win32 \
    ../../include/x86/png

HEADERS += \
    ../../include/glal/gyro_video_glal.h \
    ../../include/glal/gyro_gl_glal.h \
    ../../include/kinetic/gyro_kinetic_static.h \
    ../../include/kinetic/gyro_kinetic_physics.h \
    ../../include/kinetic/gyro_kinetic_kinetic.h \
    ../../include/kinetic/gyro_kinetic_contact.h \
    ../../include/kinetic/gyro_kinetic_collision.h \
    ../../include/kinetic/gyro_kinetic_collider.h \
    ../../include/kinetic/gyro_kinetic.h \
    ../../include/x86/gyro_video_x86.h \
    ../../include/x86/gyro_sdl_x86.h \
    ../../include/x86/gyro_input_x86.h \
    ../../include/gyro_wav.h \
    ../../include/gyro_video.h \
    ../../include/gyro_tracking_internal.h \
    ../../include/gyro_tracking.h \
    ../../include/gyro_timer.h \
    ../../include/gyro_texture.h \
    ../../include/gyro_system.h \
    ../../include/gyro_platform.h \
    ../../include/gyro_matrix_stack.h \
    ../../include/gyro_matrix.h \
    ../../include/gyro_math.h \
    ../../include/gyro_log.h \
    ../../include/gyro_input.h \
    ../../include/gyro_file.h \
    ../../include/gyro_defines.h \
    ../../include/gyro_audio.h \
    ../../include/gyro_asset_bank.h \
    ../../include/gyro.h \
    ../../include/x86/png/pngstruct.h \
    ../../include/x86/png/pngpriv.h \
    ../../include/x86/png/pnginfo.h \
    ../../include/x86/png/pngdebug.h \
    ../../include/x86/png/pngconf.h \
    ../../include/x86/png/png.h

SOURCES += \
    ../../include/soft/gyro_math_soft.inl \
    ../../source/glal/gyro_video_glal.c \
    ../../source/glal/gyro_audio_glal.c \
    ../../source/kinetic/gyro_kinetic_physics.c \
    ../../source/kinetic/gyro_kinetic_collision.c \
    ../../source/soft/gyro_matrix_soft.c \
    ../../source/x86/gyro_wav_x86.c \
    ../../source/x86/gyro_video_x86.c \
    ../../source/x86/gyro_tracking_x86.c \
    ../../source/x86/gyro_timer_x86.c \
    ../../source/x86/gyro_texture_x86.c \
    ../../source/x86/gyro_system_x86.c \
    ../../source/x86/gyro_input_x86.c \
    ../../source/gyro_wav.c \
    ../../source/gyro_tracking.c \
    ../../source/gyro_texture.c \
    ../../source/gyro_system.c \
    ../../source/gyro_matrix_stack.c \
    ../../source/gyro_log.c \
    ../../source/gyro_asset_bank.c \
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
    ../../source/x86/png/png.c


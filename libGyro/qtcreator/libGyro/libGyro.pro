#include(../../../qtcreator/ESDev/ESDev.pri)
TEMPLATE = lib
QT += opengl
CONFIG += staticlib console
CONFIG -= app_bundle
#QMAKE_CFLAGS += -std=c99
isEmpty(ESDEV_ROOT) {
    DEFINES += GYRO_PLATFORM=GYRO_PLATFORM_Qt
    DEFINES += GYRO_NO_SPINE
}

!ios {
    QMAKE_CXXFLAGS += -Wno-c++11-extensions -Wno-c++11-long-long
}

INCLUDEPATH += ../../include \
                ../../../Spine/include \
                ../../api

#==========PLATFORM-SPECIFIC BULLSHIT==========

win32 {
    INCLUDEPATH +=  ../../lib/win32 \
                    ../../lib/win32/mingw
    SOURCES +=	../../source/x86/turfs_windows.c
}

macx {
    QMAKE_CFLAGS+= -F../../lib/osx \
            -I../../lib/osx
    QMAKE_LFLAGS += -F../../lib/osx
    LIBS += -F../../lib/osx \
        -framework SDL2.framework \
        -framework SDL2_net.framework
    vulkan {
        LIBS += -framework MoltenVK.framework
    }

    INCLUDEPATH += ../../lib/osx
    INCLUDEPATH += ../../lib/osx/SDL2.framework/Headers \
                    ../../lib/osx/SDL2_net.framework/Headers
    vulkan {
        INCLUDEPATH += ../../lib/osx/MoltenVK.framework/Headers
    }
    SOURCES +=	../../source/x86/turfs_osx.c
    OBJECTIVE_SOURCES += ../../source/apple/gyro_system_apple.m
}

linux {
    LIBS += \
     -ldynamic
    SOURCES +=	../../source/x86/turfs_unix.c \
    ../../source/linux/gyro_system_linux.c
}

raspberrypi {
    SOURCES +=	../../source/x86/turfs_unix.c
}

ios {

    INCLUDEPATH += ../../lib/ios/Ogg.framework/Headers \
        ../../lib/ios/Vorbis.framework/Headers
}

dreamcast {
    INCLUDEPATH += ../../lib/dc/shlib

}

#==========/PLATFORM-SPECIFIC BULLSHIT==========

DEPENDPATH += ../../include \
    ../../source/gl \
    ../../include/gl \
    ../../source/al \
    .../../include/al

vulkan {
    DEPENDPATH += ../../include/vulkan
}

ios {
    DEPENDPATH += ../../include/ios
} else:dreamcast {
    DEPENDPATH += ../../include/dc
} else {
    DEPENDPATH += ../../include/x86
}

HEADERS += \
    ../../include/common/gyro_wav_common.h \
    ../../include/common/gyro_texture_common.h \
    ../../include/common/gyro_system_common.h \
    ../../include/common/gyro_matrix_stack.h \
    ../../source/common/gyro_asset_bank.h \
    ../../include/al/gyro_audio_al.inl \
    ../../include/gl/gyro_texture_gl.inl \
    ../../include/common/gyro_video_common.h \
    ../../source/common/gyro_system_callbacks.h \
    ../../source/common/internal.h \
    ../../include/gl/gyro_shaders_gl.h \
    ../../include/gl/gyro_video_gl.h \
    ../../include/gl/gyro_gl.h \
    ../../include/al/gyro_al.h \
    ../../include/common/turfs.h \
    ../../include/x86/gyro_file_x86.inl \
    ../../include/common/gyro_input_common.h \
    ../../include/common/gyro_spine_common.h \
    ../../include/gl/gyro_bloom_gl.h \
    ../../include/gl/gyro_gl_utils.h \
    ../../include/gl/gyro_radial_blur_gl.h \
    ../..//include/gl/gyro_chromatic_aberration_gl.h \
    ../../include/cl/gyro_cl.h \
    ../../include/al/gyro_al.h \
    ../../include/gl/gyro_gl.h \
    ../../include/gl/gyro_shaders_gl.h \
    ../../include/gl/gyro_video_gl.h \
    ../../api/libGyro/gyro.h \
    ../../api/libGyro/gyro_audio_api.h \
    ../../api/libGyro/gyro_compute_api.h \
    ../../api/libGyro/gyro_defines.h \
    ../../api/libGyro/gyro_file_api.h \
    ../../api/libGyro/gyro_input_api.h \
    ../../api/libGyro/gyro_math_api.h \
    ../../api/libGyro/gyro_matrix_api.h \
    ../../api/libGyro/gyro_motion_api.h \
    ../../api/libGyro/gyro_net_api.h \
    ../../api/libGyro/gyro_ogg_api.h \
    ../../api/libGyro/gyro_performance_api.h \
    ../../api/libGyro/gyro_platform.h \
    ../../api/libGyro/gyro_rumble_api.h \
    ../../api/libGyro/gyro_shader_api.h \
    ../../api/libGyro/gyro_system_api.h \
    ../../api/libGyro/gyro_test_api.h \
    ../../api/libGyro/gyro_texture_api.h \
    ../../api/libGyro/gyro_thread_api.h \
    ../../api/libGyro/gyro_timer_api.h \
    ../../api/libGyro/gyro_version.h \
    ../../api/libGyro/gyro_video_api.h \
    ../../api/libGyro/gyro_visual_memory_api.h \
    ../../api/libGyro/gyro_wav_api.h

vulkan {
    HEADERS += \
        ../../include/vulkan/gyro_resource_vulkan.h \
        ../../include/vulkan/gyro_descriptor_vulkan.h \
        ../../include/vulkan/gyro_query_vulkan.h \
        ../../include/vulkan/gyro_util_vulkan.h \
        ../../include/vulkan/gyro_context_vulkan.h \
        ../../include/vulkan/gyro_device_vulkan.h \
        ../../include/vulkan/gyro_video_vulkan.h \
        ../../include/vulkan/gyro_vulkan.h
}


android {

    HEADERS += ../../include/android/gyro_input_android.inl \
        ../../include/android/gyro_video_android.inl

} else: ios {
    HEADERS += ../../source/ios/GYAppDelegate.h \
        ../../source/ios/GYView.h \
        ../../source/ios/GYViewController.h \
        ../../include/ios/gyro_input_ios.inl \
        ../../include/ios/gyro_video_ios.inl \
        ../../include/arm/gyro_matrix_arm_neon.inl

} else: dreamcast {
    HEADERS += ../../include/dc/gyro_input_dc.h \
        ../../include/dc/gyro_math_dc.inl \
        ../../include/dc/gyro_matrix_dc.inl \
        ../../include/dc/gyro_texture_dc.h \
        ../../include/dc/gyro_video_dc.h

} else {
    HEADERS += ../../include/x86/gyro_input_x86.inl \
    ../../include/x86/gyro_math_sse_x86.inl \
    ../../include/x86/gyro_matrix_sse_x86.inl \
    ../../include/sdl/gyro_sdl.inl \
    ../../include/x86/gyro_video_x86.inl

}


SOURCES += \
    ../../source/common/turfs_common.c \
    ../../source/common/gyro_file.c \
    ../../source/common/gyro_asset_bank.c \
    ../../source/common/gyro_matrix_stack.c \
    ../../source/common/gyro_shader.c \
    ../../source/common/gyro_system.c \
    ../../source/common/gyro_texture.c \
    ../../source/common/gyro_wav.c \
    ../../source/common/turfs_file_stdio.c \
    ../../source/common/gyro_video.c \
    ../../source/common/gyro_timer.c \
    ../../source/common/gyro_test.c \
    ../../source/common/gyro_visual_memory_common.c \
    ../../source/gl/gyro_radial_blur_gl.c \
    ../../source/gl/gyro_chromatic_aberration_gl.c \
    ../../source/cl/gyro_compute_cl.c \
    ../../source/sdl/gyro_performance_sdl.c \
    ../../source/sdl/gyro_rumble_sdl.c \
    ../../source/gl/gyro_bloom_gl.c \
    ../../source/gl/gyro_gl_utils.c \
    ../../source/sdl/gyro_thread_sdl.c

vulkan {
    SOURCES += \
    ../../source/vulkan/gyro_video_vulkan.c \
    ../../source/vulkan/gyro_texture_vulkan.c \
    ../../source/vulkan/gyro_query_vulkan.c \
    ../../source/vulkan/gyro_shader_vulkan.c \
    ../../source/vulkan/gyro_resource_vulkan.c \
    ../../source/vulkan/gyro_allocator_vulkan.c \
    ../../source/vulkan/gyro_descriptor_vulkan.c \
    ../../source/vulkan/gyro_context_vulkan.c \
    ../../source/vulkan/gyro_util_vulkan.c \
    ../../source/vulkan/gyro_device_vulkan.c
}

dreamcast {

    SOURCES += \
        ../../source/dc/gyro_audio_dc.c \
        ../../source/dc/gyro_input_dc.c \
        ../../source/dc/gyro_system_dc.c \
        ../../source/dc/gyro_texture_dc.c \
        ../../source/dc/gyro_timer_dc.c \
        ../../source/dc/gyro_video_dc.c \
        ../../source/dc/gyro_wav_dc.c \
        ../../source/dc/gyro_file_dc.c \
        ../../source/x86/turfs_unix.c

} else {

    SOURCES+= \
        ../../source/gl/gyro_lighting_gl.c \
        ../../source/gl/gyro_shaders_gl.c \
        ../../source/gl/gyro_video_gl.c \
        ../../source/al/gyro_audio_al.c \
        ../../source/al/gyro_wav_al.c \
        ../../source/al/gyro_ogg_al.c \
        ../../source/x86/gyro_matrix_x86.c

    !ios:!android {
        SOURCES += \
            ../../source/x86/gyro_system_glfw_x86.c \
            ../../source/qt/gyro_texture_qt.cpp \
            ../../source/qt/gyro_debug_qt.cpp \
            ../../source/qt/gyro_input_qt.cpp \
            ../../source/qt/gyro_timer_qt.cpp \
            ../../source/qt/gyro_file_qt.cpp \
            ../../source/qt/gyro_video_qt.cpp \
            ../../source/sdl/gyro_controller_sdl.c \
            ../../source/sdl/gyro_net_sdl.c
    } else: android {
        SOURCES += ../../source/android/android_native_app_glue.c \
            ../../source/android/bionic_polyfill.c \
            ../../source/android/gyro_file_android.c \
            ../../source/android/gyro_input_android.c \
            ../../source/android/gyro_matrix_android.c \
            ../../source/android/gyro_system_android.c \
            ../../source/android/gyro_texture_android.c \
            ../../source/android/gyro_timer_android.c \
            ../../source/android/gyro_video_android.c \
            ../../source/android/turfs_android.c

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
            ../../source/ios/GYViewController.mm \
            ../../source/apple/gyro_system_apple.m
    }


}




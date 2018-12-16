#ifndef GYRO_PLATFORM_H
#define GYRO_PLATFORM_H

//#ifdef __cplusplus
//extern "C" {
//#endif

#define _GYRO_STRINGIFY_MACRO(a) #a
#define GYRO_STRINGIFY_MACRO(a) _GYRO_STRINGIFY_MACRO(a)
#define GYRO_STRINGIFY_VERSION(major, minor, patch) GYRO_STRINGIFY_MACRO(major) "." GYRO_STRINGIFY_MACRO(minor) "." GYRO_STRINGIFY_MACRO(patch)

#define GYRO_PLATFORM_x86		(1<<0)
#define GYRO_PLATFORM_DC		(1<<1)
#define GYRO_PLATFORM_PSP		(1<<2)
#define GYRO_PLATFORM_iOS		(1<<3)
#define GYRO_PLATFORM_ANDROID	(1<<4)
#define GYRO_PLATFORM_RASPI     (1<<5)
#define GYRO_PLATFORM_Qt		(1<<6)
#define GYRO_PLATFORM_WEB		(1<<7)

//#define GYRO_GLAL_GL_Desktop    (1<<0)
//#define GYRO_GLAL_GL_ES3        (1<<1)
//#define GYRO_GLAL_GL_ES2        (1<<2)

#define GYRO_ENDIAN_LITTLE		0x01
#define GYRO_ENDIAN_BIG			0x02

//Determine compiler information
#ifndef GYRO_COMPILER_VERSION_STRING
#   if defined(__clang__)
#       define GYRO_COMPILER_VERSION_STRING "Clang " GYRO_STRINGIFY_VERSION(__clang_major__, __clang_minor__, __clang_patchlevel__)

#   elif defined(__CC_ARM)
    #   define GYRO_COMPILER_VERSION STRING "ARM " __ARMCC_VERSION

#   elif defined(__IAR_SYSTEMS_ICC__)
    #   define GYRO_COMPILER_VERSION STRING "IAR " __VER__

#   elif defined(__ICC)
#       define GYRO_COMPILER_VERSION_STRING "ICC " __ICC

#   elif defined(__EMSCRIPTEN__)
#       define GYRO_COMPILER_VERSION_STRING "Emscripten " __EMSCRIPTEN__

#   elif defined(__MINGW32_MAJOR_VERSION)
#       define GYRO_COMPILER_VERSION_STRING "MinGW32 " GYRO_STRINGIFY_VERSION(__MINGW32_MAJOR_VERSION, __MINGW32_MINOR_VERSION, "") " : GCC " GYRO_STRINGIFY_VERSION(__GNUC__, __GNUC_MINOR__, __GNUC_PATCHLEVEL__)

#   elif defined(__MINGW64_MAJOR_VERSION)
#       define GYRO_COMPILER_VERSION_STRING "MinGW64 " GYRO_STRINGIFY_VERSION(__MINGW64_MAJOR_VERSION, __MINGW64_MINOR_VERSION, "") " : GCC " GYRO_STRINGIFY_VERSION(__GNUC__, __GNUC_MINOR__, __GNUC_PATCHLEVEL__)

#   elif defined(__ghs__)
#       define GYRO_COMPILER_VERSION_STRING "Green Hill " GYRO_STRINGIFY_MACRO(__GHS_VERSION_NUMBER__) ": " GYRO_STRINGIFY_MACRO(__GHS_REVISION_DATE)

#   elif defined(__GNUC__)
#       define GYRO_COMPILER_VERSION_STRING "GCC " GYRO_STRINGIFY_VERSION(__GNUC__, __GNUC_MINOR__, __GNUC_PATCHLEVEL__)

#   elif defined(_MSC_VER)
    #   define GYRO_COMPILER_VERSION STRING "MSVC " _MSC_FULL_VER

#   else
#       define GYRO_COMPILER_VERSION_STRING "Unknown"

#   endif
#endif


#if 0
#	define GYRO_AUDIO_PLATFORM_INCLUDE
#	define GYRO_FILE_PLATFORM_INCLUDE
#	define GYRO_INPUT_PLATFORM_INCLUDE
#	define GYRO_MATH_PLATFORM_INClUDE
#	define GYRO_MATRIX_PLATFORM_INCLUDE
#	define GYRO_OGG_PLATFORM_INCLUDE
#	define GYRO_SHADER_PLATFORM_INCLUDE
#	define GYRO_SYSTEM_PLATFORM_INCLUDE
#	define GYRO_WAV_PLATFORM_INCLUDE
#endif


//Platform-Specific Definitions


#if GYRO_PLATFORM == GYRO_PLATFORM_x86

#   define GYRO_ENDIAN					GYRO_ENDIAN_LITTLE
#	define GYRO_CACHE_BLOCK_SIZE
#	define GYRO_GLAL

#	define GYRO_AUDIO_PLATFORM_INCLUDE	"glal/gyro_audio_glal.inl"
#	define GYRO_INPUT_PLATFORM_INCLUDE	"x86/gyro_input_x86.inl"
#	define GYRO_MATH_PLATFORM_INCLUDE	"x86/gyro_math_sse_x86.inl"
#	define GYRO_MATRIX_PLATFORM_INCLUDE "x86/gyro_matrix_sse_x86.inl"
#	define GYRO_SYSTEM_PLATFORM_INCLUDE "x86/gyro_sdl_x86.inl"
#	define GYRO_VIDEO_PLATFORM_INCLUDE	"x86/gyro_video_x86.inl"
#	define GYRO_TEXTURE_PLATFORM_INCLUDE "glal/gyro_texture_glal.inl"

#	define GYRO_MATRIX4_ALIGN			__attribute__ ((aligned (16)));
#	define GYRO_VECTOR4_ALIGN			__attribute__ ((aligned (16)));

#elif GYRO_PLATFORM == GYRO_PLATFORM_DC

#	define GYRO_MATRIX4_ALIGN			__attribute__((aligned(32)));
#	define GYRO_VECTOR4_ALIGN			__attribute__((aligned(16)));

#   define GYRO_ENDIAN GYRO_ENDIAN_LITTLE
#   ifndef GYRO_MATH_SOFTWARE
#       define GYRO_MATH_ACCELERATION
#   endif
#   ifndef GYRO_MATRIX_SOFTWARE
#       define GYRO_MATRIX_ACCELERATION
#   endif

#	define GYRO_INPUT_PLATFORM_INCLUDE      "dc/gyro_input_dc.h"
#	define GYRO_MATH_PLATFORM_INCLUDE       "dc/gyro_math_dc.inl"
#	define GYRO_MATRIX_PLATFORM_INCLUDE     "dc/gyro_matrix_dc.inl"
#	define GYRO_VIDEO_PLATFORM_INCLUDE      "dc/gyro_video_dc.h"
#	define GYRO_TEXTURE_PLATFORM_INCLUDE    "dc/gyro_texture_dc.h"

#elif GYRO_PLATFORM == GYRO_PLATFORM_PSP

#elif GYRO_PLATFORM == GYRO_PLATFORM_iOS

#	define GYRO_GLAL
#   define GYRO_GLAL_GL_ES3
#	define GYRO_ENDIAN						GYRO_ENDIAN_LITTLE
#	define GYRO_MATRIX4_ALIGN			__attribute__ ((aligned (32)))
#	define GYRO_VECTOR4_ALIGN			__attribute__ ((aligned (16)))

#   define GYRO_VIDEO_PLATFORM_INCLUDE      "ios/gyro_video_ios.inl"
#	define GYRO_AUDIO_PLATFORM_INCLUDE		"glal/gyro_audio_glal.inl"
#	define GYRO_INPUT_PLATFORM_INCLUDE		"ios/gyro_input_ios.inl"
#	define GYRO_TEXTURE_PLATFORM_INCLUDE	"glal/gyro_texture_glal.inl"
#   define GYRO_SHADER_PLATFORM_INCLUDE     "glal/gyro_video_glal.h"

#ifdef GYRO_PLATFORM_iOS_SIMULATOR
#   define GYRO_MATH_NOASM
#	define GYRO_MATH_PLATFORM_INCLUDE       "common/gyro_math_common.inl"
#	define GYRO_MATRIX_PLATFORM_INCLUDE     "x86/gyro_matrix_sse_x86.inl"
#elif defined(GYRO_PLATFORM_iOS_DEVICE)
#	define GYRO_MATH_PLATFORM_INCLUDE       "common/gyro_math_common.inl"
#	define GYRO_MATRIX_PLATFORM_INCLUDE     "arm/gyro_matrix_arm_neon.inl"
#endif


#elif GYRO_PLATFORM == GYRO_PLATFORM_ANDROID
#	define GYRO_GLAL
#   define GYRO_GLAL_GL_ES2
#	define GYRO_ENDIAN						GYRO_ENDIAN_LITTLE
//#	define GYRO_AUDIO_PLATFORM_INCLUDE		"glal/gyro_audio_glal.inl"
#	define GYRO_INPUT_PLATFORM_INCLUDE		"android/gyro_input_android.inl"
#	define GYRO_MATH_PLATFORM_INCLUDE		"common/gyro_math_common.inl"
#	define GYRO_MATRIX_PLATFORM_INCLUDE		"arm/gyro_matrix_arm_neon.inl"
#	define GYRO_TEXTURE_PLATFORM_INCLUDE	"glal/gyro_texture_glal.inl"
#	define GYRO_VIDEO_PLATFORM_INCLUDE		"android/gyro_video_android.inl"

#	define GYRO_MATRIX4_ALIGN				__attribute__ ((aligned (32)));
#	define GYRO_VECTOR4_ALIGN				__attribute__ ((aligned (16)));

#elif GYRO_PLATFORM == GYRO_PLATFORM_RASPI

#   define GY_ASSERT(a)                     Q_ASSERT(a)
#	define GYRO_GLAL
#   define GYRO_GLAL_GL_ES3
#ifndef GYRO_MATH_NOASM
#	define GYRO_MATRIX4_ALIGN               __attribute__ ((aligned (16)))
#	define GYRO_VECTOR4_ALIGN               __attribute__ ((aligned (16)))
#else
#	define GYRO_MATRIX4_ALIGN
#	define GYRO_VECTOR4_ALIGN
#endif

#	define GYRO_VIDEO_PLATFORM_INCLUDE      "x86/gyro_video_x86.inl"
#	define GYRO_TEXTURE_PLATFORM_INCLUDE    "qt/gyro_texture_qt.inl"
#   define GYRO_SHADER_PLATFORM_INCLUDE     "glal/gyro_video_glal.h"
#	define GYRO_MATH_PLATFORM_INCLUDE       "common/gyro_math_common.inl"
#	define GYRO_MATRIX_PLATFORM_INCLUDE     "arm/gyro_matrix_arm_neon.inl"
#	define GYRO_AUDIO_PLATFORM_INCLUDE      "glal/gyro_audio_glal.inl"
#	define GYRO_INPUT_PLATFORM_INCLUDE      "x86/gyro_input_x86.inl"
#   define GYRO_FILE_PLATFORM_INCLUDE       "glal/gyro_file_glal.inl"
#   define GYRO_NET_PLATFORM_INCLUDE        "x86/gyro_net_x86.inl"


#elif GYRO_PLATFORM == GYRO_PLATFORM_Qt

#if defined(__linux__) && defined(__i386__)
# define GYRO_MATH_NOASM
#endif


#ifdef __cplusplus
#   define GY_ASSERT(a)                     Q_ASSERT(a)
#else
#   define GY_ASSERT(a)                     assert(a)
#endif
#   define                                  GYRO_GLAL
#   define                                  GYRO_GLAL_GL_Desktop
#   define                                  GY_CL_OPENGL_INTEROP
#ifndef GYRO_MATH_NOASM
#	define GYRO_MATRIX4_ALIGN               __attribute__ ((aligned (16)))
#	define GYRO_VECTOR4_ALIGN               __attribute__ ((aligned (16)))
#else
#	define GYRO_MATRIX4_ALIGN
#	define GYRO_VECTOR4_ALIGN
#endif

//#	define GYRO_SYSTEM_PLATFORM_INCLUDE     "glal/gyro_glal.h"
#	define GYRO_VIDEO_PLATFORM_INCLUDE      "x86/gyro_video_x86.inl"
#	define GYRO_TEXTURE_PLATFORM_INCLUDE    "gl/gyro_texture_gl.inl"
#   define GYRO_SHADER_PLATFORM_INCLUDE     "gl/gyro_video_gl.h"
#	define GYRO_MATH_PLATFORM_INCLUDE       "x86/gyro_math_sse_x86.inl"
#	define GYRO_MATRIX_PLATFORM_INCLUDE     "x86/gyro_matrix_sse_x86.inl"
#	define GYRO_AUDIO_PLATFORM_INCLUDE      "al/gyro_audio_al.inl"
#	define GYRO_INPUT_PLATFORM_INCLUDE      "x86/gyro_input_x86.inl"
#   define GYRO_FILE_PLATFORM_INCLUDE       "x86/gyro_file_x86.inl"
#   define GYRO_NET_PLATFORM_INCLUDE        "sdl/gyro_net_sdl.inl"



#elif GYRO_PLATFORM == GYRO_PLATFORM_WEB
#	define GYRO_GLAL
# define GYRO_GLAL_GL_ES2
# define GYRO_NO_SPINE
#	define GYRO_MATRIX4_ALIGN               __attribute__ ((aligned (16)))
#	define GYRO_VECTOR4_ALIGN               __attribute__ ((aligned (16)))


//#	define GYRO_SYSTEM_PLATFORM_INCLUDE     "glal/gyro_glal.h"
#	define GYRO_VIDEO_PLATFORM_INCLUDE      "x86/gyro_video_x86.inl"
#	define GYRO_TEXTURE_PLATFORM_INCLUDE    "gl/gyro_texture_gl.inl"
#   define GYRO_SHADER_PLATFORM_INCLUDE     "gl/gyro_video_gl.h"
#	define GYRO_MATH_PLATFORM_INCLUDE       "x86/gyro_math_sse_x86.inl"
#	define GYRO_MATRIX_PLATFORM_INCLUDE     "x86/gyro_matrix_sse_x86.inl"
#	define GYRO_AUDIO_PLATFORM_INCLUDE      "al/gyro_audio_al.inl"
#	define GYRO_INPUT_PLATFORM_INCLUDE      "x86/gyro_input_x86.inl"
#   define GYRO_FILE_PLATFORM_INCLUDE       "x86/gyro_file_x86.inl"
#   define GYRO_NET_PLATFORM_INCLUDE        "sdl/gyro_net_sdl.inl"

#endif


#ifndef GY_ASSERT
#   define GY_ASSERT(a) assert(a)
#endif

#endif


//#ifdef __cplusplus
//}
//#endif

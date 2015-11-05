#ifndef GYRO_PLATFORM_H
#define GYRO_PLATFORM_H

//#ifdef __cplusplus
//extern "C" {
//#endif

#define GYRO_PLATFORM_x86		(1<<0)
#define GYRO_PLATFORM_DC		(1<<1)
#define GYRO_PLATFORM_PSP		(1<<2)
#define GYRO_PLATFORM_iOS		(1<<3)
#define GYRO_PLATFORM_ANDROID	(1<<4)
#define GYRO_PLATFORM_Qt		(1<<5)

//#define GYRO_GLAL_GL_Desktop    (1<<0)
//#define GYRO_GLAL_GL_ES3        (1<<1)
//#define GYRO_GLAL_GL_ES2        (1<<2)

#define GYRO_ENDIAN_LITTLE		0x01
#define GYRO_ENDIAN_BIG			0x02

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

#ifdef GYRO_PLATFORM_iOS_SIMULATOR
#	define GYRO_MATH_PLATFORM_INCLUDE       "x86/gyro_math_sse_x86.inl"
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


#elif GYRO_PLATFORM == GYRO_PLATFORM_Qt

#if defined(__linux__) && defined(__i386__)
# define GYRO_MATH_NOASM
#endif

#   define                                  GYRO_GLAL
#   define                                  GYRO_GLAL_GL_Desktop
#	define GYRO_MATRIX4_ALIGN			__attribute__ ((aligned (16)))
#	define GYRO_VECTOR4_ALIGN			__attribute__ ((aligned (16)))

//#	define GYRO_SYSTEM_PLATFORM_INCLUDE     "glal/gyro_glal.h"
#	define GYRO_VIDEO_PLATFORM_INCLUDE      "x86/gyro_video_x86.inl"
#	define GYRO_TEXTURE_PLATFORM_INCLUDE    "qt/gyro_texture_qt.inl"
#   define GYRO_SHADER_PLATFORM_INCLUDE     "glal/gyro_video_glal.h"
#	define GYRO_MATH_PLATFORM_INCLUDE       "x86/gyro_math_sse_x86.inl"
#	define GYRO_MATRIX_PLATFORM_INCLUDE     "x86/gyro_matrix_sse_x86.inl"
#	define GYRO_AUDIO_PLATFORM_INCLUDE      "glal/gyro_audio_glal.inl"
#	define GYRO_INPUT_PLATFORM_INCLUDE      "x86/gyro_input_x86.inl"
#   define GYRO_FILE_PLATFORM_INCLUDE       "glal/gyro_file_glal.inl"


//===== FUCKING HACK REEST - DESTROY ME =====
/*
#ifdef __cplusplus
extern "C" {
#endif

    static inline void gyVidSceneBegin() {  }

    static inline void gyVidSceneEnd() { }

#ifdef __cplusplus
}*/
#endif
// =============== / END REEST ==============


#endif


//#ifdef __cplusplus
//}
//#endif


#ifndef GYRO_PLATFORM_H
#define GYRO_PLATFORM_H

#ifdef __cplusplus
extern "C" { 
#endif

#define GYRO_PLATFORM_x86		(1<<0)
#define GYRO_PLATFORM_DC		(1<<1)
#define GYRO_PLATFORM_PSP		(1<<2)
#define GYRO_PLATFORM_iOS		(1<<3)
#define GYRO_PLATFORM_ANDROID	(1<<4)
#define GYRO_PLATFORM_Qt		(1<<5)
	
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

#	define GYRO_AUDIO_PLATFORM_INCLUDE	"glal/gyro_audio_glal.inl"
#	define GYRO_INPUT_PLATFORM_INCLUDE	"x86/gyro_input_x86.inl"
#	define GYRO_MATH_PLATFORM_INCLUDE	"x86/gyro_math_sse_x86.inl"
#	define GYRO_MATRIX_PLATFORM_INCLUDE "x86/gyro_matrix_sse_x86.inl"
#	define GYRO_SYSTEM_PLATFORM_INCLUDE "x86/gyro_glfw_x86.inl"
#	define GYRO_VIDEO_PLATFORM_INCLUDE	"x86/gyro_video_x86.inl"
#	define GYRO_TEXTURE_PLATFORM_INCLUDE "glal/gyro_texture_glal.inl"

#	define GYRO_MATRIX4_ALIGN			__attribute__ ((aligned (16)));
#	define GYRO_VECTOR4_ALIGN			__attribute__ ((aligned (16)));

#elif GYRO_PLATFORM == GYRO_PLATFORM_DC

#   define GYRO_ENDIAN GYRO_ENDIAN_LITTLE
#   ifndef GYRO_MATH_SOFTWARE
#       define GYRO_MATH_ACCELERATION
#   endif
#   ifndef GYRO_MATRIX_SOFTWARE
#       define GYRO_MATRIX_ACCELERATION
#   endif


#elif GYRO_PLATFORM == GYRO_PLATFORM_PSP

#elif GYRO_PLATFORM == GYRO_PLATFORM_iOS
	#   define GYRO_ENDIAN GYRO_ENDIAN_LITTLE
	#   define GYRO_MATH_SOFTWARE
	#   define GYRO_MATRIX_SOFTWARE

#elif GYRO_PLATFORM == GYRO_PLATFORM_ANDROID
	#   define GYRO_ENDIAN GYRO_ENDIAN_LITTLE
#	define GYRO_MATH_PLATFORM_INCLUDE	"common/gyro_math_common.inl"
#	define GYRO_MATRIX_PLATFORM_INCLUDE	"common/gyro_matrix_common.inl"
#elif GYRO_PLATFORM == GYRO_PLATFORM_Qt

#	define GYRO_TEXTURE_PLATFORM_INCLUDE "glal/gyro_texture_glal.inl"
#	define GYRO_MATH_PLATFORM_INCLUDE	"x86/gyro_math_sse_x86.inl"
#	define GYRO_MATRIX_PLATFORM_INCLUDE "x86/gyro_matrix_sse_x86.inl"

#	define GYRO_MATRIX4_ALIGN			__attribute__ ((aligned (16)));
#	define GYRO_VECTOR4_ALIGN			__attribute__ ((aligned (16)));

#endif


#ifdef __cplusplus
}
#endif


#endif

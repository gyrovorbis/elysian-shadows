#ifndef GYRO_PLATFORM_H
#define GYRO_PLATFORM_H

#ifdef __cplusplus
extern "C" { 
#endif

#define GYRO_PLATFORM_x86		(1<<0)
#define GYRO_PLATFORM_DC		(1<<1)
#define GYRO_PLATFORM_PSP		(1<<2)
#define GYRO_PLATFORM_iOS		(1<<3)
#define GYRO_PLATFORM_droid 	(1<<4)
	
#define GYRO_ENDIAN_LITTLE	0x01
#define GYRO_ENDIAN_BIG		0x02
	
//Platform-Specific Definitions
#if GYRO_PLATFORM == GYRO_PLATFORM_x86

#   define GYRO_ENDIAN GYRO_ENDIAN_LITTLE
//#   ifdef __x64
//#   endif
#   define GYRO_MATH_SOFTWARE
#   define GYRO_MATRIX_SOFTWARE
	
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
#elif GYRO_PLATFORM == GYRO_PLATFORM_droid
	#   define GYRO_ENDIAN GYRO_ENDIAN_LITTLE
	#   define GYRO_MATH_SOFTWARE
	#   define GYRO_MATRIX_SOFTWARE
#endif


#ifdef __cplusplus
}
#endif


#endif

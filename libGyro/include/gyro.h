#ifndef GYRO_H
#define GYRO_H

#ifdef __cplusplus
extern "C" {
#endif

#include "gyro_platform.h"
#include "gyro_defines.h"
#include "gyro_math.h"
#include "gyro_matrix.h"
#include "gyro_matrix_stack.h"
#include "gyro_video.h"
#include "gyro_input.h"
#include "gyro_system.h"
#include "gyro_texture.h"
#include "gyro_timer.h"
#include "gyro_audio.h"
#include "gyro_log.h"
#include "gyro_tracking.h"
#include "gyro_wav.h"
#include "gyro_file.h"

#if GYRO_PLATFORM == GYRO_PLATFORM_x86
#	include "x86/gyro_sdl_x86.h"
#endif


#ifdef __cplusplus
}
#endif

#endif // GYRO_H

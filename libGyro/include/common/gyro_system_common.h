#ifndef GYRO_SYSTEM_SHARED_H
#define GYRO_SYSTEM_SHARED_H

#ifdef __cplusplus
extern "C" {
#endif

void gySysDebugEnable(GY_DEBUG_LEVEL level, void(*func)(const GY_DEBUG_LEVEL level, const char *const));
void _gyLog(GY_DEBUG_LEVEL level, const char* const str, ...);

#ifdef __cplusplus
}
#endif

#endif // GYRO_SYSTEM_SHARED_H

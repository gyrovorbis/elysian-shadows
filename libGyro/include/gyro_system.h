#ifndef GYRO_SYSTEM_H
#define GYRO_SYSTEM_H

#ifdef __cplusplus
extern "C" {
#endif

#define GYRO_LIB_VERSION "1.5.5"

typedef enum _GY_DEBUG_LEVEL { GY_DEBUG_VERBOSE, GY_DEBUG_CRITICAL, GY_DEBUG_FATAL, GY_DEBUG_OFF } GY_DEBUG_LEVEL;

int gySysInit(void);
int gySysUninit(void);

void gySysDebugEnable(GY_DEBUG_LEVEL level, void(*func)(const GY_DEBUG_LEVEL level, const char *const));
void gyPrintf(const char *const str, ...);

unsigned int gySysStatsVram(void);
unsigned int gySysStatsRam(void);

void _gyLog(GY_DEBUG_LEVEL level, const char* const str, ...);

void gyDebugSigInit(void);

#ifdef __cplusplus
}
#endif

#endif

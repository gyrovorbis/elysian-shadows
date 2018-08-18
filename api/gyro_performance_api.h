#ifndef GYRO_PERFORMANCE_API_H
#define GYRO_PERFORMANCE_API_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct GYPerfInitParams {
    int maxMonitors;
} GYPerfInitParams;

typedef struct GYPerfStatistics {
    char dummy;
    //cache hits/misses
    //clock cycles
    //branch prediction
    //execution time
} GYPerfStatistics;

int                     gyPerfInit(const GYPerfInitParams* params);
int                     gyPerfUninit(void);

int                     gyPerfMonitorBegin(unsigned index);
int                     gyPerfMonitorEnd(unsigned index);
const GYPerfStatistics* gyPerfMonitorStatistics(unsigned index);

uint64_t                gyPerfCounter(void);
uint64_t                gyPerfFrequency(void);



//=== INLINEZ =======
extern uint64_t         _gyPerfFreqCached;


static inline uint64_t gyPerfFrequencyCached(void) { return _gyPerfFreqCached; }

#ifdef __cplusplus
}
#endif

#endif // GYRO_PERFORMANCE_API_H

#ifndef GYRO_PERFORMANCE_API_H
#define GYRO_PERFORMANCE_API_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct GYPerfInitParams {
    int maxMonitors;
} GYPerfInitParams;

typedef struct GYPerfStatistics {
    //cache hits/misses
    //clock cycles
    //branch prediction
    //execution time
} GYPerfStatistics;

int gyPerfInit(const GYPerfInitParams* params);
int gyPerfUninit(void);

int gyPerfMonitorBegin(unsigned index);
int gyPerfMonitorEnd(unsigned index);
const GYPerfStatistics* gyPerfMonitorStatistics(index);





#ifdef __cplusplus
}
#endif

#endif // GYRO_PERFORMANCE_API_H

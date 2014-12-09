#ifndef GYRO_TIMER_H
#define GYRO_TIMER_H

#include "gyro_platform.h"

#ifdef __cplusplus
extern "C" {
#endif
    
int                 gyTimerInit(void);
int                 gyTimerUninit(void);
unsigned long int   gyTimerTicks(void);
void                gyTimerSleep(const unsigned long duration);

#ifdef GYRO_TIMER_PLATFORM_INCLUDE
#	include GYRO_TIMER_PLATFORM_INCLUDE
#endif
    
#ifdef __cplusplus
}
#endif

#endif // GYRO_TIMER_H

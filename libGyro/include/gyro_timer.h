#ifndef GYRO_TIMER_H
#define GYRO_TIMER_H

#ifdef __cplusplus
extern "C" {
#endif
    
int gyTimerInit(void);
unsigned long int gyTimerTicks(void);
void gyTimerSleep(const unsigned long duration);
    
#ifdef __cplusplus
}
#endif

#endif // GYRO_TIMER_H

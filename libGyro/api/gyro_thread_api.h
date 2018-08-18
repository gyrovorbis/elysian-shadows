#ifndef GYRO_THREAD_API_H
#define GYRO_THREAD_API_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define GY_THREAD_DEBUG

typedef uint32_t GYCpuAffinityMask[4];

typedef int (*GYThreadFunction)(void*);

typedef struct GYThreadInitParams {
    char dummy;
} GYThreadInitParams;


typedef enum GY_THREAD_PRIORITY {
    GY_THREAD_PRIORITY_LOW,
    GY_THREAD_PRIORITY_MEDIUM,
    GY_THREAD_PRIORITY_HIGH,
    GY_THREAD_PRIORITY_COUNT
} GY_THREAD_PRIORITY;

typedef struct GYThread {

} GYThread;

typedef struct GYMutex {


} GYMutex;

typedef struct GYSemaphore {

} GYSemaphore;

typedef struct GYCondition {

} GYCondition;

#ifdef GY_THREAD_DEBUG
typedef struct GYThreadDebugInfo {
    unsigned    activeThreadCount;
    unsigned    activeMutexCount;
    unsigned    activeSemaphoreCount;
    unsigned    activeConditionCount;
} GYThreadDebugInfo;
#endif

int gyThreadInit(const GYThreadInitParams* params);
int gyThreadUninit(void);

//int gyProcessPriority();
//int gyProcessCpuAffinity();

//==== THREAD API ======
GYThread*       gyThreadCreate(const char* name,
                               GYThreadFunction fn,
                               void* data);
int             gyThreadDestroy(GYThread* thread);
void            gyThreadDetach(GYThread* thread);
const char*     gyThreadName(GYThread* thread);
int             gyThreadWait(GYThread* thread);

//OPERATE IMPLICITLY ON THE CURRENT THREAD!
int             gyThreadPriority(GY_THREAD_PRIORITY priority);
int             gyThreadCpuAffinity(GYCpuAffinityMask mask);


//==== MUTEX API ======
GYMutex*        gyMutexCreate(void);
int             gyMutexDestroy(GYMutex* mtx);
int             gyMutexLock(GYMutex* mtx);
int             gyMutexTryLock(GYMutex* mtx);
int             gyMutexUnlock(GYMutex* mtx);


//==== SEMAPHORE API ======
GYSemaphore*    gySemaphoreCreate(uint32_t initialValue);
int             gySemaphoreDestroy(GYSemaphore* sem);
int             gySemaphorePost(GYSemaphore* sem);
int             gySemaphoreWait(GYSemaphore* sem);
int             gySemaphoreTryWait(GYSemaphore* sem);
uint32_t        gySemaphoreValue(GYSemaphore* sem);
int             gySemaphoreWaitTimeout(GYSemaphore* sem, uint32_t ms);

//=== CONDITIONAL API =====
GYCondition*    gyConditionCreate(void);
int             gyConditionDestroy(GYCondition* cond);
int             gyConditionBroadcast(GYCondition* cond);
int             gyConditionSignal(GYCondition* cond);
int             gyConditionWait(GYCondition* cond, GYMutex* mtx);
int             gyConditionWaitTimeout(GYCondition* cond, uint32_t ms);

//=== ATOMICS API ====


#ifdef GY_THREAD_DEBUG
const GYThreadDebugInfo* gyThreadDebugQueryInfo(void);
#endif

//Atomics



//spinlocks

#ifdef __cplusplus
}
#endif

#endif // GYRO_THREAD_API_H

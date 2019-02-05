#ifndef GYRO_THREAD_API_H
#define GYRO_THREAD_API_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define GY_THREAD_DEBUG

typedef uint32_t GYCpuAffinityMask[4];
typedef int      GYAtomic;

#define GY_CPU_AFFINITY_MASK_CORE_SET(m, c) \
    m[c/32] |= (0x1 << (c%32))

#define GY_CPU_AFFINITY_MASK_CORE_GET(m, c) \
    ((m[c/32] >> (c%32)) & 0x1)

typedef int (*GYThreadFunction)(void*);

typedef struct GYThreadInitParams {
    char dummy;
} GYThreadInitParams;


typedef enum GY_THREAD_PRIORITY {
    GY_THREAD_PRIORITY_LOW,
    GY_THREAD_PRIORITY_MEDIUM,
    GY_THREAD_PRIORITY_HIGH,
    GY_THREAD_PRIORITY_REALTIME,
    GY_THREAD_PRIORITY_COUNT
} GY_THREAD_PRIORITY;

typedef struct GYThread {
    char dummy;
} GYThread;

typedef struct GYMutex {
    char dummy;

} GYMutex;

typedef struct GYSemaphore {
    char dummy;
} GYSemaphore;

typedef struct GYCondition {
    char dummy;
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
int             gyThreadKill(GYThread* thread);
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
int             gyConditionWaitTimeout(GYCondition* cond, GYMutex* mtx, uint32_t ms);

//=== ATOMICS API ====


#ifdef GY_THREAD_DEBUG
const GYThreadDebugInfo* gyThreadDebugQueryInfo(void);
#endif

//Atomics
int gyAtomicAdd(GYAtomic* a, int v);
int gyAtomicGet(GYAtomic* a);
int gyAtomicSet(GYAtomic* a, int v);
void gyCompilerBarrier(void);



//spinlocks

#ifdef __cplusplus
}
#endif

#endif // GYRO_THREAD_API_H

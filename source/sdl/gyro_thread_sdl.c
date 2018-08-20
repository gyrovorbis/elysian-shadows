#include <gyro_thread_api.h>
#include <gyro_platform.h>
#include <gyro_system_api.h>
#include <gyro_defines.h>
#include <assert.h>

#ifndef __WIN32
#   define GY_THREAD_PTHREADS
#endif

#if GYRO_PLATFORM == GYRO_PLATFORM_RASPI
#   include <SDL2/SDL_thread.h>
#   include <SDL2/SDL_mutex.h>
#else
#   ifdef __WIN32 // :D Mmmmmm
#       include <windows.h>
#       include <SDL/SDL_thread.h>
#       include <SDL/SDL_mutex.h>
#   endif
#   ifdef __linux__ // :) Sweet!
#       include <SDL2/SDL_thread.h>
#       include <SDL2/SDL_mutex.h>
#   endif
#   ifdef __APPLE__ // D: Fuck my life...
#       include <SDL_thread.h>
#       include <SDL_mutex.h>
#   endif
#endif

#ifdef GY_THREAD_PTHREADS
#   include <pthread.h>


//Manually provide the pthread functionality MacOS is missing from the POSIX standard
#ifdef __APPLE__
#   include <mach/mach.h>
#   include <sys/sysctl.h>
#   include <mach/mach_time.h>
#   define SYSCTL_CORE_COUNT   "machdep.cpu.core_count"

typedef struct cpu_set {
  uint32_t    count;
} cpu_set_t;

static inline void
CPU_ZERO(cpu_set_t *cs) { cs->count = 0; }

static inline void
CPU_SET(int num, cpu_set_t *cs) { cs->count |= (1 << num); }

static inline int
CPU_ISSET(int num, cpu_set_t *cs) { return (cs->count & (1 << num)); }

static int sched_getaffinity(pid_t pid, size_t cpu_size, cpu_set_t *cpu_set)
{
  int32_t core_count = 0;
  size_t  len = sizeof(core_count);
  int ret = sysctlbyname(SYSCTL_CORE_COUNT, &core_count, &len, 0, 0);
  if (ret) {
    _gyLog(GY_DEBUG_ERROR, "GY_THREAD_API: error while geting core count: %d\n", ret);
    return -1;
  }
  cpu_set->count = 0;
  for (int i = 0; i < core_count; i++) {
    cpu_set->count |= (1 << i);
  }

  return 0;
}

static int pthread_setaffinity_np(pthread_t thread, size_t cpu_size,
                           cpu_set_t *cpu_set)
{
    int success = 1;
    thread_port_t mach_thread;
    int core = 0;
    int retVal;

    for (core = 0; core < 8 * cpu_size; core++) {
        if (CPU_ISSET(core, cpu_set)) break;
    }
    thread_affinity_policy_data_t policy = { core };
    mach_thread = pthread_mach_thread_np(thread);
    if((retVal =
        thread_policy_set(mach_thread, THREAD_AFFINITY_POLICY, (thread_policy_t)&policy, 1)
        != KERN_SUCCESS))
    {
        _gyLog(GY_DEBUG_ERROR, "GYRO_THEAD_API: Mach thread_policy_set failed! [Error Code: %d]", retVal);
        success = 0;
    }

    return success;
}
#endif

#endif

static struct SDLThreadContext {
#ifdef GY_THREAD_DEBUG
    GYThreadDebugInfo   dbg;
#endif
} _context;

static int _initialized = 0;

static inline const char* gyThreadPriorityString(GY_THREAD_PRIORITY priority) {
    switch(priority) {
    GYRO_SWITCH_CASE_STRING(GY_THREAD_PRIORITY_LOW);
    GYRO_SWITCH_CASE_STRING(GY_THREAD_PRIORITY_MEDIUM);
    GYRO_SWITCH_CASE_STRING(GY_THREAD_PRIORITY_HIGH);
    GYRO_SWITCH_CASE_STRING(GY_THREAD_PRIORITY_REALTIME);
    default:
        assert(0);
        return "UNKNOWN PRIORITY";
    }
}

int gyThreadInit(const GYThreadInitParams *params) {
    if(_initialized) return 0;
    (void)params;
    _gyLog(GY_DEBUG_VERBOSE, "GYRO_THREAD_API: Initializing SDL Thread Back-End.");
    memset(&_context, 0, sizeof(struct SDLThreadContext));
    _initialized = 1;
    return 1;
}

int gyThreadUninit(void) {
    if(!_initialized) return 0;
    int success = 1;
    _gyLog(GY_DEBUG_VERBOSE, "GYRO_THREAD_API: Uninitializing SDL Thread Back-End.");
    _gyPush();

//Check for objects that were never deallocated
#ifdef GY_THREAD_DEBUG
    if(_context.dbg.activeThreadCount) {
        _gyLog(GY_DEBUG_WARNING, "Remaining Thread(s): %u",     _context.dbg.activeThreadCount);
        success = 0;
    }
    if(_context.dbg.activeMutexCount) {
        _gyLog(GY_DEBUG_WARNING, "Remaining Mutex(es): %u",     _context.dbg.activeMutexCount);
        success = 0;
    }
    if(_context.dbg.activeSemaphoreCount) {
        _gyLog(GY_DEBUG_WARNING, "Remaining Semaphore(s): %u",  _context.dbg.activeSemaphoreCount);
        success = 0;
    }
    if(_context.dbg.activeConditionCount)   {
        _gyLog(GY_DEBUG_WARNING, "Remaining Condition(s): %u",  _context.dbg.activeConditionCount);
        success = 0;
    }
#endif

    _gyPop(1);
    _initialized = 0;
    return success;
}

//==== THREAD API ======
GYThread* gyThreadCreate(const char* name,
                         GYThreadFunction fn,
                         void* data)
{
    assert(name && fn);
    SDL_Thread* thread = SDL_CreateThread(fn, name, data);
#ifdef GY_THREAD_DEBUG
    if(thread) ++_context.dbg.activeThreadCount;
    else _gyLog(GY_DEBUG_ERROR, "GY_THREAD_API: Failed to create thread: %s", name);
#endif
    return (GYThread*)thread;
}

int gyThreadKill(GYThread* thread) {

}

void gyThreadDetach(GYThread* thread) {
    //assert(thread); Safely accepts NULL threads
    SDL_DetachThread((SDL_Thread*)thread);
#ifdef GY_THREAD_DEBUG
    assert(_context.dbg.activeThreadCount);
    --_context.dbg.activeThreadCount;
#endif
}

const char* gyThreadName(GYThread* thread) {
    assert(thread);
    return SDL_GetThreadName((SDL_Thread*)thread);
}

int gyThreadWait(GYThread* thread) {
    if(!thread) return 0;
    char thdName[200] = { 0 };
    strncpy(thdName, gyThreadName(thread), 200);
    //assert(thread); Safely accepts NULL threads
    int retValue = 0;
    SDL_WaitThread((SDL_Thread*)thread, &retValue);
    _gyLog(GY_DEBUG_VERBOSE, "Thread[%s] joined with return value: [%d]", thdName, retValue);
#ifdef GY_THREAD_DEBUG
    assert(_context.dbg.activeThreadCount);
    --_context.dbg.activeThreadCount;
#endif
    return retValue;
}

#ifdef GY_THREAD_PTHREADS
static inline int _pthreadPriority(GY_THREAD_PRIORITY gyPriority, int priorityMax) {
    float scalar;
    switch(gyPriority) {
    case GY_THREAD_PRIORITY_LOW:
        scalar = 0.25f;
        break;
    case GY_THREAD_PRIORITY_MEDIUM:
        scalar = 0.6f;
        break;
    default:
    case GY_THREAD_PRIORITY_HIGH:
        scalar = 1.0f;
        break;
    }
    return priorityMax*scalar;
}

static inline int _macEnableRealtimePriority(void) {
    struct thread_time_constraint_policy ttcpolicy;
    kern_return_t retVal;

    const double kGuaranteedAudioDutyCycle = 0.75;
    const double kMaxAudioDutyCycle = 0.85;

    // About 128 frames @44.1KHz
    const double kTimeQuantum = 16.666;
    // Time guaranteed each quantum.
    const double kAudioTimeNeeded = kGuaranteedAudioDutyCycle * kTimeQuantum;
    // Maximum time each quantum.
    const double kMaxTimeAllowed = kMaxAudioDutyCycle * kTimeQuantum;

    mach_timebase_info_data_t tb_info;
    mach_timebase_info(&tb_info);

    double ms_to_abs_time =
          ((double)(tb_info.denom) / tb_info.numer) * 1000000;

    ttcpolicy.period = kTimeQuantum * ms_to_abs_time;
    ttcpolicy.computation = kAudioTimeNeeded * ms_to_abs_time;
    ttcpolicy.constraint = kMaxTimeAllowed * ms_to_abs_time;
    ttcpolicy.preemptible   = 1;


    if((retVal = thread_policy_set(mach_thread_self(),
                                     THREAD_TIME_CONSTRAINT_POLICY,
                                     (thread_policy_t)&ttcpolicy,
                                     THREAD_TIME_CONSTRAINT_POLICY_COUNT)) != KERN_SUCCESS) {
        _gyLog(GY_DEBUG_ERROR, "GYRO_THREAD_API: Failed to enable real-time scheduling!");
        return 0;
    }

    return 1;
}

#endif

int gyThreadPriority(GY_THREAD_PRIORITY priority) {
    if(priority == GY_THREAD_PRIORITY_REALTIME) return _macEnableRealtimePriority();

    int success = 1;

#ifdef GY_THREAD_PTHREADS
    pthread_t thId = pthread_self();
    SDL_threadID sdlId = SDL_ThreadID();
    int policy = 0;
    int max_prio_for_policy = 0;
    int errorCode;
    struct sched_param param;

    //_gyLog(GY_DEBUG_VERBOSE, "Assigning priority [%s] for thread: %d", gyThreadPriorityString(priority), sdlId);
    //_gyPush();

    if((errorCode = pthread_getschedparam(thId, &policy, &param))) {
        _gyLog(GY_DEBUG_ERROR, "Failed to retreive pthread scheduling policy! [Error: %d]", strerror(errorCode));
        success = 0;
        goto end;
    }

    if((max_prio_for_policy = sched_get_priority_max(policy)) == -1) {
        _gyLog(GY_DEBUG_ERROR, "Failed to retreive max pthread priority! [Error: %d]", strerror(errorCode));
        success = 0;
        goto end;
    } else {
        param.sched_priority = _pthreadPriority(priority, max_prio_for_policy);
        _gyLog(GY_DEBUG_VERBOSE, "Assigning Thread [%d] Priority: [%d/%d]", sdlId, param.sched_priority, max_prio_for_policy);
    }

    if((errorCode = pthread_setschedparam(thId, policy, &param))) {
        _gyLog(GY_DEBUG_ERROR, "Failed to set pthread priority [%d]! [Error: %d]", priority, strerror(errorCode));
        success = 0;
        goto end;
    }

#else

#endif

end:
   // _gyPop(1);
    return success;
}


int gyThreadCpuAffinity(GYCpuAffinityMask mask) {
    assert(mask);
    int success = 1;
    SDL_threadID sdlId = SDL_ThreadID();

    //_gyLog(GY_DEBUG_VERBOSE, "Assigning Thread [%u] CPU Affinity.", sdlId);
    //_gyPush();

    cpu_set_t cpuMask;
    memset(&cpuMask, 0, sizeof(cpu_set_t));

    const unsigned maskSize = (sizeof(cpu_set_t) > sizeof(GYCpuAffinityMask))? sizeof(GYCpuAffinityMask) : sizeof(cpu_set_t);

    char buffer[sizeof(GYCpuAffinityMask)*8*5];
    char intBuff[10];
    strcat(buffer, "<");
    int first = 1;
    int cpuCount = gySystemQueryInfo()->cpuCount;

    for(unsigned i = 0; i < maskSize*8; ++i) {
        if(i >= cpuCount) break;
        uint32_t curByte = i/8;
        uint32_t curBit = i%8;
        uint8_t gyByte = *(((uint8_t*)mask) + curByte);
        if(gyByte & (0x1<<curBit)) {
            if(!first) {
                strcat(buffer, ", ");
            } else first = 0;
            sprintf(intBuff, "%u", i);
            strcat(buffer, intBuff);
            CPU_SET(i, &cpuMask);
        }
    }
    strcat(buffer, ">");
    _gyLog(GY_DEBUG_VERBOSE, "Setting CPU Affinity for Thread [%d]: Cores: %s", sdlId, buffer);

    success &= pthread_setaffinity_np(pthread_self(), maskSize, &cpuMask);
    if(!success) {
        _gyLog(GY_DEBUG_ERROR, "GY_THREAD_API: Failed to modify thread CPU affinity for thread: %d", sdlId);
    }

   // _gyPop(1);
    return success;
}

//==== MUTEX API ======
GYMutex* gyMutexCreate(void) {
    SDL_mutex* mutex = SDL_CreateMutex();
#ifdef GY_THREAD_DEBUG
    if(mutex) ++_context.dbg.activeMutexCount;
    else _gyLog(GY_DEBUG_ERROR, "GY_THREAD_API: Failed to create SDL_mutex!");
#endif
    return (GYMutex*)mutex;
}

int gyMutexDestroy(GYMutex* mtx) {
    SDL_DestroyMutex((SDL_mutex*)mtx);
#ifdef GY_THREAD_DEBUG
    assert(_context.dbg.activeMutexCount);
    --_context.dbg.activeMutexCount;
#endif
    return 1;
}

int gyMutexLock(GYMutex* mtx) {
    if(SDL_LockMutex((SDL_mutex*)mtx)) {
        _gyLog(GY_DEBUG_ERROR, "GY_THREAD_API: Failed to lock mutex: [%s]", SDL_GetError());
        return 0;
    }
    return 1;
}

int gyMutexTryLock(GYMutex* mtx) {
    int retVal = SDL_TryLockMutex((SDL_mutex*)mtx);
    if(retVal) {
        if(retVal != SDL_MUTEX_TIMEDOUT)
            _gyLog(GY_DEBUG_ERROR, "GY_THREAD_API: Failed to unlock mutex: [%s]", SDL_GetError());
        return 0;
    }
    return 1;
}

int gyMutexUnlock(GYMutex* mtx) {
    if(SDL_UnlockMutex((SDL_mutex*)mtx)) {
        _gyLog(GY_DEBUG_ERROR, "GY_THREAD_API: Failed to unlock mutex: [%s]", SDL_GetError());
        return 0;
    }
    return 1;
}

//==== SEMAPHORE API ======
GYSemaphore* gySemaphoreCreate(uint32_t initialValue) {
    SDL_sem* sem = SDL_CreateSemaphore(initialValue);
#ifdef GY_THREAD_DEBUG
    if(sem) ++_context.dbg.activeSemaphoreCount;
    else _gyLog(GY_DEBUG_ERROR, "GY_THREAD_API: Failed to create SDL Semaphore!");
#endif
    return (GYSemaphore*)sem;
}
int gySemaphoreDestroy(GYSemaphore* sem) {
    SDL_DestroySemaphore((SDL_sem*)sem);
#ifdef GY_THREAD_DEBUG
    --_context.dbg.activeSemaphoreCount;
#endif
}
int gySemaphorePost(GYSemaphore* sem) {
    if(SDL_SemPost((SDL_sem*)sem)) {
        _gyLog(GY_DEBUG_ERROR, "GY_THREAD_API: Failed posting semaphore! [Error: %s]", SDL_GetError());
        return 0;
    }
    return 1;
}
int gySemaphoreWait(GYSemaphore* sem) {
    if(SDL_SemWait((SDL_sem*)sem)) {
        _gyLog(GY_DEBUG_ERROR, "GY_THREAD_API: Failed waiting on a semaphore! [Error: %s]", SDL_GetError());
        return 0;
    }
    return 1;
}

int gySemaphoreTryWait(GYSemaphore* sem) {
    int retVal;
    if((retVal = SDL_SemWait((SDL_sem*)sem))) {
        if(retVal != SDL_MUTEX_TIMEDOUT)
            _gyLog(GY_DEBUG_ERROR, "GY_THREAD_API: TryWait on a semaphore failed! [Error: %s]", SDL_GetError());
        return 0;
    }
    return 1;
}

uint32_t gySemaphoreValue(GYSemaphore* sem) {
    assert(sem);
    return SDL_SemValue((SDL_sem*)sem);
}

int gySemaphoreWaitTimeout(GYSemaphore* sem, uint32_t ms) {
    int retVal;
    if((retVal = SDL_SemWaitTimeout((SDL_sem*)sem, ms))) {
        if(retVal != SDL_MUTEX_TIMEDOUT)
            _gyLog(GY_DEBUG_ERROR, "GY_THREAD_API: WaitTimeout on a semaphore failed! [Error: %s]", SDL_GetError());
        return 0;
    }
    return 1;
}

//=== CONDITIONAL API =====
GYCondition* gyConditionCreate(void) {
    SDL_cond* cond = SDL_CreateCond();
#ifdef GY_THREAD_DEBUG
    if(cond) ++_context.dbg.activeConditionCount;
    else _gyLog(GY_DEBUG_ERROR, "GY_THREAD_API: Failed to create SDL Condition variable!");
#endif
    return (GYCondition*)cond;
}

int gyConditionDestroy(GYCondition* cond) {
    if(!cond) return 0;
    SDL_DestroyCond((SDL_cond*)cond);
#ifdef GY_THREAD_DEBUG
    assert(_context.dbg.activeConditionCount);
    --_context.dbg.activeConditionCount;
#endif
    return 1;
}

int gyConditionBroadcast(GYCondition* cond) {
    if(SDL_CondBroadcast((SDL_cond*)cond)) {
        _gyLog(GY_DEBUG_ERROR, "GY_THREAD_API: Failed to Broadcast SDL Condition variable: [%s]", SDL_GetError());
        return 0;
    }
    return 1;
}

int gyConditionSignal(GYCondition* cond) {
    if(SDL_CondSignal((SDL_cond*)cond)) {
        _gyLog(GY_DEBUG_ERROR, "GY_THREAD_API: Failed to Signal SDL Condition variable: [%s]", SDL_GetError());
        return 0;
    }
    return 1;
}

int gyConditionWait(GYCondition* cond, GYMutex* mtx) {
    if(SDL_CondWait((SDL_cond*)cond, (SDL_mutex*)mtx)) {
        _gyLog(GY_DEBUG_ERROR, "GY_THREAD_API: Failed to wait on SDL Condition variable: [%s]", SDL_GetError());
        return 0;
    }
    return 1;
}

int gyConditionWaitTimeout(GYCondition* cond, GYMutex* mtx, uint32_t ms) {
    int retVal = SDL_CondWaitTimeout((SDL_cond*)cond, (SDL_mutex*)mtx, ms);
    if(retVal) {
        if(retVal != SDL_MUTEX_TIMEDOUT)
            _gyLog(GY_DEBUG_ERROR, "GY_THREAD_API: Failed to wait on SDL Condition variable: [%s]", SDL_GetError());
        return 0;
    }
    return 1;
}

const GYThreadDebugInfo* gyThreadDebugQueryInfo(void) { return &_context.dbg; }

//==== ATOMICS ====

int gyAtomicAdd(GYAtomic* a, int v) {
    return SDL_AtomicAdd((SDL_atomic_t*)a, v);
}

int gyAtomicGet(GYAtomic* a) {
    return SDL_AtomicGet((SDL_atomic_t*)a);
}

int gyAtomicSet(GYAtomic* a, int v) {
    return SDL_AtomicSet((SDL_atomic_t*)a, v);
}

void gyCompilerBarrier(void) {
    SDL_CompilerBarrier();
}


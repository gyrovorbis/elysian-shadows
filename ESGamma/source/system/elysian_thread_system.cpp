/*
 * Copyright (c) 2013-2026 Elysian Shadows Team
 * SPDX-License-Identifier: MIT
 * Additional permission for Elysian Shadows Kickstarter backers: see LICENSE.
 */

#include <ESGamma/system/elysian_thread_system.hpp>
#include <ESGamma/crtp/elysian_debug.hpp>
#include <libGyro/gyro_thread_api.h>
#include <climits>

namespace elysian {

int Thread::_gyThreadFunc(void* argData) {
    Thread* self = static_cast<Thread*>(argData);
    gyThreadCpuAffinity(self->_cpuAffinityMask);
    gyThreadPriority(self->_priority);
    return self->getThreadFunc()(self->getArgData());
}

int AsyncTask::execTask(void*) {
    assert(false);
    return 0;
}

bool ThreadSystem::initialize(void) {
    bool success = true;
    DbgEvent event("ThreadSystem initializing.");

    GYThreadInitParams params;
    memset(&params, 0, sizeof(GYThreadInitParams));
    success &= gyThreadInit(&params);

    if(success) {
        DbgEvent event2("Creating AsyncTaskQueueThreads for %u cores.", getCpuCoreCount());
        _taskThreads = new AsyncTaskQueueThread*[getCpuCoreCount()];
        for(unsigned c = 0; c < getCpuCoreCount(); ++c) {
            char buffer[100] = { 0 };
            GYCpuAffinityMask cpuMask;
            memset(&cpuMask, 0, sizeof(GYCpuAffinityMask));
            GY_CPU_AFFINITY_MASK_CORE_SET(cpuMask, c);
            sprintf(buffer, "AsyncTaskQueueThread[%d]", c);
            _taskThreads[c] = new AsyncTaskQueueThread(cpuMask, buffer);
            success &= _taskThreads[c]->isValid();
        }
    }

    gyThreadPriority(GY_THREAD_PRIORITY_REALTIME);

    return success;
}

bool ThreadSystem::uninitialize(void) {
    bool success = true;
     DbgEvent event("ThreadSystem uninitializing.");

    dbgprintf("Killing %u active threads.", getThreadCount());
    dbgpush();
    for(unsigned t = 0; t < _threads.size(); ++t) {
        delete _threads[t];
    }
    dbgpop();

    dbgprintf("Killing Async Task Queue threads.");
    dbgpush();
    for(unsigned c = 0; c < getCpuCoreCount(); ++c) {
        success &= _taskThreads[c]->forceQuit(false);
        _taskThreads[c]->enqueueTask(nullptr); //Enqueue dummy task to wake the fucker up to commit suicide
    }
    for(unsigned c = 0; c < getCpuCoreCount(); ++c) {
        success &= _taskThreads[c]->wait();
        delete _taskThreads[c];
    }
    delete[] _taskThreads;
    dbgpop();

    return gyThreadUninit();
}

unsigned ThreadSystem::findLeastBusyTaskQueueThread(void) const {
    size_t  smallestLoad    = std::numeric_limits<size_t>::max();
    int     smallestIndex   = -1;

    for(int t = 0; t < (int)getCpuCoreCount(); ++t) {
        auto size = getTaskQueueThread(t).getQueueSize();
        if(size < smallestLoad) {
            smallestIndex = t;
            smallestLoad = size;
        }
    }
    assert(smallestIndex != -1);
    return (unsigned)smallestIndex;
}

bool ThreadSystem::enqueueAsyncTask(AsyncTask* task, void* argData) {
    assert(task);
#if 0
    return getTaskQueueThread(findLeastBusyTaskQueueThread()).enqueueTask(task, argData);
#else
    auto thr = findLeastBusyTaskQueueThread();
    dbgprintf("Scheduling %s on %d", task->getTaskName(), thr);
        return getTaskQueueThread(thr).enqueueTask(task, argData);
#endif
}




}

#ifndef ELYSIAN_THREAD_SYSTEM_HPP
#define ELYSIAN_THREAD_SYSTEM_HPP

#include <vector>
#include <queue>
#include <libGyro/gyro_thread_api.h>
#include <ESGamma/system/elysian_system.hpp>
#include <ESGamma/utility/elysian_performance_monitor.hpp>

#define ELYSIAN_THREAD_DEFAULT_PRIORITY             GY_THREAD_PRIORITY_HIGH
#define ELYSIAN_ASYNC_TASK_QUEUE_THREAD_PRIORITY    GY_THREAD_PRIORITY_REALTIME
#define ELYSIAN_ASYNC_TASK_NAME_SIZE                50

#ifndef ELYSIAN_THREAD_PROFILING_ENABLED
#   define ELYSIAN_THREAD_PROFILING_ENABLED   1
#endif

namespace elysian {

    class Mutex {
    private:
        GYMutex*    _gyMtx  = nullptr;
    public:
            Mutex(void);
            ~Mutex(void);

        bool acquire(void);
        bool release(void);
        bool tryAcquire(void);

        template<typename F>
        bool protectedBlock(F&& func);
    };

    class RAIIMutexGuard {
    private:
        Mutex*  _mutex = nullptr;
    public:

        RAIIMutexGuard(Mutex* mtx);
        ~RAIIMutexGuard(void);
    };

    class Semaphore {
    private:
        GYSemaphore*    _gySem = nullptr;
    public:
                    Semaphore(uint32_t initialValue=0);
                    ~Semaphore(void);
        bool        post(void);
        bool        wait(void);
        bool        tryWait(void);
        uint32_t    getValue(void) const;
    };

    class Condition: private GYCondition {

    };

    class Thread {
    public:
        using ThreadFunc    = std::function<int(void*)>;
    protected:
#ifndef ELYSIAN_THREAD_PROFILING_ENABLED
        PerformanceMonitor  _perfMonitor;
#endif
        GYCpuAffinityMask   _cpuAffinityMask;
        ThreadFunc          _threadFunc;
        GYThread*           _gyThread           = nullptr;
        void*               _argData            = nullptr;
        GY_THREAD_PRIORITY  _priority           = ELYSIAN_THREAD_DEFAULT_PRIORITY;
        bool                _forceQuit          = false;
        bool                _valid              = false;

        static int          _gyThreadFunc(void* argData);

        inline constexpr static const GYCpuAffinityMask _defaultCpuAffinityMask = {
            0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff
        };

    public:
                            Thread(const char*              name,
                                   ThreadFunc               fn          = ThreadFunc(),    //No explicit handler defaults to execThread() virtual function
                                   void*                    argData     = nullptr,
                                   GY_THREAD_PRIORITY       priority    = ELYSIAN_THREAD_DEFAULT_PRIORITY,
                                   const GYCpuAffinityMask  cpuAffinity = _defaultCpuAffinityMask);

        virtual             ~Thread(void) = default;

        virtual int         execThread(void* argData=nullptr)  = 0;

        ThreadFunc          getThreadFunc(void) const;
        void*               getArgData(void) const;
        bool                isValid(void) const;
        bool                wait(void) const;
        bool                detach(void);
        bool                forceQuit(const bool blocking=true);
        bool                setPriority(GY_THREAD_PRIORITY priority);
        bool                setCpuAffinity(GYCpuAffinityMask affinity);


#ifndef ELYSIAN_THREAD_PROFILING_ENABLED
        bool                dumpInfo(void);
#endif
    };

    class AsyncTask {
    public:
        using TaskFunc      = std::function<int(void*)>;
    private:
        TaskFunc            _taskFunc;
        char                _name[ELYSIAN_ASYNC_TASK_NAME_SIZE] = { 0 };
    public:
                            AsyncTask(const char* name,
                                      TaskFunc fn = TaskFunc());
        virtual             ~AsyncTask(void) = default;
        TaskFunc            getTaskFunc(void) const;
        const char*         getTaskName(void) const;

        virtual int         execTask(void* argData=nullptr);

    };

    //LuaAsyncTask? (requires multithreading VM)

    class AsyncTaskQueueThread: public Thread {
    private:
        struct TaskEntry {
            AsyncTask*          task    = nullptr;
            void*               argData = nullptr;
        };

        std::queue<TaskEntry>   _taskQueue;
        mutable Mutex           _queueMutex;
        Semaphore               _queueSemaphore;

        TaskEntry               popFront(void);
        TaskEntry               getFront(void) const;
    public:
                                AsyncTaskQueueThread(const GYCpuAffinityMask affinity, const char* name);
                                ~AsyncTaskQueueThread(void);
        bool                    enqueueTask(AsyncTask* task, void* argData=nullptr);

        size_t                  getQueueSize(void) const;
        bool                    finishTasks(void);

        //overridden from Thread
        virtual int             execThread(void* argData=nullptr) override;

#ifndef ELYSIAN_THREAD_PROFILING_ENABLED
        bool                    dumpInfo(void);
#endif

    };


    class ThreadSystem: public System {
    private:
        AsyncTaskQueueThread**      _taskThreads    = nullptr;
        std::vector<Thread*>        _threads;


        AsyncTaskQueueThread&       getTaskQueueThread(unsigned core);

    public:
#ifndef ELYSIAN_THREAD_PROFILING_ENABLED
        bool                        dumpInfo(void);
#endif

        bool                        initialize(void);
        bool                        uninitialize(void);

        bool                        setMainThreadPriority(GY_THREAD_PRIORITY priority) const;
        bool                        setMainThreadCpuAffinity(GYCpuAffinityMask mask) const;

        //General threads
        unsigned                    getThreadCount(void) const;
        void                        addThread(Thread* thr);
        Thread*                     findThread(const char* name) const;
        bool                        removeThread(const Thread* thr);
        bool                        removeThread(const char* name);

        //Async task queues
        unsigned                    getCpuCoreCount(void) const;
        const AsyncTaskQueueThread& getTaskQueueThread(unsigned core) const;
        bool                        enqueueAsyncTask(AsyncTask* task, void* argData=nullptr);
        size_t                      getTaskQueuesTotalSize(void) const;
        unsigned                    findLeastBusyTaskQueueThread(void) const;

    };




    //=========== INLINEZ ===========
    inline Mutex::Mutex(void):
        _gyMtx(gyMutexCreate())
    {}

    inline Mutex::~Mutex(void) {
        gyMutexDestroy(_gyMtx);
    }

    template<typename F>
    inline bool Mutex::protectedBlock(F&& func) {
        bool success = acquire();
        if(success) {
            func();
            success &= release();
        }
        return success;
    }

    inline bool Mutex::acquire(void) {
        return gyMutexLock(_gyMtx);
    }
    inline bool Mutex::release(void) {
        return gyMutexUnlock(_gyMtx);
    }
    inline bool Mutex::tryAcquire(void) {
        return gyMutexTryLock(_gyMtx);
    }

    inline RAIIMutexGuard::RAIIMutexGuard(Mutex* mtx):
        _mutex(mtx)
    {
        assert(_mutex->acquire());
    }

    inline RAIIMutexGuard::~RAIIMutexGuard(void) {
        assert(_mutex->release());
    }

    inline Semaphore::Semaphore(uint32_t initialValue):
        _gySem(gySemaphoreCreate(initialValue)) {
    }

    inline Semaphore::~Semaphore(void) {
        gySemaphoreDestroy(_gySem);
    }

    inline bool Semaphore::post(void) {
        return gySemaphorePost(_gySem);
    }

    inline bool Semaphore::wait(void) {
        return gySemaphoreWait(_gySem);
    }

    inline bool Semaphore::tryWait(void) {
        return gySemaphoreTryWait(_gySem);
    }

    inline Thread::Thread(const char*   name,
           std::function<int(void*)>    fn,
           void*                        argData,
           GY_THREAD_PRIORITY           priority,
           const GYCpuAffinityMask      cpuAffinity):
        _threadFunc(fn ? std::move(fn) : std::bind(&Thread::execThread, this, std::placeholders::_1)),
        _argData(argData),
        _priority(priority)
    {
        memcpy(_cpuAffinityMask, cpuAffinity, sizeof(GYCpuAffinityMask));
        _gyThread = gyThreadCreate(name, &_gyThreadFunc, this);
    }

    inline void* Thread::getArgData(void) const { return _argData; }

    inline auto Thread::getThreadFunc(void) const -> ThreadFunc {
        return _threadFunc;
    }

    inline bool Thread::forceQuit(const bool blocking) {
        _forceQuit = true;
        return blocking? wait() : true;
    }

    inline bool Thread::wait(void) const {
        return gyThreadWait(_gyThread);
    }

    inline bool Thread::isValid(void) const {
        return _valid;
    }

    inline bool Thread::setPriority(GY_THREAD_PRIORITY priority) {
     //   return gyThreadPriority(this, priority);
    }
    inline bool Thread::setCpuAffinity(GYCpuAffinityMask mask) {
     //   return gyThreadCpuAffinity(this, mask);
    }

    inline AsyncTask::AsyncTask(const char* name,
                                TaskFunc fn):
        _taskFunc(fn? std::move(fn) :
                      std::bind(&AsyncTask::execTask, this, std::placeholders::_1))
    {
        strncpy(_name, name, ELYSIAN_ASYNC_TASK_NAME_SIZE);
    }

    inline const char* AsyncTask::getTaskName(void) const { return _name; }

    inline auto AsyncTask::getTaskFunc(void) const -> TaskFunc { return _taskFunc; }

    inline AsyncTaskQueueThread::AsyncTaskQueueThread(const GYCpuAffinityMask cpuMask, const char* name):
        Thread(name,
               nullptr,
               nullptr,
               ELYSIAN_ASYNC_TASK_QUEUE_THREAD_PRIORITY,
               cpuMask)
    {

    }

    inline AsyncTaskQueueThread::~AsyncTaskQueueThread(void) {}

    inline bool AsyncTaskQueueThread::enqueueTask(AsyncTask* task, void* argData) {
        bool success = _queueMutex.acquire();
        if(success) {
            _taskQueue.push({task, argData});
            success &= _queueMutex.release();
            success &= _queueSemaphore.post();
        }
        return success;
    }

    inline auto AsyncTaskQueueThread::popFront(void) -> TaskEntry {
        assert(_taskQueue.size());
        TaskEntry entry = _taskQueue.front();

        _queueMutex.protectedBlock([&](){
            _taskQueue.pop();
        });

        return entry;
    }

    inline size_t AsyncTaskQueueThread::getQueueSize(void) const {
        size_t size = 0;
        _queueMutex.protectedBlock([&](){
            size = _taskQueue.size();
        });
        return size;
    }

    inline int AsyncTaskQueueThread::execThread(void*) {
        bool success = true;
        while(!_forceQuit) {
            success &= _queueSemaphore.wait();
            if(_forceQuit) break;
            TaskEntry entry = popFront();

            if(entry.task) {
                entry.task->getTaskFunc()(entry.argData);
            }
        }
        return (int)success;
    }

    //==== ThreadSystem inlinez ======

    inline unsigned ThreadSystem::getThreadCount(void) const {
        return _threads.size();
    }

    inline const AsyncTaskQueueThread& ThreadSystem::getTaskQueueThread(unsigned core) const {
        assert(core < getCpuCoreCount());
        return *_taskThreads[core];
    }

    inline AsyncTaskQueueThread& ThreadSystem::getTaskQueueThread(unsigned core) {
        assert(core < getCpuCoreCount());
        return *_taskThreads[core];
    }

    inline unsigned ThreadSystem::getCpuCoreCount(void) const {
        return (unsigned)gySystemQueryInfo()->cpuCount;
    }


}

#endif // ELYSIAN_THREAD_SYSTEM_HPP

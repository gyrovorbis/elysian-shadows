#ifndef ELYSIAN_COMPUTE_SYSTEM_HPP
#define ELYSIAN_COMPUTE_SYSTEM_HPP

#include <ESGamma/system/elysian_system.hpp>
#include <libGyro/gyro_compute_api.h>
#include <libGyro/gyro_platform.h>
#include <cstring>
#include <cassert>
#include <initializer_list>
#include <vector>
#include <map>
#include <functional>
#include <AssetIO/utility/elysian_cstr_map.hpp>

#ifndef ELYSIAN_COMPUTE_PROFILING_ENABLED
#   define ELYSIAN_COMPUTE_PROFILING_ENABLED   1
#endif

#ifdef ELYSIAN_COMPUTE_PROFILING_ENABLED
#   include <ESGamma/utility/elysian_performance_monitor.hpp>
#endif

#if GYRO_PLATFORM != GYRO_PLATFORM_DC
#   define ELYSIAN_COMPUTE_CONTEXT_MAX_DEVICES 5
#else
#   define ELYSIAN_COMPUTE_CONTEXT_MAX_DEVICES 1
#endif

#define ELYSIAN_COMPUTE_CONTEXTS_PER_DEVICE    2
#define ELYSIAN_COMPUTE_PLATFORM_INVALID       -1
#define ELYSIAN_COMPUTE_DEVICE_INVALID         -1

#define ELYSIAN_COMPUTE_HEADER_FILES_MAX       3




struct GYComputeDevice;

#if ELYSIAN_COMPUTE_PROFILING_ENABLED == 1
ELYSIAN_ASSET_TYPEDEF(TaskMonitor)
#endif

namespace elysian {

    class ComputeSystem;
    class ComputeDevice;
    class ComputeCommandQueue;
    class ComputeBuffer;
    class ComputeProgram;
    class ComputeKernel;

    enum class ComputeDeviceCategory {
        GpuRendererOnline,
        GpuRendererOffline,
        GpuAuxiliary,
        CpuRenderer,
        CpuAuxiliary,
        OtherAccelerator,
        HardwareDeviceCount,
        NativeFallback,
        Invalid
    };


    class ComputeEvent {
    private:
        GYComputeEvent* _gyEvent    = nullptr;

    public:
                                    ComputeEvent(void);
                                    ~ComputeEvent(void);
        bool                        wait(void) const;
        GY_COMPUTE_EVENT_STATUS     getStatus(void) const;
        GYComputeEvent*             getGyEvent(void) const;
#ifdef ELYSIAN_COMPUTE_PROFILING_ENABLED
        uint64_t                    getExecutionTime(void) const;
#endif
    };

    class ComputeEventList {
    private:
        std::vector<ComputeEvent*> _list;
    public:
        bool        wait(void);//?
        unsigned    getSize(void) const;
    };



    class ComputeContext {
        friend class ComputeSystem;
    private:
        ComputeDevice*      _devices[ELYSIAN_COMPUTE_CONTEXT_MAX_DEVICES] = { nullptr };
        GYComputeContext*   _gyContext          = nullptr;
        unsigned            _deviceCount        = 0;
        int                 _gyPlatformIdx      = -1;
        bool                _isRendererContext  = false;

        void _addDevice(ComputeDevice* dev) {
            assert(_deviceCount+1 < ELYSIAN_COMPUTE_CONTEXT_MAX_DEVICES);
            _devices[_deviceCount++] = dev;
        }

        ComputeContext(GYComputeContext* ctx, unsigned gyPlatformIdx, bool isRendererContext, unsigned deviceCount, ComputeDevice** devices):
            _gyContext(ctx),
            _deviceCount(deviceCount),
            _gyPlatformIdx(gyPlatformIdx),
            _isRendererContext(isRendererContext)
        {
            assert(ctx && deviceCount && devices);
            for(unsigned d = 0; d < deviceCount; ++d) _devices[d] = devices[d];
        }

    public:
        GYComputeContext* getGyContext(void) const { return _gyContext; }

        bool isRendererContext(void) const { return _isRendererContext; }

        ComputeCommandQueue*    createCommandQueue(ComputeDevice* device, int flags);
        ComputeBuffer*          createBuffer(int memType, size_t size, void *data=nullptr);
        ComputeBuffer*          createBufferFromRendererBuffer(intptr_t rendererBuffer, GY_COMPUTE_MEMORY_TYPE memType);
        ComputeBuffer*          createBufferFromRendererTexture(intptr_t rendererTexture, GY_COMPUTE_MEMORY_TYPE memType, int target);
        ComputeProgram*         loadProgram(const char* filePath);

    };


    class ComputeCommandQueue {
    friend class ComputeContext;
public:
        GYComputeCommandQueue*  _gyCmdQueue = nullptr;
        ComputeDevice*          _device     = nullptr;
        ComputeCommandQueue(GYComputeCommandQueue* queue, ComputeDevice* device);
    public:
        //int                             gyComputeCommandQueueEnqueueBufferWrite(GYComputeCommandQueue* queue, const GYComputeBuffer* buffer, int blocking, size_t offset, size_t cb, const void* data, unsigned eventCount, void* eventList, void* event);
        bool enqueueBufferWrite(ComputeBuffer* buffer, size_t offset, size_t size, void* data, bool blocking=true);
        bool enqueueBufferRead(ComputeBuffer* buffer, size_t offset, size_t size, void* ptr, bool blocking=true, const ComputeEventList* eventList=nullptr, ComputeEvent* event=nullptr);
        bool enqueueKernel(ComputeKernel* kernel, unsigned workDimensions, const size_t* globalWorkOffset, const size_t* globalWorkSize, const size_t* localWorkSize, ComputeEventList* eventList=nullptr, ComputeEvent* event=nullptr);
        bool enqueueAcquireRendererSharedObject(unsigned count, ComputeBuffer** buffers);
        bool enqueueReleaseRendererSharedObject(unsigned count, ComputeBuffer** buffers, const ComputeEventList* eventList=nullptr, ComputeEvent* event=nullptr);
        bool        finish(void);

                ~ComputeCommandQueue(void);
    };

    class ComputeBuffer {
        friend class ComputeContext;
    private:
        GYComputeBuffer*    _gyBuffer       = nullptr;
        ComputeBuffer(GYComputeBuffer* buffer);
    public:
        ~ComputeBuffer(void);
        GYComputeBuffer* getGyBuffer(void) const;
    };

    class ComputeProgram {
    friend class ComputeContext;
    private:
        GYComputeProgram*               _gyProgram      = nullptr;
        ComputeContext*                 _context        = nullptr;
        bool                            _compiled       = false;
        std::vector<ComputeDevice*>     _compiledDevices;

                                        ComputeProgram(GYComputeProgram* program, ComputeContext* context);
    public:
                                        ~ComputeProgram(void);

        bool                            addHeaderFile(const char* filePath);
        bool                            addBuildFlag(const char* flag);

        bool                            compile(ComputeDevice* devices, unsigned deviceCount, const GYComputeProgramBuildOptions* options, GYComputeBuildInfo* buildInfo);
        ComputeKernel*                  createKernel(const char* name) const;

        bool                            isValid(void) const; //valid for at least one device
        bool                            isValidForDevice(const ComputeDevice* device) const;
        bool                            isValidForDevice(unsigned int deviceIdx) const;
    };


    class ComputeKernel {
    friend class ComputeProgram;
    private:
        GYComputeKernel*    _gyKernel       = nullptr;
        ComputeKernel(GYComputeKernel* kernel);

    public:
        ~ComputeKernel(void);
        bool setArg(unsigned index, size_t size, void* data) const;
        bool setArg(unsigned index, const ComputeBuffer* buffer) const;
        int findArgIndexFromName(const char* const name);
      //  bool setArg(const char* name, );
        GYComputeKernel* getGyKernel(void) const;
        const GYComputeKernelInfo* getInfo(void) const;
    };



    class ComputeTask: public System {
        friend class ComputeSystem;
    public:

        struct ProgramCreateInfo {
            const char*                         sourceFilePath = nullptr;
            GYComputeProgramBuildOptions        buildOptions;
                                                ProgramCreateInfo(const char* sourcePath);
        };

        struct NativeThreadData {
            uint32_t         workDim             = 0;
            uint32_t         globalSize[3]       = { 0 };
            uint32_t         localSize[3]        = { 0 };
            uint32_t         globalId[3]         = { 0 };
            uint32_t         localId[3]          = { 0 };

            uint32_t         get_work_dim(void) const;
            uint32_t         get_global_size(uint32_t dim) const;
            uint32_t         get_global_id(uint32_t dim) const;
            uint32_t         get_local_size(uint32_t dim) const;
            uint32_t         get_local_id(uint32_t dim) const;
            uint32_t         get_num_groups(uint32_t dim) const;
            uint32_t         get_group_id(uint32_t dim) const;
        };

        using NativeKernel                  = std::function<void(NativeThreadData*)>;
        using DeviceCategoryPriorityList    = std::vector<ComputeDeviceCategory>;
        using DevicePriorityList            = std::vector<ComputeDevice>;

    protected:
#if ELYSIAN_COMPUTE_PROFILING_ENABLED == 1
        PerformanceMonitor          _perfMonitor;
        ComputeEvent                _profilerEvent;
        bool                        _profilerEventPending = false;
#endif
        DeviceCategoryPriorityList  _categoryPriorityList;
        DevicePriorityList          _devicePriorityList;
        NativeKernel                _nativeFallback;
        ComputeDevice*              _device                 = nullptr;
        ComputeProgram*             _program                = nullptr;
        ComputeKernel*              _kernel                 = nullptr;
        ComputeCommandQueue*        _cmdQueue               = nullptr;
        ComputeContext*             _context                = nullptr;
        bool                        _useNative              = false;
        bool                        _isValid                = false;

        bool                        _resolvePrioritizedDevices(bool requirerenderSharedContext);
        bool                        _initializeResources(const char* kernelName,
                                                        const ProgramCreateInfo& programInfo,
                                                        bool outOfOrderExecution,
                                                        bool requireRendererSharedContext);
        bool                        _uninitializeResources(void);

    public:

        ComputeTask(const char*                         kernelName,
                    const ProgramCreateInfo&            progInfo,
                    DeviceCategoryPriorityList          deviceCategoryPriorities,
                    NativeKernel                        nativeFallbackFunc              = NativeKernel(),
                    bool                                requireRendererSharedContext    = false,
                    bool                                outOfOrderExecution             = false);

        ~ComputeTask(void);

        int                     getPlatformIndex(void) const;
        ComputeDeviceCategory   getDeviceAtPriority(unsigned priority) const;
        ComputeDevice*          getDevice(void) const;
        ComputeProgram*         getProgram(void) const;
        ComputeKernel*          getKernel(void) const;
        ComputeCommandQueue*    getCommandQueue(void) const;
        ComputeContext*         getContext(void) const;
        NativeKernel            getNativeFallback(void) const;
#if ELYSIAN_COMPUTE_PROFILING_ENABLED == 1
        PerformanceMonitor*     getPerfMonitor(void);
#endif


        bool                    enqueueKernel(size_t globalSize);
        bool                    enqueueKernel(size_t globalSizeX, size_t globalSizeY);
        bool                    enqueueKernel(size_t globalSizeX, size_t globalSizeY, size_t globalSizeZ);
        bool                    enqueueKernel(unsigned workDimensions, const size_t* globalWorkOffset, const size_t* globalWorkSize, const size_t* localWorkSize, ComputeEventList* eventList=nullptr, ComputeEvent* event=nullptr);

        bool                    finish(void);

        void                    optimizeWorkSize(size_t globalSize, size_t& paddedGlobalSize, size_t& localSize);

        bool                    isValid(void) const; //can call kernel in any form
        void                    setValid(bool val=true);
        bool                    isAccelerationAvailable(void) const;
        bool                    isAccelerated(void) const;
        void                    setAccelerated(bool val=true);
        void                    setNativeFallback(NativeKernel nativeFunc);

    };


    class ComputeSystem: public System {
    public:

        const constexpr static int InvalidPlatform = ELYSIAN_COMPUTE_PLATFORM_INVALID;


        static const char*      _deviceCategoryStrings[(int)ComputeDeviceCategory::HardwareDeviceCount];

        static const char*      getDeviceTypeString(ComputeDeviceCategory category);
        static bool             isRendererSharedContextDeviceCategory(ComputeDeviceCategory category);

        bool                    initialize(void);
        bool                    uninitialize(void);

        bool                    isComputeSupported(void) const;
        bool                    isComputeEnabled(void) const;
        void                    setComputeEnabled(bool enabled=true);



        ComputeTask*            findComputeTask(const char*const kernelName) const;
        ComputeTask*            createComputeTask(const char*                                      kernelName,
                                                  ComputeTask::ProgramCreateInfo                   progInfo,
                                                  ComputeTask::DeviceCategoryPriorityList          deviceCategoryPriorities,
                                                  ComputeTask::NativeKernel                        nativeFallbackFunc              = ComputeTask::NativeKernel(),
                                                  bool                                             requireRendererSharedContext    = false,
                                                  bool                                             outOfOrderExecution             = false);
        void                    addComputeTask(ComputeTask* task);
        ComputeContext*         createContext(int gyPlatformIdx, bool isRendererContext, unsigned deviceCount, ComputeDevice** devices);
        ComputeDevice*          getDevice(int platformIdx, int deviceIdx);
        ComputeDevice*          getDeviceByCategory(ComputeDeviceCategory type, unsigned idx = 0);
        unsigned                getDeviceCountForCategory(ComputeDeviceCategory type) const;
        ComputeContext*         getPlatformContextFromDeviceCategory(ComputeDeviceCategory type, unsigned idx = 0) const;
        ComputeContext*         getRendererContext(void) const;
        int                     getRendererPlatform(void) const;
        ComputeContext*         getPlatformContext(int platformIdx) const;
        unsigned                getPlatformDeviceCount(int platformIdx) const;
        unsigned                getPlatformCount(void) const;
        bool                    isRendererInteropSupported(void) const;
        bool                    queryEnvironmentInfo(void) const;

#if ELYSIAN_COMPUTE_PROFILING_ENABLED == 1
        PerformanceMonitor*     getPerfMonitor(void);
#endif


    private:

#if ELYSIAN_COMPUTE_PROFILING_ENABLED == 1
        PerformanceMonitor      _perfMonitor = PerformanceMonitor("ComputeTasks");
#endif
        bool                    _assignRendererPlatform(void);
        bool                    _assignDeviceTypes(void);
        bool                    _sortDeviceTypesByPerformance(void);
        bool                    _createContexts(void);


        bool                    _initialized                = false;
        bool                    _enabled                    = true;
        unsigned                _platformCount              = 0;
        unsigned*               _deviceCount                = nullptr;
        int                     _rendererPlatformIdx        = InvalidPlatform;
        ComputeContext*         _rendererContext            = nullptr;
        ComputeContext**        _platformContexts           = nullptr;
        ComputeDevice**			_devices;

        std::vector<ComputeDevice*>
                                _categorizedDevices[(int)ComputeDeviceCategory::HardwareDeviceCount];

        std::map<const char*, ComputeTask*, cstr_map>
                                _computeTasks;
    };



    class ComputeDevice {
        friend class ComputeProgram;
    private:
        struct CompiledKernel {
            ComputeKernel*                  kernel;
            GYComputeKernelWorkgroupInfo    workgroupInfo;
                                            CompiledKernel(ComputeKernel* kernel);
        };

        std::vector<CompiledKernel>         _compiledKernels;
        GYComputeDevice                     _gyDevice;
        ComputeContext*                     _context        = nullptr;
        bool                                _isValid        = false;
        ComputeDeviceCategory               _category       = ComputeDeviceCategory::Invalid;


        bool                                _addCompiledKernel(ComputeKernel* kernel);
    public:
        void                                setGyDevice(GYComputeDevice dev) { _gyDevice = dev; _isValid = true; }
        void                                setDeviceCategory(ComputeDeviceCategory category) { _category = category; }
        auto                                getDeviceType(void) const { return _category; }
        void                                setContext(ComputeContext* ctx) { assert(ctx); _context = ctx; }
        const GYComputeDevice*              getGyDevice(void) const { return (_isValid)? &_gyDevice : nullptr; }
        ComputeContext*                     getContext(void) const;
        const GYComputeKernelWorkgroupInfo* getKernelWorkGroupInfo(const ComputeKernel* kernel) const;
        const GYComputeDeviceInfo*          getInfo(void) const;
        int                                 getPlatformIdx(void) const;
        int                                 getDeviceIdx(void) const;
        ComputeDeviceCategory               getDeviceCategory(void) const;
    };



    //==================== COMPUTE TASK INLINEZ =========================
    inline uint32_t ComputeTask::NativeThreadData::get_work_dim(void) const {
        return workDim;
    }

    inline uint32_t ComputeTask::NativeThreadData::get_global_size(uint32_t dim) const {
        assert(dim >= 0 && dim < workDim);
        return  globalSize[dim];
    }

    inline uint32_t ComputeTask::NativeThreadData::get_global_id(uint32_t dim) const {
        assert(dim >= 0 && dim < workDim);
        return globalId[dim];
    }

    inline uint32_t ComputeTask::NativeThreadData::get_local_size(uint32_t dim) const {
        assert(dim >= 0 && dim < workDim);
        return localSize[dim];
    }

    inline uint32_t ComputeTask::NativeThreadData::get_local_id(uint32_t dim) const {
        assert(dim >= 0 && dim < workDim);
        return localId[dim];
    }

    inline uint32_t ComputeTask::NativeThreadData::get_num_groups(uint32_t dim) const {
        assert(dim >= 0 && dim < workDim);
        return globalSize[dim] / localSize[dim];
    }

    inline uint32_t ComputeTask::NativeThreadData::get_group_id(uint32_t dim) const {
        assert(dim >= 0 && dim < workDim);
        return globalId[dim] / localSize[dim];
    }

    inline ComputeTask::ProgramCreateInfo::ProgramCreateInfo(const char* sourcePath):
        sourceFilePath(sourcePath)
    {
        memset(&buildOptions, 0, sizeof(GYComputeProgramBuildOptions));
    }

    inline ComputeContext* ComputeTask::getContext(void) const {
        return _context;
    }

    inline ComputeCommandQueue* ComputeTask::getCommandQueue(void) const {
        return _cmdQueue;
    }

    inline ComputeDevice* ComputeTask::getDevice(void) const {
        return _device;
    }

    inline ComputeKernel* ComputeTask::getKernel(void) const {
        return _kernel;
    }

    inline ComputeProgram* ComputeTask::getProgram(void) const {
        return _program;
    }
#if ELYSIAN_COMPUTE_PROFILING_ENABLED == 1
    inline PerformanceMonitor* ComputeTask::getPerfMonitor(void) { return &_perfMonitor; }
#endif

    inline void ComputeTask::setValid(bool val) { _isValid = val; }
    inline bool ComputeTask::isValid(void) const { return _isValid; }
    inline bool ComputeTask::isAccelerated(void) const { return !_useNative; }
    inline void ComputeTask::setAccelerated(bool val) {
        if(_nativeFallback) {
            _useNative = true;
        } else {
            _isValid = false;
        }
    }

    inline ComputeContext* ComputeDevice::getContext(void) const { return _context; }

    inline ComputeCommandQueue::ComputeCommandQueue(GYComputeCommandQueue* queue, ComputeDevice* device):
        _gyCmdQueue(queue),
        _device(device)
    {
        assert(queue && device);
    }


    inline ComputeBuffer::ComputeBuffer(GYComputeBuffer *buffer):
        _gyBuffer(buffer)
    {
        assert(buffer);
    }

    inline ComputeProgram::ComputeProgram(GYComputeProgram* program, ComputeContext *context):
        _gyProgram(program),
        _context(context)
    {
        assert(program && context);
    }

    inline ComputeKernel::ComputeKernel(GYComputeKernel *kernel):
        _gyKernel(kernel)
    {
        assert(kernel);
    }

    inline ComputeEvent::ComputeEvent(void):
        _gyEvent(gyComputeEventCreate()) {

    }

    inline ComputeEvent::~ComputeEvent(void) {
        gyComputeEventDestroy(_gyEvent);
    }

    inline GYComputeEvent* ComputeEvent::getGyEvent(void) const { return _gyEvent; }

    inline unsigned ComputeEventList::getSize(void) const { return _list.size(); }

    inline GYComputeBuffer* ComputeBuffer::getGyBuffer(void) const {
        return _gyBuffer;
    }

    inline GYComputeKernel* ComputeKernel::getGyKernel(void) const {
        return _gyKernel;
    }

    inline const GYComputeDeviceInfo* ComputeDevice::getInfo(void) const {
        return gyComputeDeviceQueryInfo(&_gyDevice);
    }

    inline int ComputeDevice::getPlatformIdx(void) const {
        return _gyDevice.platformIdx;
    }

    inline int ComputeDevice::getDeviceIdx(void) const {
        return _gyDevice.deviceIdx;
    }

    inline ComputeDeviceCategory ComputeDevice::getDeviceCategory(void) const { return _category; }


    //===== COMPUTE SYSTEM INLINEZ ==========
    inline ComputeDevice* ComputeSystem::getDevice(int platformIdx, int deviceIdx) {
        assert(platformIdx < (int)_platformCount && platformIdx >= 0 && deviceIdx < (int)_deviceCount[platformIdx] && deviceIdx >= 0);
        return &_devices[platformIdx][deviceIdx];
    }

    inline ComputeDevice* ComputeSystem::getDeviceByCategory(ComputeDeviceCategory type, unsigned idx) {
        assert((int)type >= 0 && (int)type < (int)ComputeDeviceCategory::HardwareDeviceCount);
        return (idx < _categorizedDevices[(int)type].size())? _categorizedDevices[(int)type][idx] : nullptr;
    }

    inline unsigned ComputeSystem::getDeviceCountForCategory(ComputeDeviceCategory type) const {
        assert((int)type >= 0 && (int)type < (int)ComputeDeviceCategory::HardwareDeviceCount);
        return _categorizedDevices[(int)type].size();
    }

    inline ComputeContext* ComputeSystem::getPlatformContextFromDeviceCategory(ComputeDeviceCategory type, unsigned idx) const {
        assert((int)type >= 0 && (int)type < (int)ComputeDeviceCategory::HardwareDeviceCount);
        return getPlatformContext(_categorizedDevices[(int)type][idx]->getPlatformIdx());
    }

    inline ComputeContext* ComputeSystem::getRendererContext(void) const { return _rendererContext; }
    inline int ComputeSystem::getRendererPlatform(void) const { return _rendererPlatformIdx; }


    inline ComputeContext* ComputeSystem::getPlatformContext(int platformIdx) const {
        assert(platformIdx < (int)_platformCount && platformIdx >= 0);
        return _platformContexts[platformIdx];
    }


    inline unsigned ComputeSystem::getPlatformDeviceCount(int platformIdx) const {
        assert(platformIdx < (int)_platformCount && platformIdx >= 0);
        return _deviceCount[platformIdx];
    }

    inline ComputeTask* ComputeSystem::findComputeTask(const char * const kernelName) const {
        auto task = _computeTasks.find(kernelName);
        return (task != _computeTasks.end())? task->second : nullptr;
    }

    inline bool ComputeSystem::isComputeSupported(void) const { return _initialized; }
    inline bool ComputeSystem::isComputeEnabled(void) const { return isComputeSupported() && _enabled; }
    inline void ComputeSystem::setComputeEnabled(bool enabled) { _enabled = enabled; }
    inline bool ComputeSystem::isRendererInteropSupported(void) const { return getPlatformContextFromDeviceCategory(ComputeDeviceCategory::GpuRendererOnline); }
    inline unsigned ComputeSystem::getPlatformCount(void) const { return _platformCount; }
    inline bool ComputeSystem::isRendererSharedContextDeviceCategory(ComputeDeviceCategory category) {
        switch(category) {
        case ComputeDeviceCategory::GpuRendererOnline:
        case ComputeDeviceCategory::GpuRendererOffline:
        case ComputeDeviceCategory::CpuRenderer:
            return true;
        default:
            return false;
        }
    }

#if ELYSIAN_COMPUTE_PROFILING_ENABLED == 1
    inline PerformanceMonitor* ComputeSystem::getPerfMonitor(void) { return &_perfMonitor; }
#endif

}

#endif // ELYSIAN_COMPUTE_SYSTEM_HPP

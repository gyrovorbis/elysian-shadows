#ifndef ELYSIAN_COMPUTE_SYSTEM_HPP
#define ELYSIAN_COMPUTE_SYSTEM_HPP

#include <system/elysian_system.hpp>
#include <gyro_compute_api.h>
#include <gyro_platform.h>
#include <cstring>
#include <cassert>
#include <initializer_list>
#include <vector>
#include <functional>

#if GYRO_PLATFORM != GYRO_PLATFORM_DC
#   define ELYSIAN_COMPUTE_CONTEXT_MAX_DEVICES 5
#else
#   define ELYSIAN_COMPUTE_CONTEXT_MAX_DEVICES 1
#endif

#define ELYSIAN_COMPUTE_CONTEXTS_PER_DEVICE     2
#define ELYSIAN_COMPUTE_PLATFORM_INVALID        -1
#define ELYSIAN_COMPUTE_DEVICE_INVALID          -1

#define ELYSIAN_COMPUTE_HEADER_FILES_MAX        3

struct GYComputeDevice;

namespace elysian {

    class ComputeSystem;
    class ComputeDevice;
    class ComputeCommandQueue;
    class ComputeBuffer;
    class ComputeProgram;
    class ComputeKernel;

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
        ComputeBuffer*          createBuffer(GY_COMPUTE_MEMORY_TYPE memType, size_t size, void *data);
        ComputeBuffer*          createBufferFromRendererBuffer(intptr_t rendererBuffer, GY_COMPUTE_MEMORY_TYPE memType);
        ComputeProgram*         loadProgram(const char* filePath);



    };



    class ComputeCommandQueue {
    friend class ComputeContext;
    private:
        GYComputeCommandQueue*  _gyCmdQueue = nullptr;
        ComputeDevice*          _device     = nullptr;
        ComputeCommandQueue(GYComputeCommandQueue* queue, ComputeDevice* device);
    public:
        //int                             gyComputeCommandQueueEnqueueBufferWrite(GYComputeCommandQueue* queue, const GYComputeBuffer* buffer, int blocking, size_t offset, size_t cb, const void* data, unsigned eventCount, void* eventList, void* event);
        bool enqueueBufferWrite(ComputeBuffer* buffer, size_t offset, size_t size, void* data, bool blocking=true);
        bool enqueueBufferRead(ComputeBuffer* buffer, size_t offset, size_t size, void* ptr, bool blocking=true);
        bool enqueueKernel(ComputeKernel* kernel, unsigned workDimensions, const size_t* globalWorkOffset, const size_t* globalWorkSize, const size_t* localWorkSize);
        bool enqueueAcquireRendererSharedObject(unsigned count, ComputeBuffer** buffers);
        bool enqueueReleaseRendererSharedObject(unsigned count, ComputeBuffer** buffers);
        bool finish(void);

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

        bool                            compile(ComputeDevice* devices, unsigned deviceCount, const GYComputeProgramBuildOptions* options);
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


    class ComputeSystem: public System {
    public:

        const constexpr static int InvalidPlatform = ELYSIAN_COMPUTE_PLATFORM_INVALID;


        static const char*      _deviceTypeStrings[(int)ComputeDeviceCategory::HardwareDeviceCount];

        static const char*      getDeviceTypeString(ComputeDeviceCategory category);
        static bool             isRendererSharedContextDeviceCategory(ComputeDeviceCategory category);


        bool                    initialize(void);
        bool                    uninitialize(void);

        bool                    isComputeSupported(void) const;
        bool                    isComputeEnabled(void) const;
        void                    setComputeEnabled(bool enabled=true);


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


    private:

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
                                _deviceTypes[(int)ComputeDeviceCategory::HardwareDeviceCount];
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
        ComputeDeviceCategory               _deviceCategory = ComputeDeviceCategory::Invalid;


        bool                                _addCompiledKernel(ComputeKernel* kernel);
    public:
        void                                setGyDevice(GYComputeDevice dev) { _gyDevice = dev; _isValid = true; }
        void                                setDeviceType(ComputeDeviceCategory category) { _deviceCategory = category; }
        auto                                getDeviceType(void) const { return _deviceCategory; }
        void                                setContext(ComputeContext* ctx) { assert(ctx); _context = ctx; }
        const GYComputeDevice*              getGyDevice(void) const { return (_isValid)? &_gyDevice : nullptr; }
        ComputeContext*                     getContext(void) const;
        const GYComputeKernelWorkgroupInfo* getKernelWorkGroupInfo(const ComputeKernel* kernel) const;
        const GYComputeDeviceInfo*          getInfo(void) const;
        int                                 getPlatformIdx(void) const;
        int                                 getDeviceIdx(void) const;
    };


    //subclass in ESTk for perf monitoring shit
    class ComputeTask: public System {
    public:

        struct ProgramCreateInfo {
            const char* sourceFilePath                                      = nullptr;
            const char* buildFlags                                          = nullptr;
            const char* includeFilePaths[ELYSIAN_COMPUTE_HEADER_FILES_MAX]  = { nullptr };
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

    private:
        DeviceCategoryPriorityList  _ctegoryPriorityList;
        std::vector<ComputeDevice*> _devicePriorityList;
        NativeKernel            _nativeFallback;
        ComputeDevice*          _device                 = nullptr;
        ComputeProgram*         _program                = nullptr;
        ComputeKernel*          _kernel                 = nullptr;
        ComputeCommandQueue*    _cmdQueue               = nullptr;
        ComputeContext*         _context                = nullptr;
        bool                    _useNative              = false;
        bool                    _isValid                = false;

        bool                    _resolvePrioritizedDevices(bool requirerenderSharedContext);

    public:

                                ComputeTask(const char*                         kernelName,
                                            const ProgramCreateInfo&            progInfo,
                                            DeviceCategoryPriorityList          deviceCategoryPriorities,
                                            NativeKernel                        nativeFallbackFunc              = NativeKernel(),
                                            bool                                requireRendererSharedContext    = false,
                                            bool                                outOfOrderExecution             = false);

                                //IMPLEMENT MEEEEE
                                ~ComputeTask(void);

        int                     getPlatformIndex(void) const;
        ComputeDeviceCategory   getDeviceAtPriority(unsigned priority) const;
        ComputeDevice*          getDevice(void) const;
        ComputeProgram*         getProgram(void) const;
        ComputeKernel*          getKernel(void) const;
        ComputeCommandQueue*    getCommandQueue(void) const;
        ComputeContext*         getContext(void) const;
        NativeKernel            getNativeFallback(void) const;

        bool                    isValid(void) const; //can call kernel in any form
        bool                    isAccelerationAvailable(void) const;
        bool                    isAccelerated(void) const;
        void                    setAccelerated(bool val);
        void                    setNativeFallback(NativeKernel nativeFunc);

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


    //===== COMPUTE SYSTEM INLINEZ ==========
    inline ComputeDevice* ComputeSystem::getDevice(int platformIdx, int deviceIdx) {
        assert(platformIdx < (int)_platformCount && platformIdx >= 0 && deviceIdx < (int)_deviceCount[platformIdx] && deviceIdx >= 0);
        return &_devices[platformIdx][deviceIdx];
    }

    inline ComputeDevice* ComputeSystem::getDeviceByCategory(ComputeDeviceCategory type, unsigned idx) {
        assert((int)type >= 0 && (int)type < (int)ComputeDeviceCategory::HardwareDeviceCount);
        return (idx < _deviceTypes[(int)type].size())? _deviceTypes[(int)type][idx] : nullptr;
    }

    inline unsigned ComputeSystem::getDeviceCountForCategory(ComputeDeviceCategory type) const {
        assert((int)type >= 0 && (int)type < (int)ComputeDeviceCategory::HardwareDeviceCount);
        return _deviceTypes[(int)type].size();
    }

    inline ComputeContext* ComputeSystem::getPlatformContextFromDeviceCategory(ComputeDeviceCategory type, unsigned idx) const {
        assert((int)type >= 0 && (int)type < (int)ComputeDeviceCategory::HardwareDeviceCount);
        return getPlatformContext(_deviceTypes[(int)type][idx]->getPlatformIdx());
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


}

#endif // ELYSIAN_COMPUTE_SYSTEM_HPP

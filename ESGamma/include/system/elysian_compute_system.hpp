#ifndef ELYSIAN_COMPUTE_SYSTEM_HPP
#define ELYSIAN_COMPUTE_SYSTEM_HPP

#include <system/elysian_system.hpp>
#include <gyro_compute_api.h>
#include <gyro_platform.h>
#include <cstring>
#include <cassert>
#include <initializer_list>
#include <vector>

#if GYRO_PLATFORM != GYRO_PLATFORM_DC
#   define ELYSIAN_COMPUTE_CONTEXT_MAX_DEVICES 5
#else
#   define ELYSIAN_COMPUTE_CONTEXT_MAX_DEVICES 1
#endif

#define ELYSIAN_COMPUTE_CONTEXTS_PER_DEVICE     2
#define ELYSIAN_COMPUTE_PLATFORM_INVALID        -1
#define ELYSIAN_COMPUTE_DEVICE_INVALID          -1

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
        GYComputeProgram*           _gyProgram      = nullptr;
        ComputeContext*             _context        = nullptr;
        bool                        _compiled       = false;
        std::vector<ComputeDevice*> _compiledDevices;
        ComputeProgram(GYComputeProgram* program, ComputeContext* context);
    public:
       ~ComputeProgram(void);
        bool compile(ComputeDevice* devices, unsigned deviceCount, const GYComputeProgramBuildOptions* options);
        ComputeKernel* createKernel(const char* name) const;
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


    class ComputeSystem: public System {
    public:

        const constexpr static int InvalidPlatform = ELYSIAN_COMPUTE_PLATFORM_INVALID;

        enum DEVICE_TYPE {
            DEVICE_GPU_RENDERER_ONLINE,
            DEVICE_GPU_RENDERER_OFFLINE,
            DEVICE_GPU_AUXILIARY,
            DEVICE_CPU_RENDERER,
            DEVICE_CPU_AUXILIARY,
            DEVICE_ACCELERATOR,
            DEVICE_NATIVE,
            DEVICE_TYPE_COUNT
        };

        static const char*      _deviceTypeStrings[DEVICE_TYPE_COUNT];

        static const char*      getDeviceTypeString(DEVICE_TYPE type);

        bool                    initialize(void);
        bool                    uninitialize(void);

        bool                    isComputeSupported(void) const;
        bool                    isComputeEnabled(void) const;
        void                    setComputeEnabled(bool enabled=true);


        ComputeContext*         createContext(int gyPlatformIdx, bool isRendererContext, unsigned deviceCount, ComputeDevice** devices);
        ComputeDevice*          getDevice(int platformIdx, int deviceIdx);
        ComputeDevice*          getDeviceByType(DEVICE_TYPE type, unsigned idx = 0);
        unsigned                getDeviceCountForType(DEVICE_TYPE type) const;
        ComputeContext*         getPlatformContextFromDeviceType(DEVICE_TYPE type, unsigned idx = 0) const;
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
                                _deviceTypes[DEVICE_TYPE_COUNT];
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
        ComputeContext*                     _context = nullptr;
        bool                                _isValid    = false;
        ComputeSystem::DEVICE_TYPE          _deviceType = ComputeSystem::DEVICE_NATIVE;



        bool    _addCompiledKernel(ComputeKernel* kernel);
    public:
        void                                setGyDevice(GYComputeDevice dev) { _gyDevice = dev; _isValid = true; }
        void                                setDeviceType(ComputeSystem::DEVICE_TYPE type) { _deviceType = type; }
        auto                                getDeviceType(void) const { return _deviceType; }
        void                                setContext(ComputeContext* ctx) { assert(ctx); _context = ctx; }
        const GYComputeDevice*              getGyDevice(void) const { return (_isValid)? &_gyDevice : nullptr; }
        ComputeContext*                     getContext(void) const;
        const GYComputeKernelWorkgroupInfo* getKernelWorkGroupInfo(const ComputeKernel* kernel) const;
        const GYComputeDeviceInfo* getInfo(void) const;
        int getPlatformIdx(void) const;
        int getDeviceIdx(void) const;
    };



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

    inline ComputeDevice* ComputeSystem::getDeviceByType(DEVICE_TYPE type, unsigned idx) {
        assert(type >= 0 && type < DEVICE_TYPE_COUNT);
        return (idx < _deviceTypes[type].size())? _deviceTypes[type][idx] : nullptr;
    }

    inline unsigned ComputeSystem::getDeviceCountForType(DEVICE_TYPE type) const {
        assert(type >= 0 && type < DEVICE_TYPE_COUNT);
        return _deviceTypes[type].size();
    }

    inline ComputeContext* ComputeSystem::getPlatformContextFromDeviceType(DEVICE_TYPE type, unsigned idx) const {
        assert(type >= 0 && type < DEVICE_TYPE_COUNT && idx < _deviceTypes[type].size());
        return getPlatformContext(_deviceTypes[type][idx]->getPlatformIdx());
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
    inline bool ComputeSystem::isRendererInteropSupported(void) const { return getPlatformContextFromDeviceType(DEVICE_GPU_RENDERER_ONLINE); }
    inline unsigned ComputeSystem::getPlatformCount(void) const { return _platformCount; }


}

#endif // ELYSIAN_COMPUTE_SYSTEM_HPP

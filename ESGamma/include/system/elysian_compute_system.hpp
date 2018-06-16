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

        ComputeContext(GYComputeContext* ctx, unsigned gyPlatformIdx, bool isRendererContext, unsigned deviceCount, ComputeDevice* devices):
            _gyContext(ctx),
            _deviceCount(deviceCount),
            _gyPlatformIdx(gyPlatformIdx),
            _isRendererContext(isRendererContext)
        {
            assert(ctx && deviceCount && devices);
            memcpy(_devices, devices, sizeof(ComputeDevice*)*deviceCount);
        }

    public:
        const GYComputeContext* getGyContext(void) const { return _gyContext; }

        bool isRendererContext(void) const { return _isRendererContext; }

        ComputeCommandQueue*    createCommandQueue(ComputeDevice* device, int flags);
        ComputeBuffer*          createBuffer(GY_COMPUTE_MEMORY_TYPE memType, size_t size, void *data);
        ComputeBuffer*          createBufferFromRendererBuffer(intptr_t rendererBuffer, GY_COMPUTE_MEMORY_TYPE memType);
        ComputeProgram*         loadProgram(const char* filePath);



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
        ComputeContext*                     _context;
        bool                                _isValid    = false;

        bool    _addCompiledKernel(ComputeKernel* kernel);
    public:
        void setGyDevice(GYComputeDevice dev) { _gyDevice = dev; _isValid = true; }
        void setContext(ComputeContext* ctx) { assert(ctx); _context = ctx; }
        const GYComputeDevice* getGyDevice(void) const { return (_isValid)? &_gyDevice : nullptr; }
        ComputeContext* getContext(void) const;
        const GYComputeKernelWorkgroupInfo* getKernelWorkGroupInfo(const ComputeKernel* kernel) const;
        const GYComputeDeviceInfo* getInfo(void) const;
        int getPlatformIdx(void) const;
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
        enum COMPUTE_DEVICE_BUILTIN_TYPE {
            COMPUTE_DEVICE_GPU_RENDERER_ACTIVE,
            COMPUTE_DEVICE_GPU_RENDERER_INACTIVE,
            COMPUTE_DEVICE_GPU_AUXILIARY,
            COMPUTE_DEVICE_CPU,
            COMPUTE_DEVICE_BUILTIN_COUNT
        };

        static const char*      _computeDeviceTypeStrings[COMPUTE_DEVICE_BUILTIN_COUNT];

        bool                    initialize(void);
        bool                    uninitialize(void);

        ComputeDevice*          getBuiltinComputeDevice(COMPUTE_DEVICE_BUILTIN_TYPE type);
    private:

        void                    _setBuiltinComputeDevice(COMPUTE_DEVICE_BUILTIN_TYPE type, ComputeDevice* dev);


        //GYThread*				_cpuThreads;

        unsigned                _platformCount      = 0;
        unsigned*               _deviceCount        = nullptr;
        int                     _rendererPlatform   = -1;
        ComputeContext**        _contexts           = nullptr;
        ComputeDevice**			_devices;
        ComputeDevice*          _builtinDevices[COMPUTE_DEVICE_BUILTIN_COUNT] = { nullptr };
    };




    inline ComputeDevice* ComputeSystem::getBuiltinComputeDevice(COMPUTE_DEVICE_BUILTIN_TYPE type) {
        assert(type >= 0 && type <= COMPUTE_DEVICE_BUILTIN_COUNT);
        return _builtinDevices[type];
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
}

#endif // ELYSIAN_COMPUTE_SYSTEM_HPP

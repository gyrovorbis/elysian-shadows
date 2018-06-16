#include <system/elysian_compute_system.hpp>
#include <gyro_compute_api.h>
#include <crtp/elysian_debug.hpp>

namespace elysian {

const char* ComputeSystem::_computeDeviceTypeStrings[ComputeSystem::COMPUTE_DEVICE_BUILTIN_COUNT] = {
   "COMPUTE_DEVICE_GPU_RENDERER_ACTIVE",
   "COMPUTE_DEVICE_GPU_RENDERER_INACTIVE",
   "COMPUTE_DEVICE_GPU_AUXILIARY",
   "COMPUTE_DEVICE_CPU"
};

bool ComputeSystem::initialize(void) {
    bool success = true;
    unsigned deviceCount = 0;
    GYComputeInitParams params = {
        4
    };

    dbgprintf("ComputeSystem::initialize()");
    dbgpush();

    if(!gyComputeInit(&params)) {
        errprintf("Failed to initialize ComputeSystem!");
        success = false;
        goto done;
    }

    dbgprintf("Determining hardware configuration.");
    dbgpush();
    _platformCount = gyComputePlatformCount();
    if(!_platformCount) {
        warnprintf("No Compute Platforms found!");
        success = false;
        goto done;
    }
    _devices = new ComputeDevice*[_platformCount];
    _deviceCount = new unsigned[_platformCount];
    _contexts = new ComputeContext*[_platformCount];
    memset(_deviceCount, 0, sizeof(unsigned)*_platformCount);
    memset(_contexts, 0, sizeof(ComputeContext*)*_platformCount);

    for(unsigned p = 0; p < _platformCount; ++p) {
        dbgprintf("Examining platform %u", p);
        dbgpush();

        const GYComputePlatformInfo* info = gyComputePlatformQueryInfo(p);
        GY_ASSERT(info);
        if(info->isRenderPlatform) _rendererPlatform = p;

        _devices[p] = new ComputeDevice[info->deviceCount];
        _deviceCount[p] = info->deviceCount;

        for(unsigned d = 0; d < info->deviceCount; ++d) {
            dbgpush("Examining Device %u", d);
            dbgpush();
            GYComputeDevice dev = gyComputeDeviceHandle(p, d);
            _devices[p][d].setGyDevice(dev);
            const GYComputeDeviceInfo* devInfo = _devices[p][d].getInfo();

            if(_rendererPlatform == (int)p) {
                if(devInfo->devType == GY_COMPUTE_DEVICE_GPU) {
                    if(devInfo->isRenderDevice) _setBuiltinComputeDevice(COMPUTE_DEVICE_GPU_RENDERER_ACTIVE, &_devices[p][d]);
                    else _setBuiltinComputeDevice(COMPUTE_DEVICE_GPU_RENDERER_INACTIVE, &_devices[p][d]);
                } else if(devInfo->devType == GY_COMPUTE_DEVICE_CPU) {
                    _setBuiltinComputeDevice(COMPUTE_DEVICE_CPU, &_devices[p][d]);
                } else {
                    dbgprintf("Unknown device type found at Platform[%u], Device[%u]: Type [%u]", p, d, devInfo->devType);
                }
            } else {
                if(devInfo->devType == GY_COMPUTE_DEVICE_GPU) {
                    _setBuiltinComputeDevice(COMPUTE_DEVICE_GPU_AUXILIARY, &_devices[p][d]);
                } else if(devInfo->devType == GY_COMPUTE_DEVICE_CPU) {
                    _setBuiltinComputeDevice(COMPUTE_DEVICE_CPU, &_devices[p][d]);
                } else {
                    dbgprintf("Unknown device type found at Platform[%u], Device[%u]: Type [%u]", p, d, devInfo->devType);
                }
            }
            dbgpop();
        }
        dbgpop();
    }

    dbgpop();

    //Create contexts
    dbgprintf("Creating contexts.");
    dbgpush();

    GYComputeDevice gyDevices[ELYSIAN_COMPUTE_CONTEXT_MAX_DEVICES];
    for(unsigned p = 0; p < _platformCount; ++p) {
        deviceCount = 0;
        for(unsigned d = 0; d < _deviceCount[p]; ++d) {

            if((int)p == _rendererPlatform) {
                if(!_devices[p][d].getInfo()->rendererSharedContext) continue;
            }

            gyDevices[deviceCount++] = *_devices[p][d].getGyDevice();
        }

        GYComputeContextInitParams initParams = {
            0,
            gyDevices,
            deviceCount
        };

        if((int)p == _rendererPlatform) initParams.flags |= GY_COMPUTE_CONTEXT_FLAG_RENDERER_SHARED;

        GYComputeContext* context = gyComputeContextCreate(p, &initParams);
        if(context) {
            ComputeContext* ctx = new ComputeContext(context, p, (int)p == _rendererPlatform, _deviceCount[p], _devices[p]);
            _contexts[p] = ctx;
            for(unsigned d = 0; d < _deviceCount[p]; ++d) {
                _devices[p][d].setContext(ctx);
            }
        }
    }
    dbgpop();

done:
    dbgpop();
    return success;

    //Create Command Queues

}

bool ComputeSystem::uninitialize(void) {
    bool success = true;
    dbgprintf("ComputeSystem::uninitialize()");
    dbgpush();

    for(unsigned p = 0; p < _platformCount; ++p) {
        delete[] _devices[p];
        success &= gyComputeContextDestroy(_contexts[p]->_gyContext);
    }
    delete[] _contexts;
    delete[] _devices;
    delete[] _deviceCount;

    success &= gyComputeUninit();

    dbgpop();

    return success;
}

void ComputeSystem::_setBuiltinComputeDevice(COMPUTE_DEVICE_BUILTIN_TYPE type, ComputeDevice* dev) {
    dbgprintf("_setBuiltinComputeDevice(%s, <%u, %u>)", _computeDeviceTypeStrings[type], dev->getGyDevice()->platformIdx, dev->getPlatformIdx());
    GY_ASSERT(dev && dev->getGyDevice());
    GY_ASSERT(type >= 0 && type < COMPUTE_DEVICE_BUILTIN_COUNT);
    if(_builtinDevices[type]) {
        if(dev->getPlatformIdx() == _rendererPlatform) {
            warnprintf("Reassigning %s from <%u, %u> to <%u, %u>, to share a context with the renderer platform: %u.",
                       _computeDeviceTypeStrings[type], _builtinDevices[type]->getGyDevice()->platformIdx,
                       _builtinDevices[type]->getGyDevice()->deviceIdx, dev->getGyDevice()->platformIdx, dev->getGyDevice()->deviceIdx);
        } else {

           // warnprintf("%s already exists as <%u, %u>, reassigning to <%u, %u>.", _computeDeviceTypeStrings[type], _builtinDevices[type]->getGyDevice()->platformIdx, _builtinDevices[type]->getGyDevice()->deviceIdx, dev->getGyDevice()->platformIdx, dev->getGyDevice()->deviceIdx);
        }
    } else {
        dbgprintf("%s assigned to device at <%u, %u>.", _computeDeviceTypeStrings[type], dev->getGyDevice()->platformIdx, dev->getGyDevice()->deviceIdx);
    }

    _builtinDevices[type] = dev;
}

ComputeCommandQueue* ComputeContext::createCommandQueue(ComputeDevice* device, int flags) {
    GYComputeCommandQueue* queue = gyComputeCommandQueueCreate(_gyContext, device->getGyDevice(), flags);
    if(queue) {
        return new ComputeCommandQueue(queue, device);
    } else {
        return nullptr;
    }
}

ComputeBuffer* ComputeContext::createBuffer(GY_COMPUTE_MEMORY_TYPE memType, size_t size, void *data) {
    GYComputeBuffer* buffer = gyComputeBufferCreate(_gyContext, memType, size, data);
    if(buffer) {
        return new ComputeBuffer(buffer);
    } else {
        return nullptr;
    }
}

ComputeBuffer* ComputeContext::createBufferFromRendererBuffer(intptr_t rendererBuffer, GY_COMPUTE_MEMORY_TYPE memType) {
    GYComputeBuffer* gyBuff = gyComputeBufferCreateFromRendererBuffer(_gyContext, memType, rendererBuffer);
    if(gyBuff) {
        return new ComputeBuffer(gyBuff);
    } else {
        return nullptr;
    }
}

ComputeProgram* ComputeContext::loadProgram(const char *filePath) {
    assert(filePath);
    GYComputeProgram* program = gyComputeProgramLoad(filePath, _gyContext);
    if(program) {
        return new ComputeProgram(program, this);
    } else {
        return nullptr;
    }
}

ComputeKernel* ComputeProgram::createKernel(const char* name) const {
    assert(name);
    if(!_compiled) return nullptr;

    GYComputeKernel* kern = gyComputeKernelCreate(_gyProgram, name, nullptr);
    if(kern) {
        ComputeKernel* k = new ComputeKernel(kern);
        for(unsigned d = 0; d < _compiledDevices.size(); ++d)
            _compiledDevices[d]->_addCompiledKernel(k);
        return k;
    } else {
        return nullptr;
    }

}

ComputeCommandQueue::~ComputeCommandQueue(void) {
    gyComputeCommandQueueDestroy(_gyCmdQueue);
}

ComputeBuffer::~ComputeBuffer(void) {
    gyComputeBufferDestroy(_gyBuffer);
}

ComputeProgram::~ComputeProgram(void) {
    gyComputeProgramUnload(_gyProgram);
}

ComputeKernel::~ComputeKernel(void) {
    gyComputeKernelDestroy(_gyKernel);
}

ComputeDevice::CompiledKernel::CompiledKernel(ComputeKernel *_kernel):
    kernel(_kernel){}

bool ComputeDevice::_addCompiledKernel(ComputeKernel *kernel) {
    bool success = true;
    assert(kernel);
    CompiledKernel compKernel(kernel);
    success &= gyComputeKernelQueryWorkgroupInfo(kernel->getGyKernel(), &_gyDevice, &compKernel.workgroupInfo);
    _compiledKernels.push_back(compKernel);
    return success;
}

const GYComputeKernelWorkgroupInfo* ComputeDevice::getKernelWorkGroupInfo(const ComputeKernel *kernel) const {
    for(unsigned i = 0; i < _compiledKernels.size(); ++i) {
        if(_compiledKernels[i].kernel == kernel) {
            return &_compiledKernels[i].workgroupInfo;
        }
    }
    return nullptr;
}

bool ComputeProgram::compile(ComputeDevice* devices, unsigned deviceCount, const GYComputeProgramBuildOptions* options) {
    _compiled = gyComputeProgramCompile(_gyProgram, devices[0].getGyDevice(), deviceCount, options);
    if(_compiled) {
        for(unsigned d = 0; d < deviceCount; ++d)
            _compiledDevices.push_back(&devices[d]);
    }
    return _compiled;
}

bool ComputeKernel::setArg(unsigned index, size_t size, void *data) const {
    assert(data);
    return gyComputeKernelArg(_gyKernel, index, size, data);
}

bool ComputeKernel::setArg(unsigned index, const ComputeBuffer *buffer) const {
    assert(buffer);
    return gyComputeKernelArgBuffer(_gyKernel, index, buffer->getGyBuffer());
}

const GYComputeKernelInfo* ComputeKernel::getInfo(void) const {
    return gyComputeKernelQueryInfo(_gyKernel);
}

bool ComputeCommandQueue::enqueueBufferRead(ComputeBuffer *buffer, size_t offset, size_t size, void *ptr, bool blocking) {
    assert(buffer && ptr);
    if(!size) return 1;
    return gyComputeCommandQueueEnqueueBufferRead(_gyCmdQueue, buffer->getGyBuffer(), blocking, offset, size, ptr, 0, nullptr, nullptr);
}

bool ComputeCommandQueue::enqueueBufferWrite(ComputeBuffer *buffer, size_t offset, size_t size, void *data, bool blocking) {
    assert(buffer && size && data);
    return gyComputeCommandQueueEnqueueBufferWrite(_gyCmdQueue, buffer->getGyBuffer(), blocking, offset, size, data, 0, nullptr, nullptr);
}

bool ComputeCommandQueue::enqueueKernel(ComputeKernel *kernel, unsigned workDimensions, const size_t *globalWorkOffset, const size_t *globalWorkSize, const size_t *localWorkSize) {
    assert(kernel && workDimensions && globalWorkOffset && globalWorkSize && localWorkSize);
    return gyComputeCommandQueueEnqueueKernel(_gyCmdQueue, kernel->getGyKernel(), workDimensions, globalWorkOffset, globalWorkSize, localWorkSize, 0, nullptr, nullptr);
}

bool ComputeCommandQueue::enqueueAcquireRendererSharedObject(unsigned count, ComputeBuffer **buffers) {
    assert(count > 0 && count < ELYSIAN_COMPUTE_CONTEXT_MAX_DEVICES && buffers);
    GYComputeBuffer* gyBuffers[ELYSIAN_COMPUTE_CONTEXT_MAX_DEVICES];
    for(unsigned i = 0; i < count; ++i) {
        GY_ASSERT(buffers[i]);
        gyBuffers[i] = buffers[i]->getGyBuffer();
    }
    return gyComputeCommandQueueEnqueueAcquireRendererObject(_gyCmdQueue, count, (const GYComputeBuffer**)gyBuffers);
}

bool ComputeCommandQueue::enqueueReleaseRendererSharedObject(unsigned count, ComputeBuffer **buffers) {
    assert(count > 0 && count < ELYSIAN_COMPUTE_CONTEXT_MAX_DEVICES && buffers);
    GYComputeBuffer* gyBuffers[ELYSIAN_COMPUTE_CONTEXT_MAX_DEVICES];
    for(unsigned i = 0; i < count; ++i) {
        gyBuffers[i] = buffers[i]->getGyBuffer();
    }
    return gyComputeCommandQueueEnqueueReleaseRendererObject(_gyCmdQueue, count, (const GYComputeBuffer**)gyBuffers);
}

//acquire/release GL SHIT

bool ComputeCommandQueue::finish(void) {
    return gyComputeCommandQueueFinish(_gyCmdQueue);
}

}

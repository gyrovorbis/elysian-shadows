#include <system/elysian_compute_system.hpp>
#include <gyro_compute_api.h>
#include <crtp/elysian_debug.hpp>

namespace elysian {

const char* ComputeSystem::_deviceTypeStrings[ComputeSystem::DEVICE_TYPE_COUNT] = {
    "DEVICE_GPU_RENDERER_ONLINE",
    "DEVICE_GPU_RENDERER_OFFLINE",
    "DEVICE_GPU_AUXILIARY",
    "DEVICE_CPU_RENDERER",
    "DEVICE_CPU_AUXILIARY",
    "DEVICE_ACCELERATOR"
};

const char* ComputeSystem::getDeviceTypeString(DEVICE_TYPE type) {
    assert(type >= 0 && type < DEVICE_TYPE_COUNT);
    return _deviceTypeStrings[type];
}

bool ComputeSystem::_assignRendererPlatform(void) {
    bool success = false;
    dbgprintf("ComputeSystem: Assigning Renderer Platform");
    dbgpush();

    for(unsigned p = 0; p < _platformCount; ++p) {
        const GYComputePlatformInfo* platInfo = gyComputePlatformQueryInfo(p);
        if(platInfo->isRenderPlatform) {
            dbgprintf("RENDERER_PLATFORM [%u]: %s", p, platInfo->platformName);
            _rendererPlatformIdx = p;
            success = true;
            break;
        }
    }

    dbgpop();
    return success;

}

bool ComputeSystem::_sortDeviceTypesByPerformance(void) {
    bool success = true;
    //Implement me later!
    return success;
}


bool ComputeSystem::_assignDeviceTypes(void) {
    bool success = true;
    dbgprintf("ComputeSystem: Assigning Device Types");
    dbgpush();

    /*1 - Find GPU_RENDERER_ONLINE type(s):
     *  a. main GPU associated with rendering the scene
     *  b. must support interop between graphics and compute APIs.
     */
    if(_rendererPlatformIdx != InvalidPlatform) {
        for(unsigned d = 0; d < getPlatformDeviceCount(_rendererPlatformIdx); ++d) {
            ComputeDevice* dev = getDevice(_rendererPlatformIdx, d);
            const GYComputeDeviceInfo* devInfo = dev->getInfo();

            if(devInfo->devType == GY_COMPUTE_DEVICE_GPU) {
                if(devInfo->isRenderDevice) {
                    if(devInfo->rendererSharedContext) {
                        _deviceTypes[DEVICE_GPU_RENDERER_ONLINE].push_back(dev);
                        dev->setDeviceType(DEVICE_GPU_RENDERER_ONLINE);
                        size_t size = _deviceTypes[DEVICE_GPU_RENDERER_ONLINE].size();
                        Debug::Logf((size == 1)? Debug::LOG_VERBOSE : Debug::LOG_WARNING,
                                    "GPU_RENDERER_ONLINE[%u]: <%u, %u> [%s]", size-1, dev->getPlatformIdx(), dev->getDeviceIdx(), devInfo->deviceName);
                    }
                }
            }
        }
    }

    /* 2 - Find GPU_RENDERER_OFFLINE type(s)
     *  a. requires an online renderer to have been assigned previously
     *  b. located within the same driver platform4
     *  c. supports graphics/compute API interop
     */
    if(!_deviceTypes[DEVICE_GPU_RENDERER_ONLINE].size()) {
        dbgprintf("GPU_RENDERER_ONLINE Device(s): NONE");
    } else {

        for(unsigned d = 0; d < getPlatformDeviceCount(_rendererPlatformIdx); ++d) {
            ComputeDevice* dev = getDevice(_rendererPlatformIdx, d);
            const GYComputeDeviceInfo* devInfo = dev->getInfo();

            if(devInfo->devType == GY_COMPUTE_DEVICE_GPU) {
                if(devInfo->rendererSharedContext) {
                    if(!devInfo->isRenderDevice) {
                        _deviceTypes[DEVICE_GPU_RENDERER_OFFLINE].push_back(dev);
                        dev->setDeviceType(DEVICE_GPU_RENDERER_OFFLINE);
                        size_t size = _deviceTypes[DEVICE_GPU_RENDERER_ONLINE].size();
                        Debug::Logf(Debug::LOG_VERBOSE, "GPU_RENDERER_OFFLINE[%u]: <%u, %u> [%s]", size-1, dev->getPlatformIdx(), dev->getDeviceIdx(), devInfo->deviceName);
                    }
                }
            }
        }
    }

    if(!_deviceTypes[DEVICE_GPU_RENDERER_OFFLINE].size()) {
        dbgprintf("GPU_RENDERER_OFFLINE Devices: NONE");
    }

    /*3 - Find GPU_AUXILIARY type(s) (any remaining GPUs)
     *  a. GPUs which are not located within the same platform as the GPU_RENDERER_ONLINE/OFFLINE devices
        b. GPUs which are not capable of context sharing between graphics and compute APIs.
    */
    for(unsigned p = 0; p < getPlatformCount(); ++p) {
        for(unsigned d = 0; d < getPlatformDeviceCount(p); ++d) {
            ComputeDevice* dev = getDevice(p, d);
            const GYComputeDeviceInfo* devInfo = dev->getInfo();
             if(devInfo->devType == GY_COMPUTE_DEVICE_GPU) {
                 if((int)p != _rendererPlatformIdx || !devInfo->rendererSharedContext || !_deviceTypes[DEVICE_GPU_RENDERER_ONLINE].size()) {
                    _deviceTypes[DEVICE_GPU_AUXILIARY].push_back(dev);
                    dev->setDeviceType(DEVICE_GPU_AUXILIARY);
                    size_t size = _deviceTypes[DEVICE_GPU_AUXILIARY].size();
                    Debug::Logf(Debug::LOG_VERBOSE, "GPU_AUXILIARY[%u]: <%u, %u> [%s]", size-1, dev->getPlatformIdx(), dev->getDeviceIdx(), devInfo->deviceName);
                 }
             }
        }
    }

    if(!_deviceTypes[DEVICE_GPU_AUXILIARY].size()) {
        dbgprintf("GPU_AUXILIARY Devices: NONE");
    }

    /*4 - Find CPU_RENDERER type(s)
     *  a. CPU device located within the same platform as the GPU_RENDERER_ONLINE/OFFLINE devices
     *  b. supports graphics/compute API interop with the GPUs
    */
    if(_deviceTypes[DEVICE_GPU_RENDERER_ONLINE].size()) {
        for(unsigned d = 0; d < getPlatformDeviceCount(_rendererPlatformIdx); ++d) {
            ComputeDevice* dev = getDevice(_rendererPlatformIdx, d);
            const GYComputeDeviceInfo* devInfo = dev->getInfo();

            if(devInfo->devType == GY_COMPUTE_DEVICE_CPU) {
                if(devInfo->rendererSharedContext) {
                    _deviceTypes[DEVICE_CPU_RENDERER].push_back(dev);
                    dev->setDeviceType(DEVICE_CPU_RENDERER);
                    size_t size = _deviceTypes[DEVICE_CPU_RENDERER].size();
                    Debug::Logf(Debug::LOG_VERBOSE, "CPU_RENDERER[%u]: <%u, %u> [%s]", size-1, dev->getPlatformIdx(), dev->getDeviceIdx(), devInfo->deviceName);
                }
            }
        }
    }

    if(!_deviceTypes[DEVICE_CPU_RENDERER].size()) {
        dbgprintf("CPU_RENDERER Device(s): NONE");
    }

    /*5 - Find CPU_AUXILIARY type(s) (any remaining CPUs)
     *  a. CPU(s) which are not located within the same platform as the GPU_RENDERER_ONLINE/OFFLINE devices
     *  b. CPU(s) which are not capable of context sharing between graphics and compute APIs.
    */
    for(unsigned p = 0; p < getPlatformCount(); ++p) {
        for(unsigned d = 0; d < getPlatformDeviceCount(p); ++d) {
            ComputeDevice* dev = getDevice(p, d);
            const GYComputeDeviceInfo* devInfo = dev->getInfo();

             if(devInfo->devType == GY_COMPUTE_DEVICE_CPU) {

                 if(((int)p != _rendererPlatformIdx && !devInfo->rendererSharedContext) || !_deviceTypes[DEVICE_CPU_RENDERER].size()) {
                     _deviceTypes[DEVICE_CPU_AUXILIARY].push_back(dev);
                     size_t size = _deviceTypes[DEVICE_CPU_AUXILIARY].size();
                     dev->setDeviceType(DEVICE_CPU_AUXILIARY);
                     Debug::Logf(Debug::LOG_VERBOSE, "CPU_AUXILIARY[%u]: <%u, %u> [%s]", size-1, dev->getPlatformIdx(), dev->getDeviceIdx(), devInfo->deviceName);
                 }
             }
        }
    }

    if(!_deviceTypes[DEVICE_CPU_AUXILIARY].size()) {
        dbgprintf("CPU_AUXILIARY Device(s): NONE");
    }

    /*6 - Find ACCELERATOR type(s) (any remaining devices)
     *  a. Devices which are neither a GPU or CPU (DSPs, FGPAs, etc.)
     *  b. Devices are assumed to not support context sharing between graphics and compute APIs.
    */

    for(unsigned p = 0; p < getPlatformCount(); ++p) {
        for(unsigned d = 0; d < getPlatformDeviceCount(p); ++d) {
            ComputeDevice* dev = getDevice(p, d);
            const GYComputeDeviceInfo* devInfo = dev->getInfo();

             if(devInfo->devType != GY_COMPUTE_DEVICE_GPU && devInfo->devType != GY_COMPUTE_DEVICE_CPU) {
                 _deviceTypes[DEVICE_ACCELERATOR].push_back(dev);
                 dev->setDeviceType(DEVICE_ACCELERATOR);
                 size_t size = _deviceTypes[DEVICE_ACCELERATOR].size();
                 Debug::Logf(Debug::LOG_VERBOSE, "ACCELERATOR[%u]: <%u, %u> [%s]", size-1, dev->getPlatformIdx(), dev->getDeviceIdx(), devInfo->deviceName);
             }
        }
    }

    if(!_deviceTypes[DEVICE_ACCELERATOR].size()) {
        dbgprintf("ACCELERATOR Device(s): NONE");
    }

    //Now check whether we even have a single fucking compute device/accelerator and don't bother even trying otherwise.
    unsigned computeDevCount = 0;
    for(unsigned t = 0; t < DEVICE_TYPE_COUNT; ++t)
        computeDevCount += _deviceTypes[t].size();

    if(!computeDevCount) {
        warnprintf("No compute devices found. Disabling compute acceleration!");
        _enabled = false;
        success = false;
    } else {

        unsigned totalDevCount = 0;
        for(unsigned p = 0; p < getPlatformCount(); ++p) {
            totalDevCount += getPlatformDeviceCount(p);
        }

        dbgprintf("Successfully categorized (%u/%u) Compute Devices.", computeDevCount, totalDevCount);
    }

    dbgpop();
    return success;
}

bool ComputeSystem::_createContexts(void) {
    bool success = true;

    dbgprintf("ComputeSystem: Creating Contexts");
    dbgpush();


    ComputeDevice* rendererDevice = getDeviceByType(DEVICE_GPU_RENDERER_ONLINE);
    if(rendererDevice) {
        dbgprintf("Creating Renderer Context.");
        dbgpush();
        std::vector<ComputeDevice*> devices;

        devices.push_back(rendererDevice);

        for(unsigned d = 0; d < getDeviceCountForType(DEVICE_GPU_RENDERER_OFFLINE); ++d) {
            devices.push_back(getDeviceByType(DEVICE_GPU_RENDERER_OFFLINE, d));
        }

        for(unsigned d = 0; d < getDeviceCountForType(DEVICE_CPU_RENDERER); ++d) {
            devices.push_back(getDeviceByType(DEVICE_CPU_RENDERER, d));
        }

        _rendererContext = createContext(_rendererPlatformIdx, true, devices.size(), devices.data());

        if(!_rendererContext) {
            errprintf("Could not create a compute/shared renderer context! Disabling renderer interop!");
            success = false;
        }

        dbgpop();
    } else {
        dbgprintf("Skipping Renderer Context.");
    }

    _platformContexts = new ComputeContext*[getPlatformCount()];
    memset(_platformContexts, 0, sizeof(ComputeContext*)*getPlatformCount());

    for(unsigned p = 0; p < getPlatformCount(); ++p) {
        dbgprintf("Creating Compute Context for Platform: [%u]", p);
        dbgpush();

        std::vector<ComputeDevice*> devices;
        for(unsigned d = 0; d < getPlatformDeviceCount(p); ++d) {
            devices.push_back(getDevice(p, d));
        }

        if(devices.size()) {
            _platformContexts[p] = createContext(p, false, devices.size(), devices.data());

            if(!_platformContexts[p]) {
                errprintf("Failed to create Compute Context!");
                success = false;
            }
        }

        dbgpop();
    }

    //Check whether there are any valid contexts
    if(!_rendererContext) {
        unsigned platContextCount = 0;
        for(unsigned p = 0; p < getPlatformCount(); ++p) {
            if(getPlatformContext(p)) ++platContextCount;
        }

        if(!platContextCount) {
            errprintf("No valid Compute Contexts were created! Disabling Compute Acceleration!");
            _enabled = false;
            success = false;
        }
    }


    dbgpop();
    return success;
}

bool ComputeSystem::initialize(void) {
    bool success = true;
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
    memset(_deviceCount, 0, sizeof(unsigned)*_platformCount);

    for(unsigned p = 0; p < _platformCount; ++p) {
        const GYComputePlatformInfo* info = gyComputePlatformQueryInfo(p);
        GY_ASSERT(info);

        _devices[p] = new ComputeDevice[info->deviceCount];
        _deviceCount[p] = info->deviceCount;

        for(unsigned d = 0; d < info->deviceCount; ++d) {
            GYComputeDevice dev = gyComputeDeviceHandle(p, d);
            _devices[p][d].setGyDevice(dev);
        }
    }

    success &= _assignRendererPlatform();
    success &= _assignDeviceTypes();
    if(success) {
        success &= _sortDeviceTypesByPerformance();

        if(success) success &= _createContexts();
    }


#if 0
    //Create contexts
    dbgprintf("Creating contexts.");

        _platformContexts = new ComputeContext*[_platformCount];
    dbgpush();

    GYComputeDevice gyDevices[ELYSIAN_COMPUTE_CONTEXT_MAX_DEVICES];
    for(unsigned p = 0; p < _platformCount; ++p) {
        renderPlatform = (p == _rendererPlatform && gyComputePlatformQueryInfo(_rendererPlatform)->rendererContextSharing);
        deviceCount = 0;
        for(unsigned d = 0; d < _deviceCount[p]; ++d) {

            if(renderPlatform) {
                if(!_devices[p][d].getInfo()->rendererSharedContext) continue;
            }

            gyDevices[deviceCount++] = *_devices[p][d].getGyDevice();
        }

        GYComputeContextInitParams initParams = {
            0,
            gyDevices,
            deviceCount
        };

        if(renderPlatform) initParams.flags |= GY_COMPUTE_CONTEXT_FLAG_RENDERER_SHARED;

        GYComputeContext* context = gyComputeContextCreate(p, &initParams);
        if(context) {
            ComputeContext* ctx = new ComputeContext(context, p, renderPlatform, deviceCount, gyDevices);
            _contexts[p] = ctx;
            for(unsigned d = 0; d < _deviceCount[p]; ++d) {
                _devices[p][d].setContext(ctx);
            }
        } else goto done;
    }
    dbgpop();

    _initialized = true;
#endif

done:
    _initialized = success;
    dbgpop();
    return success;

    //Create Command Queues

}

bool ComputeSystem::uninitialize(void) {
    bool success = true;
    dbgprintf("ComputeSystem::uninitialize()");
    dbgpush();

    if(_rendererContext) {
        gyComputeContextDestroy(_rendererContext->getGyContext());
        delete _rendererContext;
    }

    for(unsigned p = 0; p < _platformCount; ++p) {
        gyComputeContextDestroy(_platformContexts[p]->getGyContext());
        delete[] _devices[p];
    }
    delete[] _devices;
    delete[] _deviceCount;
    delete[] _platformContexts;

    success &= gyComputeUninit();

    dbgpop();

    return success;
}

ComputeContext* ComputeSystem::createContext(int gyPlatformIdx, bool isRendererContext, unsigned deviceCount, ComputeDevice **devices) {
    ComputeContext* ctx = nullptr;

    dbgprintf("ComputeSystem::createContext(platformIdx = %d, rendererContext = %d, deviceCount = %u)", gyPlatformIdx, isRendererContext, deviceCount);
    dbgpush();
    assert(gyPlatformIdx >= 0 && gyPlatformIdx < (int)_platformCount && deviceCount && devices);

    std::vector<GYComputeDevice> _gyDevices;
    for(unsigned d = 0; d < deviceCount; ++d) {
        assert(devices[d]);
        _gyDevices.push_back(*devices[d]->getGyDevice());
        dbgprintf("device[%u]: <%u, %u> [%s] (%s)", d, devices[d]->getPlatformIdx(), devices[d]->getDeviceIdx(),
                  getDeviceTypeString(devices[d]->getDeviceType()), devices[d]->getInfo()->deviceName);
    }

    GYComputeContextInitParams params = {
        isRendererContext? GY_COMPUTE_CONTEXT_FLAG_RENDERER_SHARED : 0,
        _gyDevices.data(),
        (unsigned)_gyDevices.size()
    };

    GYComputeContext* gyCtx = gyComputeContextCreate(gyPlatformIdx, &params);


    if(!gyCtx) {
        errprintf("Failed to create ComputeContext!");
    } else {
        ctx = new ComputeContext(gyCtx, gyPlatformIdx, isRendererContext, deviceCount, devices);

        for(unsigned d = 0; d < deviceCount; ++d) devices[d]->setContext(ctx);
    }

    dbgpop();
    return ctx;
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

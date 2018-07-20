#include <system/elysian_compute_system.hpp>
#include <gyro_compute_api.h>
#include <crtp/elysian_debug.hpp>

namespace elysian {

const char* ComputeSystem::_deviceCategoryStrings[(int)ComputeDeviceCategory::HardwareDeviceCount] = {
    "DEVICE_GPU_RENDERER_ONLINE",
    "DEVICE_GPU_RENDERER_OFFLINE",
    "DEVICE_GPU_AUXILIARY",
    "DEVICE_CPU_RENDERER",
    "DEVICE_CPU_AUXILIARY",
    "DEVICE_ACCELERATOR"
};

const char* ComputeSystem::getDeviceTypeString(ComputeDeviceCategory type) {
    assert((int)type >= 0 && (int)type < (int)ComputeDeviceCategory::HardwareDeviceCount);
    return _deviceCategoryStrings[(int)type];
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
                        _categorizedDevices[(int)ComputeDeviceCategory::GpuRendererOnline].push_back(dev);
                        dev->setDeviceCategory(ComputeDeviceCategory::GpuRendererOnline);
                        size_t size = _categorizedDevices[(int)ComputeDeviceCategory::GpuRendererOnline].size();
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
    if(!_categorizedDevices[(int)ComputeDeviceCategory::GpuRendererOnline].size()) {
        dbgprintf("GPU_RENDERER_ONLINE Device(s): NONE");
    } else {

        for(unsigned d = 0; d < getPlatformDeviceCount(_rendererPlatformIdx); ++d) {
            ComputeDevice* dev = getDevice(_rendererPlatformIdx, d);
            const GYComputeDeviceInfo* devInfo = dev->getInfo();

            if(devInfo->devType == GY_COMPUTE_DEVICE_GPU) {
                if(devInfo->rendererSharedContext) {
                    if(!devInfo->isRenderDevice) {
                        _categorizedDevices[(int)ComputeDeviceCategory::GpuRendererOffline].push_back(dev);
                        dev->setDeviceCategory(ComputeDeviceCategory::GpuRendererOffline);
                        size_t size = _categorizedDevices[(int)ComputeDeviceCategory::GpuRendererOffline].size();
                        Debug::Logf(Debug::LOG_VERBOSE, "GPU_RENDERER_OFFLINE[%u]: <%u, %u> [%s]", size-1, dev->getPlatformIdx(), dev->getDeviceIdx(), devInfo->deviceName);
                    }
                }
            }
        }
    }

    if(!_categorizedDevices[(int)ComputeDeviceCategory::GpuRendererOffline].size()) {
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
                 if((int)p != _rendererPlatformIdx || !devInfo->rendererSharedContext || !_categorizedDevices[(int)ComputeDeviceCategory::GpuRendererOnline].size()) {
                    _categorizedDevices[(int)ComputeDeviceCategory::GpuAuxiliary].push_back(dev);
                    dev->setDeviceCategory(ComputeDeviceCategory::GpuAuxiliary);
                    size_t size = _categorizedDevices[(int)ComputeDeviceCategory::GpuAuxiliary].size();
                    Debug::Logf(Debug::LOG_VERBOSE, "GPU_AUXILIARY[%u]: <%u, %u> [%s]", size-1, dev->getPlatformIdx(), dev->getDeviceIdx(), devInfo->deviceName);
                 }
             }
        }
    }

    if(!_categorizedDevices[(int)ComputeDeviceCategory::GpuAuxiliary].size()) {
        dbgprintf("GPU_AUXILIARY Devices: NONE");
    }

    /*4 - Find CPU_RENDERER type(s)
     *  a. CPU device located within the same platform as the GPU_RENDERER_ONLINE/OFFLINE devices
     *  b. supports graphics/compute API interop with the GPUs
    */
    if(_categorizedDevices[(int)ComputeDeviceCategory::GpuRendererOnline].size()) {
        for(unsigned d = 0; d < getPlatformDeviceCount(_rendererPlatformIdx); ++d) {
            ComputeDevice* dev = getDevice(_rendererPlatformIdx, d);
            const GYComputeDeviceInfo* devInfo = dev->getInfo();

            if(devInfo->devType == GY_COMPUTE_DEVICE_CPU) {
                if(devInfo->rendererSharedContext) {
                    _categorizedDevices[(int)ComputeDeviceCategory::CpuRenderer].push_back(dev);
                    dev->setDeviceCategory(ComputeDeviceCategory::CpuRenderer);
                    size_t size = _categorizedDevices[(int)ComputeDeviceCategory::CpuRenderer].size();
                    Debug::Logf(Debug::LOG_VERBOSE, "CPU_RENDERER[%u]: <%u, %u> [%s]", size-1, dev->getPlatformIdx(), dev->getDeviceIdx(), devInfo->deviceName);
                }
            }
        }
    }

    if(!_categorizedDevices[(int)ComputeDeviceCategory::CpuRenderer].size()) {
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

                 if((int)p != _rendererPlatformIdx || !devInfo->rendererSharedContext || !_categorizedDevices[(int)ComputeDeviceCategory::CpuRenderer].size()) {
                     _categorizedDevices[(int)ComputeDeviceCategory::CpuAuxiliary].push_back(dev);
                     size_t size = _categorizedDevices[(int)ComputeDeviceCategory::CpuAuxiliary].size();
                     dev->setDeviceCategory(ComputeDeviceCategory::CpuAuxiliary);
                     Debug::Logf(Debug::LOG_VERBOSE, "CPU_AUXILIARY[%u]: <%u, %u> [%s]", size-1, dev->getPlatformIdx(), dev->getDeviceIdx(), devInfo->deviceName);
                 }
             }
        }
    }

    if(!_categorizedDevices[(int)ComputeDeviceCategory::CpuAuxiliary].size()) {
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
                 _categorizedDevices[(int)ComputeDeviceCategory::OtherAccelerator].push_back(dev);
                 dev->setDeviceCategory(ComputeDeviceCategory::OtherAccelerator);
                 size_t size = _categorizedDevices[(int)ComputeDeviceCategory::OtherAccelerator].size();
                 Debug::Logf(Debug::LOG_VERBOSE, "ACCELERATOR[%u]: <%u, %u> [%s]", size-1, dev->getPlatformIdx(), dev->getDeviceIdx(), devInfo->deviceName);
             }
        }
    }

    if(!_categorizedDevices[(int)ComputeDeviceCategory::OtherAccelerator].size()) {
        dbgprintf("ACCELERATOR Device(s): NONE");
    }

    //Now check whether we even have a single fucking compute device/accelerator and don't bother even trying otherwise.
    unsigned computeDevCount = 0;
    for(unsigned t = 0; t < (int)ComputeDeviceCategory::HardwareDeviceCount; ++t)
        computeDevCount += _categorizedDevices[t].size();

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

    ComputeDevice* rendererDevice = getDeviceByCategory(ComputeDeviceCategory::GpuRendererOnline);
    if(rendererDevice) {
        dbgprintf("Creating Renderer Context.");
        dbgpush();
        std::vector<ComputeDevice*> devices;

        devices.push_back(rendererDevice);

        for(unsigned d = 0; d < getDeviceCountForCategory(ComputeDeviceCategory::GpuRendererOffline); ++d) {
            devices.push_back(getDeviceByCategory(ComputeDeviceCategory::GpuRendererOffline, d));
        }

        for(unsigned d = 0; d < getDeviceCountForCategory(ComputeDeviceCategory::CpuRenderer); ++d) {
            devices.push_back(getDeviceByCategory(ComputeDeviceCategory::CpuRenderer, d));
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

done:
    _initialized = success;
    dbgpop();
    return success;

}

bool ComputeSystem::uninitialize(void) {
    bool success = true;
    dbgprintf("ComputeSystem::uninitialize()");
    dbgpush();

    for(auto it: _computeTasks) delete it.second;

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

ComputeBuffer* ComputeContext::createBufferFromRendererTexture(intptr_t rendererTexture, GY_COMPUTE_MEMORY_TYPE memType) {
    GYComputeBuffer* gyBuff = gyComputeBufferCreateFromRendererTexture(_gyContext, memType, rendererTexture);
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

bool ComputeProgram::compile(ComputeDevice* devices, unsigned deviceCount, const GYComputeProgramBuildOptions* options, GYComputeBuildInfo* info) {
    assert(deviceCount);
    std::vector<GYComputeDevice> gyDevices;
    for(unsigned d = 0; d < deviceCount; ++d) gyDevices.push_back(*devices[d].getGyDevice());
    _compiled = gyComputeProgramCompile(_gyProgram, &gyDevices.front(), deviceCount, options, nullptr);
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


ComputeTask::ComputeTask(const char*                        kernelName,
                         const ProgramCreateInfo&           programInfo,
                         DeviceCategoryPriorityList         categoryPriorityList,
                         NativeKernel                       nativeFallbackFunc,
                         bool                               requireRendererSharedContext,
                         bool                               outOfOrderExecution):
    _categoryPriorityList(std::move(categoryPriorityList)),
    _nativeFallback(nativeFallbackFunc)
{
    bool valid = true;
    DbgEvent event("Creating ComputeTask: [%s]", kernelName);

    valid &= _resolvePrioritizedDevices(requireRendererSharedContext);

    if(valid) {

        if(nativeFallbackFunc) {
            dbgprintf("[NATIVE_FALLBACK]: %x", &nativeFallbackFunc);
        } else {
            warnprintf("[NATIVE_FALLBACK]: NONE");
        }

        if(!_devicePriorityList.size()) {
            if(nativeFallbackFunc) {
                dbgprintf("No accelerator device found, using native fallback!");
                _useNative = true;
            } else {
                errprintf("No accelerator device found, and no native callback specified!");
                valid = false;
            }
        }
    }

    if(valid && !_useNative) {
        valid &= _initializeResources(kernelName, programInfo, outOfOrderExecution, requireRendererSharedContext);

        //Attemt to fall-back to native if all compute devices failed
        if(!valid) {
            if(nativeFallbackFunc) {
                warnprintf("Falling back to native callback kernel!");
                _useNative = true;
            } else {
                errprintf("No native kernel was specified for fallback! Compute Task not available!");
                valid = false;
            }

        }
    }

    _isValid = valid;
}

bool ComputeTask::_resolvePrioritizedDevices(bool rendererSharedContext) {
    bool success = true;

    DbgEvent event("Resolving Prioritized Category List");

    enum {
        INVALID,
        UNAVAILABLE,
        INCOMPATIBLE,
        OK
    } status = INVALID;
    char buff[100];
    ComputeDevice* firstDev = nullptr;
    ComputeDevice* dev;
    Debug::SEVERITY severity = Debug::LOG_VERBOSE;


    for(size_t i = 0; i < _categoryPriorityList.size(); ++i) {

        if(rendererSharedContext && !ComputeSystem::isRendererSharedContextDeviceCategory(_categoryPriorityList[i])) {
            status      = INVALID;
            severity    = Debug::LOG_ERROR;
            success     = false;
            strcpy(buff, "INVALID");
        } else if(!(dev = getComputeSystem()->getDeviceByCategory(_categoryPriorityList[i]))) {
            status = UNAVAILABLE;
            strcpy(buff, "UNAVAILABLE");
        } else {
            if(firstDev && dev->getPlatformIdx() != firstDev->getPlatformIdx()) {
                status = INCOMPATIBLE;
                strcpy(buff, "INCOMPATIBLE PLATFORM");
            } else if(!firstDev) {
                firstDev = dev;
            }

            status = OK;
            _devicePriorityList.push_back(*dev);
            sprintf(buff, "[Priority: %u, Device: %s <%d, %d>]", _devicePriorityList.size()-1, dev->getInfo()->deviceName, dev->getPlatformIdx(), dev->getDeviceIdx());
        }

        Debug::Logf(severity, "[%d] - %s", i, buff);
    }

    return success;
}

bool ComputeTask::_initializeResources(const char* kernelName,
                                       const ProgramCreateInfo& programInfo,
                                       bool outOfOrderExecution,
                                       bool requireRendererSharedContext)
{
    bool success = true;

    DbgEvent dbgEvent("Initializing Compute Resources");

    int flags = outOfOrderExecution? GY_COMPUTE_COMMAND_QUEUE_FLAG_OUT_OF_ORDER_EXECUTION : 0;
#if ELYSIAN_COMPUTE_PROFILING_ENABLED == 1
    flags |= GY_COMPUTE_COMMAND_QUEUE_FLAG_ENABLE_PROFILING;
#endif

    _device = &_devicePriorityList[0];
    if(!_device) {
        errprintf("Failed to select the highest priority device!");
        success = false;
        goto done;
    }

    _context = requireRendererSharedContext? getComputeSystem()->getRendererContext() :
                                             getComputeSystem()->getPlatformContext(_device->getPlatformIdx());
    if(!_context) {
        errprintf("Failed to assign context!");
        success = false;
        goto done;
    }

    _program = _context->loadProgram(programInfo.sourceFilePath);
    if(!_program) {
        errprintf("Failed to create program: [%s]", programInfo.sourceFilePath);
        success = false;
        goto done;
    }

    GYComputeBuildInfo buildInfo[ELYSIAN_COMPUTE_CONTEXT_MAX_DEVICES];

    if(!_program->compile(&_devicePriorityList[0], _devicePriorityList.size(), &programInfo.buildOptions, buildInfo)) {
        for(size_t d = 0; d < _devicePriorityList.size(); ++d) {
            ComputeDevice* curDev = &_devicePriorityList[d];

            if(buildInfo[d].buildStatus == GY_COMPUTE_BUILD_STATUS_SUCCESS) {
                _device = curDev;
                dbgprintf("Defaulting to Device <%d, %d> at Priority: %d", curDev->getGyDevice()->platformIdx,
                                                                           curDev->getGyDevice()->deviceIdx,
                                                                           d);
                break;
            } else {
                _device = nullptr;
                errprintf("Build failed for device <%d, %d> at Priority [%u], falling through to next priority...", curDev->getGyDevice()->platformIdx,
                                                                                              curDev->getGyDevice()->deviceIdx,
                                                                                              d);
            }
        }
    }

    if(!_device) {
        success = false;
        goto done;
    }

    _kernel = _program->createKernel(kernelName);
    if(!_kernel) {
        errprintf("Failed to create kernel: [%s]", kernelName);
        success = false;
        goto done;
    }

    _cmdQueue = _context->createCommandQueue(_device, flags);
    if(!_cmdQueue) {
        errprintf("Failed to create command queue for the chosen Device!");
        success = false;
    }

done:
    if(!success) {
        errprintf("Failed to initialize hardware acceleration resources for ComputeTask!");
    }
    return success;

}

ComputeTask::~ComputeTask(void) {
    delete _cmdQueue;
    delete _kernel;
    delete _program;
}



ComputeTask* ComputeSystem::createComputeTask(const char*                               kernelName,
                                              ComputeTask::ProgramCreateInfo            progInfo,
                                              ComputeTask::DeviceCategoryPriorityList   deviceCategoryPriorities,
                                              ComputeTask::NativeKernel                 nativeFallbackFunc,
                                              bool                                      requireRendererSharedContext,
                                              bool                                      outOfOrderExecution)
{
    ComputeTask* task = new ComputeTask(kernelName, progInfo, deviceCategoryPriorities,
                                        nativeFallbackFunc, requireRendererSharedContext, outOfOrderExecution);

    _computeTasks[kernelName] = task;
    return task;
}

bool ComputeTask::enqueueKernel(size_t globalSize) {
    assert(_isValid);
    size_t globalOffsets = 0;
    size_t paddedGlobalSize;
    size_t localWorkSize;
    optimizeWorkSize(globalSize, paddedGlobalSize, localWorkSize);
    const bool retVal = getCommandQueue()->enqueueKernel(getKernel(), 1, &globalOffsets, &paddedGlobalSize, &localWorkSize);
    if(!retVal) _isValid = false;
    return retVal;
}

// Recursive function to return gcd of a and b
size_t gcd(size_t a, size_t b)
{
    // Everything divides 0
    if (a == 0 || b == 0)
       return 0;

    // base case
    if (a == b)
        return a;

    // a is greater
    if (a > b)
        return gcd(a-b, b);
    return gcd(a, b-a);
}

void ComputeTask::optimizeWorkSize(size_t globalSize, size_t& paddedGlobalSize, size_t& localWorkSize) {
    assert(_isValid);
    const GYComputeKernelWorkgroupInfo* info = getDevice()->getKernelWorkGroupInfo(getKernel());

    size_t prefMult = info->preferredWorkGroupSizeMultiple;
    size_t maxSize  = info->workGroupSize;

    paddedGlobalSize = globalSize;
    size_t rem = globalSize % maxSize;
    if(rem) paddedGlobalSize += (maxSize - rem);

#if 0
    localWorkSize = (paddedGlobalSize >= maxSize)?
        maxSize :
        paddedGlobalSize;
#else
#if 0
    size_t multiples = paddedGlobalSize / prefMult;

    localWorkSize = multiples * prefMult;

    if(localWorkSize > globalSize) {
        while(localWorkSize )


    }

#if 0
    while((paddedGlobalSize % workSize == 0) && workSize < maxSize) {
        workSize += prefMult;
    }
#endif
#else
    localWorkSize = maxSize;//prefMult;
#endif
#endif

    size_t error = paddedGlobalSize % localWorkSize;

    assert(error == 0);
}


}

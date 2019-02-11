#include <ESGamma/crtp/elysian_render_system.hpp>
#include <ESGamma/crtp/elysian_debug.hpp>
#include <libGyro/gyro_video_api.h>
#include <AssetIO/level/elysian_opaque_map_scene_object.hpp>
#include <AssetIO/crtp/elysian_area.hpp>
#include <AssetIO/crtp/elysian_particle_emitter.hpp>
#include <ESGamma/crtp/elysian_camera_system.hpp>
#include <AssetIO/crtp/elysian_level.hpp>
#include <gl/gyro_video_gl.h>
#include <gl/gyro_gl.h>
#include <gl/gyro_shaders_gl.h>
#include <gl/gyro_gl_utils.h>

extern int glalVertexArray;
extern GLuint                      _streamVao[2];
extern GLuint _glalColorTex;
extern GLuint  _glalDepthBuffer;
extern GLuint glalLightingUBO;

namespace elysian {

GLuint _glParticleVao           = 0;
GLuint _glParticleVbo           = 0;
GLuint _glParticleIndirectBuff  = 0;

const constexpr RenderableObjectPipelineAttrib::BlendModeFactors RenderableObjectPipelineAttrib::_blendFactors[BLEND_MODE_COUNT];
ComputeBuffer* RendererComputeGlobalDataManager::_cameraBuffer  = nullptr;
const Camera* RendererComputeGlobalDataManager::_camera         = nullptr;


#if 0
bool RenderSystemBase::isParticleAccelerationAvailable(void) const {
    return _particleComputeTask->isAccelerationAvailable();
}

RenderSystemBase::getUseParticleAcceleration(void) const {
    return _particleComputeTask->isAccelerated();
}

void RenderSystemBase::setUseParticleAcceleration(bool val) {
    _particleComputeTask->setAccelerated(val);
}

void RenderSystemBase::_setParticleAccelerationAvailable(bool val) {
    _particleAccelerationAvailable = val;
}
#endif

bool RenderSystemBase::initialize(void) {
  //  getEventSys()->addEventObserver(this, EVENT_VIEWPORT_SIZE_CHANGED);
    getEventSys()->addEventObserver(this, EVENT_AREA_CHANGED);
    bool success = true;

    dbgprintf("RenderSystem::initialize()");
    dbgpush();

    if(getComputeSystem()->isComputeSupported() && getComputeSystem()->isRendererInteropSupported()) {

        success &= _initializeComputeGlobalData();

        glGenBuffers(1, &_glParticleVbo);
        glBindBuffer(GL_ARRAY_BUFFER, _glParticleVbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(GYVertex)*ELYSIAN_RENDER_SYSTEM_PARTICLE_VBO_KERNEL_VERTEX_MAX, nullptr, GL_DYNAMIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glGenBuffers(1, &_glParticleIndirectBuff);
        glBindBuffer(GL_DRAW_INDIRECT_BUFFER, _glParticleIndirectBuff);
        glBufferData(GL_DRAW_INDIRECT_BUFFER, sizeof(DrawIndirectBuffer), nullptr, GL_DYNAMIC_DRAW);
        glBindBuffer(GL_DRAW_INDIRECT_BUFFER, 0);

        glGenVertexArrays(1, &_glParticleVao);
        glBindVertexArray(_glParticleVao);
        glBindBuffer(GL_ARRAY_BUFFER, _glParticleVbo);

        glEnableVertexAttribArray(glalLightingShader.attribLoc[GLAL_SHADER_POS]);
        glEnableVertexAttribArray(glalLightingShader.attribLoc[GLAL_SHADER_TEX_COORD]);
        glEnableVertexAttribArray(glalLightingShader.attribLoc[GLAL_SHADER_COLOR]);
        glEnableVertexAttribArray(glalLightingShader.attribLoc[GLAL_SHADER_NORMAL]);
        glEnableVertexAttribArray(glalLightingShader.attribLoc[GLAL_SHADER_TANGENT]);
        glEnableVertexAttribArray(glalLightingShader.attribLoc[GLAL_SHADER_NORMAL_Z]);

        glVertexAttribPointer(glalLightingShader.attribLoc[GLAL_SHADER_POS], 4, GL_FLOAT, GL_FALSE, sizeof(GYVertex), (void*)0);
        glVertexAttribPointer(glalLightingShader.attribLoc[GLAL_SHADER_TEX_COORD], 2, GL_FLOAT, GL_TRUE, sizeof(GYVertex), (void*)(4*sizeof(float)));
        glVertexAttribPointer(glalLightingShader.attribLoc[GLAL_SHADER_COLOR], 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(GYVertex), (void*)(6*sizeof(float)));
        glVertexAttribPointer(glalLightingShader.attribLoc[GLAL_SHADER_NORMAL], 3, GL_FLOAT, GL_TRUE, sizeof(GYVertex), (void*)(7*sizeof(float)));
        glVertexAttribPointer(glalLightingShader.attribLoc[GLAL_SHADER_TANGENT], 3, GL_FLOAT, GL_TRUE, sizeof(GYVertex), (void*)(11*sizeof(float)));
        glVertexAttribPointer(glalLightingShader.attribLoc[GLAL_SHADER_NORMAL_Z], 1, GL_FLOAT, GL_TRUE, sizeof(GYVertex), (void*)(10*sizeof(float)));

      _particleComputeTask = getComputeSystem()->createComputeTask(ELYSIAN_RENDER_SYSTEM_PARTICLE_VBO_KERNEL_NAME,
                                                                   {
                                                                        ELYSIAN_RENDER_SYSTEM_PARTICLE_VBO_KERNEL_PATH
                                                                   },
                                                                   {
                                                                       ComputeDeviceCategory::GpuRendererOnline,
                                                                       ComputeDeviceCategory::GpuRendererOffline,
                                                                       ComputeDeviceCategory::CpuRenderer
                                                                   },
                                                                   nullptr,
                                                                   true);

        if(_particleComputeTask->isAccelerated() && _particleComputeTask->isValid()) {
            _particleInBuffer = _particleComputeTask->getContext()->createBuffer(GY_COMPUTE_MEMORY_TYPE_READ_ONLY, sizeof(GYVertex)*ELYSIAN_RENDER_SYSTEM_PARTICLE_VBO_KERNEL_VERTEX_MAX, nullptr);
            _particleVboOutBuffer = _particleComputeTask->getContext()->createBufferFromRendererBuffer(_glParticleVbo, GY_COMPUTE_MEMORY_TYPE_WRITE_ONLY);
            _particleDrawIndirectOutBuffer = _particleComputeTask->getContext()->createBufferFromRendererBuffer(_glParticleIndirectBuff, GY_COMPUTE_MEMORY_TYPE_READ_WRITE);
            _particleViewProjInBuffer = _particleComputeTask->getContext()->createBuffer(GY_COMPUTE_MEMORY_TYPE_READ_ONLY, sizeof(GYMatrix4), nullptr);
            if(_particleVboOutBuffer && _particleInBuffer && _particleDrawIndirectOutBuffer && _particleViewProjInBuffer) {
                ComputeKernel* kernel =  _particleComputeTask->getKernel();
                if(kernel) {
                    success &= kernel->setArg(1, _particleInBuffer);
                    success &= kernel->setArg(2, _particleViewProjInBuffer);
                    success &= kernel->setArg(3, _particleDrawIndirectOutBuffer);
                    success &= kernel->setArg(4, _particleVboOutBuffer);
                }
            }
        }

        _luminanceComputeTask = new LuminanceHistogramComputeTask(_glalColorTex);
        if(_luminanceComputeTask->isValid()) {
            getComputeSystem()->addComputeTask(_luminanceComputeTask);
            dbgprintf("HISTOGRAMZZZZZ - <%d, %d>", _luminanceComputeTask->getDevice()->getPlatformIdx(), _luminanceComputeTask->getDevice()->getDeviceIdx());
        }

        //_lightCullComputeTask = new LightCullingComputeTask(_glalDepthBuffer,
          //                                                  glalLightingUBO);

        _populateMapIboComputeTask = new PopulateMapIboComputeTask();

    }

    if(_particleComputeTask && _particleComputeTask->isValid()) {
        dbgprintf("RendererSystem: Compute accelerated particles enabled.");

        success = true;
    } else {
        warnprintf("RenderSystem: No compute acceleration available for particles!");
        success = false;
    }

    dbgpop();
    return success;
}

bool RenderSystemBase::uninitialize(void) {
    bool success = true;
    success &= _uninitializeComputeGlobalData();
    getEventSys()->removeEventObserver(this, EVENT_VIEWPORT_SIZE_CHANGED);
    getEventSys()->removeEventObserver(this, EVENT_AREA_CHANGED);
    delete _particleVboOutBuffer;
    return success;
}

bool RenderSystemBase::populateParticleVbos(void) {
    static KernelParticle kernParts[ELYSIAN_RENDER_SYSTEM_PARTICLE_VBO_KERNEL_VERTEX_MAX/6];
    bool success = true;

    if(gyVidShadowPass()) return true;
    if(_particleComputeTask && !_particleComputeTask->isValid()) return false;

    _activeParticles = 0;
    for(unsigned i = 0; i < getCurrentArea()->particleEmitters.size(); ++i) {
        auto* emitter = getCurrentArea()->particleEmitters[i];

        for(auto it = emitter->_pool.begin(); it < emitter->_pool.end(); ++it) {
            KernelParticle* part = &kernParts[_activeParticles++];
            memset(part, 0, sizeof(KernelParticle));
            auto& p = (*it);

            part->pos[0] = p.getPosition().x;
            part->pos[1] = p.getPosition().y;
            part->pos[2] = p.getPosition().z;
                     //   dbgprintf("[%u] = <%f, %f, %f>", partCount-1, part->pos[0], part->pos[1], part->pos[2]);
            part->pos[3] = 0.0f;
            part->size[0] = p.size;
            part->size[1] = p.size;
            part->color = p.color;
        }
    }

    if(_activeParticles) {

        auto* wgInfo = _particleComputeTask->getDevice()->getKernelWorkGroupInfo(_particleComputeTask->getKernel());
        size_t offset = 0;
        size_t globalWorkSize = _activeParticles;

        //if(globalWorkSize%wgInfo->preferredWorkGroupSizeMultiple) globalWorkSize += (wgInfo->preferredWorkGroupSizeMultiple-globalWorkSize%wgInfo->preferredWorkGroupSizeMultiple);

        size_t localWorkSize = wgInfo->workGroupSize;
        if(localWorkSize > 64) localWorkSize = 64;

         if(globalWorkSize%localWorkSize) globalWorkSize += (localWorkSize-globalWorkSize%localWorkSize);

        if(localWorkSize > globalWorkSize) globalWorkSize = localWorkSize;

        ComputeBuffer* sharedBuffs[] = {
            _particleVboOutBuffer,
            _particleDrawIndirectOutBuffer
        };

        auto res = getCameraSys()->getScreenResolution();
        int32_t viewportSize[2] = {
            (int32_t)res.x, (int32_t)res.y
        };

        success &= _particleComputeTask->getCommandQueue()->enqueueAcquireRendererSharedObject(2, sharedBuffs);
        success &= _particleComputeTask->getCommandQueue()->enqueueBufferWrite(_particleViewProjInBuffer, 0, sizeof(GYMatrix4), getCameraSys()->getCurrentState()->projViewMat, false);
        success &= _particleComputeTask->getCommandQueue()->enqueueBufferWrite(_particleInBuffer, 0, sizeof(KernelParticle)*_activeParticles, kernParts, false);
        success &= _particleComputeTask->getKernel()->setArg(0, sizeof(uint32_t), &_activeParticles);
        success &= _particleComputeTask->getKernel()->setArg(5, sizeof(int32_t)*2, viewportSize);
        //success &= _particleComputeTask->getCommandQueue()->enqueueKernel(_particleComputeTask->getKernel(), 1, &offset, &globalWorkSize, &localWorkSize);
        success &= _particleComputeTask->enqueueKernel(1, &offset, &globalWorkSize, &localWorkSize);
        success &= _particleComputeTask->getCommandQueue()->enqueueReleaseRendererSharedObject(2, sharedBuffs);

        if(!success) {
            errprintf("RenderSystem: GPGPU Particle VBO population failed! Falling back to software from now on!");
        }
    }

    return success;
}

bool RenderSystemBase::renderParticles(void) {
    bool success = true;
    if(gyVidShadowPass()) return true;

    if(!_particleComputeTask || !_particleComputeTask->isValid()) {
        for(unsigned i = 0; i < getCurrentArea()->particleEmitters.size(); ++i) {
            auto* emitter = getCurrentArea()->particleEmitters[i];
            emitter->render();
        }
    } else if(_activeParticles) {

        if(success) {
            renderParticlesMonitor.start();
            success &= _particleComputeTask->finish();
            renderParticlesMonitor.stop();

            const GYVidPipelineState pipelineState = {
                0,
                nullptr,
                nullptr,
                nullptr,
                GY_ALPHA_FULL,
                GY_BLEND_SRC_ALPHA,
                GY_BLEND_ONE_MINUS_SRC_ALPHA,
                false,
                GY_VID_DEPTH_ALWAYS,
                { 0, 0, 0, 0 }
            };
            gyVidPipelineState(&pipelineState);
            gyVidMaterialBind(nullptr);

            glBindVertexArray(_glParticleVao);
            glBindBuffer(GL_DRAW_INDIRECT_BUFFER, _glParticleIndirectBuff);

            glDrawArraysIndirect(GL_TRIANGLE_STRIP, 0);

            glBindBuffer(GL_DRAW_INDIRECT_BUFFER, 0);
            glBindVertexArray(_streamVao[0]);
        } else {
            errprintf("RenderSystem: Failed to render GPGPU accelerated particles! Falling back to software from now on!");
           // setUseParticleAcceleration(false);
        }
    }
    return success;
}

void RenderSystemBase::createLuminanceHistogram(void) {
    bool success = true;
    unsigned width, height;
    std::tie(width, height) = getCameraSys()->getViewportSize();

    if(_luminanceComputeTask->isAccelerated() && _luminanceComputeTask->isValid()) {
        success &= _luminanceComputeTask->getCommandQueue()->enqueueAcquireRendererSharedObject(1, &_luminanceComputeTask->_fboBuffer);
        success &= _luminanceComputeTask->enqueueKernel(width*height);
        success &= _luminanceComputeTask->getCommandQueue()->enqueueReleaseRendererSharedObject(1, &_luminanceComputeTask->_fboBuffer);
        success &= _luminanceComputeTask->getCommandQueue()->enqueueBufferRead(_luminanceComputeTask->_statsBuffer,
                                                                               0,
                                                                               sizeof(LuminanceHistogramComputeTask::LuminanceStats),
                                                                               &_luminanceComputeTask->_lumStats,
                                                                               false);

        if(!success) {
            errprintf("Something reested with the LuminanceComputeTask! Disabling acceleration!");
            _luminanceComputeTask->setAccelerated(false);
        }

    }

}


void RenderSystemBase::updateHDRExposure(void) {


    if(_luminanceComputeTask->isAccelerated()) {
        bool retVal;

        updateHDRExposureMonitor.start();
        retVal = _luminanceComputeTask->finish();
        updateHDRExposureMonitor.stop();

        if(!retVal) {
            return;
        }


        double avg = 0.0f;
        uint64_t count = 0;
        //DbgEvent event("Luminance Stats");
        //dbgprintf("Min - %u, Max - %u", _luminanceComputeTask->_lumStats.min, _luminanceComputeTask->_lumStats.max);
        for(unsigned i = 0; i < 256; ++i) {
            count += _luminanceComputeTask->_lumStats.histBins[i];
            avg += _luminanceComputeTask->_lumStats.histBins[i]*(float)i;
        }

        if(count != 0.0f) {
            avg /= (double)(count);
#if 0
        int max = 0;

        for(unsigned i = 1; i < 256; ++i) {
            if(_luminanceComputeTask->_lumStats.histBins[i] > _luminanceComputeTask->_lumStats.histBins[max]) max = i;
        }
#endif
//           avg = max;

        const float key = _curArea->getAmbientColor().getW();
        if((key/avg) >= 1.1) {

            _luminanceComputeTask->_exposure *= 1.005f;
            gyVidHighDynamicRangeExposure(_luminanceComputeTask->_exposure);
        } else if((key/avg) <= 0.9) {
            _luminanceComputeTask->_exposure *= 0.995f;
            //_curArea->getAmbientColor().setW(_curArea->getAmbientColor().getW() * 0.995);
            gyVidHighDynamicRangeExposure(_luminanceComputeTask->_exposure);
        }
        }
#if 0
        static int framez = 0;

        if(++framez %60 == 0) {
            dbgprintf("Avg - %lf, Exp - %f", avg, _luminanceComputeTask->_exposure);
                    framez = 0;

        }
#endif

    //if fucking w/ exp and NOT modifying average, STOP



         //   dbgprintf("AVG = %lf", avg);
            //dbgprintf("[%u] - %u", i, _luminanceComputeTask->_lumStats.histBins[i]);



       // if(fabs(150.0f - avg) >= 20) {

         //   _curArea->getAmbientColor().setW(150.0f/avg);

        //}

    }


}


bool RenderSystemBase::_initializeAutomaticExposureKernel(void) {
}


bool RenderSystemBase::_uninitializeAutomaticExposureKernel(void) {

}

LuminanceHistogramComputeTask::LuminanceHistogramComputeTask(uintptr_t textureId):
    ComputeTask(ELYSIAN_RENDER_SYSTEM_LUMINANCE_HISTOGRAM_KERNEL_NAME,
        {
             ELYSIAN_RENDER_SYSTEM_LUMINANCE_HISTOGRAM_KERNEL_PATH
        },
        {
            //ComputeDeviceCategory::GpuRendererOffline,
            ComputeDeviceCategory::GpuRendererOnline,
            //ComputeDeviceCategory::CpuRenderer
        },
        NULL,
        true)
{
    DbgEvent event("LuminanceHistogramTask - Initializing Kernel");

    if(isValid()) {
        _fboBuffer      = getContext()->createBufferFromRendererTexture(textureId, GY_COMPUTE_MEMORY_TYPE_READ_WRITE, GL_TEXTURE_2D);
        if(_fboBuffer) _statsBuffer    = getContext()->createBuffer(GY_COMPUTE_MEMORY_TYPE_READ_ONLY, sizeof(LuminanceStats));

        if(!_fboBuffer || !_statsBuffer) {
            errprintf("Histogram compute resource allocation failed.");
            _useNative = false;
        } else {

            int success = 1;
            success &= getKernel()->setArg(0, _statsBuffer);
            success &= getKernel()->setArg(1, _fboBuffer);

            if(!success) {
                errprintf("Failed to set initial kernel arguments!");
                _useNative = false;
            }
        }
    }

}


LightCullingComputeTask::LightCullingComputeTask(uintptr_t depthBuffTexId,
                                                 uintptr_t lightingUboId):
    ComputeTask(ELYSIAN_RENDER_SYSTEM_LIGHT_CULLING_KERNEL_NAME,
        {
             ELYSIAN_RENDER_SYSTEM_LIGHT_CULLING_KERNEL_PATH
        },
        {
            //ComputeDeviceCategory::GpuRendererOffline,
            ComputeDeviceCategory::GpuRendererOnline,
            //ComputeDeviceCategory::CpuRenderer
        },
        NULL,
        true),
    _tileSize(ELYSIAN_RENDER_SYSTEM_LIGHT_CULLING_TILE_SIZE_DEFAULT)
{
    DbgEvent event("CullLights - Initializing Kernel");
    if(isValid()) {

       // _tileSizeBuffer     = getContext()->createBuffer(GY_COMPUTE_MEMORY_TYPE_READ_ONLY, sizeof(uint32_t));
      //  _projViewMatBuff    = getContext()->createBuffer(GY_COMPUTE_MEMORY_TYPE_READ_ONLY, sizeof(float)*16);
        _lightingUboBuff    = getContext()->createBufferFromRendererBuffer(lightingUboId,       GY_COMPUTE_MEMORY_TYPE_READ_ONLY);

        glGenTextures(1, &_lightMapTexId);
        glBindTexture(GL_TEXTURE_1D, _lightMapTexId);
        glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        unsigned scrWidth, scrHeight;
        gyVidResolution(&scrWidth, &scrHeight);
        int tileCount = scrWidth*scrHeight / (_tileSize*_tileSize);
            glTexImage1D(GL_TEXTURE_1D, 0, GL_RGBA8, tileCount*ELYSIAN_RENDER_SYSTEM_LIGHT_CULLING_LIGHTS_PER_TILE_MAX/4, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
        //glTexImage3D(GL_TEXTURE_3D, 0, GL_RGBA, width, height, ELYSIAN_RENDER_SYSTEM_LIGHT_CULLING_LIGHTS_PER_TILE_MAX/4, 0, GL_RGBA8, GL_UNSIGNED_BYTE, NULL);

        _lightMapBuff = getContext()->createBufferFromRendererTexture(_lightMapTexId, GY_COMPUTE_MEMORY_TYPE_WRITE_ONLY, GL_TEXTURE_1D);
        glBindTexture(GL_TEXTURE_1D, 0);
     //   _depthBuffer = getContext()->createBufferFromRendererTexture(depthBuffTexId, GY_COMPUTE_MEMORY_TYPE_READ_ONLY, GL_TEXTURE_2D);

       // getKernel()->setArg(0, _tileSizeBuffer);
        //getKernel()->setArg(0, _projViewMatBuff);
        getKernel()->setArg(0, _lightingUboBuff);
        getKernel()->setArg(1, _lightMapBuff);
       // getKernel()->setArg(3, _depthBuffer);

    }
}

bool LightCullingComputeTask::updateScreenResolution(unsigned int width, unsigned int height) {
    if(!isValid()) return false;
    glBindTexture(GL_TEXTURE_1D, _lightMapTexId);
    glTexImage1D(GL_TEXTURE_1D, 0, GL_RGBA, width*height/(_tileSize*_tileSize)*ELYSIAN_RENDER_SYSTEM_LIGHT_CULLING_LIGHTS_PER_TILE_MAX/4, 0, GL_RGBA8, GL_UNSIGNED_BYTE, NULL);
    glBindTexture(GL_TEXTURE_1D, 0);
    return true;
}


bool RenderSystemBase::forwardPlusCullLights(void) {
    bool success = true;
    static unsigned prevWidth = 0;
    static unsigned prevHeight = 0;
    unsigned scrWidth, scrHeight;
    gyVidResolution(&scrWidth, &scrHeight);


    assert(_lightCullComputeTask->isValid());
    if(!_lightCullComputeTask->isValid()) return false;

    size_t globalOffsets[]  = { 0, 0 };
    size_t localWorkSize[]  = {  8, 8 };
    size_t globalWorkSize[] = { scrWidth, scrHeight };

    ComputeBuffer* buffers[] = {
     //   _lightCullComputeTask->_depthBuffer,
        _lightCullComputeTask->_lightMapBuff//,
       // _lightCullComputeTask->_projViewMatBuff,
        ,_lightCullComputeTask->_lightingUboBuff
    };

    for(unsigned i = 0; i < 2; ++i) {
        if(globalWorkSize[i] % 8) globalWorkSize[i] += (8 - globalWorkSize[i]%8);
    }



  // success &= _lightCullComputeTask->getCommandQueue()->enqueueBufferWrite(_lightCullComputeTask->_projViewMatBuff, 0, sizeof(float)*16, getCameraSys()->getCurrentState()->projViewMat, false);
    success &= _lightCullComputeTask->getCommandQueue()->enqueueAcquireRendererSharedObject(2, buffers);
    success &= _lightCullComputeTask->enqueueKernel(scrWidth*scrHeight);
    if(!success) errprintf("ALREADY REAESTED!!!");
  /*  success &= _lightCullComputeTask->getCommandQueue()->enqueueKernel(_lightCullComputeTask->getKernel(),
                                                                       2,
                                                                       globalOffsets,
                                                                       globalWorkSize,
                                                                       localWorkSize);*/
 //   success &= _lightCullComputeTask->getCommandQueue()->seRendererSharedObject(2, buffers, nullptr, &_lightCullComputeTask->_completeEvent);
    //gyComputeCommandQueueFinish(_lightCullComputeTask->getCommandQueue()->_gyCmdQueue);
       success &= _lightCullComputeTask->getCommandQueue()->enqueueReleaseRendererSharedObject(2, buffers, nullptr, nullptr);
       _lightCullComputeTask->finish();
    //success &= _lightCullComputeTask->getComm
    // success &= _lightCullComputeTask->getCommandQueue()->enqueueReleaseRendererSharedObject(1, buffers);
    return success;
}




void RenderSystemBase::beginScene(void) {
#ifdef ELYSIAN_RENDER_SYSTEM_DEBUG
    memcpy(_prevSceneStats, _curSceneStats, sizeof(SceneStats));
    memset(_curSceneStats, 0, sizeof(SceneStats));
    _curSceneStats.totalSceneObjs = _sceneObjs.size();
    for(auto& obj : _sceneObjs) {
        obj.setSceneObjTransformDirty(true);
    }
#endif
}

void RenderSystemBase::endScene(void) {
#ifdef ELYSIAN_RENDER_SYSTEM_DEBUG
    _curSceneStats.avgBatchSize /= _curSceneStats.batchCount;
#endif
}

void RenderSystemBase::enableTransparentGeometry(bool val) {
    _transparentGeometryEnabled = val;
}

bool RenderSystemBase::isTransparentGeometryEnabled(void) const {
    return _transparentGeometryEnabled;
}

void RenderSystemBase::onObserveEvent(Event* event, EventTarget*) {
    switch(event->getEventType()) {
    case EVENT_AREA_CHANGED:
        _onAreaChange();
        break;
    case EVENT_VIEWPORT_SIZE_CHANGED:
        _onViewportSizeChange();
        break;
    default:
        break;
    }
}
void RenderSystemBase::_onViewportSizeChange() {
    unsigned scrWidth, scrHeight;
    std::tie(scrWidth, scrHeight) = getCameraSys()->getViewportSize();
    _lightCullComputeTask->updateScreenResolution(scrWidth, scrHeight);
}

//Don't recalculate world-space geometric shit if it's the same shit used again later
//Do all of the opaque shit in parallel with all of the transparent shit
void RenderSystemBase::renderScene(void) {
    if(gyVidShadowPass()) return;
#if 1
bool success = _populateMapIboComputeTask->populateIbo();


    success &= _populateMapIboComputeTask->waitForIboFilled();

    assert(success);

    if(success) {

        gyVidVertexBatchFlush();
        gyVidVertexBatchEnd();

        GYVidPipelineState state = {
            0,
            getCurrentLevel()->getTileSheet(),
            getCurrentLevel()->getNormalSheet(),
            getCurrentLevel()->getEmissiveSheet(),
            GY_ALPHA_NONE,
            GY_BLEND_ONE,
            GY_BLEND_ONE,
            true,
            GY_VID_DEPTH_LEQUAL,
            { 0, 0, 0, 0 }
        };

        gyVidPipelineState(&state);
        gyVidMaterialBind(nullptr);
        GY_GL_CHECK_ERROR();

        gyVidVertexArrayBind(_curArea->vao[gyVidShadowPass()]);
        GY_GL_CHECK_ERROR();

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _curArea->elementVbo[0]);
        GY_GL_CHECK_ERROR();
        glBindBuffer(GL_DRAW_INDIRECT_BUFFER, _populateMapIboComputeTask->getDrawIndirectBuffer());
        GY_GL_CHECK_ERROR();
        glDrawElementsIndirect(GL_TRIANGLE_STRIP, GL_UNSIGNED_INT, 0);

        //glDrawElements(GL_TRIANGLE_STRIP, (_size-_lastSize), GL_UNSIGNED_INT, (const GLvoid*)_lastOffset);
        GY_GL_CHECK_ERROR();
#if 0
          glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint)*ELYSIAN_AREA_GLAL_STRIP_INDEX_ARRAY_SIZE, elementsArray, ELYSIAN_AREA_ELEMENT_ARRAY_VBO_TYPE);

        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint)*ELYSIAN_AREA_GLAL_STRIP_INDEX_ARRAY_SIZE, nullptr, ELYSIAN_AREA_ELEMENT_ARRAY_VBO_TYPE);
                GY_GL_CHECK_ERROR();
#endif

        gyVidVertexArrayBind(0);
        glBindBuffer(GL_DRAW_INDIRECT_BUFFER, 0);

    } else {
        errprintf("FUCKED");
    }


#endif




#if 0
#ifdef ELYSIAN_RENDER_SYSTEM_DEBUG
    _curSceneStats.totalSceneObjs += _sceneObjs.size();
    ++_curSceneStats.sceneRenderPasses;
#endif

    _opaqueSceneObjs.clear();
    _alphaSceneObjs.clear();

    //Look at every object in the scene (PARALLELIZE ME!!!)
    for(auto& obj : _sceneObjs) {
        //skip transparency if alpha is disabled this pass
        if(obj.isTransparent() && !_transparentGeometryEnabled) {
            continue;
        }

        //update object's internals (if necessary)
        obj.updateBoundingVolume(_curCamera->viewProjMat);

        //world-space frustum culling
        if(cullSceneObj(obj)) {
            obj.setVisible(false);
#ifdef ELYSIAN_RENDER_SYSTEM_DEBUG
            ++_curSceneStats.culledSceneObjs;
#endif
            continue;
        } else {
            obj.setVisible(true);

            //update priority values
            obj.updateRenderPriority();

            //write object into respective queue
            if(obj.isTransparent())
                _alphaSceneObjs.push_back(obj);
            else
                _opaqueSceneObjs.push_back(obj);
        }

    }

    //do both in parallel
    quicksort(&_opaqueSceneObjs); //ascending Z depth
    quicksort(&_alphaSceneObjs);  //descending Z depth

#ifdef ELYSIAN_RENDER_SYSTEM_DEBUG
    _curSceneStats.opaqueSceneObjs 	+= _opaqueSceneObjs.size();
    _curSceneStats.alphaSceneObjs 	+= _alphaSceneObjs.size();
#endif

    std::vector<SceneObject*>& objLists[2] = {
        _curSceneStats.opaqueSceneObjs,
        _curSceneStats.alphaSceneObjs
    };

    //Pass 0: opaque geometry, Pass 1: transparent geometry (optional)
    for(unsigned p = 0; p < _transparentGeometryEnabled? 2 : 1; ++p) {

        for(size_t i = 0; i < objLists[p].size(); ++i) {
            const SceneObject*const obj = _objLists[p][i];

            //Determine number of objects within current batch
            const uint32_t batchId = (obj->getRenderPriority()&0xffffffff);
            size_t end = i;
            for(size_t e = i+1; e < objLists[i].size(); ++e) {
                if(objLists[e]->getRenderPriority()&0xffffffff == batchId) {
                    end = e;
                } else break;
            }

#ifdef ELYSIAN_RENDER_SYSTEM_DEBUG
            ++_curSceneStats.batchCount;
            _curSceneStats.avgBatchSize += e-i + 1;
#endif

            GYVidPipelineState pipeState = {
                obj->getTextureId(),
                obj->getBlendMode(),
                obj->shaderId()
            };

            gyVidPipelineState(&pipeState);
            for(size_t o = i; o <= end; ++o) {
                obj->render();
#ifdef ELYSIAN_RENDER_SYSTEM_DEBUG
                ++_curSceneStats.renderCalls;
#endif
            }

            i = end;
        }

    }
#endif
}


#ifdef GY_VIDEO_DEBUG

bool RenderSystemBase::dumpStats(void) const {
    return gyVidDebugStatsDump();
}

#endif

void RenderSystemBase::_onAreaChange(void) const {
    if(_populateMapIboComputeTask) _populateMapIboComputeTask->updateArea(getCurrentArea());
}


bool RenderSystemBase::updateCamera(const Camera* cam) const {
    assert(cam);
    _camera = cam;
    return true;
}

bool RenderSystemBase::_initializeComputeGlobalData(void) const {
    bool success = true;

    _cameraBuffer = getComputeSystem()->getRendererContext()->createBuffer(GY_COMPUTE_MEMORY_TYPE_READ_ONLY, sizeof(CLCamera));

    if(!_cameraBuffer) success = false;

    return success;

}
bool RenderSystemBase::_uninitializeComputeGlobalData(void) const {
    bool success = true;

    delete _cameraBuffer;

    return success;

}

}

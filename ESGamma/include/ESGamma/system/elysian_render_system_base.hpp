#ifndef ELYSIAN_RENDER_SYSTEM_BASE_HPP
#define ELYSIAN_RENDER_SYSTEM_BASE_HPP

#include <ESGamma/system/elysian_system.hpp>
#include <ESGamma/system/elysian_compute_system.hpp>
#include <ESGamma/system/elysian_thread_system.hpp>
#include <ESGamma/system/elysian_event_system.hpp>
#include <libGyro/gyro_matrix_api.h>
#include <libGyro/gyro_video_api.h>


#define ELYSIAN_RENDER_SYSTEM_PARTICLE_VBO_KERNEL_VERTEX_MAX    10000
#define ELYSIAN_RENDER_SYSTEM_PARTICLE_VBO_KERNEL_PATH          "/assets/opencl/particle_vbo.cl"
#define ELYSIAN_RENDER_SYSTEM_PARTICLE_VBO_KERNEL_NAME          "PopulateParticleVbo"
#define ELYSIAN_RENDER_SYSTEM_LUMINANCE_HISTOGRAM_KERNEL_PATH   "/assets/opencl/luminance_histogram.cl"
#define ELYSIAN_RENDER_SYSTEM_LUMINANCE_HISTOGRAM_KERNEL_NAME   "CreateGlobalLuminanceHistogram"
#define ELYSIAN_RENDER_SYSTEM_LIGHT_CULLING_KERNEL_NAME         "CullLights"
#define ELYSIAN_RENDER_SYSTEM_LIGHT_CULLING_KERNEL_PATH         "/assets/opencl/light_culling.cl"


#define ELYSIAN_RENDER_SYSTEM_LIGHT_CULLING_TILE_SIZE_DEFAULT       16
#define ELYSIAN_RENDER_SYSTEM_LIGHT_CULLING_LIGHTS_PER_TILE_MAX     256

namespace elysian {

    class PopulateMapIboComputeTask;
    class Camera;

    enum BLEND_MODE {
        BLEND_MODE_NONE,
        BLEND_MODE_NORMAL,
        BLEND_MODE_ADDITIVE,
        //BLEND_MODE_MULTIPLY,
        //BLEND_MODE_SCREEN,
        //BLEND_MODE_DARKEN,
        //BLEND_MODE_LIGHTEN,
        //BLEND_MODE_DIFFERENCE,
        //BLEND_MODE_NEGATION,
        //BLEND_MODE_EXCLUSION,
        //BLEND_MODE_DODGE,
        //BLEND_MODE_BURN,
        BLEND_MODE_CUSTOM,
        BLEND_MODE_COUNT
    };

    class RenderableObjectPipelineAttrib {
    private:
        struct BlendModeFactors {
            GY_BLEND_FACTOR src;
            GY_BLEND_FACTOR dst;
        };

        const static constexpr BlendModeFactors _blendFactors[BLEND_MODE_COUNT] = {
            { GY_BLEND_NONE,            GY_BLEND_NONE },                 //BLEND_MODE_NONE
            { GY_BLEND_SRC_ALPHA,       GY_BLEND_ONE_MINUS_SRC_ALPHA},
            { GY_BLEND_SRC_ALPHA,       GY_BLEND_ONE },
            { GY_BLEND_ONE,             GY_BLEND_ONE }                      //BLEND_MODE_CUSTOM
        };

        uint16_t            _blendSrcFactor = GY_BLEND_NONE;
        uint16_t            _blendDstFactor = GY_BLEND_NONE;
        GYTexture*          _texture        = nullptr;
        //Shader*             _shader         = nullptr;


    public:
                            RenderableObjectPipelineAttrib(void);

        bool                hasAlpha(void) const;
        bool                isOpaque(void) const;

        void                setBlendFactors(GY_BLEND_FACTOR src, GY_BLEND_FACTOR dst);
        void                setBlendMode(BLEND_MODE mode);
        BLEND_MODE          getBlendMode(void) const;
        GY_BLEND_FACTOR     getSrcBlendFactor(void) const;
        GY_BLEND_FACTOR     getDstBlendFactor(void) const;

        GYTexture*          getTexture(void) const;
        void                setTexture(GYTexture* tex);


    };


    class Thread;

    struct KernelParticle {
        float       pos[4];
        float       size[2];
        GYTexCoord2 texCoord;
        float       orient;
        GYColor4    color;
    };

    struct DrawIndirectBuffer {
       uint32_t  count;
       uint32_t  instanceCount;
       uint32_t  first;
       uint32_t  baseInstance;
    };

    struct DrawElementsIndirectBuffer {
        uint32_t    count;
        uint32_t    instanceCount;
        uint32_t    firstIndex;
        uint32_t    baseVertex;
        uint32_t    baseInstance;
    };

    class ParticleComputeTask: public ComputeTask {


    };

    struct CLCamera {
        float planes[4][6];
        float viewProj[4][4];
    };


    class LuminanceHistogramComputeTask: public ComputeTask {
    public:

        struct LuminanceStats {
            uint32_t max;
            uint32_t min;
            uint32_t histBins[256];
            float    exp        = 1.0f;
            float    prevExp    = 1.0f;
        }               _lumStats;

        LuminanceHistogramComputeTask(uintptr_t textureId);

        ComputeEvent    _readCompleteEvent;
        ComputeBuffer*  _statsBuffer        = nullptr;
        ComputeBuffer*  _fboBuffer          = nullptr;
        float           _exposure           = 1.0f;
    };



    class LightCullingComputeTask: public ComputeTask {
    public:

                        LightCullingComputeTask(uintptr_t depthBuffTexId,
                                                uintptr_t lightingUboId);

        void            setSampleDepthBuffer(bool val=true);
        bool            isSamplingDepthBuffer(void) const;
        bool            updateScreenResolution(unsigned int width, unsigned int height);

        bool            _sampleDepthBuffer      = false;
        uint32_t        _tileSize               = ELYSIAN_RENDER_SYSTEM_LIGHT_CULLING_TILE_SIZE_DEFAULT;
        uint32_t        _lightMapTexId           = 0;
        ComputeEvent    _completeEvent;

        ComputeBuffer*  _tileSizeBuffer         = nullptr;
        ComputeBuffer*  _depthBuffer            = nullptr;
        ComputeBuffer*  _lightMapBuff           = nullptr;
        ComputeBuffer*  _lightingUboBuff        = nullptr;
        ComputeBuffer*  _projViewMatBuff        = nullptr;
    };

    class RendererThread: public Thread {

    };

    class RendererComputeGlobalDataManager {
    protected:
        static ComputeBuffer*   _cameraBuffer;
        static const Camera*    _camera;
      //  static ComputeBuffer* _frameUbo;
      //  static ComputeBuffer* _mapVbo;


    };

    class RenderSystemBase: public System, public EventObserver, public RendererComputeGlobalDataManager {
    private:

    #ifdef ELYSIAN_RENDER_SYSTEM_DEBUG
        struct SceneStats {
            size_t totalSceneObjs 		= 0;
            size_t opaqueSceneObjs 		= 0;
            size_t transparentSceneObjs	= 0;
            size_t culledSceneObjs      = 0;
            size_t batchCount           = 0;
            size_t avgBatchSize         = 0;
            size_t renderCalls          = 0;
            size_t sceneRenderPasses	= 0;
        } _prevSceneStats, _curSceneStats;
    #endif

        Thread*                 _rendererThread                 = nullptr;

        //Particles
        size_t                  _activeParticles                = 0;
        ComputeTask*            _particleComputeTask            = nullptr;
        LuminanceHistogramComputeTask* _luminanceComputeTask    = nullptr;
#if 0
        ComputeDevice*          _particleDevice                 = nullptr;
        ComputeProgram*         _particleVboProgram             = nullptr;
        ComputeKernel*          _particleVboKernel              = nullptr;
#else
        ComputeBuffer*          _particleInBuffer               = nullptr;
        ComputeBuffer*          _particleVboOutBuffer           = nullptr;
        ComputeBuffer*          _particleDrawIndirectOutBuffer  = nullptr;
        ComputeBuffer*          _particleViewProjInBuffer       = nullptr;
#endif
        ComputeBuffer*          _fboBuffer                      = nullptr;
        ComputeBuffer*          _lumStatsBuffer                 = nullptr;
        ComputeBuffer*          _lumStatsProgram                = nullptr;
        ComputeBuffer*          _lumStatsKernel                 = nullptr;
        bool                    _automaticExposureActive        = false;

        bool                    _transparentGeometryEnabled     = true;


        LightCullingComputeTask*    _lightCullComputeTask       = nullptr;

        PopulateMapIboComputeTask*  _populateMapIboComputeTask  = nullptr;

        bool                    _depthTest                      = true;
        GY_VID_DEPTH_MODE       _depthMode                      = GY_VID_DEPTH_LESS;
        GYVidPipelineState      _pipelineState;


    protected:
       // void                    _setParticleAccelerationAvailable(bool val);
        bool                    _initializeAutomaticExposureKernel(void);
        bool                    _uninitializeAutomaticExposureKernel(void);
        void                    _onViewportSizeChange(void);
        void                    _onAreaChange(void) const;
        bool                    _initializeComputeGlobalData(void) const;
        bool                    _uninitializeComputeGlobalData(void) const;

    public:


        PerformanceMonitor renderParticlesMonitor             = PerformanceMonitor("Render Particles");
        PerformanceMonitor updateHDRExposureMonitor           = PerformanceMonitor("Update HDR Exposure");

                                RenderSystemBase(void);


        bool                    initialize(void);
        bool                    uninitialize(void);
        void                    render(void);
        bool                    forwardPlusCullLights(void);
        void                    createLuminanceHistogram(void);
        void                    updateHDRExposure(void);
        bool                    populateParticleVbos(void);
        bool                    renderParticles(void);

        bool                    updateCamera(const Camera* cam) const;

        //newfangled

        void                    beginScene(void);
        void                    endScene(void);
        void                    enableTransparentGeometry(bool val=true);
        bool                    isTransparentGeometryEnabled(void) const;
        void                    renderScene(void);

        void                    setPipelineStateFromAttrib(const RenderableObjectPipelineAttrib& attrib);
        auto                    getPipelineState(void) const -> const GYVidPipelineState&;

        void                    setDepthMode(bool enabled=true, GY_VID_DEPTH_MODE mode=GY_VID_DEPTH_LESS);


#ifdef GY_VIDEO_DEBUG
        bool                    dumpStats(void) const;
#endif

        Thread*                 getRendererThread(void) const;

        //Overridden from EventObserver
        virtual void            onObserveEvent(Event* event, EventTarget* target) override;

    public:


#ifdef ELYSIAN_RENDER_SYSTEM_DEBUG
        const SceneStats& getSceneStats(void) const;
#endif


    };





    //========== INLINEZ ===========
    inline Thread* RenderSystemBase::getRendererThread(void) const { return _rendererThread; }




    //=== RenderPipelineState =====
    inline RenderableObjectPipelineAttrib::RenderableObjectPipelineAttrib(void):
       _blendSrcFactor(GY_BLEND_NONE),
       _blendDstFactor(GY_BLEND_NONE)
    {}

    inline bool RenderableObjectPipelineAttrib::hasAlpha(void) const {
        return (_blendSrcFactor != GY_BLEND_NONE && _blendDstFactor != GY_BLEND_NONE);
    }
    inline bool RenderableObjectPipelineAttrib::isOpaque(void) const {
        return (_blendSrcFactor == GY_BLEND_NONE || _blendDstFactor == GY_BLEND_NONE);
    }

    inline void RenderableObjectPipelineAttrib::setBlendFactors(GY_BLEND_FACTOR src, GY_BLEND_FACTOR dst) {
        _blendSrcFactor = src;
        _blendDstFactor = dst;
    }

    inline BLEND_MODE RenderableObjectPipelineAttrib::getBlendMode(void) const {
        for(unsigned i = 0; i < BLEND_MODE_COUNT; ++i) {

            if(_blendFactors[i].src == _blendSrcFactor &&
                _blendFactors[i].dst == _blendDstFactor)
            {
                return static_cast<BLEND_MODE>(i);
            }
        }
        return BLEND_MODE_CUSTOM;
    }

    inline void RenderableObjectPipelineAttrib::setBlendMode(BLEND_MODE mode) {
        assert(mode >= 0 && mode < BLEND_MODE_COUNT);
        _blendSrcFactor = _blendFactors[mode].src;
        _blendSrcFactor = _blendFactors[mode].dst;
    }

    inline GY_BLEND_FACTOR RenderableObjectPipelineAttrib::getSrcBlendFactor(void) const {
        return static_cast<GY_BLEND_FACTOR>(_blendSrcFactor);
    }

    inline GY_BLEND_FACTOR RenderableObjectPipelineAttrib::getDstBlendFactor(void) const {
        return static_cast<GY_BLEND_FACTOR>(_blendDstFactor);
    }

    inline GYTexture* RenderableObjectPipelineAttrib::getTexture(void) const {
        return _texture;
    }

    inline void RenderableObjectPipelineAttrib::setTexture(GYTexture* tex) {
        _texture = tex;
    }



    inline void RenderSystemBase::setPipelineStateFromAttrib(const RenderableObjectPipelineAttrib& attrib) {
        if(attrib.hasAlpha()) {
            _pipelineState.alphaLevel       = GY_ALPHA_FULL;
            _pipelineState.srcBlendFactor   = attrib.getSrcBlendFactor();
            _pipelineState.dstBlendFactor   = attrib.getDstBlendFactor();
            _pipelineState.depthTestEnabled = false;
        } else {
            _pipelineState.alphaLevel       = GY_ALPHA_NONE;
            _pipelineState.depthTestEnabled = _depthTest;
            _pipelineState.depthTestMode    = _depthMode;
        }

        _pipelineState.texture = attrib.getTexture();

        gyVidPipelineState(&_pipelineState);

    }

    inline RenderSystemBase::RenderSystemBase(void):
        _pipelineState({
            0,
            nullptr,
            nullptr,
            nullptr,
            GY_ALPHA_NONE,
            GY_BLEND_NONE,
            GY_BLEND_NONE,
            1,
            GY_VID_DEPTH_LEQUAL,
            {0, 0, 0, 0}})
    {}


    inline auto RenderSystemBase::getPipelineState(void) const -> const GYVidPipelineState& {
        return _pipelineState;
    }

    inline void RenderSystemBase::setDepthMode(bool enabled, GY_VID_DEPTH_MODE mode) {
        _depthTest = enabled;
        _depthMode =  mode;
    }

}





#endif // ELYSIAN_RENDER_SYSTEM_BASE_HPP

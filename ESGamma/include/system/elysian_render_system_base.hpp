#ifndef ELYSIAN_RENDER_SYSTEM_BASE_HPP
#define ELYSIAN_RENDER_SYSTEM_BASE_HPP

#include <system/elysian_system.hpp>
#include <system/elysian_compute_system.hpp>
#include <gyro_matrix_api.h>
#include <gyro_video_api.h>


#define ELYSIAN_RENDER_SYSTEM_PARTICLE_VBO_KERNEL_VERTEX_MAX    10000
#define ELYSIAN_RENDER_SYSTEM_PARTICLE_VBO_KERNEL_PATH          "/assets/opencl/particle_vbo.cl"
#define ELYSIAN_RENDER_SYSTEM_PARTICLE_VBO_KERNEL_NAME          "PopulateParticleVbo"
#define ELYSIAN_RENDER_SYSTEM_LUMINANCE_HISTOGRAM_KERNEL_PATH   "/assets/opencl/luminance_histogram.cl"
#define ELYSIAN_RENDER_SYSTEM_LUMINANCE_HISTOGRAM_KERNEL_NAME   "CreateGlobalLuminanceHistogram"
#define ELYSIAN_RENDER_SYSTEM_LIGHT_CULLING_KERNEL_NAME         "CullLights"
#define ELYSIAN_RENDER_SYSTEM_LIGHT_CULLING_KERNEL_PATH         "/assets/opencl/light_culling.cl"

namespace elysian {

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

    class ParticleComputeTask: public ComputeTask {


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

                        LightCullingComputeTask(uintptr_t depthBuffTexId);

    private:

        ComputeBuffer*  _depthBuffer         = nullptr;
        ComputeBuffer*  _lightingUbo         = nullptr;


    };


    class RenderSystemBase: public System {
    public:


    private:

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


        LightCullingComputeTask*    _lightCullComputeTask       = nullptr;

    protected:
       // void                    _setParticleAccelerationAvailable(bool val);
        bool                    _initializeAutomaticExposureKernel(void);
        bool                    _uninitializeAutomaticExposureKernel(void);

    public:
        bool                    initialize(void);
        bool                    uninitialize(void);
        void                    render(void);
        void                    postRender(void);
        bool                    populateParticleVbos(void);
        bool                    renderParticles(void);


    };





    //========== INLINEZ ===========



}


#endif // ELYSIAN_RENDER_SYSTEM_BASE_HPP

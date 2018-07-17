#ifndef ELYSIAN_RENDER_SYSTEM_BASE_HPP
#define ELYSIAN_RENDER_SYSTEM_BASE_HPP

#include <system/elysian_system.hpp>
#include <gyro_matrix_api.h>
#include <gyro_video_api.h>

#define ELYSIAN_RENDER_SYSTEM_PARTICLE_VBO_KERNEL_VERTEX_MAX    1000000
#define ELYSIAN_RENDER_SYSTEM_PARTICLE_VBO_KERNEL_PATH          "/assets/opencl/particle_vbo.cl"
#define ELYSIAN_RENDER_SYSTEM_PARTICLE_VBO_KERNEL_NAME          "PopulateParticleVbo"

namespace elysian {

    class ComputeDevice;
    class ComputeContext;
    class ComputeCommandQueue;
    class ComputeBuffer;
    class ComputeProgram;
    class ComputeKernel;

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

    class RenderSystemBase: public System {

    private:
        ComputeCommandQueue*    _renderCmdQueue                 = nullptr;

        //Particles
        bool                    _particleAccelerationAvailable  = false;
        bool                    _useParticleAcceleration        = false;
        size_t                  _activeParticles                = 0;

        ComputeDevice*          _particleDevice                 = nullptr;
        ComputeBuffer*          _particleInBuffer               = nullptr;
        ComputeBuffer*          _particleVboOutBuffer           = nullptr;
        ComputeBuffer*          _particleDrawIndirectOutBuffer  = nullptr;
        ComputeBuffer*          _particleViewProjInBuffer       = nullptr;
        ComputeProgram*         _particleVboProgram             = nullptr;
        ComputeKernel*          _particleVboKernel              = nullptr;

        ComputeBuffer*          _fboBuffer                      = nullptr;
        ComputeBuffer*          _lumStatsBuffer                 = nullptr;
        ComputeBuffer*          _lumStatsProgram                = nullptr;
        ComputeBuffer*          _lumStatsKernel                 = nullptr;
        bool                    _automaticExposureActive        = false;

    protected:
        void                    _setParticleAccelerationAvailable(bool val);
        bool                    _initializeAutomaticExposureKernel(void);
        bool                    _uninitializeAutomaticExposureKernel(void);

    public:
        bool                    initialize(void);
        bool                    uninitialize(void);
        void                    render(void);
        bool                    populateParticleVbos(void);
        bool                    renderParticles(void);

        bool                    isParticleAccelerationAvailable(void) const;
        bool                    getUseParticleAcceleration(void) const;
        void                    setUseParticleAcceleration(bool val=true);
    };





    //========== INLINEZ ===========

    inline bool RenderSystemBase::isParticleAccelerationAvailable(void) const {
        return _particleAccelerationAvailable;
    }

    inline bool RenderSystemBase::getUseParticleAcceleration(void) const {
        return _useParticleAcceleration;
    }

    inline void RenderSystemBase::setUseParticleAcceleration(bool val) {
        _useParticleAcceleration = _particleAccelerationAvailable? val : false;
    }

    inline void RenderSystemBase::_setParticleAccelerationAvailable(bool val) {
        _particleAccelerationAvailable = val;
    }

}


#endif // ELYSIAN_RENDER_SYSTEM_BASE_HPP

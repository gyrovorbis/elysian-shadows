#ifndef ELYSIAN_RENDER_SYSTEM_HPP
#define ELYSIAN_RENDER_SYSTEM_HPP

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
        float  pos[4];
        float   size[2];
        GYTexCoord2 texCoord;
        float   orient;
        GYColor4   color;
    };

    struct DrawIndirectBuffer {
       uint32_t  count;
       uint32_t  instanceCount;
       uint32_t  first;
       uint32_t  baseInstance;
    };

    class RenderSystem: public System {

    private:
        ComputeCommandQueue*    _renderCmdQueue                 = nullptr;

        //Particles
        bool                    _computeAccelerated             = false;

        ComputeDevice*          _particleDevice                 = nullptr;
        ComputeBuffer*          _particleInBuffer               = nullptr;
        ComputeBuffer*          _particleVboOutBuffer           = nullptr;
        ComputeBuffer*          _particleDrawIndirectOutBuffer  = nullptr;
        ComputeBuffer*          _particleViewProjInBuffer       = nullptr;
        ComputeProgram*         _particleVboProgram             = nullptr;
        ComputeKernel*          _particleVboKernel              = nullptr;


    public:
            bool initialize(void);
            bool uninitialize(void);
            void render(void);
            bool populateParticleVbos(void);
            bool renderParticles(void);
    };

#if 0
    void RenderSystem::populateParticleVbos(void) {
        auto& emitters = getCurrentArea()->_particleEmitters;
        unsigned emitterCount = emitters.size();

        for(unsigned i = 0; i < emitterCount; ++i) {
            auto* curEmitter = emitters[i];
        }

    }
#endif

}


#endif // ELYSIAN_RENDER_SYSTEM_HPP

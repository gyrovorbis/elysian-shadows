#include <system/elysian_render_system.hpp>
#include <system/elysian_compute_system.hpp>
#include <crtp/elysian_debug.hpp>
#include <gyro_video_api.h>
#include <crtp/elysian_area.hpp>
#include <crtp/elysian_particle_emitter.hpp>
#include <crtp/elysian_camera_system.hpp>
#include <glal/gyro_video_glal.h>
#include <glal/gyro_gl.h>
#include <glal/gyro_shaders_glal.h>

extern int glalVertexArray;
extern GLuint                      _streamVao[2];

namespace elysian {

GLuint _glParticleVao           = 0;
GLuint _glParticleVbo           = 0;
GLuint _glParticleIndirectBuff  = 0;



bool RenderSystem::initialize(void) {
    bool success = true;

    dbgprintf("RenderSystem::initialize()");
    dbgpush();

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
    glVertexAttribPointer(glalLightingShader.attribLoc[GLAL_SHADER_TANGENT], 3, GL_FLOAT, GL_TRUE, sizeof(GYVertex), (void*)(10*sizeof(float)));
    glVertexAttribPointer(glalLightingShader.attribLoc[GLAL_SHADER_NORMAL_Z], 1, GL_FLOAT, GL_TRUE, sizeof(GYVertex), (void*)(13*sizeof(float)));

    _particleDevice = getComputeSystem()->getBuiltinComputeDevice(ComputeSystem::COMPUTE_DEVICE_GPU_RENDERER_ACTIVE);
    if(_particleDevice) {
        ComputeContext* ctx = _particleDevice->getContext();
        _renderCmdQueue = ctx->createCommandQueue(_particleDevice, 0);
        if(_renderCmdQueue) {
            _particleVboOutBuffer = ctx->createBufferFromRendererBuffer(_glParticleVbo, GY_COMPUTE_MEMORY_TYPE_WRITE_ONLY);
            _particleInBuffer = ctx->createBuffer(GY_COMPUTE_MEMORY_TYPE_READ_ONLY, sizeof(GYVertex)*ELYSIAN_RENDER_SYSTEM_PARTICLE_VBO_KERNEL_VERTEX_MAX, nullptr);
            _particleDrawIndirectOutBuffer = ctx->createBufferFromRendererBuffer(_glParticleIndirectBuff, GY_COMPUTE_MEMORY_TYPE_READ_WRITE);
            _particleViewProjInBuffer = ctx->createBuffer(GY_COMPUTE_MEMORY_TYPE_READ_ONLY, sizeof(GYMatrix4), nullptr);
            if(_particleVboOutBuffer && _particleInBuffer && _particleDrawIndirectOutBuffer && _particleViewProjInBuffer) {
                _particleVboProgram = ctx->loadProgram(ELYSIAN_RENDER_SYSTEM_PARTICLE_VBO_KERNEL_PATH);
                if(_particleVboProgram) {
                    if(_particleVboProgram->compile(_particleDevice, 1, nullptr)) {
                        _particleVboKernel = _particleVboProgram->createKernel(ELYSIAN_RENDER_SYSTEM_PARTICLE_VBO_KERNEL_NAME);
                        if(_particleVboKernel) {
                            bool success = true;
                            success &= _particleVboKernel->setArg(1, _particleInBuffer);
                            success &= _particleVboKernel->setArg(2, _particleViewProjInBuffer);
                            success &= _particleVboKernel->setArg(3, _particleDrawIndirectOutBuffer);
                            success &= _particleVboKernel->setArg(4, _particleVboOutBuffer);

                            if(success) _computeAccelerated = true;
                        }
                    }
                }
            }
        }
    }
    dbgpop();
    return success;
}

bool RenderSystem::uninitialize(void) {
    delete _particleVboOutBuffer;
    delete _renderCmdQueue;
    return true;
}

#define PARTICLE_CL

bool RenderSystem::populateParticleVbos(void) {
    static KernelParticle kernParts[ELYSIAN_RENDER_SYSTEM_PARTICLE_VBO_KERNEL_VERTEX_MAX/6];
    static GYVertex verts[ELYSIAN_RENDER_SYSTEM_PARTICLE_VBO_KERNEL_VERTEX_MAX];
    unsigned partCount = 0;

    if(gyVidShadowPass()) return true;
    //glFlush();
    for(unsigned i = 0; i < getCurrentArea()->particleEmitters.size(); ++i) {
        auto* emitter = getCurrentArea()->particleEmitters[i];

        for(auto it = emitter->_pool.begin(); it < emitter->_pool.end(); ++it) {
            KernelParticle* part = &kernParts[partCount++];
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
#ifndef PARTICLE_CL
        emitter->render();
#endif
    }

#ifndef PARTICLE_CL
    partCount = 0;
#endif
    if(partCount) {

        auto* wgInfo = _particleDevice->getKernelWorkGroupInfo(_particleVboKernel);

        bool success = true;
        size_t offset = 0;
        size_t globalWorkSize = partCount;

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

        glFinish();
        success &= _renderCmdQueue->enqueueAcquireRendererSharedObject(2, sharedBuffs);
        success &= _renderCmdQueue->enqueueBufferWrite(_particleViewProjInBuffer, 0, sizeof(GYMatrix4), getCameraSys()->getCurrentState()->projViewMat, false);
        success &= _renderCmdQueue->enqueueBufferWrite(_particleInBuffer, 0, sizeof(KernelParticle)*partCount, kernParts, false);
        //success &= _particleVboKernel->setArg(1, _particleInBuffer);
        //success &= _particleVboKernel->setArg(2, _particleViewProjInBuffer);
        //success &= _particleVboKernel->setArg(3, _particleDrawIndirectOutBuffer);
        //success &= _particleVboKernel->setArg(4, _particleVboOutBuffer);
        success &= _particleVboKernel->setArg(0, sizeof(uint32_t), &partCount);
        success &= _particleVboKernel->setArg(5, sizeof(int32_t)*2, viewportSize);
        //success &= _renderCmdQueue->enqueue
        success &= _renderCmdQueue->enqueueKernel(_particleVboKernel, 1, &offset, &globalWorkSize, &localWorkSize);
        //DrawIndirectBuffer indirectBuff;
        //success &= _renderCmdQueue->enqueueBufferRead(_particleDrawIndirectOutBuffer, 0, sizeof(DrawIndirectBuffer), &indirectBuff, true);
        success &= _renderCmdQueue->enqueueReleaseRendererSharedObject(2, sharedBuffs);


        //if(success) renderParticles();
        //dbgprintf("global - %u, local - %u", globalWorkSize, localWorkSize);



      //  glDrawArraysIndirect(GL_TRIANGLE_STRIP, 0);
     //   if(success) {
      //      dbgprintf("WHOOO!");
       // } else errprintf("REEEEST!!!");
    }


}

//#define RENDER_CL_CPU

bool RenderSystem::renderParticles(void) {
    DrawIndirectBuffer indirectBuff;
    static GYVertex verts[ELYSIAN_RENDER_SYSTEM_PARTICLE_VBO_KERNEL_VERTEX_MAX];
    ComputeBuffer* sharedBuffs[] = {
        _particleVboOutBuffer,
        _particleDrawIndirectOutBuffer
    };
    bool success = true;
                    success &= _renderCmdQueue->finish();
#ifdef RENDER_CL_CPU
                            success &= _renderCmdQueue->enqueueBufferRead(_particleDrawIndirectOutBuffer, 0, sizeof(DrawIndirectBuffer), &indirectBuff, true);
                success &= _renderCmdQueue->enqueueBufferRead(_particleVboOutBuffer, 0, sizeof(GYVertex)*indirectBuff.count, verts, true);
#endif



#ifndef PARTICLE_CL
    for(unsigned i = 0; i < getCurrentArea()->particleEmitters.size(); ++i) {
        auto* emitter = getCurrentArea()->particleEmitters[i];
        emitter->render();
    }
#else



    if(success) {
        //errprintf("%u verts, %u parts", indirectBuff.count, partCount);
        if(/*indirectBuff.count &&*/ !gyVidShadowPass()) {
            //gyVidVertexBatchFlush();

            //for(unsigned i = 0; i < indirectBuff.count; ++i) {
            //   GYVertex* cur = &verts[i];
             //   dbgprintf("vert[%u]: %f, pos <%f, %f, %f> color <%u, %u, %u, %u>", i, cur->normalZ, cur->pos.x, cur->pos.y, cur->pos.z, cur->color.r, cur->color.g, cur->color.b, cur->color.a);
           // }

            gyVidTexBind(nullptr);

            gyVidNormalMapBind(nullptr);
            gyVidSpecularMapBind(nullptr);

            gyVidMaterialBind(nullptr);
            gyVidAlphaLevel(GY_ALPHA_FULL);

            gyVidBlendMode(GY_BLEND_ONE_MINUS_SRC_ALPHA);
            //gyVidDepthTest(false);
#ifndef RENDER_CL_CPU
            glBindVertexArray(_glParticleVao);
             //       glBindBuffer(GL_ARRAY_BUFFER, _glParticleVbo);
            glBindBuffer(GL_DRAW_INDIRECT_BUFFER, _glParticleIndirectBuff);
            glDrawArraysIndirect(GL_TRIANGLE_STRIP, 0);
            glBindBuffer(GL_DRAW_INDIRECT_BUFFER, 0);
            glBindVertexArray(_streamVao[0]);

#else
            gyVidTriangleStripVertices(verts, indirectBuff.count);
#endif
            //glBindBuffer(GL_DRAW_INDIRECT_BUFFER, 0);
            //gyVidDepthTest(true);


        }

    } //else errprintf("REEST");
#endif
}


}

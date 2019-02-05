#include <AssetIO/level/elysian_opaque_map_scene_object.hpp>
#include <ESGamma/crtp/elysian_render_system.hpp>
#include <AssetIO/crtp/elysian_area.hpp>
#include <ESGamma/crtp/elysian_camera_system.hpp>

#include <libGyro/gyro_video_api.h>
#include <gl/gyro_video_gl.h>
#include <gl/gyro_gl_utils.h>

namespace elysian {

PopulateMapIboComputeTask::PopulateMapIboComputeTask():
    ComputeTask(ELYSIAN_RENDER_SYSTEM_POPULATE_MAP_IBO_KERNEL_NAME,
{
     ELYSIAN_RENDER_SYSTEM_POPULATE_MAP_IBO_KERNEL_PATH
},
{
    //ComputeDeviceCategory::GpuRendererOffline,
    ComputeDeviceCategory::GpuRendererOnline,
    //ComputeDeviceCategory::CpuRenderer
},
nullptr,
true){
    DbgEvent event("PopulateMapIboComputeTask - Initializing Kernel");
    bool success = true;

    if(isValid()) {

        //GL ERROR CHECK
        GY_GL_CHECK_ERROR();
        GLuint buffer;
        glGenBuffers(1, &buffer);
        _glDrawIndirectBuffer = buffer;
        glBindBuffer(GL_DRAW_INDIRECT_BUFFER, _glDrawIndirectBuffer);
        glBufferData(GL_DRAW_INDIRECT_BUFFER, sizeof(DrawElementsIndirectBuffer), nullptr, GL_DYNAMIC_DRAW);
        glBindBuffer(GL_DRAW_INDIRECT_BUFFER, 0);
        GY_GL_CHECK_ERROR();

        _drawIndirectBuffer = getContext()->createBufferFromRendererBuffer(_glDrawIndirectBuffer, GY_COMPUTE_MEMORY_TYPE_READ_WRITE);

        success &= (_drawIndirectBuffer != nullptr);

        success &= getKernel()->setArg(0, _drawIndirectBuffer);
        success &= getKernel()->setArg(3, _tilesPerWorkItem);
        success &= getKernel()->setArg(6, _cameraBuffer);

        if(!success) setValid(false);

    }
}

PopulateMapIboComputeTask::~PopulateMapIboComputeTask(void) {
    delete _mapIboBuffer;
    delete _mapVboBuffer;
    delete _drawIndirectBuffer;
}

void PopulateMapIboComputeTask::setTilesPerWorkItem(uint32_t tiles) {
    bool success = getKernel()->setArg(3, tiles);
    if(!success) setValid(false);
    _tilesPerWorkItem = tiles;
}


bool PopulateMapIboComputeTask::waitForIboFilled(void) const {
    return finish();//true;//finish();//getCommandQueue()->finish();//true;//_completeEvent.wait();

}

void PopulateMapIboComputeTask::updateArea(Area* area) {
    if(!area) return;

    delete _mapIboBuffer;
    delete _mapVboBuffer;

    bool success = true;

    if(isValid()) {

        _mapIboBuffer = getContext()->createBufferFromRendererBuffer(area->elementVbo[0], GY_COMPUTE_MEMORY_TYPE_WRITE_ONLY);
        _mapVboBuffer = getContext()->createBufferFromRendererBuffer(area->vertexVbo, GY_COMPUTE_MEMORY_TYPE_READ_ONLY);
        _width = static_cast<uint32_t>(area->width);
        _height = static_cast<uint32_t>(area->height);

        success &= (_mapIboBuffer && _mapVboBuffer);

        success &= getKernel()->setArg(1, _width);
        success &= getKernel()->setArg(2, _height);
        success &= getKernel()->setArg(4, _mapIboBuffer);
        success &= getKernel()->setArg(5, _mapVboBuffer);

        if(!success) setValid(false);

    }

}




bool PopulateMapIboComputeTask::populateIbo(void) {
    bool success = true;

    if(isValid()) {
        ComputeBuffer* sharedObjs[3] = {
            _drawIndirectBuffer,
            _mapIboBuffer,
           _mapVboBuffer
        };

        static CLCamera clCam, clCam2;
        memcpy(clCam.planes, _camera->viewProjVolume.planes, sizeof(float)*4*6);
        memcpy(clCam.viewProj, _camera->projViewMat, sizeof(float)*4*4);

        success &= enqueueBufferWrite(_cameraBuffer, 0, sizeof(CLCamera), &clCam, false);
        success &= enqueueAcquireRendererSharedObject(2, sharedObjs);
        success &= enqueueKernel(_width * _height * ELYSIAN_AREA_MAP_LAYERS);
        success &= enqueueReleaseRendererSharedObject(2, sharedObjs, nullptr);


#if 0
        DrawElementsIndirectBuffer drawInd;
        uint32_t indices[100];
        success &= enqueueBufferRead(_drawIndirectBuffer, 0, sizeof(DrawElementsIndirectBuffer), &drawInd);
        //success &= enqueueBufferRead(_mapIboBuffer, 0, sizeof(uint32_t)*100, indices);

        //success &= enqueueBufferRead(_cameraBuffer, 0, sizeof(CLCamera), &clCam2);


        static size_t prev = 0;
        size_t next = drawInd.count;
        if(prev != next) {
        dbgprintf("*****POPULATE IBO RESULTS!*****");
        dbgprintf("cout = %d, instCount = %d, firstIndex = %d, baseVert = %d, baseInst = %d", drawInd.count, drawInd.instanceCount, drawInd.firstIndex, drawInd.baseVertex, drawInd.baseInstance);
            prev = next;
        }

        /*
        for(unsigned i = 0; i < 20; ++i) {
            dbgprintf("Index[%u] = %u", i, indices[i]);
        }

        for(unsigned i = 0; i < 6; ++i) {
            for(unsigned j = 0; j < 4; ++j) {
                const GYPlane* vec = &_camera->viewProjVolume.planes[i];
                float pval[4] = {
                    vec->x,
                    vec->y,
                    vec->z,
                    vec->w
                };

                dbgprintf("Plane[%u][%u] = %f, %f, ", i, j, clCam.planes[j][i], pval[j]);
            }
        }

        dbgprintf("*****POPULATE IBO RESULTS!*****");*/

#endif


        if(!success) setValid(false);


    } else success = false;

    return success;

}





}

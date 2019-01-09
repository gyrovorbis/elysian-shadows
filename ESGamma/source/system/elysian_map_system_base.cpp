#include <ESGamma/system/elysian_map_system_base.hpp>
#include <gl/gyro_gl.h>
#include <gl/gyro_video_gl.h>
#include <gl/gyro_shaders_gl.h>



namespace elysian {

int _mapCount = 0;


void GLMapSystemCrtp::_setDepthPrepassState(DEPTH_PASS_STATE value) {
    if(value == DEPTH_Z_PREPASS) {
        //gyVidDepthTest(1);
        glDepthMask(1);
        glColorMask(0, 0, 0, 0);
        //glEnable(GL_DEPTH_TEST);
        //glDepthFunc(GL_LESS);

        gyVidVertexBatchEnd();
        gyVidVertexBatchFlush();
        gyVidSetCurrentShaderProgram(glalShadowMapShader.program);
        glalCurLightShader = &glalShadowMapShader;

        gyVidAlphaLevel(GY_ALPHA_NONE);
    } else if(value == LIGHTING_PASS){
        gyVidVertexBatchEnd();
        gyVidVertexBatchFlush();
        gyVidSetDefaultShaderProgram();
        //glalDefaultShadersLoadUniforms();
        glalCurLightShader = &glalLightingShader;
        //glDepthMask(0);
        //glColorMask(1, 1, 1, 1);
        glDepthFunc(GL_LEQUAL);
    } else if(value == POST_PASSES) {
        //gyVidDepthTest(1);
        gyVidVertexBatchEnd();
        gyVidVertexBatchFlush();
        //glDepthMask(1);
       // glDepthFunc(GL_LEQUAL);
    }
    _depthPassState = value;

}

void GLMapSystemCrtp::_bindArrayVbo(int buffer) {
#ifdef GYRO_GLAL
  //glBindBuffer(GL_ARRAY_BUFFER, buffer);
#endif
}

void GLMapSystemCrtp::deactivateArea(void) {
   // gyVidVertexBatchFlush();
   // gyVidVertexBatchEnd();
}

void GLMapSystemCrtp::_orphanVbo(void) {
#ifdef GYRO_GLAL
  //  if(!_elements) return;
    //glFlushMappedBufferRange(GL_ELEMENT_ARRAY_BUFFER, _lastOffset, );
  //  glFlushBuffer
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint)*ELYSIAN_AREA_GLAL_STRIP_INDEX_ARRAY_SIZE, nullptr, ELYSIAN_AREA_ELEMENT_ARRAY_VBO_TYPE);
#endif
}


void GLMapSystemCrtp::_drawElements(void) {
    if(!_elements) return;
    if(_size-_lastSize == 0) return;
#if GYRO_PLATFORM != GYRO_PLATFORM_DC && GYRO_PLATFORM != GYRO_PLATFORM_ANDROID
#if 1
    GLuint* ptr = (GLuint*)glMapBufferRange(GL_ELEMENT_ARRAY_BUFFER, _lastOffset, sizeof(GLuint)*(_size-_lastSize), /*GL_MAP_INVALIDATE_RANGE_BIT | GL_MAP_FLUSH_EXPLICIT_BIT |*/ GL_MAP_WRITE_BIT | GL_MAP_UNSYNCHRONIZED_BIT);
    if(ptr) {
        memcpy(ptr, &_elements[_lastSize], sizeof(GLuint)*(_size-_lastSize));
        //glFlushMappedBufferRange(GL_ELEMENT_ARRAY_BUFFER, _lastOffset, sizeof(GLuint)*(_size-_lastSize));
        glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);
#if 0
        qDebug() << _mapCount++ << " GLMAPBUFFERRANGE SIZE - " << _size-_lastSize;
#endif
        // glDrawElements(GL_TRIANGLE_STRIP, elemArraySize, GL_UNSIGNED_SHORT, elemArray /*nullptr*/);


                 //glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLushort)*elemArraySize, elemArray, GL_STREAM_DRAW);
        // glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLushort)*elemArraySize, elemArray, GL_STREAM_DRAW);
         glDrawElements(GL_TRIANGLE_STRIP, (_size-_lastSize), GL_UNSIGNED_INT, (const GLvoid*)_lastOffset);
     }

#else
    glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, _lastOffset, sizeof(GLuint)*(_size-_lastSize), &_elements[_lastSize]);
    glDrawElements(GL_TRIANGLE_STRIP, (_size-_lastSize), GL_UNSIGNED_INT, (const GLvoid*)_lastOffset);
#endif
#endif
}


//safety mechanism to keep shit from accidentally winding up off of the maps!
void MapSystemBase::update(void) {
    MapSystemCrtp<MapSystem, MapDepthBuffEntryImpl, EntityDepthBuffEntryImpl>::update();
    for(size_t i = 0; i < _curArea->entities.size(); ++i) {
        auto* entity = _curArea->entities[i];
        if(entity->getCollidable()) {
            auto& xform = entity->getTransform();
            if(xform.getPosX() < 0) xform.setPosX(0);
            else if(xform.getPosX() > (_curArea->width-1) * ELYSIAN_TILE_WIDTH) xform.setPosX((_curArea->width-1)*ELYSIAN_TILE_WIDTH);
            if(xform.getPosY() < 0) xform.setPosY(0);
            else if(xform.getPosY() > (_curArea->height-1) * ELYSIAN_TILE_HEIGHT) xform.setPosY((_curArea->width-1)*ELYSIAN_TILE_HEIGHT);
        }

    }





}


}

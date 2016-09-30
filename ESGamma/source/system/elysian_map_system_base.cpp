#include "elysian_map_system_base.hpp"
#include <glal/gyro_gl.h>



namespace elysian {

int _mapCount = 0;

void GLMapSystemCrtp::_bindArrayVbo(int buffer) {
#ifdef GYRO_GLAL
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
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


}

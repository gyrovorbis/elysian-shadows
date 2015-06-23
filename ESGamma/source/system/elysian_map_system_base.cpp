#include "elysian_map_system_base.hpp"
#include <glal/gyro_gl.h>


namespace elysian {

void GLMapSystemBase::_bindArrayVbo(int buffer) {
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
}

void GLMapSystemBase::deactivateArea(void) {
   // gyVidVertexBatchFlush();
   // gyVidVertexBatchEnd();
}

void GLMapSystemBase::_orphanVbo(void) {
  //  if(!_elements) return;
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint)*ELYSIAN_AREA_GLAL_STRIP_INDEX_ARRAY_SIZE, nullptr, GL_STREAM_DRAW);
}


void GLMapSystemBase::_drawElements(void) {
    if(!_elements) return;
    GLuint* ptr = (GLuint*)glMapBufferRange(GL_ELEMENT_ARRAY_BUFFER, _lastOffset, sizeof(GLuint)*(_size-_lastSize), GL_MAP_INVALIDATE_RANGE_BIT /*| GL_MAP_FLUSH_EXPLICIT_BIT*/ | GL_MAP_WRITE_BIT | GL_MAP_UNSYNCHRONIZED_BIT);
    if(ptr) {memcpy(ptr, &_elements[_lastSize], sizeof(GLuint)*(_size-_lastSize));
     glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);

    // glDrawElements(GL_TRIANGLE_STRIP, elemArraySize, GL_UNSIGNED_SHORT, elemArray /*nullptr*/);


             //glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLushort)*elemArraySize, elemArray, GL_STREAM_DRAW);
    // glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLushort)*elemArraySize, elemArray, GL_STREAM_DRAW);
     glDrawElements(GL_TRIANGLE_STRIP, (_size-_lastSize), GL_UNSIGNED_INT, (const void*)_lastOffset);
     }
}


}

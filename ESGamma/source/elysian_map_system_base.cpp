
#include <elysian_map_system_base.hpp>
#include <glal/gyro_gl.h>

;
namespace elysian {

void MapSystemBaseGLImpl::_drawElements(const Area*const area) {
    const GLintptr          _lastOffset    = area->lastOffset[0];
    const GLintptr          _lastSize      = area->lastElemArraySize[0];
    const GLintptr          _count         = area->elementsArraySize[0]-_lastSize;
    const GLushort*const    _elements       = area->elementsArray[0];



 GLushort* ptr = (GLushort*)glMapBufferRange(GL_ELEMENT_ARRAY_BUFFER, _lastOffset, sizeof(GLushort)*_count, GL_MAP_INVALIDATE_RANGE_BIT /*| GL_MAP_FLUSH_EXPLICIT_BIT*/ | GL_MAP_WRITE_BIT /* | GL_MAP_UNSYNCHRONIZED_BIT*/);
 memcpy(ptr, &_elements[_lastSize], sizeof(GLushort)*(_count));
  glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);

  glDrawElements(GL_TRIANGLE_STRIP, _count, GL_UNSIGNED_SHORT, (const void*)_lastOffset);


}

void MapSystemBaseGLImpl::_orphanVbo(void) {
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLushort)*ELYSIAN_AREA_GLAL_STRIP_INDEX_ARRAY_SIZE, nullptr, GL_STREAM_DRAW);

}

}

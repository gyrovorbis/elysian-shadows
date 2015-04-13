#include <gyro_video_api.h>
#include <glal/gyro_video_glal.h>
#include <x86/gyro_input_x86.inl>
#include <glal/gyro_gl.h>

//reested-ass hack
extern GYInputInitParams    _initParams;

void glalVidBindDefaultFbo(void) {
    //fucking disgusting.
    glBindFramebuffer(GL_FRAMEBUFFER, glalVidDefaultFbo());
}

void gyVidSceneBegin(void) {
    glalPrevVertexRenderCount = glalVertexRenderCount;
    glalPrevUniformChangeCount = glalUniformChangeCount;
    glalPrevTextureChangeCount = glalTextureChangeCount;
    glalVertexRenderCount = 0;
    glalUniformChangeCount = 0;
    glalTextureChangeCount = 0;
}

void gyVidSceneEnd(void) {
    gyVidVertexBatchFlush();
  //  glFlush();
}

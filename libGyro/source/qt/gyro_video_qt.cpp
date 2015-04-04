#include <gyro_video_api.h>
#include <glal/gyro_video_glal.h>
#include <QOpenGLFramebufferObject>
#include <x86/gyro_input_x86.inl>
#include <QOpenGLWidget>

//reested-ass hack
extern GYInputInitParams    _initParams;

void glalVidBindDefaultFbo(void) {
    glBindFramebuffer(GL_FRAMEBUFFER, reinterpret_cast<QOpenGLWidget*>(_initParams.viewWidget)->defaultFramebufferObject());
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
 //   glFlush();
}

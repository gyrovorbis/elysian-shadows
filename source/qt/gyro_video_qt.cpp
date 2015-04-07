#include <gyro_video_api.h>
#include <glal/gyro_video_glal.h>
#include <QOpenGLFramebufferObject>
#include <x86/gyro_input_x86.inl>
#include <QOpenGLWidget>
#include <QGraphicsView>

//reested-ass hack
extern GYInputInitParams    _initParams;

void glalVidBindDefaultFbo(void) {
    //fucking disgusting.
    glBindFramebuffer(GL_FRAMEBUFFER, static_cast<QOpenGLWidget*>(reinterpret_cast<QGraphicsView*>(_initParams.viewWidget)->viewport())->defaultFramebufferObject());
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
    //glFlush();
}

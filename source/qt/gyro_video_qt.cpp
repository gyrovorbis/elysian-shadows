#include <gyro_video_api.h>
#include <glal/gyro_video_glal.h>

void gyVidSceneBegin(void) {
    glalPrevVertexRenderCount = glalVertexRenderCount;
    glalPrevUniformChangeCount = glalUniformChangeCount;
    glalPrevTextureChangeCount = glalTextureChangeCount;
    glalVertexRenderCount = 0;
    glalUniformChangeCount = 0;
    glalTextureChangeCount = 0;
}

void gyVidSceneEnd(void) {

}

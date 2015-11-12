#include <gyro_video_api.h>
#include <glal/gyro_video_glal.h>


void glalVidBindDefaultFbo(void) {
    //fucking disgusting.
   // glBindFramebuffer(GL_FRAMEBUFFER, glalDefaultFbo());
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
//    glDrawBuffer(GL_BACK);
}


int gyVidInit(const GYVidInitParams * const params) {
    glalVidOglInit(1080, 720, 1080, 720);
    return 1;
}

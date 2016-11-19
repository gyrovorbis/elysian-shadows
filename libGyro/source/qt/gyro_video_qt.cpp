
#include <glal/gyro_gl.h>

extern "C" int glalVidDefaultFbo(void) {
    return 0;
}


extern "C" void glalVidBindDefaultFbo(void) {
    //fucking disgusting.
   // glBindFramebuffer(GL_FRAMEBUFFER, glalDefaultFbo());
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
#ifdef GYRO_GLAL_GL_Desktop
    glDrawBuffer(GL_BACK);
#endif
}

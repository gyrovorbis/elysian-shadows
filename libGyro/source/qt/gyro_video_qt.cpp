
#include <glal/gyro_gl.h>

extern "C" void glalVidBindDefaultFbo(void) {
    //fucking disgusting.
   // glBindFramebuffer(GL_FRAMEBUFFER, glalDefaultFbo());
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

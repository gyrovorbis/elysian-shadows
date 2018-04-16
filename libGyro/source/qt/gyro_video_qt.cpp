
#include <glal/gyro_gl.h>


uint32_t glalDefaultFbo(void);


extern "C" void glalVidBindDefaultFbo(void) {
    //fucking disgusting.
   // glBindFramebuffer(GL_FRAMEBUFFER, glalDefaultFbo());
    glBindFramebuffer(GL_FRAMEBUFFER, glalDefaultFbo());

    GLenum buffs[] = {
        GL_BACK_LEFT
    };

    //glDrawBuffers(1, buffs);
#ifdef GYRO_GLAL_GL_Desktop
 //  glDrawBuffer(GL_BACK);
#endif
}

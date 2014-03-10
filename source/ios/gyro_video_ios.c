#include "gyro_video.h"
#include "glal/gyro_video_glal.h"
#include "ios/gyro_gles_ios.h"
#include "glal/gyro_gl_glal.h"

int gyVidInit(const GYVidInitParams *const params) {
    //gyVidOglInit(1280, 720, 1280, 720);
    return 1; 
}

int gyVidUninit() {
    return 1; 
}

void gyVidSceneBegin(void) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void gyVidSceneEnd(void) {
    //nothing :D
}

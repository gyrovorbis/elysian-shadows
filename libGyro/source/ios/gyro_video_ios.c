#include "gyro_video_api.h"
#include "gyro_video_glal.h"
#include "gyro_gles_ios.h"
#include "gyro_glal.h"

int gyVidInit(const struct _GYVidInitParams *const params) {
    gyVidOglInit(1280, 720, 1280, 720);
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

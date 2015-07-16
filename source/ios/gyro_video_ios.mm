#include <gyro_video_api.h>
#include <glal/gyro_video_glal.h>

int gyVidInit(const GYVidInitParams * const params) {
    glalVidOglInit(1080, 720, 1080, 720);
    return 1;
}

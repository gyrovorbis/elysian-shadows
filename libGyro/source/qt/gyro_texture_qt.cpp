#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <gyro_file_api.h>
#include <gyro_texture_api.h>
//#include <x86/gyro_glfw_x86.inl>
#include "glal/gyro_glal.h"


int _gyVidTexAlloc(GYTexture *const tex, const unsigned width, const unsigned height) {
    (void)width;
    (void)height;

    return 1;
}

int _gyVidTexFree(GYTexture *const tex) {
    return 1;
}


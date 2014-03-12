#include "gyro.h"
#include <stdlib.h>
#include <stdio.h>
#include "math.h"
#include "time.h"

GYTexture *tex = NULL;
GYInput input;
float offset = 240.0f;

#define OBJECT_COUNT 1000

typedef struct _Object {
    float x, y, z;
    float sx, sy, sz;
    float orient;
    float xvel, yvel;
    float rotVel;
    int frame;
} Object;
Object object[OBJECT_COUNT];


void render() {
	GYColor4 color = {255, 255, 255, 127};

	unsigned int i = 0;
	gyVidSceneBegin();
	gyVidTexBind(tex);
	gyVidAlphaLevel(GY_ALPHA_FULL);
	gyVidQuadListBegin(&color);
        for(i = 0; i < OBJECT_COUNT; ++i) {
		gyMatLoadIdentity();
		gyMatTranslate(object[i].x, object[i].y, object[i].z);
		gyMatScale(object[i].sx, object[i].sy, object[i].sz);
		gyMatRotateZ(object[i].orient);
		gyVidRect(object[i].frame);
	}
	gyVidQuadListEnd();
	gyVidSceneEnd();
};

void initExample() {
	GYVidInitParams params = {
#if GYRO_PLATFORM == GYRO_PLATFORM_x86
	GY_VID_1280x720, GY_WIN_RESIZABLE | GY_WIN_STRETCH, 320, 240, 1280, 720, "TESTZOR", "icon.BMP"
#endif
};

		int i = 0;
	gySysInit();
	gyFileInit("Team Gyro", "Basic Example");
	gyVidInit(&params);
	gyInputInit();
#if GYRO_PLATFORM == GYRO_PLATFORM_x86
        for(;i < GY_KEY_LAST;++i) {
		input.keyDown[i] = input.keyTapped[i] = 0;
        }  
#endif

    //tex = gyVidTexCreate(gyFilePath("test.png"), 32, 32);
	if(!tex) {
                gyPrintf("TEX NOT FOUND\n");
	}
        else {
            gyPrintf("TEX OKAY\n");
        }

        for(i = 0; i < OBJECT_COUNT; ++i) {
            object[i].x = rand()%630;
            object[i].y = rand()%470;
            object[i].z = 1.0f;
            object[i].orient = (rand()%180)/3.141f;
            object[i].sx = rand()%90+10;
            object[i].sy = rand()%90+10;
            object[i].sz = 1.0f;
            object[i].frame = rand()%10+2;
            object[i].xvel = rand()%9+1;
            object[i].yvel = rand()%9+1;
            object[i].rotVel = (rand()%2)/3.141f;
        }
}


void init_render(unsigned int width, unsigned int height) {
	//do nothing right now!
}

int update() { //ios version of input
	unsigned int i = 0;
	unsigned int screenWidth, screenHeight;
	gyInputPoll(&input);
	gyVidResolution(&screenWidth, &screenHeight);

#if GYRO_PLATFORM==GYRO_PLATFORM_DC
	if(input.butTapped[GY_CONT_BUT_Y]) vid_screen_shot("dcscr.ppm");
#endif
#if GYRO_PLATFORM == GYRO_PLATFORM_x86
        if(input.keyDown[GY_KEY_DPAD_D])
		offset += 1.0f;
	if(input.keyDown[GY_KEY_ESC])
                return 1;
#endif
	if(1) return 0;
        for(i = 0; i < OBJECT_COUNT; ++i) {
            object[i].x += object[i].xvel;
            object[i].y += object[i].yvel;
            object[i].orient += object[i].rotVel;

            if(object[i].x >= screenWidth || object[i].x <= 0.0f) object[i].xvel = -object[i].xvel;
            if(object[i].y >= screenHeight || object[i].y <= 0.0f) object[i].yvel = -object[i].yvel;
        }

	return 0;
}

void destroy() {
	gyVidTexDestroy(tex);
	gyInputUninit();
	gyVidUninit();
    gyFileQuit();
	gySysUninit();
}

int main(int argc, char *argv[]) {
#if GYRO_PLATFORM == GYRO_PLATFORM_iOS
	IOS_SetInitFunc(initExample);
	gyPrintf("main - init_render %p\n", init_render);
	IOS_SetInitRenderFunc(init_render);
	IOS_SetRenderFunc(render);
	IOS_SetUpdateFunc(update);
	CallUIApplicationMain(argc, argv);
#endif

	initExample();
	while(1)
	{
		if(update()) break;
		render();
	}
	destroy();
	return 0;
};

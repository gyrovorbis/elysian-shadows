#include <gyro_video_api.h>
#include <gyro_system_api.h>
#include "../glal/gyro_video_glal.h"
#include <glal/gyro_glal.h>

//Global SDL shit--shared with input (which has to resize window)
GYVidInitParams vidParams;
int vidBpp;

int gyVidInit(const GYVidInitParams *const params) {
	const SDL_VideoInfo *info = NULL;
	SDL_Surface *icon = NULL;
	int flags = 0;
    unsigned int viewWidth=0, viewHeight=0, projWidth=0, projHeight=0;
    int videoInit;

	memcpy(&vidParams, params, sizeof(GYVidInitParams));

    /* First, initialize SDL's video subsystem. */
    videoInit = SDL_Init(SDL_INIT_VIDEO);

/*    UNredirect our stdout/stderr, because SDL is a fucking prick.

#if defined(__WIN32__)
    FILE * ctt = fopen("CON", "w" );
    //freopen("CON", "w", stdout);
    freopen("CON", "w", stderr);
#endif
*/
    if(videoInit < 0) { //Shit has reested.
        _gyLog(GY_DEBUG_CRITICAL, "Video initialization failed: %s\n", SDL_GetError());
    }

    info = SDL_GetVideoInfo();

    if(!info) { //do some performance checks.
        _gyLog(GY_DEBUG_CRITICAL, "Video query failed: %s\n", SDL_GetError());
		SDL_Quit();
    }

	switch(params->res) {
	case GY_VID_320x240:
		viewWidth = projWidth = 320;
		viewHeight = projHeight = 240;
		break;
	case GY_VID_640x480:
		viewWidth = projWidth = 640;
		viewHeight = projHeight = 480;
		break;
	case GY_VID_1280x720:
		viewWidth = projWidth = 1280;
		viewHeight = projHeight = 720;
		break;
	}

    vidBpp = info->vfmt->BitsPerPixel;

/*
    Should be SDL_GL_SetAttribute(SDL_GL_SWAP_CONTROL, TRUE),
    but for some reason SDL_GL_SWAP_CONTROL can't be found,
	so we have to cast the integer value.
	(VSYNC)
*/
	SDL_GL_SetAttribute((SDL_GLattr)16, 1);
	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 5);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 5);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 5);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

	flags = SDL_OPENGL | SDL_ANYFORMAT; //SDL_HWSURFACE | SDL_HWPALETTE | SDL_DOUBLEBUF;
	//flags = SDL_OPENGL | SDL_DOUBLEBUF;
	if(params->prop & GY_WIN_RESIZABLE) flags |= SDL_RESIZABLE;
	if(params->prop & GY_WIN_FULLSCREEN) flags |= SDL_FULLSCREEN;

	icon = SDL_LoadBMP(params->icon);
	uint32_t colorkey = SDL_MapRGB(icon->format, 255, 0, 255);
	SDL_SetColorKey(icon, SDL_SRCCOLORKEY, colorkey);
	SDL_WM_SetIcon(icon, NULL);
	SDL_WM_SetCaption(params->title, NULL);

	if(SDL_SetVideoMode(viewWidth, viewHeight, vidBpp, flags) == 0) {
		_gyLog(GY_DEBUG_CRITICAL, "Video mode set failed: %s\n", SDL_GetError());
		SDL_Quit();
    }

    atexit(SDL_Quit);
	gyVidOglInit(viewWidth, viewHeight, projWidth, projHeight);
	return 1;
}

int gyVidUninit(void) {
	return 1;
}

void gyVidSceneBegin(void) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    glLoadIdentity();
}

void gyVidSceneEnd(void) {
    SDL_GL_SwapBuffers();
}

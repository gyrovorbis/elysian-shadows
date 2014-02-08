#include <gyro_input_api.h>
#include <gyro_system_api.h>
#include <gyro_video_api.h>
#include <gyro_texture_api.h>
#include <gyro_shader_api.h>
#include <x86/gyro_sdl_x86.inl>
#include "../glal/gyro_video_glal.h"

extern GYVidInitParams vidParams;
extern int vidBpp;

static inline void _SetKeyDown(GYInput *const input, const GY_X86_KEY key) {
    input->keyDown[key] = 1;
    input->keyTapped[key] = 1;
}

static inline void _SetKeyUp(GYInput *const input, const GY_X86_KEY key) {
    input->keyDown[key] = 0;
    input->keyTapped[key] = 0;
}

static inline GY_X86_KEY _TranslateFromSDL(const SDLKey keyCode) {
    switch(keyCode) {
    case SDLK_ESCAPE:
        //exit(1);
        return GY_KEY_ESC;
    case SDLK_UP:
        return GY_KEY_DPAD_U;
    case SDLK_DOWN:
        return GY_KEY_DPAD_D;
    case SDLK_LEFT:
        return GY_KEY_DPAD_L;
    case SDLK_RIGHT:
        return GY_KEY_DPAD_R;
    case SDLK_a:
        return GY_KEY_A;
    case SDLK_b:
        return GY_KEY_B;
    case SDLK_c:
        return GY_KEY_C;
    case SDLK_d:
        return GY_KEY_D;
    case SDLK_e:
        return GY_KEY_E;
    case SDLK_f:
        return GY_KEY_F;
    case SDLK_g:
        return GY_KEY_G;
    case SDLK_h:
        return GY_KEY_H;
    case SDLK_i:
        return GY_KEY_I;
    case SDLK_j:
        return GY_KEY_J;
    case SDLK_k:
        return GY_KEY_K;
    case SDLK_l:
        return GY_KEY_L;
    case SDLK_m:
        return GY_KEY_M;
    case SDLK_n:
        return GY_KEY_N;
    case SDLK_o:
        return GY_KEY_O;
    case SDLK_p:
        return GY_KEY_P;
    case SDLK_q:
        return GY_KEY_Q;
    case SDLK_r:
        return GY_KEY_R;
    case SDLK_s:
        return GY_KEY_S;
    case SDLK_t:
        return GY_KEY_T;
    case SDLK_u:
        return GY_KEY_U;
    case SDLK_v:
        return GY_KEY_V;
    case SDLK_w:
        return GY_KEY_W;
    case SDLK_x:
        return GY_KEY_X;
    case SDLK_y:
        return GY_KEY_Y;
    case SDLK_z:
        return GY_KEY_Z;
	case SDLK_0:
		return GY_KEY_0;
	case SDLK_1:
		return GY_KEY_1;
	case SDLK_2:
		return GY_KEY_2;
	case SDLK_3:
		return GY_KEY_3;
	case SDLK_4:
		return GY_KEY_4;
	case SDLK_5:
		return GY_KEY_5;
	case SDLK_6:
		return GY_KEY_6;
	case SDLK_7:
		return GY_KEY_7;
	case SDLK_8:
		return GY_KEY_8;
	case SDLK_9:
		return GY_KEY_9;
	case SDLK_SPACE:
		return GY_KEY_SPACE;
	case SDLK_RETURN:
		return GY_KEY_RETURN;
	case SDLK_LSHIFT:
		return GY_KEY_LSHIFT;
	case SDLK_RSHIFT:
		return GY_KEY_RSHIFT;
	case SDLK_LCTRL:
		return GY_KEY_LCTRL;
	case SDLK_RCTRL:
		return GY_KEY_RCTRL;
	case SDLK_LALT:
		return GY_KEY_LALT;
	case SDLK_RALT:
		return GY_KEY_RALT;
	case SDLK_BACKQUOTE:
		return GY_KEY_BACKQUOTE;
	case SDLK_BACKSLASH:
		return GY_KEY_BACKSLASH;
	case SDLK_SLASH:
		return GY_KEY_FORWARDSLASH;
    default:
        return GY_KEY_NONE;
    }
}

int gyInputInit(void) {


    return 1;
}

int gyInputUninit(void) {
    return 1;
}

void gyInputPoll(GYInput *const input) {
    SDL_Event event;
    GY_X86_KEY retval;
	int flags;
	unsigned int viewWidth, viewHeight, projWidth, projHeight;
    unsigned int i;

    //keyTap is only true if it was pressed last frame, so prime it
    for (i = 0; i < GY_KEY_LAST; ++i)
        input->keyTapped[i] = 0;

        while(SDL_PollEvent(&event)) {
            switch(event.type) {
            case SDL_KEYDOWN:
                retval = _TranslateFromSDL(event.key.keysym.sym);
                if(retval > GY_KEY_NONE && retval < GY_KEY_LAST) _SetKeyDown(input, retval);
                break;
            case SDL_KEYUP:
                retval = _TranslateFromSDL(event.key.keysym.sym);
                if(retval > GY_KEY_NONE && retval < GY_KEY_LAST) _SetKeyUp(input, retval);
                break;
           case SDL_QUIT:
               SDL_Quit();
                exit(0);
			   return;
		   case SDL_VIDEORESIZE:
				viewWidth = event.resize.w;
				viewHeight = event.resize.h;

				//resize projection to view
				//if projection is outside of bounds, THEN stretch to fit
				if(!(vidParams.prop & GY_WIN_STRETCH)) { 
					if(vidParams.minWidth != -1 && event.resize.w < vidParams.minWidth) projWidth = vidParams.minWidth;
					else if(vidParams.maxWidth != -1 && event.resize.w > vidParams.maxWidth) projWidth = vidParams.maxWidth;
					else projWidth = event.resize.w;

					if(vidParams.minHeight != -1 && event.resize.h < vidParams.minHeight) projHeight = vidParams.minHeight;
					else if(vidParams.maxHeight != -1 && event.resize.h > vidParams.maxHeight) projHeight = vidParams.maxHeight;
					else projHeight = event.resize.h;
				}
				else { //stretch the same projection to fit the view
					gyVidResolution(&projWidth, &projHeight);	
				}

			    flags = SDL_OPENGL | SDL_ANYFORMAT | SDL_HWPALETTE;
				if(vidParams.prop & GY_WIN_RESIZABLE) flags |= SDL_RESIZABLE;
				if(vidParams.prop & GY_WIN_FULLSCREEN) flags |= SDL_FULLSCREEN;

				if(SDL_SetVideoMode(viewWidth, viewHeight, vidBpp, flags) == 0) {
					_gyLog(GY_DEBUG_CRITICAL, "Video mode set failed: %s\n", SDL_GetError());
					SDL_Quit();
				}

				gyShadersReloadAll();
			   gyVidOglInit(viewWidth, viewHeight, projWidth, projHeight);
			   gyVidTexReloadAll();

			   break;
            }
        }
}


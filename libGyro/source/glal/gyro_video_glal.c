#include "gyro_platform.h"
#include "gyro_video.h"
#include "gyro_texture.h"
#include "gyro_matrix.h"
#include "gyro_matrix_stack.h"
#include "glal/gyro_gl_glal.h"
#include "glal/gyro_video_glal.h"

#include <assert.h>

//OPENGL/OPENGL_ES COMMONS, BITCHES!

//Internal video state variables
static const GYTexture *_curTex = 0;
static unsigned int _projWidth = 0;
static unsigned int _projHeight = 0;
static GY_RECT_TEX_ORIENT _rectTexOrient = GY_RECT_TEX_ORIENT_NORMAL;

void gyVidResolution(unsigned int *const projWidth, unsigned int *const projHeight) {
	*projWidth = _projWidth; *projHeight = _projHeight;
}

int gyVidOglInit(const int viewWidth, const int viewHeight, const int projWidth, const int projHeight) {
    _projWidth = projWidth;
    _projHeight = projHeight;

    glClearColor(0, 0, 0, 0);
    glClearDepth (1.0f);
    glDepthFunc(GL_LEQUAL);
    glEnable(GL_DEPTH_TEST);
    glViewport(0, 0, viewWidth, viewHeight);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, projWidth, projHeight, 0, -1000, 1000);
    glMatrixMode(GL_MODELVIEW);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    /*glTexParameteri( GL_TEXTURE_2D, 
                    GL_TEXTURE_WRAP_S, 
                    GL_CLAMP ); */
    glLoadIdentity();
    return 1;
}

void gyVidRectTexOrient(const GY_RECT_TEX_ORIENT orient) {
	_rectTexOrient = orient;
}

static inline void gyVidTexCoordArrayFromFrame(GYTexCoord2 *const coord, const GYTexCoordFrame *const frame) {
	switch(_rectTexOrient) {
		case GY_RECT_TEX_ORIENT_NORMAL:
			coord[0].u = frame->topLeft.u; coord[0].v = frame->topLeft.v; 
			coord[1].u = frame->botRight.u; coord[1].v = frame->topLeft.v; 
			coord[2].u = frame->topLeft.u; coord[2].v = frame->botRight.v; 
			coord[3].u = frame->botRight.u; coord[3].v = frame->botRight.v; 
			break;
		case GY_RECT_TEX_ORIENT_FLIP_HORIZ:
			coord[0].u = frame->botRight.u; coord[0].v = frame->topLeft.v; 
			coord[1].u = frame->topLeft.u; coord[1].v = frame->topLeft.v; 
			coord[2].u = frame->botRight.u; coord[2].v = frame->botRight.v; 
			coord[3].u = frame->topLeft.u; coord[3].v = frame->botRight.v; 
			break;
		case GY_RECT_TEX_ORIENT_FLIP_VERT:
			coord[0].u = frame->topLeft.u; coord[0].v = frame->botRight.v; 
			coord[1].u = frame->botRight.u; coord[1].v = frame->botRight.v; 
			coord[2].u = frame->topLeft.u; coord[2].v = frame->topLeft.v; 
			coord[3].u = frame->botRight.u; coord[3].v = frame->topLeft.v; 
			break;
		case GY_RECT_TEX_ORIENT_ROT_90:
			coord[2].u = frame->topLeft.u; coord[2].v = frame->topLeft.v; 
			coord[0].u = frame->botRight.u; coord[0].v = frame->topLeft.v; 
			coord[3].u = frame->topLeft.u; coord[3].v = frame->botRight.v; 
			coord[1].u = frame->botRight.u; coord[1].v = frame->botRight.v; 
			break;
		case GY_RECT_TEX_ORIENT_ROT_180:
			coord[3].u = frame->topLeft.u; coord[3].v = frame->topLeft.v; 
			coord[2].u = frame->botRight.u; coord[2].v = frame->topLeft.v; 
			coord[1].u = frame->topLeft.u; coord[1].v = frame->botRight.v; 
			coord[0].u = frame->botRight.u; coord[0].v = frame->botRight.v; 
			break;
		case GY_RECT_TEX_ORIENT_ROT_270:
			coord[1].u = frame->topLeft.u; coord[1].v = frame->topLeft.v; 
			coord[3].u = frame->botRight.u; coord[3].v = frame->topLeft.v; 
			coord[0].u = frame->topLeft.u; coord[0].v = frame->botRight.v; 
			coord[2].u = frame->botRight.u; coord[2].v = frame->botRight.v; 
			break;
		case GY_RECT_TEX_ORIENT_FLIP_HORIZ_ROT_90:
			coord[3].u = frame->topLeft.u; coord[3].v = frame->topLeft.v; 
			coord[1].u = frame->botRight.u; coord[1].v = frame->topLeft.v; 
			coord[2].u = frame->topLeft.u; coord[2].v = frame->botRight.v; 
			coord[0].u = frame->botRight.u; coord[0].v = frame->botRight.v; 
			break;
		case GY_RECT_TEX_ORIENT_FLIP_HORIZ_ROT_270:
			coord[0].u = frame->topLeft.u; coord[0].v = frame->topLeft.v; 
			coord[2].u = frame->botRight.u; coord[2].v = frame->topLeft.v; 
			coord[1].u = frame->topLeft.u; coord[1].v = frame->botRight.v; 
			coord[3].u = frame->botRight.u; coord[3].v = frame->botRight.v; 
			break;
		default:
			assert(0);
		}
}

void gyVidQuadListBegin(const GYColor4 *const color) {
    glColor4ub(color->r, color->g, color->b, color->a);
	if(_curTex) {
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	}
	glEnableClientState(GL_VERTEX_ARRAY);
}

void gyVidQuadListEnd(void) {
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);}

void gyVidQuad(const GYVector4 *const verts, const unsigned int frame) {
	GYMatrix4 modelViewMat;
	GYTexCoord2 coord[4];
    
	gyMatStore(&modelViewMat);
    glLoadMatrixf((float *)modelViewMat);
	
	if(_curTex) {
		gyVidTexCoordArrayFromFrame(coord, &_curTex->_coord->texCoord[frame]);

		glTexCoordPointer(2, GL_FLOAT, 0, coord);
		_rectTexOrient = GY_RECT_TEX_ORIENT_NORMAL; //reset after each rect
	}

    glVertexPointer(4, GL_FLOAT, 0, verts);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

void gyVidRect(const unsigned int frame) {
    const static GYVector4 localVertArray[4] = {
        {-0.5f, -0.5f, 0.0f, 1.0f},
        {0.5f, -0.5f, 0.0f, 1.0f},
        {-0.5f, 0.5f, 0.0f, 1.0f},
        {0.5f, 0.5f, 0.0f, 1.0f}
    };
	GYMatrix4 modelViewMat;
    GYTexCoord2 coord[4];

	gyMatStore(&modelViewMat);
    glLoadMatrixf((float *)modelViewMat);
	
	if(_curTex) {
		gyVidTexCoordArrayFromFrame(coord, &_curTex->_coord->texCoord[frame]);

		glTexCoordPointer(2, GL_FLOAT, 0, coord);
		_rectTexOrient = GY_RECT_TEX_ORIENT_NORMAL; //reset after each rect
	}

    glVertexPointer(4, GL_FLOAT, 0, localVertArray);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

void gyVidTriangleStrip(const struct _GYVector4 *const vertexArray, const unsigned int startIndex, const unsigned int amount) {
    GYMatrix4 modelViewMat;
    gyMatStore(&modelViewMat);
    glLoadMatrixf((float *)modelViewMat);

    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(4, GL_FLOAT, 0, vertexArray);
    glDrawArrays(GL_TRIANGLE_STRIP, startIndex, amount);

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);

}

void gyVidColorArray(const struct _GYColor4 *const ptr) {
	glEnableClientState(GL_COLOR_ARRAY);
    glColorPointer(4, GL_UNSIGNED_BYTE, 0, ptr);
}

void gyVidCoord(const struct _GYTexCoord2 *const ptr) {
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glTexCoordPointer(2, GL_FLOAT, 0, ptr);
}

void gyVidTexBind(const GYTexture *const tex) {
    GYMatrix4 texMatrix;
	
	if(tex) {
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, *(GLuint *)&tex->_data);
		
		//Update the texture matrix to normalize our uint16 texture coordinates
		/*glMatrixMode(GL_TEXTURE);
		gyMatPush();
		gyMatLoadIdentity();
		gyMatScale(1.0f/(float)tex->_coord->sheetWidth, 1.0f/(float)tex->_coord->sheetHeight, 1.0f);
		gyMatStore(&texMatrix);
		gyMatPop();
		glLoadMatrixf((float *)texMatrix);
		glMatrixMode(GL_MODELVIEW);*/
	}
	else {
		glDisable(GL_TEXTURE_2D);
	}
	_curTex = tex;
}

void gyVidAlphaLevel(const GY_ALPHA_LEVEL lev) {
    switch(lev) {
	case GY_ALPHA_NONE:
		glDisable(GL_BLEND);
		break;
	default:
		glEnable(GL_BLEND);
            break;
    }
}



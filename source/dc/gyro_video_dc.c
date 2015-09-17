#include <gyro_video_api.h>
#include <gyro_matrix_api.h>
#include <gyro_texture_api.h>
#include <gyro_system_api.h>
//#include "stripheader.h"
#include <stdint.h>
#include <assert.h>

#include <kos.h>
#include <png/png.h>

//Vertex buffer sizes (bytes)
#define GYRO_PVR_VBUFSIZE          (1024 * 1024)
#define GYRO_OP_POLY_VBUFSIZE      (1024 * 1024)
#define GYRO_OP_MOD_VBUFSIZE       (256 * 1024)
#define GYRO_TR_POLY_VBUFSIZE      (512 * 1024)
#define GYRO_TR_MOD_VBUFSIZE       (256 * 1024)
#define GYRO_PT_POLY_VBUFSIZE      (1024 * 1024*2)

#define GYRO_WORLD_VERT_BUFSIZE     256

//SH4 assembly, lawl
#define PREFETCH(addr) __asm__ __volatile__("pref @%0" : : "r" (addr));

//Vertex List buffers
static uint8_t _op_poly_buf[GYRO_OP_POLY_VBUFSIZE] __attribute__((aligned(32)));
static uint8_t _op_mod_buf[GYRO_OP_MOD_VBUFSIZE]   __attribute__((aligned(32)));
static uint8_t _tr_poly_buf[GYRO_TR_POLY_VBUFSIZE] __attribute__((aligned(32)));
static uint8_t _tr_mod_buf[GYRO_TR_MOD_VBUFSIZE]   __attribute__((aligned(32)));
static uint8_t _pt_poly_buf[GYRO_PT_POLY_VBUFSIZE] __attribute__((aligned(32)));

//Vertex buffer state
static uint32_t *_start_ptr = NULL;
static uint32_t *_current_ptr = NULL;

static const GYVector4 localVert[4]  = {
    {-0.5f, -0.5f, 0.0f, 1.0f},
    {0.5f, -0.5f, 0.0f, 1.0f},
    {0.5f, 0.5f, 0.0f, 1.0f},
    {-0.5f, 0.5f, 0.0f, 1.0f}
};

//Internal transformed vertex buffer (SH4 handles transformations on DC)
static GYVector4 _worldVert[GYRO_WORLD_VERT_BUFSIZE];
//current pvr texture
//static texture_t *_shTex = NULL;
static GYTexCoordSheet *_texCoordSheet = NULL;
//current pvr vertex list
static pvr_list_t _list = PVR_LIST_OP_POLY;
//current pvr resolution
static unsigned int _projWidth = 0;
static unsigned int _projHeight = 0;
//triangle strip array state
static const GYTexCoord2 *_texCoordArray = NULL;
static const GYColor4 *_colorArray = NULL;
//texture orientation
static GY_RECT_TEX_ORIENT _texOrient = GY_RECT_TEX_ORIENT_NORMAL;
#if 0
//Debug callback supplied to Tvspelsfreak's SH (Strip Header) library
static void shErrorCallback(SHERROR error, const char *fname) {
    _gyLog(GY_DEBUG_CRITICAL, "DC_SHLIB ERROR! code: %d, fname: %s\n", error, fname);
}
#endif

int gyVidInit(const struct GYVidInitParams *const params) {
    //KOS automatically initializes 640x480 video mode
    //TODO: Init param choosing between 320x240 and 640x480!
    _projWidth = 640;
    _projHeight = 480;

    //Initialize the PVR
    pvr_init_params_t pvr_params;
    pvr_params.vertex_buf_size = GYRO_PVR_VBUFSIZE;
    pvr_params.dma_enabled = 1;
    pvr_params.fsaa_enabled = 0;

    //Initialize tile buffers
    pvr_params.opb_sizes[PVR_LIST_OP_POLY] = PVR_BINSIZE_32;
    pvr_params.opb_sizes[PVR_LIST_OP_MOD]  = PVR_BINSIZE_8;
    pvr_params.opb_sizes[PVR_LIST_TR_POLY] = PVR_BINSIZE_16;
    pvr_params.opb_sizes[PVR_LIST_TR_MOD]  = PVR_BINSIZE_8;
    pvr_params.opb_sizes[PVR_LIST_PT_POLY] = PVR_BINSIZE_16;

    if(pvr_init(&pvr_params) < 0) return 0;

    //Set vertex buffers
    pvr_set_vertbuf(PVR_LIST_OP_POLY, _op_poly_buf, GYRO_OP_POLY_VBUFSIZE);
    pvr_set_vertbuf(PVR_LIST_OP_MOD,  _op_mod_buf,  GYRO_OP_MOD_VBUFSIZE);
    pvr_set_vertbuf(PVR_LIST_TR_POLY, _tr_poly_buf, GYRO_TR_POLY_VBUFSIZE);
    pvr_set_vertbuf(PVR_LIST_TR_MOD,  _tr_mod_buf,  GYRO_TR_MOD_VBUFSIZE);
    pvr_set_vertbuf(PVR_LIST_PT_POLY, _pt_poly_buf, GYRO_PT_POLY_VBUFSIZE);

    //Set palette format
    pvr_set_pal_format(PVR_PAL_ARGB8888);

    //resgister debug function pointer to Tvspelsfreak's SHlib
   // shErrorHandler(&shErrorCallback);
    return 1;
}

void gyVidCamUpdate(const GYCamera *const cam) {
}

void gyVidSetCamWorldPos(const GYVector3*const pos) {
}


void gyVidClearColor(const GYColor4 color) {

}

void gyVidRectPlaneType(const GY_RECT_PLANE_TYPE type) {}


void gyVidResolution(unsigned int *const width, unsigned int *const height) {
    *width = _projWidth;
    *height = _projHeight;
}

int gyVidUninit(void) {
    return 1;
}

void gyVidSceneBegin(void) {
    vid_border_color(255, 0, 0); //idle time
    pvr_wait_ready();
    vid_border_color(0, 255, 0); //gpu time
    pvr_scene_begin();
}

void gyVidSceneEnd(void) {
    vid_border_color(0, 0, 255); //cpu time
    pvr_scene_finish();
    //KOS initializes DMA transfer from RAM vertex buffers to VRAM begins now
}

void gyVidTexBind(const GYTexture *const tex) {
    //update our cached PVR texture
    if(tex) {
        //_shTex = tex->_data;
        _texCoordSheet = tex->_coord;
    } else {
       // _shTex = NULL;
        _texCoordSheet = NULL;
    }
}

void gyVidAlphaLevel(const GY_ALPHA_LEVEL lev) {
    //update our internal KOS pvr list
    _list = (lev == GY_ALPHA_NONE)?
                    PVR_LIST_OP_POLY : (lev == GY_ALPHA_FULL)?
                    PVR_LIST_TR_POLY : PVR_LIST_PT_POLY;
}


void gyVidColorArray(const GYColor4 *const ptr) {
    _colorArray = ptr;
}

void gyVidTexCoordArray(const struct GYTexCoord2 *const ptr) {
    _texCoordArray = ptr;
}

void gyVidRectTexOrient(const GY_RECT_TEX_ORIENT orient) {
    _texOrient = orient;
}

#if 0
const struct GYTexCoord2* gyVidTexCoordArrayFromFrame(const struct GYTexCoordFrame*const frame,
                                               const GY_RECT_TEX_ORIENT orient) {

	switch(_texOrient) {
		case GY_RECT_TEX_ORIENT_NORMAL:
/*
            coord[0].u = frame->topLeft.u; coord[0].v = frame->topLeft.v; 
			coord[1].u = frame->botRight.u; coord[1].v = frame->topLeft.v; 
			coord[2].u = frame->topLeft.u; coord[2].v = frame->botRight.v; 
			coord[3].u = frame->botRight.u; coord[3].v = frame->botRight.v; 
*/
            coord[0].uv = frame->topLeft.uv;
            coord[1].uv = frame->topRight.uv;
            coord[2].uv = frame->botRight.uv;
			break;
		case GY_RECT_TEX_ORIENT_FLIP_HORIZ:
/*			coord[0].u = frame->botRight.u; coord[0].v = frame->topLeft.v; 
			coord[1].u = frame->topLeft.u; coord[1].v = frame->topLeft.v; 
			coord[2].u = frame->botRight.u; coord[2].v = frame->botRight.v; 
			coord[3].u = frame->topLeft.u; coord[3].v = frame->botRight.v; */
            coord[0].uv = frame->topRight.uv;
            coord[1].uv = frame->topLeft.uv;
            coord[2].uv = frame->botLeft.uv;
			break;
		case GY_RECT_TEX_ORIENT_FLIP_VERT:
		/*	coord[0].u = frame->topLeft.u; coord[0].v = frame->botRight.v; 
			coord[1].u = frame->botRight.u; coord[1].v = frame->botRight.v; 
			coord[2].u = frame->topLeft.u; coord[2].v = frame->topLeft.v; 
			coord[3].u = frame->botRight.u; coord[3].v = frame->topLeft.v; */
            coord[0].uv = frame->botLeft.uv;
            coord[1].uv = frame->botRight.uv;
            coord[2].uv = frame->topRight.uv;
			break;
		case GY_RECT_TEX_ORIENT_ROT_90:
		/*	coord[2].u = frame->topLeft.u; coord[2].v = frame->topLeft.v; 
			coord[0].u = frame->botRight.u; coord[0].v = frame->topLeft.v; 
			coord[3].u = frame->topLeft.u; coord[3].v = frame->botRight.v; 
			coord[1].u = frame->botRight.u; coord[1].v = frame->botRight.v; */
            /*coord[0].uv = frame->botLeft.uv;
            coord[1].uv = frame->topLeft.uv;
            coord[2].uv = frame->topRight.uv;*/
            coord[0].uv = frame->topRight.uv;
            coord[1].uv = frame->botRight.uv;
            coord[2].uv = frame->botLeft.uv;
			break;
		case GY_RECT_TEX_ORIENT_ROT_180:
		/*	coord[3].u = frame->topLeft.u; coord[3].v = frame->topLeft.v; 
			coord[2].u = frame->botRight.u; coord[2].v = frame->topLeft.v; 
			coord[1].u = frame->topLeft.u; coord[1].v = frame->botRight.v; 
			coord[0].u = frame->botRight.u; coord[0].v = frame->botRight.v; */
            coord[0].uv = frame->botRight.uv;
            coord[1].uv = frame->botLeft.uv;
            coord[2].uv = frame->topLeft.uv;
			break;
		case GY_RECT_TEX_ORIENT_ROT_270:
	/*		coord[1].u = frame->topLeft.u; coord[1].v = frame->topLeft.v; 
			coord[3].u = frame->botRight.u; coord[3].v = frame->topLeft.v; 
			coord[0].u = frame->topLeft.u; coord[0].v = frame->botRight.v; 
			coord[2].u = frame->botRight.u; coord[2].v = frame->botRight.v; */
            coord[0].uv = frame->botLeft.uv;
            coord[1].uv = frame->topLeft.uv;
            coord[2].uv = frame->topRight.uv;
			break;
		case GY_RECT_TEX_ORIENT_FLIP_HORIZ_ROT_90:
		/*	coord[3].u = frame->topLeft.u; coord[3].v = frame->topLeft.v; 
			coord[1].u = frame->botRight.u; coord[1].v = frame->topLeft.v; 
			coord[2].u = frame->topLeft.u; coord[2].v = frame->botRight.v; 
			coord[0].u = frame->botRight.u; coord[0].v = frame->botRight.v;*/ 
            coord[0].uv = frame->botRight.uv;
            coord[1].uv = frame->topRight.uv;
            coord[2].uv = frame->topLeft.uv;
			break;
		case GY_RECT_TEX_ORIENT_FLIP_HORIZ_ROT_270:
		/*	coord[0].u = frame->topLeft.u; coord[0].v = frame->topLeft.v; 
			coord[2].u = frame->botRight.u; coord[2].v = frame->topLeft.v; 
			coord[1].u = frame->topLeft.u; coord[1].v = frame->botRight.v; 
			coord[3].u = frame->botRight.u; coord[3].v = frame->botRight.v; */
            coord[0].uv = frame->topLeft.uv;
            coord[1].uv = frame->botLeft.uv;
            coord[2].uv = frame->botRight.uv;
			break;
		default:
			assert(0);
    }

}
#endif

/*
  Triangle strip API works like OGL's vertex array API.
*/
void gyVidTriangleStrip(const GYVector4 *const vertexArray, const unsigned int startIndex, const unsigned int amount) {
 //   stripheader_t header;
    unsigned int i;
    register uint32_t *ptr = (uint32_t *)pvr_vertbuf_tail(_list);

    //Initialize SH4 store-queues to bypass cache
    QACR0 = ((((uint32_t)ptr) >> 26) << 2) & 0x1c;
    QACR1 = ((((uint32_t)ptr) >> 26) << 2) & 0x1c;
    _start_ptr = _current_ptr = (uint32_t *) (0xe0000000 | (((uint32_t)ptr) & 0x03ffffe0));
    ptr = _current_ptr;

    assert(amount <= GYRO_WORLD_VERT_BUFSIZE); //that would suck for you...
    //transform local vertices by internal SH4 matrix accumulator
    gyMatMultVec(vertexArray+startIndex, _worldVert, amount);

    //Commit polygon strip header
  //  shInit(&header, (_shTex)? 4 : 0, _list, _shTex, NULL);
    //ptr += shCommit(&header, ptr);

    //Commit vertices
    //branch avoidance optimization here? Copy everything into one buffer with no branches?
    for(i = startIndex; i < amount; ++i) {
        ptr += 8; //32-bit vertices
        *--ptr = 0;
		if(_colorArray) {
			/**(uint8_t*)--ptr = _colorArray[i].a;
			*(uint8_t*)--ptr = _colorArray[i].b;	STORE QUEUS APPARENTLY SHIT THEIR PANTS IF THEY AREN'T
			*(uint8_t*)--ptr = _colorArray[i].g;	ACCESSED ON A WORD BOUNDARY!!!!!
			*(uint8_t*)--ptr = _colorArray[i].r;*/
			*--ptr = (_colorArray[i].a<<24) | (_colorArray[i].r<<16) | (_colorArray[i].g<<8) | _colorArray[i].b;    
		}
		else {
			*--ptr = 0xffffffff;
		}
		--ptr; //padding
		if(_texCoordArray) {
  //          *--ptr = _texCoordArray[i].uv;//PVR_PACK_16BIT_UV(_texCoordArray[i].u, _texCoordArray[i].v);
        }
        else {
            ptr -= 1;
        }
        *(float*)--ptr = _worldVert[i].z;
        *(float*)--ptr = _worldVert[i].y;
        *(float*)--ptr = _worldVert[i].x;
        *--ptr = (i == amount-1)? PVR_CMD_VERTEX_EOL : PVR_CMD_VERTEX; //flag last strip vertex
        PREFETCH((void *)ptr);
        ptr += 8;
    }
    _current_ptr = ptr;

    //report number of bytes written to KOS
    uint32_t bytes = (uint32_t)_current_ptr - (uint32_t)_start_ptr;
    if(bytes != 0) pvr_vertbuf_written(_list, bytes);

	_colorArray = NULL;
    _texCoordArray = NULL;
}

void gyVidQuadListBegin(const GYColor4 *const color) {
//	stripheader_t header;
    register uint32_t *ptr = (uint32_t *)pvr_vertbuf_tail(_list);

    //Initialize store queue transfers to bypass the cache
    QACR0 = ((((uint32_t)ptr) >> 26) << 2) & 0x1c;
    QACR1 = ((((uint32_t)ptr) >> 26) << 2) & 0x1c;
    _start_ptr = _current_ptr = (uint32_t *) (0xe0000000 | (((uint32_t)ptr) & 0x03ffffe0));
    ptr = _current_ptr;
  
    //Commit hardware sprite header
//    shInit(&header, (_shTex)? 16 : 15, _list, _shTex, NULL);
    //shSpriteColor(&header, (void*)color);
  //  ptr += shCommit(&header, ptr);
    _current_ptr = ptr;
}

void gyVidQuadListEnd(void) {
    //report bytes written to buffers to KOS
    uint32_t bytes = (uint32_t)_current_ptr - (uint32_t)_start_ptr;
    if(bytes != 0) pvr_vertbuf_written(_list, bytes);
}

/*
  Hardware sprites are optimized structures on the Dreamcast that
  can be packed into a single vertex (rather than 4 for a quad).
  */
void gyVidRect(const int frame) {
  /*  static GYVector4 localVert[4]  = {
        {-0.5f, -0.5f, 1.0f, 1.0f},
        {0.5f, -0.5f, 1.0f, 1.0f},
        {0.5f, 0.5f, 1.0f, 1.0f},
        {-0.5f, 0.5f, 1.0f, 1.0f}
    };*/
    register uint32_t *ptr = _current_ptr;
    GYTexCoordFrame *texCoordFrame = (_texCoordSheet)? &_texCoordSheet->texCoord[frame] : NULL;
    GYTexCoord2 texCoord[3];

    //transform local vertices by internal SH4 matrix accumulator
    gyMatMultVec(localVert, &_worldVert[0], 4);
    
    //gyPrintf("<<<%f, %f, %f, %f>>>\n", _worldVert[0].z, _worldVert[1].z, _worldVert[2].z, _worldVert[3].z);

    //commit hardware sprite vertex
    ptr += 16; //64-byte vertex
    if(texCoordFrame) {
#if 0
        gyVidTexCoordArrayFromFrame(texCoord, texCoordFrame);
        *--ptr = texCoord[2].uv;
        *--ptr = texCoord[1].uv;
        *--ptr = texCoord[0].uv;
#endif
        //*--ptr = texCoordFrame->botRight.uv; //prepacking these bitches is
        //*--ptr = texCoordFrame->topRight.uv; //a potential optimization
        //*--ptr = texCoordFrame->topLeft.uv; //(that has now been implemented)
        //*--ptr = PVR_PACK_16BIT_UV(texCoordFrame->botRight.u, texCoordFrame->botRight.v);
        //*--ptr = PVR_PACK_16BIT_UV(texCoordFrame->botRight.u, texCoordFrame->topLeft.v);
        //*--ptr = PVR_PACK_16BIT_UV(texCoordFrame->topLeft.u, texCoordFrame->topLeft.v);
        _texOrient = GY_RECT_TEX_ORIENT_NORMAL;
    }
    else ptr -= 3;
    *--ptr = 0;
    *(float*)--ptr = _worldVert[3].y;
    *(float*)--ptr = _worldVert[3].x;
    *(float*)--ptr = _worldVert[2].z;
    *(float*)--ptr = _worldVert[2].y;
    PREFETCH((void*)ptr);
    *(float*)--ptr = _worldVert[2].x;
    *(float*)--ptr = _worldVert[1].z;
    *(float*)--ptr = _worldVert[1].y;
    *(float*)--ptr = _worldVert[1].x;
    *(float*)--ptr = _worldVert[0].z;
    *(float*)--ptr = _worldVert[0].y;
    *(float*)--ptr = _worldVert[0].x;
    *--ptr = PVR_CMD_VERTEX_EOL;
    PREFETCH((void*)ptr);
    ptr += 16;

    _current_ptr = ptr;
}

void gyVidQuad(const struct GYVector4 *const verts, const int frame) {
    register uint32_t *ptr = _current_ptr;
    GYTexCoordFrame *texCoordFrame = (_texCoordSheet)? &_texCoordSheet->texCoord[frame] : NULL;
    GYTexCoord2 texCoord[3];

    //transform local vertices by internal SH4 matrix accumulator
    gyMatMultVec(verts, &_worldVert[0], 4);

    //commit hardware sprite vertex
    ptr += 16; //64-byte vertex
    if(texCoordFrame) {
#if 0
        gyVidTexCoordArrayFromFrame(texCoord, texCoordFrame);
        *--ptr = texCoord[2].uv;
        *--ptr = texCoord[1].uv;
        *--ptr = texCoord[0].uv;
#endif
        //*--ptr = texCoordFrame->botRight.uv; //prepacking these bitches is
        //*--ptr = texCoordFrame->topRight.uv; //a potential optimization
        //*--ptr = texCoordFrame->topLeft.uv; //(that has now been implemented) 
        //**--ptr = PVR_PACK_16BIT_UV(texCoordFrame->botRight.u, texCoordFrame->botRight.v);
        //*--ptr = PVR_PACK_16BIT_UV(texCoordFrame->botRight.u, texCoordFrame->topLeft.v);
        //*--ptr = PVR_PACK_16BIT_UV(texCoordFrame->topLeft.u, texCoordFrame->topLeft.v);*/
        _texOrient = GY_RECT_TEX_ORIENT_NORMAL;
    }
    else ptr -= 3;
    *--ptr = 0;
    *(float*)--ptr = _worldVert[2].y; //Note the swapped vert 2/3 order:
    *(float*)--ptr = _worldVert[2].x; //GYAPI assumes these are submitted
    *(float*)--ptr = _worldVert[3].z; //as triangle strips (GL), but they
    *(float*)--ptr = _worldVert[3].y; //are SPRITES on Dreamcast
    PREFETCH((void*)ptr);
    *(float*)--ptr = _worldVert[3].x;
    *(float*)--ptr = _worldVert[1].z;
    *(float*)--ptr = _worldVert[1].y;
    *(float*)--ptr = _worldVert[1].x;
    *(float*)--ptr = _worldVert[0].z;
    *(float*)--ptr = _worldVert[0].y;
    *(float*)--ptr = _worldVert[0].x;
    *--ptr = PVR_CMD_VERTEX_EOL;
    PREFETCH((void*)ptr);
    ptr += 16;

    _current_ptr = ptr;
}




#include <gyro_shader_api.h>

// ===== NEWFANGLED BULLSHIT ======
int gyVidShadowPass(void) {}
void gyVidLightingSetEnable(const int enabled) {}
void gyVidLightSourceInit(GYLightSource* const light, const GY_LIGHT_TYPE type){}
void gyVidLightSourceUninit(GYLightSource* const light) {}
void gyVidMaterialBind(const GYMaterial*const material) {}
void gyVidLightSourceAdd(GYLightSource* const light){}
void gyVidLightSourceRemove(const GYLightSource* const light) {}
void gyVidSetGlobalAmbience(const GYVector4*const color) {}
int gyShadersReloadAll(void) {}
void gyVidLightSourceReplace(const GYLightSource*const old, GYLightSource*const next) {
}

void gyVidLightSourceClear(void) {}
void gyVidLightSourceBeginShadowPasses(void) {}
void gyVidLightSourceBeginShadowPass(GYLightSource* const light, const GY_CUBE_MAP_FACE face) {}
void gyVidLightSourceEndShadowPasses(void) {}
unsigned gyVidDbgVertexRenderCount(void) { return 0; }
unsigned gyVidDbgShaderUniformChangeCount(void) { return 0; }
unsigned gyVidDbgTextureChangeCount(void) { return 0; }
#include <spine/spine.h>
void gyVidSkeleton(const spSkeleton*const skeleton) {}
void gyVidDepthTest(const int enable){}
void gyVidBlendMode(const GY_BLEND_MODE mode){}
void gyVidVertexArrayBind(int id) {}
void gyVidVertexBatchBegin(void (*flushCallback)(void*), void* obj) {

}

void gyVidVertexBatchEnd(void) {

}

void gyVidVertexBatchFlush(void) {
}

void glalVidNormalMapZFactorUpdateUniform(const float factor) {}
void gyVidNormalMapBind(const GYTexture * const tex) {}
void gyVidSpecularMapBind(const GYTexture * const tex) {}


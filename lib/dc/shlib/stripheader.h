///////////////////////////////////////////////////////////
//      _____ _____ __    _____ _____    ___   ___       //
//     |   __|  |  |  |  |     | __  |  |_  | |   |      //
//     |__   |     |  |__|-   -| __ -|  |  _|_| | |      //
//     |_____|__|__|_____|_____|_____|  |___|_|___|      //
//                                                       //
///////////////////////////////////////////////////////////
// Strip header library 2.0                              //
//                                                       //
// This library is used to generate and update strip     //
// headers for use with the SEGA Dreamcast hardware in   //
// an OpenGL-like manner.                                //
//                                                       //
// Author: Anton Norgren (Tvspelsfreak) (2011)           //
///////////////////////////////////////////////////////////

/*
 Strip header generation library.

 This can generate strip headers for use with the dreamcast.
 It supports most parameters that are of interest.

 NOTE: Constants and functions ending with a "2" are meant for
    the secondary area for two-parameter polygons and are
    only valid to set for those types.
*/

/*
 Type   Primitive   Color       Modifier type   Textured   32BIT UV
 ----   ---------   -----       -------------   --------   --------
 00     Polygon     Packed      Shadow          No         -
 01     Polygon     Float       Shadow          No         -
 02     Polygon     Intensity   Shadow          No         -
 03     Polygon     Packed      Shadow          Yes        Yes
 04     Polygon     Packed      Shadow          Yes        No
 05     Polygon     Float       Shadow          Yes        Yes
 06     Polygon     Float       Shadow          Yes        No
 07     Polygon     Intensity   Shadow          Yes        Yes
 08     Polygon     Intensity   Shadow          Yes        No
 09     Polygon     Packed      Two-parameter   No         -
 10     Polygon     Intensity   Two-parameter   No         -
 11     Polygon     Packed      Two-parameter   Yes        Yes
 12     Polygon     Packed      Two-parameter   Yes        No
 13     Polygon     Intensity   Two-parameter   Yes        Yes
 14     Polygon     Intensity   Two-parameter   Yes        No
 15     Sprite      Packed      -               No         -
 16     Sprite      Packed      -               Yes        No
 17     Modifier    -           -               -          -
*/

#ifndef __STRIPHEADER_H__
#define __STRIPHEADER_H__

#include <kos.h>
#include "shtexture.h"


/***** Error handling *****/

typedef enum 
{
    SH_ERROR_OK,
    SH_ERROR_INVALID_TYPE,          // The strip header type is not 0..17
    SH_ERROR_INVALID_LIST,          // Invalid list for the current header type
    SH_ERROR_CAPABILITY,            // Invalid capability for shEnable/shDisable
    SH_ERROR_NOT_PALETTED,          // Trying to perform a palette operation when the current texture is not paletted
    SH_ERROR_PALETTE_OUT_OF_BOUNDS, // Palette index is out of bounds
    SH_ERROR_TEXTURE_SIZE,          // Invalid texture size
    SH_ERROR_NOT_ALLOWED            // Operation is not allowed for this type
} SHERROR;

// I came up with this since checking return values for every function sucks.
// Use this to register an error handler function. This is called whenever
// an error occurs, and one of the error codes above are passed. 
// To remove the error handler, simply set this to NULL.
// fname is the name of the function which the error occured in.
void shErrorHandler( void (*hnd)(SHERROR, const char* fname) );


/***** Capabilities for shEnable/shDisable *****/

typedef enum 
{
    // Controls whether or not polygons are affected by modifiers.
    // This setting can be changed for types 0-8.
    // It is mandatory for types 9-14.
    SH_AFFECTED_BY_MODIFIER,

    // Controls smooth (gouraud) shading.
    // Valid for polygon types.
    SH_SMOOTH_SHADING,

    // When enabled, offset color is valid and will be added to base color.
    // Valid for textured types.
    SH_OFFSET_COLOR,

    // When enabled, the color specified in a previous intensity color
    // header will be used. This is slightly faster, but requires that
    // a color has already been sent in a previous header.
    // Valid for intensity color types.
    SH_USE_PREVIOUS_COLOR,

    // Enabling this will improve mipmap calculation at the
    // expense of processing time.
    // Valid for textured types.
    SH_DCALC_CONTROL,

    // Controls whether or not alpha processing is enabled.
    // Valid for types 0-16.
    SH_ALPHA,
    SH_ALPHA_2,

    // When enabled, the secondary accumulation buffer is used as input
    // for blending.
    // Valid for types 0-16.
    SH_SRC_SELECT,
    SH_SRC_SELECT_2,

    // When enabled, the secondary accumulation buffer is used as output
    // for blending.
    // Valid for types 0-16.
    SH_DST_SELECT,
    SH_DST_SELECT_2,

    // Controls whether or not texture alpha processing is enabled.
    // Valid for textured types.
    SH_TEXTURE_ALPHA,
    SH_TEXTURE_ALPHA_2,

    // Controls whether or not perform super-sampling of textures.
    // Valid for textured types.
    SH_TEX_SUPER_SAMPLING,
    SH_TEX_SUPER_SAMPLING_2

} SHCAPABILITY;

/***** Function-specific values *****/

// Fog
typedef enum
{
    SH_FOG_LOOKUP_TABLE		= 0,
    SH_FOG_PER_VERTEX		= 1,
    SH_FOG_DISABLE		= 2,
    SH_FOG_LOOKUP_TABLE_2	= 3
} SHFOGMODE;

// Culling
typedef enum
{
    SH_CULL_NONE		= 0,
    SH_CULL_SMALL		= 1,
    SH_CULL_CW			= 3,
    SH_CULL_CCW			= 2
} SHCULLMODE;

// Mipmap adjustment
typedef enum
{
    SH_MIPMAP_ADJUST_0_25	= 1,
    SH_MIPMAP_ADJUST_0_50	= 2,
    SH_MIPMAP_ADJUST_0_75	= 3,
    SH_MIPMAP_ADJUST_1_00	= 4,
    SH_MIPMAP_ADJUST_1_25	= 5,
    SH_MIPMAP_ADJUST_1_50	= 6,
    SH_MIPMAP_ADJUST_1_75	= 7,
    SH_MIPMAP_ADJUST_2_00	= 8,
    SH_MIPMAP_ADJUST_2_25	= 9,
    SH_MIPMAP_ADJUST_2_50	= 10,
    SH_MIPMAP_ADJUST_2_75	= 11,
    SH_MIPMAP_ADJUST_3_00	= 12,
    SH_MIPMAP_ADJUST_3_25	= 13,
    SH_MIPMAP_ADJUST_3_50	= 14,
    SH_MIPMAP_ADJUST_3_75	= 15
} SHMIPMAPADJUST;

// Blend func
typedef enum
{
    SH_BLEND_ZERO		= 0,
    SH_BLEND_ONE		= 1,
    SH_BLEND_DST_COLOR		= 2,
    SH_BLEND_INVERSE_DST_COLOR	= 3,
    SH_BLEND_SRC_ALPHA		= 4,
    SH_BLEND_INVERSE_SRC_ALPHA	= 5,
    SH_BLEND_DST_ALPHA		= 6,
    SH_BLEND_INVERSE_DST_ALPHA	= 7
} SHBLENDFUNC;

// Texture filter
typedef enum
{
    SH_FILTER_POINT		= 0,
    SH_FILTER_BILINEAR		= 1,
    SH_FILTER_TRILINEAR_PASS_A	= 2,
    SH_FILTER_TRILINEAR_PASS_B	= 3
} SHTEXTUREFILTER;

// Modifier instruction
typedef enum
{
    SH_MODIFIER_NORMAL		= 0,
    SH_MODIFIER_INSIDE_LAST	= 1,
    SH_MODIFIER_OUTSIDE_LAST	= 2
} SHMODIFIERINSTRUCTION;


// Strip header, 64 bytes
// Never edit this manually!
typedef struct stripheader
{
    uint32	type;
    uint32	words[6];
    uint32	unused;
    float	color0[4];
    float	color1[4];
    uint8	sprColor[4];
} stripheader_t;


// Initializes a strip header of the given type.
// See the table at the top of this file for an explanation of types.
// For untextured types, pass NULL instead of textures. You don't HAVE to pass a texture
// even for textured types, but you need to set one before you actually use the header.
int shInit( stripheader_t* hdr, uint32 type, pvr_list_t list, const texture_t* tex0, const texture_t* tex1 );

// Enable or disable strip capabilities.
// These work like glEnable/glDisable.
int shEnable( stripheader_t* hdr, SHCAPABILITY cap );
int shDisable( stripheader_t* hdr, SHCAPABILITY cap );

// Set cull mode for this strip.
// Valid for all types.
int shCullMode( stripheader_t* hdr, SHCULLMODE mode );

// Set fog mode for this strip.
// Use SH_FOG_* values.
// Valid for types 0-16.
int shFogMode( stripheader_t* hdr, SHFOGMODE mode );
int shFogMode2( stripheader_t* hdr, SHFOGMODE mode );

// Set mipmap adjustment.
// Use SH_MIPMAP_ADJUST_* values.
// Valid for textured types.
int shMipmapAdjust( stripheader_t* hdr, SHMIPMAPADJUST adjust );
int shMipmapAdjust2( stripheader_t* hdr, SHMIPMAPADJUST adjust );

// Set source and destination blend functions.
// Use SH_BLEND_* values.
// Valid for types 0-16.
int shBlendFunc( stripheader_t* hdr, SHBLENDFUNC src, SHBLENDFUNC dst );
int shBlendFunc2( stripheader_t* hdr, SHBLENDFUNC src, SHBLENDFUNC dst );

// Controls what texture filter to use.
// Use SH_FILTER* values.
// Valid for textured types.
int shTextureFilter( stripheader_t* hdr, SHTEXTUREFILTER filter );
int shTextureFilter2( stripheader_t* hdr, SHTEXTUREFILTER filter );

// Controls what hardware palette to use.
// Valid for textured types.
// NOTE: Only valid to set if the current texture is paletted.
// NOTE: The value given for this isn't a named constant, but a number.
//       0-3 for 8BPP palettes and 0-63 for 4BPP palettes.
int shPalette( stripheader_t* hdr, uint32 index );
int shPalette2( stripheader_t* hdr, uint32 index );

// Set texture.
// Valid for textured types.
int shTexture( stripheader_t* hdr, const texture_t* tex );
int shTexture2( stripheader_t* hdr, const texture_t* tex );

// Set modifier instruction.
// ONLY valid for type 17.
int shModifierInstruction( stripheader_t* hdr, SHMODIFIERINSTRUCTION instr );

// Set base color.
// Valid for intensity color polygons and sprites.
int shBaseColor( stripheader_t* hdr, float a, float r, float g, float b );
int shBaseColor2( stripheader_t* hdr, float a, float r, float g, float b );
int shSpriteColor( stripheader_t* hdr, uint8 *const color);

// Set offset color.
// Valid for textured intensity color polygons and textured sprites.
// NOTE: NOT valid for two-parameter polygons.
int shOffsetColor( stripheader_t* hdr, float a, float r, float g, float b );

// Copies the finished structure of a strip header to the given pointer
// using store queues and returns number of copied 32-bit words.
int shCommit( stripheader_t* hdr, uint32* ptr );

// TODO: Missing functionality
//int shDepthFunc();
//int shTexEnv();
//int shFlipUV();
//int shClampUV();

#endif // __STRIPHEADER_H__

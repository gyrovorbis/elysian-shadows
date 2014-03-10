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

#include "stripheader.h"

/////////////////////////////////////////////////////
// Parameter control word                          //
/////////////////////////////////////////////////////

// Type, bits 31-29
#define PCW_TYPE_SHIFT				29
#define PCW_TYPE_END_OF_LIST			(0 << PCW_TYPE_SHIFT)
#define PCW_TYPE_USER_TILE_CLIP			(1 << PCW_TYPE_SHIFT)
#define PCW_TYPE_OBJECT_LIST_SET		(2 << PCW_TYPE_SHIFT)
#define PCW_TYPE_POLYGON			(4 << PCW_TYPE_SHIFT)
#define PCW_TYPE_MODIFIER			(4 << PCW_TYPE_SHIFT)
#define PCW_TYPE_SPRITE				(5 << PCW_TYPE_SHIFT)
#define PCW_TYPE_MASK				(7 << PCW_TYPE_SHIFT)

// List, bits 26-24
#define PCW_LIST_SHIFT				24
#define PCW_LIST_OP_POLYGON			(0 << PCW_LIST_SHIFT)
#define PCW_LIST_OP_MODIFIER			(1 << PCW_LIST_SHIFT)
#define PCW_LIST_TR_POLYGON			(2 << PCW_LIST_SHIFT)
#define PCW_LIST_TR_MODIFIER			(3 << PCW_LIST_SHIFT)
#define PCW_LIST_PT_POLYGON			(4 << PCW_LIST_SHIFT)
#define PCW_LIST_MASK				(7 << PCW_LIST_SHIFT)

// Update strip length & user clip, bit 23
#define PCW_UPDATE_GROUP_SHIFT			23
#define PCW_UPDATE_GROUP_OFF			(0 << PCW_UPDATE_GROUP_SHIFT)
#define PCW_UPDATE_GROUP_ON			(1 << PCW_UPDATE_GROUP_SHIFT)
#define PCW_UPDATE_GROUP_MASK			(1 << PCW_UPDATE_GROUP_SHIFT)

// Strip length, bits 19-18
#define PCW_STRIP_LENGTH_SHIFT			18
#define PCW_STRIP_LENGTH_1			(0 << PCW_STRIP_LENGTH_SHIFT)
#define PCW_STRIP_LENGTH_2			(1 << PCW_STRIP_LENGTH_SHIFT)
#define PCW_STRIP_LENGTH_4			(2 << PCW_STRIP_LENGTH_SHIFT)
#define PCW_STRIP_LENGTH_6			(3 << PCW_STRIP_LENGTH_SHIFT)
#define PCW_STRIP_LENGTH_MASK			(3 << PCW_STRIP_LENGTH_SHIFT)

// User clip, bits 17-16
#define PCW_USER_CLIP_SHIFT			16
#define PCW_USER_CLIP_DISABLE			(0 << PCW_USER_CLIP_SHIFT)
#define PCW_USER_CLIP_INSIDE			(2 << PCW_USER_CLIP_SHIFT)
#define PCW_USER_CLIP_OUTSIDE			(3 << PCW_USER_CLIP_SHIFT)
#define PCW_USER_CLIP_MASK			(3 << PCW_USER_CLIP_SHIFT)

// Enable modifiers (for polygons), bit 7
#define PCW_MODIFIER_SHIFT			7
#define PCW_MODIFIER_DISABLE			(0 << PCW_MODIFIER_SHIFT)
#define PCW_MODIFIER_ENABLE			(1 << PCW_MODIFIER_SHIFT)
#define PCW_MODIFIER_MASK			(1 << PCW_MODIFIER_SHIFT)

// Modifier type (for polygons), bit 6
#define PCW_MODIFIER_TYPE_SHIFT			6
#define PCW_MODIFIER_TYPE_SHADOW		(0 << PCW_MODIFIER_TYPE_SHIFT)
#define PCW_MODIFIER_TYPE_NORMAL		(1 << PCW_MODIFIER_TYPE_SHIFT)
#define PCW_MODIFIER_TYPE_MASK			(1 << PCW_MODIFIER_TYPE_SHIFT)

// Last triangle in volume (for modifiers), bit 6
#define PCW_MODIFIER_TRIANGLE_SHIFT		6
#define PCW_MODIFIER_TRIANGLE			(0 << PCW_MODIFIER_TRIANGLE_SHIFT)
#define PCW_MODIFIER_TRIANGLE_LAST		(1 << PCW_MODIFIER_TRIANGLE_SHIFT)
#define PCW_MODIFIER_TRIANGLE_MASK		(1 << PCW_MODIFIER_TRIANGLE_SHIFT)

// Color type, bits 5-4
#define PCW_COLOR_TYPE_SHIFT			4
#define PCW_COLOR_TYPE_PACKED			(0 << PCW_COLOR_TYPE_SHIFT)
#define PCW_COLOR_TYPE_FLOAT			(1 << PCW_COLOR_TYPE_SHIFT)
#define PCW_COLOR_TYPE_INTENSITY		(2 << PCW_COLOR_TYPE_SHIFT)
#define PCW_COLOR_TYPE_PREV_INTENSITY		(3 << PCW_COLOR_TYPE_SHIFT)
#define PCW_COLOR_TYPE_MASK			(3 << PCW_COLOR_TYPE_SHIFT)

// Texture enable, bit 3
#define PCW_TEXTURE_SHIFT			3
#define PCW_TEXTURE_DISABLE			(0 << PCW_TEXTURE_SHIFT)
#define PCW_TEXTURE_ENABLE			(1 << PCW_TEXTURE_SHIFT)
#define PCW_TEXTURE_MASK			(1 << PCW_TEXTURE_SHIFT)

// Offset color enable, bit 2
#define PCW_OFFSET_COLOR_SHIFT			2
#define PCW_OFFSET_COLOR_DISABLE		(0 << PCW_OFFSET_COLOR_SHIFT)
#define PCW_OFFSET_COLOR_ENABLE			(1 << PCW_OFFSET_COLOR_SHIFT)
#define PCW_OFFSET_COLOR_MASK			(1 << PCW_OFFSET_COLOR_SHIFT)

// Shading, bit 1
#define PCW_SHADING_SHIFT			1
#define PCW_SHADING_FLAT			(0 << PCW_SHADING_SHIFT)
#define PCW_SHADING_GOURAUD			(1 << PCW_SHADING_SHIFT)
#define PCW_SHADING_MASK			(1 << PCW_SHADING_SHIFT)

// UV, bit 0
#define PCW_UV_SHIFT				0
#define PCW_UV_32BIT				(0 << PCW_UV_SHIFT)
#define PCW_UV_16BIT				(1 << PCW_UV_SHIFT)
#define PCW_UV_MASK				(1 << PCW_UV_SHIFT)

/////////////////////////////////////////////////////
// ISP/TSP instruction word                        //
/////////////////////////////////////////////////////

// Depth compare (for polygons), bits 31-29
#define ISP_TSP_DEPTH_COMPARE_SHIFT		29
#define ISP_TSP_DEPTH_COMPARE_NEVER		(0 << ISP_TSP_DEPTH_COMPARE_SHIFT)
#define ISP_TSP_DEPTH_COMPARE_LESS		(1 << ISP_TSP_DEPTH_COMPARE_SHIFT)
#define ISP_TSP_DEPTH_COMPARE_EQUAL		(2 << ISP_TSP_DEPTH_COMPARE_SHIFT)
#define ISP_TSP_DEPTH_COMPARE_LESS_OR_EQUAL	(3 << ISP_TSP_DEPTH_COMPARE_SHIFT)
#define ISP_TSP_DEPTH_COMPARE_GREATER		(4 << ISP_TSP_DEPTH_COMPARE_SHIFT)
#define ISP_TSP_DEPTH_COMPARE_NOT_EQUAL		(5 << ISP_TSP_DEPTH_COMPARE_SHIFT)
#define ISP_TSP_DEPTH_COMPARE_GREATER_OR_EQUAL	(6 << ISP_TSP_DEPTH_COMPARE_SHIFT)
#define ISP_TSP_DEPTH_COMPARE_ALWAYS		(7 << ISP_TSP_DEPTH_COMPARE_SHIFT)
#define ISP_TSP_DEPTH_COMPARE_MASK		(7 << ISP_TSP_DEPTH_COMPARE_SHIFT)

// Volume instruction (for modifiers), bits 31-29
#define ISP_TSP_VOLUME_INSTRUCTION_SHIFT	29
#define ISP_TSP_VOLUME_INSTRUCTION_NORMAL	(0 << ISP_TSP_VOLUME_INSTRUCTION_SHIFT)
#define ISP_TSP_VOLUME_INSTRUCTION_INSIDE_LAST	(1 << ISP_TSP_VOLUME_INSTRUCTION_SHIFT)
#define ISP_TSP_VOLUME_INSTRUCTION_OUTSIDE_LAST	(2 << ISP_TSP_VOLUME_INSTRUCTION_SHIFT)
#define ISP_TSP_VOLUME_INSTRUCTION_MASK		(7 << ISP_TSP_VOLUME_INSTRUCTION_SHIFT)

// Cull mode (for polygons and modifiers), bits 28-27
#define ISP_TSP_CULL_MODE_SHIFT			27
#define ISP_TSP_CULL_MODE_NONE			(0 << ISP_TSP_CULL_MODE_SHIFT)
#define ISP_TSP_CULL_MODE_SMALL			(1 << ISP_TSP_CULL_MODE_SHIFT)
#define ISP_TSP_CULL_MODE_COUNTER_CLOCKWISE	(2 << ISP_TSP_CULL_MODE_SHIFT)
#define ISP_TSP_CULL_MODE_CLOCKWISE		(3 << ISP_TSP_CULL_MODE_SHIFT)
#define ISP_TSP_CULL_MODE_MASK			(3 << ISP_TSP_CULL_MODE_SHIFT)

// Z write disable, bit 26
#define ISP_TSP_Z_WRITE_SHIFT			26
#define ISP_TSP_Z_WRITE_ENABLE			(0 << ISP_TSP_Z_WRITE_SHIFT)
#define ISP_TSP_Z_WRITE_DISABLE			(1 << ISP_TSP_Z_WRITE_SHIFT)
#define ISP_TSP_Z_WRITE_MASK			(1 << ISP_TSP_Z_WRITE_SHIFT)

// DCalc control, bit 20
#define ISP_TSP_DCALC_SHIFT			20
#define ISP_TSP_DCALC_DISABLE			(0 << ISP_TSP_DCALC_SHIFT)
#define ISP_TSP_DCALC_ENABLE			(1 << ISP_TSP_DCALC_SHIFT)
#define ISP_TSP_DCALC_MASK			(1 << ISP_TSP_DCALC_SHIFT)

/////////////////////////////////////////////////////
// TSP instruction word                            //
/////////////////////////////////////////////////////

// SRC Alpha instruction, bits 31-29
#define TSP_SRC_ALPHA_INSTR_SHIFT		29
#define TSP_SRC_ALPHA_INSTR_ZERO		(0 << TSP_SRC_ALPHA_INSTR_SHIFT)
#define TSP_SRC_ALPHA_INSTR_ONE			(1 << TSP_SRC_ALPHA_INSTR_SHIFT)
#define TSP_SRC_ALPHA_INSTR_DST_COLOR		(2 << TSP_SRC_ALPHA_INSTR_SHIFT)
#define TSP_SRC_ALPHA_INSTR_INVERSE_DST_COLOR	(3 << TSP_SRC_ALPHA_INSTR_SHIFT)
#define TSP_SRC_ALPHA_INSTR_SRC_ALPHA		(4 << TSP_SRC_ALPHA_INSTR_SHIFT)
#define TSP_SRC_ALPHA_INSTR_INVERSE_SRC_ALPHA	(5 << TSP_SRC_ALPHA_INSTR_SHIFT)
#define TSP_SRC_ALPHA_INSTR_DST_ALPHA		(6 << TSP_SRC_ALPHA_INSTR_SHIFT)
#define TSP_SRC_ALPHA_INSTR_INVERSE_DST_ALPHA	(7 << TSP_SRC_ALPHA_INSTR_SHIFT)
#define TSP_SRC_ALPHA_INSTR_MASK		(7 << TSP_SRC_ALPHA_INSTR_SHIFT)

// DST Alpha instruction, bits 28-26
#define TSP_DST_ALPHA_INSTR_SHIFT		26
#define TSP_DST_ALPHA_INSTR_ZERO		(0 << TSP_DST_ALPHA_INSTR_SHIFT)
#define TSP_DST_ALPHA_INSTR_ONE			(1 << TSP_DST_ALPHA_INSTR_SHIFT)
#define TSP_DST_ALPHA_INSTR_DST_COLOR		(2 << TSP_DST_ALPHA_INSTR_SHIFT)
#define TSP_DST_ALPHA_INSTR_INVERSE_DST_COLOR	(3 << TSP_DST_ALPHA_INSTR_SHIFT)
#define TSP_DST_ALPHA_INSTR_SRC_ALPHA		(4 << TSP_DST_ALPHA_INSTR_SHIFT)
#define TSP_DST_ALPHA_INSTR_INVERSE_SRC_ALPHA	(5 << TSP_DST_ALPHA_INSTR_SHIFT)
#define TSP_DST_ALPHA_INSTR_DST_ALPHA		(6 << TSP_DST_ALPHA_INSTR_SHIFT)
#define TSP_DST_ALPHA_INSTR_INVERSE_DST_ALPHA	(7 << TSP_DST_ALPHA_INSTR_SHIFT)
#define TSP_DST_ALPHA_INSTR_MASK		(7 << TSP_DST_ALPHA_INSTR_SHIFT)

// SRC select enable, bit 25
#define TSP_SRC_SELECT_SHIFT			25
#define TSP_SRC_SELECT_DISABLE			(0 << TSP_SRC_SELECT_SHIFT)
#define TSP_SRC_SELECT_ENABLE			(1 << TSP_SRC_SELECT_SHIFT)
#define TSP_SRC_SELECT_MASK			(1 << TSP_SRC_SELECT_SHIFT)

// DST select enable, bit 24
#define TSP_DST_SELECT_SHIFT			24
#define TSP_DST_SELECT_DISABLE			(0 << TSP_DST_SELECT_SHIFT)
#define TSP_DST_SELECT_ENABLE			(1 << TSP_DST_SELECT_SHIFT)
#define TSP_DST_SELECT_MASK			(1 << TSP_DST_SELECT_SHIFT)

// Fog mode, bits 23-22
#define TSP_FOG_MODE_SHIFT			22
#define TSP_FOG_MODE_LOOKUP_TABLE		(0 << TSP_FOG_MODE_SHIFT)
#define TSP_FOG_MODE_PER_VERTEX			(1 << TSP_FOG_MODE_SHIFT)
#define TSP_FOG_MODE_DISABLE			(2 << TSP_FOG_MODE_SHIFT)
#define TSP_FOG_MODE_LOOKUP_TABLE_2		(3 << TSP_FOG_MODE_SHIFT)
#define TSP_FOG_MODE_MASK			(3 << TSP_FOG_MODE_SHIFT)

// Color clamp enable, bit 21
#define TSP_COLOR_CLAMP_SHIFT			21
#define TSP_COLOR_CLAMP_DISABLE			(0 << TSP_COLOR_CLAMP_SHIFT)
#define TSP_COLOR_CLAMP_ENABLE			(1 << TSP_COLOR_CLAMP_SHIFT)
#define TSP_COLOR_CLAMP_MASK			(1 << TSP_COLOR_CLAMP_SHIFT)

// Alpha enable, bit 20
#define TSP_ALPHA_SHIFT				20
#define TSP_ALPHA_DISABLE			(0 << TSP_ALPHA_SHIFT)
#define TSP_ALPHA_ENABLE			(1 << TSP_ALPHA_SHIFT)
#define TSP_ALPHA_MASK				(1 << TSP_ALPHA_SHIFT)

// Texture alpha enable, bit 19
#define TSP_TEXTURE_ALPHA_SHIFT			19
#define TSP_TEXTURE_ALPHA_DISABLE		(1 << TSP_TEXTURE_ALPHA_SHIFT)
#define TSP_TEXTURE_ALPHA_ENABLE		(0 << TSP_TEXTURE_ALPHA_SHIFT)
#define TSP_TEXTURE_ALPHA_MASK			(1 << TSP_TEXTURE_ALPHA_SHIFT)

// Flip UV, bits 18-17
#define TSP_UV_FLIP_SHIFT			17
#define TSP_UV_FLIP_NONE			(0 << TSP_UV_FLIP_SHIFT)
#define TSP_UV_FLIP_V				(1 << TSP_UV_FLIP_SHIFT)
#define TSP_UV_FLIP_U				(2 << TSP_UV_FLIP_SHIFT)
#define TSP_UV_FLIP_UV				(3 << TSP_UV_FLIP_SHIFT)
#define TSP_UV_FLIP_MASK			(3 << TSP_UV_FLIP_SHIFT)

// Clamp UV, bits 16-15
#define TSP_UV_CLAMP_SHIFT			15
#define TSP_UV_CLAMP_NONE			(0 << TSP_UV_CLAMP_SHIFT)
#define TSP_UV_CLAMP_V				(1 << TSP_UV_CLAMP_SHIFT)
#define TSP_UV_CLAMP_U				(2 << TSP_UV_CLAMP_SHIFT)
#define TSP_UV_CLAMP_UV				(3 << TSP_UV_CLAMP_SHIFT)
#define TSP_UV_CLAMP_MASK			(3 << TSP_UV_CLAMP_SHIFT)

// Texture filter, bits 14-13
#define TSP_TEXTURE_FILTER_SHIFT		13
#define TSP_TEXTURE_FILTER_POINT		(0 << TSP_TEXTURE_FILTER_SHIFT)
#define TSP_TEXTURE_FILTER_BILINEAR		(1 << TSP_TEXTURE_FILTER_SHIFT)
#define TSP_TEXTURE_FILTER_TRILINEAR_PASS_A	(2 << TSP_TEXTURE_FILTER_SHIFT)
#define TSP_TEXTURE_FILTER_TRILINEAR_PASS_B	(3 << TSP_TEXTURE_FILTER_SHIFT)
#define TSP_TEXTURE_FILTER_MASK			(3 << TSP_TEXTURE_FILTER_SHIFT)

// Texture super sampling enable, bit 12
#define TSP_SUPER_SAMPLING_SHIFT		12
#define TSP_SUPER_SAMPLING_DISABLE		(0 << TSP_SUPER_SAMPLING_SHIFT)
#define TSP_SUPER_SAMPLING_ENABLE		(1 << TSP_SUPER_SAMPLING_SHIFT)
#define TSP_SUPER_SAMPLING_MASK			(1 << TSP_SUPER_SAMPLING_SHIFT)

// Mipmap adjust, bits 11-8
#define TSP_MIPMAP_ADJUST_SHIFT			8
#define TSP_MIPMAP_ADJUST_0_25			(1 << TSP_MIPMAP_ADJUST_SHIFT)
#define TSP_MIPMAP_ADJUST_0_50			(2 << TSP_MIPMAP_ADJUST_SHIFT)
#define TSP_MIPMAP_ADJUST_0_75			(3 << TSP_MIPMAP_ADJUST_SHIFT)
#define TSP_MIPMAP_ADJUST_1_00			(4 << TSP_MIPMAP_ADJUST_SHIFT)
#define TSP_MIPMAP_ADJUST_1_25			(5 << TSP_MIPMAP_ADJUST_SHIFT)
#define TSP_MIPMAP_ADJUST_1_50			(6 << TSP_MIPMAP_ADJUST_SHIFT)
#define TSP_MIPMAP_ADJUST_1_75			(7 << TSP_MIPMAP_ADJUST_SHIFT)
#define TSP_MIPMAP_ADJUST_2_00			(8 << TSP_MIPMAP_ADJUST_SHIFT)
#define TSP_MIPMAP_ADJUST_2_25			(9 << TSP_MIPMAP_ADJUST_SHIFT)
#define TSP_MIPMAP_ADJUST_2_50			(10 << TSP_MIPMAP_ADJUST_SHIFT)
#define TSP_MIPMAP_ADJUST_2_75			(11 << TSP_MIPMAP_ADJUST_SHIFT)
#define TSP_MIPMAP_ADJUST_3_00			(12 << TSP_MIPMAP_ADJUST_SHIFT)
#define TSP_MIPMAP_ADJUST_3_25			(13 << TSP_MIPMAP_ADJUST_SHIFT)
#define TSP_MIPMAP_ADJUST_3_50			(14 << TSP_MIPMAP_ADJUST_SHIFT)
#define TSP_MIPMAP_ADJUST_3_75			(15 << TSP_MIPMAP_ADJUST_SHIFT)
#define TSP_MIPMAP_ADJUST_MASK			(15 << TSP_MIPMAP_ADJUST_SHIFT)

// Texture shading instruction, bits 7-6
#define TSP_TEXTURE_INSTRUCTION_SHIFT		6
#define TSP_TEXTURE_INSTRUCTION_DECAL		(0 << TSP_TEXTURE_INSTRUCTION_SHIFT)
#define TSP_TEXTURE_INSTRUCTION_MODULATE	(1 << TSP_TEXTURE_INSTRUCTION_SHIFT)
#define TSP_TEXTURE_INSTRUCTION_DECAL_ALPHA	(2 << TSP_TEXTURE_INSTRUCTION_SHIFT)
#define TSP_TEXTURE_INSTRUCTION_MODULATE_ALPHA	(3 << TSP_TEXTURE_INSTRUCTION_SHIFT)
#define TSP_TEXTURE_INSTRUCTION_MASK		(3 << TSP_TEXTURE_INSTRUCTION_SHIFT)

// Texture U size, bits 5-3
#define TSP_TEXTURE_U_SIZE_SHIFT		3
#define TSP_TEXTURE_U_SIZE_8			(0 << TSP_TEXTURE_U_SIZE_SHIFT)
#define TSP_TEXTURE_U_SIZE_16			(1 << TSP_TEXTURE_U_SIZE_SHIFT)
#define TSP_TEXTURE_U_SIZE_32			(2 << TSP_TEXTURE_U_SIZE_SHIFT)
#define TSP_TEXTURE_U_SIZE_64			(3 << TSP_TEXTURE_U_SIZE_SHIFT)
#define TSP_TEXTURE_U_SIZE_128			(4 << TSP_TEXTURE_U_SIZE_SHIFT)
#define TSP_TEXTURE_U_SIZE_256			(5 << TSP_TEXTURE_U_SIZE_SHIFT)
#define TSP_TEXTURE_U_SIZE_512			(6 << TSP_TEXTURE_U_SIZE_SHIFT)
#define TSP_TEXTURE_U_SIZE_1024			(7 << TSP_TEXTURE_U_SIZE_SHIFT)
#define TSP_TEXTURE_U_SIZE_MASK			(7 << TSP_TEXTURE_U_SIZE_SHIFT)

// Texture V size, bits 2-0
#define TSP_TEXTURE_V_SIZE_SHIFT		0
#define TSP_TEXTURE_V_SIZE_8			(0 << TSP_TEXTURE_V_SIZE_SHIFT)
#define TSP_TEXTURE_V_SIZE_16			(1 << TSP_TEXTURE_V_SIZE_SHIFT)
#define TSP_TEXTURE_V_SIZE_32			(2 << TSP_TEXTURE_V_SIZE_SHIFT)
#define TSP_TEXTURE_V_SIZE_64			(3 << TSP_TEXTURE_V_SIZE_SHIFT)
#define TSP_TEXTURE_V_SIZE_128			(4 << TSP_TEXTURE_V_SIZE_SHIFT)
#define TSP_TEXTURE_V_SIZE_256			(5 << TSP_TEXTURE_V_SIZE_SHIFT)
#define TSP_TEXTURE_V_SIZE_512			(6 << TSP_TEXTURE_V_SIZE_SHIFT)
#define TSP_TEXTURE_V_SIZE_1024			(7 << TSP_TEXTURE_V_SIZE_SHIFT)
#define TSP_TEXTURE_V_SIZE_MASK			(7 << TSP_TEXTURE_V_SIZE_SHIFT)

/////////////////////////////////////////////////////
// Texture control word                            //
/////////////////////////////////////////////////////

// Mipmapped, bit 31
#define TCW_MIPMAP_SHIFT			31
#define TCW_MIPMAP_DISABLED			(0 << TCW_MIPMAP_SHIFT)
#define TCW_MIPMAP_ENABLED			(1 << TCW_MIPMAP_SHIFT)
#define TCW_MIPMAP_MASK				(1 << TCW_MIPMAP_SHIFT)

// VQ compressed, bit 30
#define TCW_VQ_COMPRESSED_SHIFT			30
#define TCW_VQ_COMPRESSED_DISABLED		(0 << TCW_VQ_COMPRESSED_SHIFT)
#define TCW_VQ_COMPRESSED_ENABLED		(1 << TCW_VQ_COMPRESSED_SHIFT)
#define TCW_VQ_COMPRESSED_MASK			(1 << TCW_VQ_COMPRESSED_SHIFT)

// Texture format, bits 29-27
#define TCW_PIXEL_FORMAT_SHIFT			27
#define TCW_PIXEL_FORMAT_ARGB1555		(0 << TCW_PIXEL_FORMAT_SHIFT)
#define TCW_PIXEL_FORMAT_RGB565			(1 << TCW_PIXEL_FORMAT_SHIFT)
#define TCW_PIXEL_FORMAT_ARGB4444		(2 << TCW_PIXEL_FORMAT_SHIFT)
#define TCW_PIXEL_FORMAT_YUV422			(3 << TCW_PIXEL_FORMAT_SHIFT)
#define TCW_PIXEL_FORMAT_BUMP_MAP		(4 << TCW_PIXEL_FORMAT_SHIFT)
#define TCW_PIXEL_FORMAT_PAL_4BPP		(5 << TCW_PIXEL_FORMAT_SHIFT)
#define TCW_PIXEL_FORMAT_PAL_8BPP		(6 << TCW_PIXEL_FORMAT_SHIFT)
#define TCW_PIXEL_FORMAT_MASK			(7 << TCW_PIXEL_FORMAT_SHIFT)

// Twiddled (for non-paletted textures), bit 26
#define TCW_TWIDDLED_SHIFT			26
#define TCW_TWIDDLED_DISABLED			(1 << TCW_TWIDDLED_SHIFT)
#define TCW_TWIDDLED_ENABLED			(0 << TCW_TWIDDLED_SHIFT)
#define TCW_TWIDDLED_MASK			(1 << TCW_TWIDDLED_SHIFT)

// Stride enable (for non-paletted textures), bit 25
#define TCW_STRIDE_SHIFT			25
#define TCW_STRIDE_DISABLED			(0 << TCW_STRIDE_SHIFT)
#define TCW_STRIDE_ENABLED			(1 << TCW_STRIDE_SHIFT)
#define TCW_STRIDE_MASK				(1 << TCW_STRIDE_SHIFT)

// Palette index (for paletted textures), bits 26-21
#define TCW_PALETTE_INDEX_4BPP_SHIFT		21
#define TCW_PALETTE_INDEX_4BPP_MASK		(63 << TCW_PALETTE_INDEX_4BPP_SHIFT)
#define TCW_PALETTE_INDEX_8BPP_SHIFT		25
#define TCW_PALETTE_INDEX_8BPP_MASK		(63 << TCW_PALETTE_INDEX_8BPP_SHIFT)

// Texture address, bits 20-0
#define TCW_TEXTURE_ADDRESS(addr)		((((uint32)(void*)(addr))&0x7fffff)>>3)
#define TCW_TEXTURE_ADDRESS_MASK		(0x000FFFFF)


/*
===============================================================================

ERROR HANDLER FUNCTIONS

===============================================================================
*/

static void (*_error_handler)(SHERROR, const char* fnname) = NULL;

void shErrorHandler( void (*hnd)(SHERROR, const char* fnname) )
{
    _error_handler = hnd;
}

static inline void report_error( SHERROR err, const char* fnname )
{
    if ( _error_handler != NULL )
        (*_error_handler)( err, fnname );
}

/*
===============================================================================

STRIP HEADER UTILITIES

===============================================================================
*/

// Used to index the "words" array of a strip header
#define PCW	0
#define ISPTSP	1
#define TSP0	2
#define TCW0	3
#define TSP1	4
#define TCW1	5

// There's an "allowed" system in place to make sure bits aren't set for
// header types where they cannot be changed.
// These will help a lot for setting up "allowed masks".
#define BIT(n)			(1<<(n))
// All polygon types
#define TYPES_POLYGON		(BIT(0)|BIT(1)|BIT(2)|BIT(3)|BIT(4)|BIT(5)|BIT(6)|BIT(7)|BIT(8)|BIT(9)|BIT(10)|BIT(11)|BIT(12)|BIT(13)|BIT(14))
// All sprite types
#define TYPES_SPRITE		(BIT(15)|BIT(16))
// Modifier type
#define TYPES_MODIFIER  	(BIT(17))
// Polygons and sprites
#define TYPES_POLYSPRITE	(TYPES_POLYGON|TYPES_SPRITE)
// All types
#define TYPES_ALL		(TYPES_POLYGON|TYPES_SPRITE|TYPES_MODIFIER)
// Textured types
#define TYPES_TEXTURED		(BIT(3)|BIT(4)|BIT(5)|BIT(6)|BIT(7)|BIT(8)|BIT(11)|BIT(12)|BIT(13)|BIT(14)|BIT(16))
// Types affected by cheap shadow modifiers
#define TYPES_SHADOW		(BIT(0)|BIT(1)|BIT(2)|BIT(3)|BIT(4)|BIT(5)|BIT(6)|BIT(7)|BIT(8))
// Types affected by two-parameter modifiers
#define TYPES_POLYGON_2		(BIT(9)|BIT(10)|BIT(11)|BIT(12)|BIT(13)|BIT(14))
// Textured types affected by two-parameter modifiers
#define TYPES_TEXTURED_2	(TYPES_POLYGON_2 & TYPES_TEXTURED)
// Intensity color types
#define TYPES_INTENSITY		(BIT(2)|BIT(7)|BIT(8)|BIT(10)|BIT(13)|BIT(14))

// Returns 1 if bit <type> can be found in bitfield <types>, otherwise 0.
static inline int check_allowed( uint32 type, uint32 types )
{
    return ( ( types & BIT(type) ) != 0 );
}

// The following two functions are "safe" ways of setting boolean and generic values
// of a header. These are the meat of the lib as most other functions use them.
// Most errors are probably cought here as well. While I'd definitely prefer to
// do the error handling at a higher level to avoid passing top level functions
// everywhere, the amount of top level error checking would just be insane.

static int set_boolean_safe( stripheader_t* hdr, const char* fnname, int cond, int affected_word, uint32 allowed_types, uint32 bitmask, uint32 tval, uint32 fval )
{
    if ( hdr->type > 17 )
    {
        report_error( SH_ERROR_INVALID_TYPE, fnname );
        return 0;
    }

    if ( !check_allowed( hdr->type, allowed_types ) )
    {
        report_error( SH_ERROR_NOT_ALLOWED, fnname );
        return 0;
    }

    hdr->words[ affected_word ] &= ~bitmask;
    hdr->words[ affected_word ] |= ( cond ? tval : fval );
    return 1;
}

static int set_generic_safe( stripheader_t* hdr, const char* fnname, int affected_word, uint32 allowed_types, uint32 bitmask, uint32 shift, uint32 value )
{
    if ( hdr->type > 17 )
    {
        report_error( SH_ERROR_INVALID_TYPE, fnname );
        return 0;
    }

    if ( !check_allowed( hdr->type, allowed_types ) )
    {
        report_error( SH_ERROR_NOT_ALLOWED, fnname );
        return 0;
    }

    hdr->words[ affected_word ] &= ~bitmask;
    hdr->words[ affected_word ] |= ( ( value << shift ) & bitmask );
    return 1;
}

// Default parameter control words (just OR in the list and you're set)
static const uint32 default_pcw[18] = 
{
/*00*/	PCW_TYPE_POLYGON  | PCW_UPDATE_GROUP_ON | PCW_STRIP_LENGTH_2 | PCW_COLOR_TYPE_PACKED    | PCW_MODIFIER_DISABLE | PCW_MODIFIER_TYPE_SHADOW | PCW_TEXTURE_DISABLE | PCW_SHADING_GOURAUD,
/*01*/	PCW_TYPE_POLYGON  | PCW_UPDATE_GROUP_ON | PCW_STRIP_LENGTH_2 | PCW_COLOR_TYPE_FLOAT     | PCW_MODIFIER_DISABLE | PCW_MODIFIER_TYPE_SHADOW | PCW_TEXTURE_DISABLE | PCW_SHADING_GOURAUD,
/*02*/	PCW_TYPE_POLYGON  | PCW_UPDATE_GROUP_ON | PCW_STRIP_LENGTH_2 | PCW_COLOR_TYPE_INTENSITY | PCW_MODIFIER_DISABLE | PCW_MODIFIER_TYPE_SHADOW | PCW_TEXTURE_DISABLE | PCW_SHADING_GOURAUD,
/*03*/	PCW_TYPE_POLYGON  | PCW_UPDATE_GROUP_ON | PCW_STRIP_LENGTH_2 | PCW_COLOR_TYPE_PACKED    | PCW_MODIFIER_DISABLE | PCW_MODIFIER_TYPE_SHADOW | PCW_TEXTURE_ENABLE  | PCW_SHADING_GOURAUD | PCW_UV_32BIT,
/*04*/	PCW_TYPE_POLYGON  | PCW_UPDATE_GROUP_ON | PCW_STRIP_LENGTH_2 | PCW_COLOR_TYPE_PACKED    | PCW_MODIFIER_DISABLE | PCW_MODIFIER_TYPE_SHADOW | PCW_TEXTURE_ENABLE  | PCW_SHADING_GOURAUD | PCW_UV_16BIT,
/*05*/	PCW_TYPE_POLYGON  | PCW_UPDATE_GROUP_ON | PCW_STRIP_LENGTH_2 | PCW_COLOR_TYPE_FLOAT     | PCW_MODIFIER_DISABLE | PCW_MODIFIER_TYPE_SHADOW | PCW_TEXTURE_ENABLE  | PCW_SHADING_GOURAUD | PCW_UV_32BIT,
/*06*/	PCW_TYPE_POLYGON  | PCW_UPDATE_GROUP_ON | PCW_STRIP_LENGTH_2 | PCW_COLOR_TYPE_FLOAT     | PCW_MODIFIER_DISABLE | PCW_MODIFIER_TYPE_SHADOW | PCW_TEXTURE_ENABLE  | PCW_SHADING_GOURAUD | PCW_UV_16BIT,
/*07*/	PCW_TYPE_POLYGON  | PCW_UPDATE_GROUP_ON | PCW_STRIP_LENGTH_2 | PCW_COLOR_TYPE_INTENSITY | PCW_MODIFIER_DISABLE | PCW_MODIFIER_TYPE_SHADOW | PCW_TEXTURE_ENABLE  | PCW_SHADING_GOURAUD | PCW_UV_32BIT,
/*08*/	PCW_TYPE_POLYGON  | PCW_UPDATE_GROUP_ON | PCW_STRIP_LENGTH_2 | PCW_COLOR_TYPE_INTENSITY | PCW_MODIFIER_DISABLE | PCW_MODIFIER_TYPE_SHADOW | PCW_TEXTURE_ENABLE  | PCW_SHADING_GOURAUD | PCW_UV_16BIT,
/*09*/	PCW_TYPE_POLYGON  | PCW_UPDATE_GROUP_ON | PCW_STRIP_LENGTH_2 | PCW_COLOR_TYPE_PACKED    | PCW_MODIFIER_ENABLE  | PCW_MODIFIER_TYPE_NORMAL | PCW_TEXTURE_DISABLE | PCW_SHADING_GOURAUD,
/*10*/	PCW_TYPE_POLYGON  | PCW_UPDATE_GROUP_ON | PCW_STRIP_LENGTH_2 | PCW_COLOR_TYPE_INTENSITY | PCW_MODIFIER_ENABLE  | PCW_MODIFIER_TYPE_NORMAL | PCW_TEXTURE_DISABLE | PCW_SHADING_GOURAUD,
/*11*/	PCW_TYPE_POLYGON  | PCW_UPDATE_GROUP_ON | PCW_STRIP_LENGTH_2 | PCW_COLOR_TYPE_PACKED    | PCW_MODIFIER_ENABLE  | PCW_MODIFIER_TYPE_NORMAL | PCW_TEXTURE_ENABLE  | PCW_SHADING_GOURAUD | PCW_UV_32BIT,
/*12*/	PCW_TYPE_POLYGON  | PCW_UPDATE_GROUP_ON | PCW_STRIP_LENGTH_2 | PCW_COLOR_TYPE_PACKED    | PCW_MODIFIER_ENABLE  | PCW_MODIFIER_TYPE_NORMAL | PCW_TEXTURE_ENABLE  | PCW_SHADING_GOURAUD | PCW_UV_16BIT,
/*13*/	PCW_TYPE_POLYGON  | PCW_UPDATE_GROUP_ON | PCW_STRIP_LENGTH_2 | PCW_COLOR_TYPE_INTENSITY | PCW_MODIFIER_ENABLE  | PCW_MODIFIER_TYPE_NORMAL | PCW_TEXTURE_ENABLE  | PCW_SHADING_GOURAUD | PCW_UV_32BIT,
/*14*/	PCW_TYPE_POLYGON  | PCW_UPDATE_GROUP_ON | PCW_STRIP_LENGTH_2 | PCW_COLOR_TYPE_INTENSITY | PCW_MODIFIER_ENABLE  | PCW_MODIFIER_TYPE_NORMAL | PCW_TEXTURE_ENABLE  | PCW_SHADING_GOURAUD | PCW_UV_16BIT,
/*15*/	PCW_TYPE_SPRITE   | PCW_UPDATE_GROUP_ON | PCW_STRIP_LENGTH_2 | PCW_COLOR_TYPE_PACKED    | PCW_TEXTURE_DISABLE  | PCW_SHADING_FLAT,
/*16*/	PCW_TYPE_SPRITE   | PCW_UPDATE_GROUP_ON | PCW_STRIP_LENGTH_2 | PCW_COLOR_TYPE_PACKED    | PCW_TEXTURE_ENABLE   | PCW_SHADING_FLAT | PCW_UV_16BIT,
/*17*/	PCW_TYPE_MODIFIER | PCW_MODIFIER_TRIANGLE
};

// Default isp/tsp words
static const uint32 default_isptsp = ISP_TSP_DEPTH_COMPARE_GREATER_OR_EQUAL | ISP_TSP_CULL_MODE_NONE | ISP_TSP_Z_WRITE_ENABLE | ISP_TSP_DCALC_DISABLE;
static const uint32 default_isptsp_mod = ISP_TSP_VOLUME_INSTRUCTION_NORMAL | ISP_TSP_CULL_MODE_NONE;

// Default tsp words
#define TSP_BASE ( TSP_SRC_SELECT_DISABLE | TSP_DST_SELECT_DISABLE | TSP_FOG_MODE_DISABLE | TSP_COLOR_CLAMP_DISABLE | TSP_UV_FLIP_NONE | TSP_UV_CLAMP_NONE | TSP_TEXTURE_FILTER_POINT | TSP_SUPER_SAMPLING_DISABLE | TSP_MIPMAP_ADJUST_1_00 )
static const uint32 default_tsp_alpha   = TSP_BASE | TSP_ALPHA_ENABLE  | TSP_TEXTURE_ALPHA_ENABLE  | TSP_SRC_ALPHA_INSTR_SRC_ALPHA | TSP_DST_ALPHA_INSTR_INVERSE_SRC_ALPHA | TSP_TEXTURE_INSTRUCTION_MODULATE_ALPHA;
static const uint32 default_tsp_noalpha = TSP_BASE | TSP_ALPHA_DISABLE | TSP_TEXTURE_ALPHA_DISABLE | TSP_SRC_ALPHA_INSTR_ONE       | TSP_DST_ALPHA_INSTR_ZERO              | TSP_TEXTURE_INSTRUCTION_MODULATE;

/*
===============================================================================

STRIP HEADER METHODS

===============================================================================
*/

int shInit( stripheader_t* hdr, uint32 type, pvr_list_t list, const texture_t* tex0, const texture_t* tex1 )
{
    if ( type > 17 )
    {
        report_error( SH_ERROR_INVALID_TYPE, __func__ );
        return 0;
    }

    // Type 17 (modifier volume) doesn't need to initialize anything
    // but the pcw and isptsp word. So there's no point in doing full
    // initialization if that's the case.
    if ( type == 17 )
    {
        // Make sure modifiers have a modifier list specified
        if ( list == PVR_LIST_OP_POLY || list == PVR_LIST_TR_POLY || list == PVR_LIST_PT_POLY )
        {
            report_error( SH_ERROR_INVALID_LIST, __func__ );
            return 0;
        }

        // Start with a cleared header
        memset( hdr, 0, sizeof(stripheader_t) );

        // Set the header type, PCW and ISP/TSP words
        hdr->type = type;
        hdr->words[PCW] = default_pcw[type] | ( list << PCW_LIST_SHIFT );
        hdr->words[ISPTSP] = default_isptsp_mod;
    }

    // Every other type needs full initialization
    else
    {
        int twoparam, textured;

        // Make sure polygons and sprites have non-modifier lists specified
        if ( list == PVR_LIST_OP_MOD || list == PVR_LIST_TR_MOD )
        {
            report_error( SH_ERROR_INVALID_LIST, __func__ );
            return 0;
        }

        // Set the header type, PCW and ISP/TSP word
        hdr->type = type;
        hdr->words[PCW] = default_pcw[type] | ( list << PCW_LIST_SHIFT );
        hdr->words[ISPTSP] = default_isptsp;

        // These will make the rest easier
        twoparam = ( hdr->words[PCW] & PCW_MODIFIER_TYPE_MASK ) == PCW_MODIFIER_TYPE_NORMAL;
        textured = ( hdr->words[PCW] & PCW_TEXTURE_MASK ) == PCW_TEXTURE_ENABLE;

        // Set texture & shading words
        hdr->words[TSP0] = ( ( list > PVR_LIST_OP_MOD ) ? default_tsp_alpha : default_tsp_noalpha );
        hdr->words[TSP1] = ( twoparam ? hdr->words[TSP0] : 0 );

        // Set texture control words
        if ( textured )
        {
            shTexture( hdr, tex0 );

            if ( twoparam )
                shTexture2( hdr, tex1 );
            else
                hdr->words[TCW1] = 0;
        }
        else
        {
            hdr->words[TCW0] = 0;
            hdr->words[TCW1] = 0;
        }

        // Clear this one as well, just to be nice.. :)
        hdr->unused = 0;

        // Clear colors to white
        hdr->color0[0] = hdr->color0[1] = hdr->color0[2] = hdr->color0[3] = 1.0f;
        hdr->color1[0] = hdr->color1[1] = hdr->color1[2] = hdr->color1[3] = 1.0f;
    }

    return 1;
}

// Generic enable/disable method so we don't have to do this large
// switch statement twice.
static int set_enabled( stripheader_t* hdr, const char* fnname, SHCAPABILITY cap, int enable )
{
    switch ( cap )
    {
        case SH_AFFECTED_BY_MODIFIER:
            return set_boolean_safe( hdr, fnname, enable, PCW, TYPES_SHADOW, PCW_MODIFIER_MASK, PCW_MODIFIER_ENABLE, PCW_MODIFIER_DISABLE );
        case SH_SMOOTH_SHADING:
            return set_boolean_safe( hdr, fnname, enable, PCW, TYPES_POLYGON, PCW_SHADING_MASK, PCW_SHADING_GOURAUD, PCW_SHADING_FLAT );
        case SH_OFFSET_COLOR:
            return set_boolean_safe( hdr, fnname, enable, PCW, TYPES_TEXTURED, PCW_OFFSET_COLOR_MASK, PCW_OFFSET_COLOR_ENABLE, PCW_OFFSET_COLOR_DISABLE );
        case SH_USE_PREVIOUS_COLOR:
            return set_boolean_safe( hdr, fnname, enable, PCW, TYPES_INTENSITY, PCW_COLOR_TYPE_MASK, PCW_COLOR_TYPE_PREV_INTENSITY, PCW_COLOR_TYPE_INTENSITY );
        case SH_DCALC_CONTROL:
            return set_boolean_safe( hdr, fnname, enable, ISPTSP, TYPES_TEXTURED, ISP_TSP_DCALC_MASK, ISP_TSP_DCALC_ENABLE, ISP_TSP_DCALC_DISABLE );
        case SH_ALPHA:
            return set_boolean_safe( hdr, fnname, enable, TSP0, TYPES_POLYSPRITE, TSP_ALPHA_MASK, TSP_ALPHA_ENABLE, TSP_ALPHA_DISABLE );
        case SH_ALPHA_2:
            return set_boolean_safe( hdr, fnname, enable, TSP1, TYPES_POLYGON_2, TSP_ALPHA_MASK, TSP_ALPHA_ENABLE, TSP_ALPHA_DISABLE );
        case SH_SRC_SELECT:
            return set_boolean_safe( hdr, fnname, enable, TSP0, TYPES_POLYSPRITE, TSP_SRC_SELECT_MASK, TSP_SRC_SELECT_ENABLE, TSP_SRC_SELECT_DISABLE );
        case SH_SRC_SELECT_2:
            return set_boolean_safe( hdr, fnname, enable, TSP1, TYPES_POLYGON_2, TSP_SRC_SELECT_MASK, TSP_SRC_SELECT_ENABLE, TSP_SRC_SELECT_DISABLE );
        case SH_DST_SELECT:
            return set_boolean_safe( hdr, fnname, enable, TSP0, TYPES_POLYSPRITE, TSP_DST_SELECT_MASK, TSP_DST_SELECT_ENABLE, TSP_DST_SELECT_DISABLE );
        case SH_DST_SELECT_2:
            return set_boolean_safe( hdr, fnname, enable, TSP1, TYPES_POLYGON_2, TSP_DST_SELECT_MASK, TSP_DST_SELECT_ENABLE, TSP_DST_SELECT_DISABLE );
        case SH_TEXTURE_ALPHA:
            return set_boolean_safe( hdr, fnname, enable, TSP0, TYPES_TEXTURED, TSP_TEXTURE_ALPHA_MASK, TSP_TEXTURE_ALPHA_ENABLE, TSP_TEXTURE_ALPHA_DISABLE );
        case SH_TEXTURE_ALPHA_2:
            return set_boolean_safe( hdr, fnname, enable, TSP1, TYPES_TEXTURED_2, TSP_TEXTURE_ALPHA_MASK, TSP_TEXTURE_ALPHA_ENABLE, TSP_TEXTURE_ALPHA_DISABLE );
        case SH_TEX_SUPER_SAMPLING:
            return set_boolean_safe( hdr, fnname, enable, TSP0, TYPES_TEXTURED, TSP_SUPER_SAMPLING_MASK, TSP_SUPER_SAMPLING_ENABLE, TSP_SUPER_SAMPLING_DISABLE );
        case SH_TEX_SUPER_SAMPLING_2:
            return set_boolean_safe( hdr, fnname, enable, TSP1, TYPES_TEXTURED_2, TSP_SUPER_SAMPLING_MASK, TSP_SUPER_SAMPLING_ENABLE, TSP_SUPER_SAMPLING_DISABLE );
    }

    report_error( SH_ERROR_CAPABILITY, fnname );
    return 0;
}

int shEnable( stripheader_t* hdr, SHCAPABILITY cap )  { return set_enabled( hdr, __func__, cap, 1 ); }
int shDisable( stripheader_t* hdr, SHCAPABILITY cap ) { return set_enabled( hdr, __func__, cap, 0 ); }

// These are all straightforward enough
int shCullMode( stripheader_t* hdr, SHCULLMODE mode )
{
    return set_generic_safe( hdr, __func__, ISPTSP, TYPES_ALL, ISP_TSP_CULL_MODE_MASK, ISP_TSP_CULL_MODE_SHIFT, mode );
}

int shFogMode( stripheader_t* hdr, SHFOGMODE mode )
{
    return set_generic_safe( hdr, __func__, TSP0, TYPES_POLYSPRITE, TSP_FOG_MODE_MASK, TSP_FOG_MODE_SHIFT, mode );
}

int shFogMode2( stripheader_t* hdr, SHFOGMODE mode )
{
    return set_generic_safe( hdr, __func__, TSP1, TYPES_POLYGON_2, TSP_FOG_MODE_MASK, TSP_FOG_MODE_SHIFT, mode );
}

int shMipmapAdjust( stripheader_t* hdr, SHMIPMAPADJUST adjust )
{
    return set_generic_safe( hdr, __func__, TSP0, TYPES_TEXTURED, TSP_MIPMAP_ADJUST_MASK, TSP_MIPMAP_ADJUST_SHIFT, adjust );
}

int shMipmapAdjust2( stripheader_t* hdr, SHMIPMAPADJUST adjust )
{
    return set_generic_safe( hdr, __func__, TSP1, TYPES_TEXTURED_2, TSP_MIPMAP_ADJUST_MASK, TSP_MIPMAP_ADJUST_SHIFT, adjust );
}

int shTextureFilter( stripheader_t* hdr, SHTEXTUREFILTER filter )
{
    return set_generic_safe( hdr, __func__, TSP0, TYPES_TEXTURED, TSP_TEXTURE_FILTER_MASK, TSP_TEXTURE_FILTER_SHIFT, filter );
}

int shTextureFilter2( stripheader_t* hdr, SHTEXTUREFILTER filter )
{
    return set_generic_safe( hdr, __func__, TSP1, TYPES_TEXTURED_2, TSP_TEXTURE_FILTER_MASK, TSP_TEXTURE_FILTER_SHIFT, filter );
}

int shBlendFunc( stripheader_t* hdr, SHBLENDFUNC src, SHBLENDFUNC dst )
{
    return set_generic_safe( hdr, __func__, TSP0, TYPES_POLYSPRITE, TSP_SRC_ALPHA_INSTR_MASK, TSP_SRC_ALPHA_INSTR_SHIFT, src ) &&
            set_generic_safe( hdr, __func__, TSP0, TYPES_POLYSPRITE, TSP_DST_ALPHA_INSTR_MASK, TSP_DST_ALPHA_INSTR_SHIFT, dst );
}

int shBlendFunc2( stripheader_t* hdr, SHBLENDFUNC src, SHBLENDFUNC dst )
{
    return set_generic_safe( hdr, __func__, TSP1, TYPES_POLYGON_2, TSP_SRC_ALPHA_INSTR_MASK, TSP_SRC_ALPHA_INSTR_SHIFT, src ) &&
            set_generic_safe( hdr, __func__, TSP1, TYPES_POLYGON_2, TSP_DST_ALPHA_INSTR_MASK, TSP_DST_ALPHA_INSTR_SHIFT, dst );
}

int shModifierInstruction( stripheader_t* hdr, SHMODIFIERINSTRUCTION instr ) 
{ 
	// Need to set both both the instruction and the "last triangle in volume" flag.
    return set_boolean_safe( hdr, __func__, instr != SH_MODIFIER_NORMAL, PCW, TYPES_MODIFIER, PCW_MODIFIER_TRIANGLE_MASK, PCW_MODIFIER_TRIANGLE_LAST, PCW_MODIFIER_TRIANGLE ) &&
            set_generic_safe( hdr, __func__, ISPTSP, TYPES_MODIFIER, ISP_TSP_VOLUME_INSTRUCTION_MASK, ISP_TSP_VOLUME_INSTRUCTION_SHIFT, instr );
}

int shPalette( stripheader_t* hdr, uint32 index )  
{ 
    const uint32 format = hdr->words[TCW0] & TCW_PIXEL_FORMAT_MASK;

    switch ( format )
    {
        case TCW_PIXEL_FORMAT_PAL_4BPP:
            if ( index < 64 )
                return set_generic_safe( hdr, __func__, TCW0, TYPES_TEXTURED, TCW_PALETTE_INDEX_4BPP_MASK, TCW_PALETTE_INDEX_4BPP_SHIFT, index );
            report_error( SH_ERROR_PALETTE_OUT_OF_BOUNDS, __func__ );
            break;

        case TCW_PIXEL_FORMAT_PAL_8BPP:
            if ( index < 4 )
                return set_generic_safe( hdr, __func__, TCW0, TYPES_TEXTURED, TCW_PALETTE_INDEX_8BPP_MASK, TCW_PALETTE_INDEX_8BPP_SHIFT, index );
            report_error( SH_ERROR_PALETTE_OUT_OF_BOUNDS, __func__ );
            break;

        default:
            report_error( SH_ERROR_NOT_PALETTED, __func__ );
            break;
    }

    return 0;
}

int shPalette2( stripheader_t* hdr, uint32 index )
{ 
    const uint32 format = hdr->words[TCW0] & TCW_PIXEL_FORMAT_MASK;

    switch ( format )
    {
        case TCW_PIXEL_FORMAT_PAL_4BPP:
            if ( index < 64 )
                return set_generic_safe( hdr, __func__, TCW1, TYPES_TEXTURED_2, TCW_PALETTE_INDEX_4BPP_MASK, TCW_PALETTE_INDEX_4BPP_SHIFT, index );
            report_error( SH_ERROR_PALETTE_OUT_OF_BOUNDS, __func__ );
            break;

        case TCW_PIXEL_FORMAT_PAL_8BPP:
            if ( index < 4 )
                return set_generic_safe( hdr, __func__, TCW1, TYPES_TEXTURED_2, TCW_PALETTE_INDEX_8BPP_MASK, TCW_PALETTE_INDEX_8BPP_SHIFT, index );
            report_error( SH_ERROR_PALETTE_OUT_OF_BOUNDS, __func__ );
            break;

        default:
            report_error( SH_ERROR_NOT_PALETTED, __func__ );
            break;
    }

    return 0;
}

// This method will do what's needed to set a texture. This involves generating
// a texture control word and setting the texture size flags in the tsp word.
// hdr: Pointer to header that is to be modified
// tsp: Index of tsp word (either TSP0 or TSP1)
// tcw: Index of tcw (either TCW0 or TCW1)
// allowed: Bitfield of allowed types
// tex: Texture to set. If given NULL, this method will simply clear all affected bits.
// Returns 1 on success or 0 on failure.
static int set_texture( stripheader_t* hdr, const char* fnname, int tsp, int tcw, uint32 allowed, const texture_t* tex )
{
    // Make sure this operation is allowed
    if ( !check_allowed( hdr->type, allowed ) )
    {
        //dbglog( DBG_DEBUG, "%u\n", hdr->type );
        report_error( SH_ERROR_NOT_ALLOWED, fnname );
        return 0;
    }

    // Clear texture size bits in tsp word
    hdr->words[tsp] &= ~TSP_TEXTURE_U_SIZE_MASK;
    hdr->words[tsp] &= ~TSP_TEXTURE_V_SIZE_MASK;

    // Clear texture control word
    hdr->words[tcw] = 0;

    if ( tex != NULL )
    {
        uint32 tmp = 0;
        int paletted = 0;

        switch ( tex->width )
        {
            case 8:	tmp |= TSP_TEXTURE_U_SIZE_8;	break;
            case 16:	tmp |= TSP_TEXTURE_U_SIZE_16;	break;
            case 32:	tmp |= TSP_TEXTURE_U_SIZE_32;	break;
            case 64:	tmp |= TSP_TEXTURE_U_SIZE_64;	break;
            case 128:	tmp |= TSP_TEXTURE_U_SIZE_128;	break;
            case 256:	tmp |= TSP_TEXTURE_U_SIZE_256;	break;
            case 512:	tmp |= TSP_TEXTURE_U_SIZE_512;	break;
            case 1024:	tmp |= TSP_TEXTURE_U_SIZE_1024;	break;
            default:
                report_error( SH_ERROR_TEXTURE_SIZE, fnname );
                return 0;
        }

        switch ( tex->height )
        {
            case 8:	tmp |= TSP_TEXTURE_V_SIZE_8;	break;
            case 16:	tmp |= TSP_TEXTURE_V_SIZE_16;	break;
            case 32:	tmp |= TSP_TEXTURE_V_SIZE_32;	break;
            case 64:	tmp |= TSP_TEXTURE_V_SIZE_64;	break;
            case 128:	tmp |= TSP_TEXTURE_V_SIZE_128;	break;
            case 256:	tmp |= TSP_TEXTURE_V_SIZE_256;	break;
            case 512:	tmp |= TSP_TEXTURE_V_SIZE_512;	break;
            case 1024:	tmp |= TSP_TEXTURE_V_SIZE_1024;	break;
            default:
                report_error( SH_ERROR_TEXTURE_SIZE, fnname );
                return 0;
        }

        // Set texture sizes in tsp word
        hdr->words[tsp] |= tmp;

        // Mipmap and compression flags
        hdr->words[tcw] |= ( ( tex->flags & TEXFLAG_MIPMAPPED ) ? TCW_MIPMAP_ENABLED : TCW_MIPMAP_DISABLED );
        hdr->words[tcw] |= ( ( tex->flags & TEXFLAG_COMPRESSED ) ? TCW_VQ_COMPRESSED_ENABLED : TCW_VQ_COMPRESSED_DISABLED );

        // Format
        switch ( tex->format )
        {
            case TEXFMT_RGB565:         hdr->words[tcw] |= TCW_PIXEL_FORMAT_RGB565;	paletted = 0;	break;
            case TEXFMT_ARGB1555:	hdr->words[tcw] |= TCW_PIXEL_FORMAT_ARGB1555;	paletted = 0;	break;
            case TEXFMT_ARGB4444:	hdr->words[tcw] |= TCW_PIXEL_FORMAT_ARGB4444;	paletted = 0;	break;
            case TEXFMT_PAL4BPP:	hdr->words[tcw] |= TCW_PIXEL_FORMAT_PAL_4BPP;	paletted = 1;	break;
            case TEXFMT_PAL8BPP:	hdr->words[tcw] |= TCW_PIXEL_FORMAT_PAL_8BPP;	paletted = 1;	break;
        }

        if ( paletted )
        {
            // Paletted textures are assumed to be twiddled and doesn't allow stride.
            // This is because these settings occupy the same bits as palette index.
        }
        else
        {
            // Twiddle and stride flags
            hdr->words[tcw] |= ( ( tex->flags & TEXFLAG_TWIDDLED ) ? TCW_TWIDDLED_ENABLED : TCW_TWIDDLED_DISABLED );
            hdr->words[tcw] |= TCW_STRIDE_DISABLED;
        }

        // Texture address
        hdr->words[tcw] |= TCW_TEXTURE_ADDRESS( tex->vram_ptr );
    }

    return 1;
}

int shTexture( stripheader_t* hdr, const texture_t* tex )  { return set_texture( hdr, __func__, TSP0, TCW0, TYPES_TEXTURED,   tex ); }
int shTexture2( stripheader_t* hdr, const texture_t* tex ) { return set_texture( hdr, __func__, TSP1, TCW1, TYPES_TEXTURED_2, tex ); }

int shBaseColor( stripheader_t* hdr, float a, float r, float g, float b )
{
    if ( check_allowed( hdr->type, TYPES_INTENSITY | TYPES_SPRITE ) )
    {
        hdr->color0[0] = a;
        hdr->color0[1] = r;
        hdr->color0[2] = g;
        hdr->color0[3] = b;
        return 1;
    }

    report_error( SH_ERROR_NOT_ALLOWED, __func__ );
    return 0;
}

int shBaseColor2( stripheader_t* hdr, float a, float r, float g, float b )
{
    if ( check_allowed( hdr->type, TYPES_INTENSITY & TYPES_POLYGON_2 ) )
    {
        hdr->color1[0] = a;
        hdr->color1[1] = r;
        hdr->color1[2] = g;
        hdr->color1[3] = b;
        return 1;
    }

    report_error( SH_ERROR_NOT_ALLOWED, __func__ );
    return 0;
}

int shSpriteColor( stripheader_t* hdr, uint8 *const color) {
	hdr->sprColor[0] = color[3];
	hdr->sprColor[1] = color[0];
	hdr->sprColor[2] = color[1];
	hdr->sprColor[3] = color[2];	
	return 1;
}

int shOffsetColor( stripheader_t* hdr, float a, float r, float g, float b )
{
    if ( check_allowed( hdr->type, ( TYPES_INTENSITY | TYPES_SPRITE ) & TYPES_TEXTURED ) && !check_allowed( hdr->type, TYPES_POLYGON_2 ) )
    {
        hdr->color1[0] = a;
        hdr->color1[1] = r;
        hdr->color1[2] = g;
        hdr->color1[3] = b;
        return 1;
    }

    report_error( SH_ERROR_NOT_ALLOWED, __func__ );
    return 0;
}


/***** Commit ****************************************************************/

#define PREFETCH(addr) __asm__ __volatile__("pref @%0" : : "r" (addr))

// TODO: Remove all 0 writes perhaps? Dunno if the pvr cares about those.
int shCommit( stripheader_t* header, uint32* ptr )
{
    int size_in_words = 0;

    // Quit now if the header is invalid
    if ( header->type > 17 )
    {
        report_error( SH_ERROR_INVALID_TYPE, __func__ );
        return 0;
    }

    switch ( header->type )
    {
        case 0:
        case 1:
        case 3:
        case 4:
        case 5:
        case 6:
        case 17:
            ptr += 8;
            *--ptr	= 0;
            *--ptr	= 0;
            *--ptr	= 0;
            *--ptr	= 0;
            size_in_words = 8;
            break;

        case 2:
            ptr += 8;
            *(float*)--ptr = header->color0[3];	// Doesn't matter when using previous face color
            *(float*)--ptr = header->color0[2];	// Doesn't matter when using previous face color
            *(float*)--ptr = header->color0[1];	// Doesn't matter when using previous face color
            *(float*)--ptr = header->color0[0];	// Doesn't matter when using previous face color
            size_in_words = 8;
            break;

        case 7:
        case 8:
            if ( ( ( header->words[PCW] & PCW_COLOR_TYPE_MASK ) == PCW_COLOR_TYPE_INTENSITY ) &&
                    ( ( header->words[PCW] & PCW_OFFSET_COLOR_MASK ) == PCW_OFFSET_COLOR_ENABLE ) )
            {
                ptr += 16;
                *(float*)--ptr = header->color1[3];
                *(float*)--ptr = header->color1[2];
                *(float*)--ptr = header->color1[1];
                *(float*)--ptr = header->color1[0];
                *(float*)--ptr = header->color0[3];
                *(float*)--ptr = header->color0[2];
                *(float*)--ptr = header->color0[1];
                *(float*)--ptr = header->color0[0];
                PREFETCH( (void*)ptr );
                *--ptr	= 0;
                *--ptr	= 0;
                *--ptr	= 0;
                *--ptr	= 0;
                size_in_words = 16;
            }
            else
            {
                ptr += 8;
                *(float*)--ptr	= header->color0[3];	// Doesn't matter when using previous face color
                *(float*)--ptr	= header->color0[2];	// Doesn't matter when using previous face color
                *(float*)--ptr	= header->color0[1];	// Doesn't matter when using previous face color
                *(float*)--ptr	= header->color0[0];	// Doesn't matter when using previous face color
                size_in_words = 8;
            }
            break;

        case 9:
        case 11:
        case 12:
            ptr += 8;
            *--ptr	= 0;
            *--ptr	= 0;
            *--ptr	= header->words[TCW1];
            *--ptr	= header->words[TSP1];
            size_in_words = 8;
            break;

        case 10:
        case 13:
        case 14:
            if ( ( header->words[PCW] & PCW_COLOR_TYPE_MASK ) == PCW_COLOR_TYPE_INTENSITY )
            {
                ptr += 16;
                *(float*)--ptr = header->color1[3];
                *(float*)--ptr = header->color1[2];
                *(float*)--ptr = header->color1[1];
                *(float*)--ptr = header->color1[0];
                *(float*)--ptr = header->color0[3];
                *(float*)--ptr = header->color0[2];
                *(float*)--ptr = header->color0[1];
                *(float*)--ptr = header->color0[0];
                PREFETCH( (void*)ptr );
                *--ptr	= 0;
                *--ptr	= 0;
                *--ptr	= header->words[TCW1];
                *--ptr	= header->words[TSP1];
                size_in_words = 16;
            }
            else
            {
                ptr += 8;
                *--ptr	= 0;
                *--ptr	= 0;
                *--ptr	= header->words[TCW1];
                *--ptr	= header->words[TSP1];
                size_in_words = 8;
            }
            break;

        case 15:
            ptr += 8;
            *--ptr	= 0;
            *--ptr	= 0;
            *--ptr	= 0;
            //*--ptr	= PVR_PACK_COLOR( header->color0[0], header->color0[1], header->color0[2], header->color0[3] );
            *--ptr =  (header->sprColor[0]<<24) | (header->sprColor[1]<<16) | (header->sprColor[2]<<8) | header->sprColor[3];   
            size_in_words = 8;
            break;

        case 16:
            ptr += 8;
            *--ptr	= 0;
            *--ptr	= 0;
            //*--ptr	= PVR_PACK_COLOR( header->color0[0], header->color0[1], header->color0[2], header->color0[3] );
            //*--ptr	= PVR_PACK_COLOR( header->color0[0], header->color0[1], header->color0[2], header->color0[3] );
            *--ptr =  (header->sprColor[0]<<24) | (header->sprColor[1]<<16) | (header->sprColor[2]<<8) | header->sprColor[3]; 
            *--ptr =  (header->sprColor[0]<<24) | (header->sprColor[1]<<16) | (header->sprColor[2]<<8) | header->sprColor[3]; 
            size_in_words = 8;
            break;
    }

    // These last four words are the same for all headers
    *--ptr	= header->words[TCW0];
    *--ptr	= header->words[TSP0];
    *--ptr	= header->words[ISPTSP];
    *--ptr	= header->words[PCW];
    PREFETCH( (void*)ptr );

    return size_in_words;
}

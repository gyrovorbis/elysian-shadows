/*
 Copyright (c) 2011, MyStackTrace Team
 All rights reserved.
 
 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions are met:
 * Redistributions of source code must retain the above copyright
 notice, this list of conditions and the following disclaimer.
 * Redistributions in binary form must reproduce the above copyright
 notice, this list of conditions and the following disclaimer in the
 documentation and/or other materials provided with the distribution.
 * Neither the name of the MyStackTrace Team nor the
 names of its contributors may be used to endorse or promote products
 derived from this software without specific prior written permission.
 
 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER(S) BE LIABLE FOR ANY
 DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "stdlib.h"
#include "string.h"
#include "png.h"
#include "stdio.h"
#include <gyro_file_api.h>
#include <gyro_texture_api.h>
#include <x86/gyro_glfw_x86.inl>
#include "glal/gyro_glal.h"

typedef struct image_s
{
	unsigned int width, height;
	int bit_depth;
	int format;
	int components;
} image_t;

static void __png_read_data_from_stream(png_structp png_ptr, png_bytep out_bytes, png_size_t byte_count)
{
    GYFile *file;
    size_t bytes_read = 0;
    file = png_get_io_ptr(png_ptr);
    if (file == NULL) {
        return;
    }
    gyFileRead(file, out_bytes, byte_count, &bytes_read);
    if (bytes_read != byte_count) {
        return;
    }
}

static unsigned char *load_png(GYFile *f, image_t *img)
{
    png_byte header[8];
	png_structp png_ptr;
	unsigned int sig_read = 8;
	png_uint_32 width, height;
	int bit_depth, colour_type, interlace_type;
	png_infop info_ptr;
    png_infop end_info;
	unsigned int i = 0;
	int rowbytes;
	unsigned char *pixels;
	png_bytep *row_pointers;

    gyFileRead(f, header, 8, NULL);
    if (png_sig_cmp(header, 0, 8)) {
        return NULL;
    }
	png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if(!png_ptr) {
		return NULL;
	}
    info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr) {
        png_destroy_read_struct(&png_ptr, NULL, NULL);
        return NULL;
    }
    end_info = png_create_info_struct(png_ptr);
    if (!end_info) {
        png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
    }
    if (setjmp(png_jmpbuf(png_ptr))) {
        png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
        return NULL;
    }
    png_set_read_fn(png_ptr, f, __png_read_data_from_stream);
	png_set_sig_bytes(png_ptr, sig_read);
	png_read_info(png_ptr, info_ptr);
	bit_depth = png_get_bit_depth(png_ptr, info_ptr);
	colour_type = png_get_color_type(png_ptr, info_ptr);
	if(bit_depth == 16) {
		png_set_strip_16(png_ptr);
    } else if(bit_depth < 8) {
		png_set_packing(png_ptr);
    }
	if(colour_type == PNG_COLOR_TYPE_PALETTE) {
		png_set_palette_to_rgb(png_ptr);
    }
	if(colour_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8) {
		png_set_expand_gray_1_2_4_to_8(png_ptr);
    }
	if(png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS)) {
		png_set_tRNS_to_alpha(png_ptr);
    }
	png_read_update_info(png_ptr, info_ptr);
	png_get_IHDR(png_ptr, info_ptr, &width, &height, &bit_depth,
                 &colour_type, &interlace_type, NULL, NULL);
	img->height = height;
	img->width = width;
	img->bit_depth = bit_depth;
	switch(colour_type) {
		case PNG_COLOR_TYPE_GRAY:
			img->format = GL_LUMINANCE;
			img->components = 1;
            break;
		case PNG_COLOR_TYPE_GRAY_ALPHA:
			img->format = GL_LUMINANCE_ALPHA;
			img->components = 2;
            break;
		case PNG_COLOR_TYPE_RGB:
			img->format = GL_RGB;
			img->components = 3;
            break;
		case PNG_COLOR_TYPE_RGBA:
			img->format = GL_RGBA;
			img->components = 4;
            break;
		default:
			img->format = GL_RGB;
			img->components = 3;
	}
	rowbytes = png_get_rowbytes(png_ptr, info_ptr);
	pixels = malloc(sizeof(char) * rowbytes * height);
	if(!pixels) {
		png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
		return NULL;
	}
	row_pointers = malloc(sizeof(png_bytep) * height);
	if(!row_pointers) {
		png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
        free(pixels);
		return NULL;
	}
    for(i = 0; i < height; i++) {
		row_pointers[height - 1 - i] = (pixels + i * rowbytes);
    }
	png_read_image(png_ptr, row_pointers);
	png_read_end(png_ptr, info_ptr);
	png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
	free(row_pointers);
	return pixels;
};

static GLuint create_texture_png(const char *filename, GLenum st, GLenum min, GLenum mag, unsigned* const success)
{
    GYFile *f;
	image_t img;
	unsigned char *pixels;
	GLuint id;
	if(!gyFileOpen(GY_PATH_ASSETS, filename, "rb", &f)) {
		*success = 0;
		return 0;
	}
	pixels = load_png(f, &img);
	if(!pixels) {
		*success = 0;
		return 0;
	}
    gyFileClose(&f);
	glGenTextures(1, &id);
	if(id == 0) { /* no texture ID */
		*success = 0;
		return 0;
	}
	glBindTexture(GL_TEXTURE_2D, id);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, min);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, mag);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, st);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, st);
	glTexImage2D(GL_TEXTURE_2D, 0, img.components, img.width, img.height, 0, img.format, GL_UNSIGNED_BYTE, pixels);
	glBindTexture(GL_TEXTURE_2D, 0);
	free(pixels);
	*success = 1;
	return id;
};

int _gyVidTexAlloc(GYTexture *const tex, const unsigned width, const unsigned height) {
	(void)width;
	(void)height;
    unsigned success = 0;
    tex->_data = NULL;
    
    switch(tex->bpp) {
        case GY_TEX_16BPP:
            *(GLuint *)&tex->_data = create_texture_png(tex->filename, GL_CLAMP, GL_NEAREST, GL_NEAREST, &success);
            break;
        case GY_TEX_8BPP:
            
            break;
        case GY_TEX_4BPP:
            
            break;
        default:
            *(GLuint *)&tex->_data = create_texture_png(tex->filename, GL_CLAMP, GL_NEAREST, GL_NEAREST, &success);
    }
    
    return success;
}

int _gyVidTexFree(GYTexture *const tex) {
    glDeleteTextures(1, (GLuint *)&tex->_data);
    return 1;
}


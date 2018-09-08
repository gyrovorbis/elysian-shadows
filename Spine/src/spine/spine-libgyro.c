/******************************************************************************
 * Spine Runtimes Software License
 * Version 2.1
 *
 * Copyright (c) 2013, Esoteric Software
 * All rights reserved.
 *
 * You are granted a perpetual, non-exclusive, non-sublicensable and
 * non-transferable license to install, execute and perform the Spine Runtimes
 * Software (the "Software") solely for internal use. Without the written
 * permission of Esoteric Software (typically granted by licensing Spine), you
 * may not (a) modify, translate, adapt or otherwise create derivative works,
 * improvements of the Software or develop new applications using the Software
 * or (b) remove, delete, alter or obscure any trademarks or any copyright,
 * trademark, patent or other intellectual property or proprietary rights
 * notices on or in the Software, including any copy thereof. Redistributions
 * in binary or source form must include this license and terms.
 *
 * THIS SOFTWARE IS PROVIDED BY ESOTERIC SOFTWARE "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
 * EVENT SHALL ESOTERIC SOFTARE BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *****************************************************************************/

#include <libGyro/gyro_video_api.h>
#include <libGyro/gyro_texture_api.h>
#include <libGyro/gyro_system_api.h>
#include <libGyro/gyro_file_api.h>
#include <common/gyro_spine_common.h>
#include <spine/extension.h>
#include <spine/spine.h>

#ifndef SPINE_MESH_VERTEX_COUNT_MAX
#define SPINE_MESH_VERTEX_COUNT_MAX 1000 //0.0267MB. Not bad.
#endif

/*
 *
 *
#define GYRO_SPINE_ATLAS_TEXTURE_NORMAL_SUFFIX      "_n"
#define GYRO_SPINE_ATLAS_TEXTURE_SPECULAR_SUFFIX    "_r"
*/

void _spAtlasPage_createTexture (spAtlasPage* self, const char* path) {
    char buffer[GYRO_PATH_MAX_SIZE];

    GYSpineRenderObject* obj    = malloc(sizeof(GYSpineRenderObject));
    obj->textureMap             = gyVidTexCreate(path);

    if(!obj->textureMap) return;

    char* filename = strtok(path, ".");
    if(path) {

        strcpy(buffer, filename);
        strcat(buffer, GYRO_SPINE_ATLAS_TEXTURE_NORMAL_SUFFIX);
        obj->normalMap = gyVidTexCreate(buffer);

        strcpy(buffer, filename);
        strcat(buffer, GYRO_SPINE_ATLAS_TEXTURE_SPECULAR_SUFFIX);
        obj->specularMap = gyVidTexCreate(buffer);

    } else {
        obj->normalMap      = NULL;
        obj->specularMap    = NULL;
    }

    self->rendererObject    = obj;
    self->width             = obj->textureMap->width;
    self->height            = obj->textureMap->height;
}

void _spAtlasPage_disposeTexture (spAtlasPage* self) {
    GYSpineRenderObject* obj = self->rendererObject;

    gyVidTexDestroy(obj->textureMap);
    gyVidTexDestroy(obj->normalMap);
    gyVidTexDestroy(obj->specularMap);

    free(obj);

}

char* _spUtil_readFile (const char* path, int* length) {
    char *data;
    GYFile *file;
    size_t bytesRead;
    size_t rawlen;

    if(!gyFileOpen(path, "rb", &file)) return NULL;

    gyFileLength(file, &rawlen);
    *length = (int)rawlen;

    data = MALLOC(char, *length+1);

    gyFileRead(file, data, *length, &bytesRead);
    data[*length] = 0;
    if(bytesRead != (size_t)*length) {
        _gyLog(GY_DEBUG_ERROR, "Error loading file [%s]. Length [%d], Bytes Read [%d].", path, length, bytesRead);
        free(data);
        data = NULL;
    }

    gyFileClose(&file);
    return data; //Spine frees this later on, not a memory leak... But still stupid for not being static.
}

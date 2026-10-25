/*
 * Copyright (c) 2013-2026 Elysian Shadows Team
 * SPDX-License-Identifier: MIT
 * Additional permission for Elysian Shadows Kickstarter backers: see LICENSE.
 */

#ifndef GYRO_FILE_API_H
#define GYRO_FILE_API_H

#include <stddef.h>
#include <sys/types.h>

#include "gyro_platform.h"

#ifdef __cplusplus
extern "C" {
#endif

#define GYRO_FILE_API_WORKING_DIR_PATH_SIZE 200

typedef struct _GYFileInitParams {
    char workingDir[GYRO_FILE_API_WORKING_DIR_PATH_SIZE];

} GYFileInitParams;

struct _GYFile;
typedef struct _GYFile GYFile;

typedef enum _GYPathType {
    /*
     * Path that houses user-specific data for this application.
     *
     * Varies by platform:
     * - OS X: ~/Library/Application Support/${AUTHOR}/${NAME}
     * - *nix: ~/.local/share/${AUTHOR}/${NAME}
     * - Windows: ~/AppData/Roaming/${AUTHOR}/${NAME}
     * - Android: /data/0/com.myapp
     * - Dreamcast: /vmu <-- Falco can correct me if I'm wrong
     */
    GY_PATH_USERDATA,

	/*
	 * Path to game assets; Relative to binary location
	 */
	GY_PATH_ASSETS
} GYPathType;

int gyFileInit(const GYFileInitParams*const params);
int gyFileQuit(void);

int gyFileResolvePath(GYPathType path_type, const char* vpath, char **path, size_t *len);


const char* gyFilePath(const char* relative);


//size_t fwrite(const void *ptr, size_t size_of_elements, size_t number_of_elements, FILE *a_file);

int gyFileOpen(const char *file_path, const char *mode, GYFile **fp);
int gyFileRead(GYFile *fp, void *buffer, size_t count, size_t *bytes_read);
size_t gyFileWrite(GYFile *fp, const void* ptr, size_t elementSize, size_t elementCount);
int gyFilePrintf(GYFile* fp, const char* str, ...);
int gyFilePuts(GYFile* fp, const char* str);
int gyFileSeek(GYFile *fp, off_t offset, int whence);
int gyFileLength(GYFile *fp, size_t *len);
int gyFileTell(GYFile *fp, long int *pos);

int gyFileClose(GYFile **fp);

#ifdef GYRO_FILE_PLATFORM_INCLUDE
#	include GYRO_FILE_PLATFORM_INCLUDE
#endif

#ifdef __cplusplus
}
#endif

#endif

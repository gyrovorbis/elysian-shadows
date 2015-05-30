#ifndef GYRO_FILE_API_H
#define GYRO_FILE_API_H

#include <stddef.h>
#include <sys/types.h>

#include "gyro_platform.h"

#ifdef __cplusplus
extern "C" {
#endif

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

typedef struct _GYFile GYFile;

int gyFileInit(const char *author, const char *name);
int gyFileQuit(void);

int gyFileResolvePath(GYPathType path_type, char **path, size_t *len);

int gyFileOpen(const char *file_path, const char *mode, GYFile **fp);
int gyFileRead(GYFile *fp, void *buffer, size_t count, size_t *bytes_read);
int gyFileSeek(GYFile *fp, off_t offset, int whence);
int gyFileLength(GYFile *fp, size_t *len);
int gyFileTell(GYFile *fp, long int *pos);

int gyFileClose(GYFile **fp);

#ifdef __cplusplus
}
#endif

#endif

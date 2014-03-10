#ifndef GYRO_FILE_H
#define GYRO_FILE_H

#include <stddef.h>
#include <sys/types.h>

#include "gyro_platform.h"

typedef enum _GYPathType {
    /*
     * On most platforms, this should point to the location
     * of the directory the executing binary resides in.
     * On Android, loading a file from this path will result
     * in loading from the APK's assets.
     *
     * It goes without saying that you shouldn't ever write
     * anything to any file within this path, unless you
     * know what you're doing.
     */
    GY_PATH_BINARY,

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
    GY_PATH_USERDATA
} GYPathType;

typedef struct _GYFile GYFile;

#ifdef GYRO_FILE_PLATFORM_INCLUDE
#	include GYRO_FILE_PLATFORM_INCLUDE
#endif

int gyFileInit(const char *author, const char *name);
int gyFileQuit(void);

int gyFileResolvePath(GYPathType path_type, char **path, size_t *len);

int gyFileOpen(GYPathType path_type, const char *file_path, const char *mode, GYFile **fp);
int gyFileRead(GYFile *fp, char *buffer, size_t count, size_t *bytes_read);
int gyFileSeek(GYFile *fp, off_t offset, int whence);
int gyFileLength(GYFile *fp, size_t *len);
int gyFileTell(GYFile *fp, long int *pos);

int gyFileClose(GYFile **fp);

#endif

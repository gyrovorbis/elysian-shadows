#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <gyro_system_api.h>
#import <Foundation/NSBundle.h>
#import <Foundation/NSString.h>
#include <unistd.h>
#include <assert.h>
#include <gyro_defines.h>
#include <gyro_file_api.h>

#include <common/turfs.h>

#include "../common/internal.h"

static NSBundle *_appBundle = NULL;
static char _resourcePath[GYRO_PATH_MAX_SIZE];

static char *s_author = NULL;
static char *s_name = NULL;

static char *s_assets_path = NULL;
static char *s_userdata_path = NULL;

int g_initialized = 0;


//static char _tempFileBuffer[GYRO_iOS_TEMP_FILENAME_BUFFER_SIZE];

// Stole this shit form the internet, because doing this safely is a bitch,
// and there was no way in FUCK I would want to do it in Obj-C.
// Oh, and I adulterated the shit out of it, since iOS doesn't want a leading '.'
// on the extensions. --Gyro
typedef struct
{
    char * directory;
    char * filename;
    char * extension;
} PATHINFO;


// Allocate a block of memory that holds the PATHINFO at the beginning
// followed by the actual path. An extra byte is allocated (+2 instead
// of just +1) to deal with shifting the extension to protect the
// leading '.'. This extra byte also doubles as the empty string, as
// well as a pad to keep from reading past the memory block.
//
// Returns a PATHINFO structure that should be freed using FreePathInfo().
static PATHINFO * _SplitPath(const char * pathSrc)
{
    size_t length = strlen(pathSrc);
    PATHINFO * pathinfo = (PATHINFO *) malloc(sizeof(PATHINFO) + length + 2);

    if (pathinfo)
    {
        char * path = (char *) &pathinfo[1];    // copy of the path
        char * theEnd = &path[length + 1];      // second null terminator
        char * extension;
        char * lastSep;

        // Copy the original string and double null terminate it.
        strcpy(path, pathSrc);
        *theEnd = '\0';
        pathinfo->directory = theEnd;   // Assume no path
        pathinfo->extension = theEnd;   // Assume no extension
        pathinfo->filename = path;      // Assume filename only

        lastSep = strrchr(path, '/');
        if (lastSep)
        {
            pathinfo->directory = path;     // Pick up the path
            *lastSep++ = '\0';              // Truncate directory
            pathinfo->filename = lastSep;   // Pick up name after path
        }

        // Start at the second character of the filename to handle
        // filenames that start with '.' like ".login".
        // We don't overrun the buffer in the cases of an empty path
        // or a path that looks like "/usr/bin/" because of the extra
        // byte.
        extension = strrchr(&pathinfo->filename[1], '.');
        if (extension)
        {
            ++extension;
            // Shift the extension over to protect the leading '.' since
            // we need to truncate the filename.
            //memmove(extension + 1, extension, strlen(extension));
            pathinfo->extension = extension;// + 1;

            *(extension-1) = '\0';          // Truncate filename
        }
    }

    return pathinfo;
}


const char* gyFilePath(const char *const name) {

    assert(_appBundle); //initialize the API first, fuckmite!

    PATHINFO *pathInfo = _SplitPath(name);

    NSString *fullPath = [_appBundle pathForResource:[NSString stringWithUTF8String:pathInfo->filename] ofType:[NSString stringWithUTF8String:pathInfo->extension] inDirectory:[NSString stringWithUTF8String:pathInfo->directory]];

    free(pathInfo);

    if(!fullPath) {
        //_gyLog(GY_DEBUG_WARNING, "Unable to locate resource in iOS bundle! - %s", name);
        return "Could not locate resource in iOS bundle!";
    }

    return [fullPath cStringUsingEncoding:NSUTF8StringEncoding];
}



int gyFileInit(const GYFileInitParams * const params) {
  return turfs_init("ESTeam", "ESTk", NULL) == TURFS_RET_OK;
}

TURFSAPI turfs_init(const char *author, const char *name, void *aactivity)
{
   /* if (author == NULL || name == NULL) {
        return TURFS_RET_INVALID_ARGS;
    }*/

    s_author = turfs__strdup(author);
    s_name = turfs__strdup(name);
    _appBundle = [NSBundle mainBundle];

    strcpy(_resourcePath, [[_appBundle resourcePath] cStringUsingEncoding:NSUTF8StringEncoding]);

    g_initialized = 1;
    return TURFS_RET_OK;
}

TURFSAPI turfs_quit(void)
{
    if (!g_initialized) {
        return TURFS_RET_NOT_INITIALIZED;
    }
#if 0
    if (s_author != NULL) {
        free(s_author);
        s_author = NULL;
    }
    if (s_name != NULL) {
        free(s_name);
        s_name = NULL;
    }
#endif
    g_initialized = 0;
    return TURFS_RET_OK;
}

static int s__get_assets_path(const char* vpath, char **path, size_t *len)
{
    char temp_path[GYRO_PATH_MAX_SIZE];
    memset(temp_path, 0, sizeof temp_path);

    *path = (char*)gyFilePath(vpath);

    if (*path == NULL) {
        return 0;
    }
    if (len != NULL) {
        *len = strlen(*path);
    }
    return 1;
}


static int s__get_userdata_path(const char* vpath, char **path, size_t *len)
{
    char temp_path[2048];
    const char *home_path;
    memset(temp_path, 0, sizeof temp_path);
    home_path = getenv("HOME");
    sprintf(temp_path, "%s/.local/share/%s/%s", home_path, s_author, s_name);
    *path = turfs__strdup(temp_path);
    if (*path == NULL) {
        return 0;
    }
    if (len != NULL) {
        *len = strlen(temp_path);
    }
    return 1;
}

TURFSAPI turfs_resolve_path(turfs_path_type type, const char* vpath, char **path, size_t *len)
{
    if (!g_initialized) {
        return TURFS_RET_NOT_INITIALIZED;
    }
    if (path == NULL) {
        return TURFS_RET_INVALID_ARGS;
    }
    switch (type) {
        case TURFS_PATH_ASSETS:
            if (!s__get_assets_path(vpath, path, len)) {
                return TURFS_RET_UNKNOWN;
            }
            break;
        case TURFS_PATH_USERDATA:
            if (!s__get_userdata_path(vpath, path, len)) {
                return TURFS_RET_UNKNOWN;
            }
            break;
        default:
            return TURFS_RET_INVALID_ARGS;
    }
    return TURFS_RET_OK;
}

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <android/native_activity.h>

#include <turfs.h>

#include "internal.h"

static char *s_author = NULL;
static char *s_name = NULL;

ANativeActivity *g_nativeActivity = NULL;

int g_initialized = 0;

TURFSAPI turfs_init(const char *author, const char *name, void *aactivity)
{
    if (author == NULL || name == NULL) {
        return TURFS_RET_INVALID_ARGS;
    }
    s_author = turfs__strdup(author);
    s_name = turfs__strdup(name);
    g_nativeActivity = aactivity;
    g_initialized = 1;
    return TURFS_RET_OK;
}

TURFSAPI turfs_quit(void)
{
    if (!g_initialized) {
        return TURFS_RET_NOT_INITIALIZED;
    }
    if (s_author != NULL) {
        free(s_author);
        s_author = NULL;
    }
    if (s_name != NULL) {
        free(s_name);
        s_name = NULL;
    }
    g_nativeActivity = NULL;
    g_initialized = 0;
    return TURFS_RET_OK;
}

static int s__get_userdata_path(char **path, size_t *len)
{
    char temp_path[2048];
    memset(temp_path, 0, sizeof temp_path);
    sprintf(temp_path, "%s/%s", g_nativeActivity->internalDataPath, "files");
    *path = turfs__strdup(temp_path);
    if (*path == NULL) {
        return 0;
    }
    if (len != NULL) {
        *len = strlen(temp_path);
    }
    return 1;
}

TURFSAPI turfs_resolve_path(turfs_path_type type, char **path, size_t *len)
{
    if (!g_initialized) {
        return TURFS_RET_NOT_INITIALIZED;
    }
    if (path == NULL) {
        return TURFS_RET_INVALID_ARGS;
    }
    switch (type) {
        case TURFS_PATH_ASSETS:
            *path = turfs__strdup("");
            if (len != NULL) {
                *len = 0;
            }
            break;
        case TURFS_PATH_USERDATA:
            if (!s__get_userdata_path(path, len)) {
                return TURFS_RET_UNKNOWN;
            }
            break;
        default:
            return TURFS_RET_INVALID_ARGS;
    }
    return TURFS_RET_OK;
}

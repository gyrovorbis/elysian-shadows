#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <common/turfs.h>

#include "../common/internal.h"

extern char **NXArgv;

static char *s_author = NULL;
static char *s_name = NULL;

//static char *s_assets_path = NULL;
//static char *s_userdata_path = NULL;

int g_initialized = 0;

TURFSAPI turfs_init(const char *author, const char *name, void *aactivity)
{
    (void)aactivity;
    if (author == NULL || name == NULL) {
        return TURFS_RET_INVALID_ARGS;
    }
    s_author = turfs__strdup(author);
    s_name = turfs__strdup(name);
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
    g_initialized = 0;
    return TURFS_RET_OK;
}

static int s__get_assets_path(char **path, size_t *len)
{
    char temp_path[2048];
    char *ptr;
    memset(temp_path, 0, sizeof temp_path);
    strncpy(temp_path, NXArgv[0], sizeof temp_path);
    if (strlen(temp_path)) {
        ptr = &temp_path[strlen(temp_path)];
        while (*(--ptr) != '/');
        ptr[1] = '\x0';
    } else {
        strcpy(temp_path, "./");
    }
    //strcat(temp_path, "../Resources/assets");
    strcat(temp_path, "../assets");
    *path = realpath(temp_path, NULL);
    if (*path == NULL) {
        return 0;
    }
    if (len != NULL) {
        *len = strlen(*path);
    }
    return 1;
}

static int s__get_userdata_path(char **path, size_t *len)
{
    char temp_path[2048];
    const char *home_path;
    memset(temp_path, 0, sizeof temp_path);
    home_path = getenv("HOME");
    sprintf(temp_path, "%s/Library/Application Support/%s/%s", home_path, s_author, s_name);
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
            if (!s__get_assets_path(path, len)) {
                return TURFS_RET_UNKNOWN;
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

#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <common/turfs.h>

#include "internal.h"

/* Fixed virtual paths for now */
#define ASSETS_PATH_STR     "/assets"
#define USERDATA_PATH_STR   "/userdata"

char *turfs__strdup(const char *str)
{
    size_t len = 1 + strlen(str);
    char *p = malloc(len);
    return p ? memcpy(p, str, len) : NULL;
}


TURFSAPI turfs_virtual_path(turfs_path_type type, char **path, size_t *len)
{
    if (!g_initialized) {
        return TURFS_RET_NOT_INITIALIZED;
    }
    switch (type) {
        case TURFS_PATH_ASSETS:
            if (path != NULL) {
                *path = turfs__strdup(ASSETS_PATH_STR);
            }
            if (len != NULL) {
                *len = strlen(ASSETS_PATH_STR);
            }
            break;
        case TURFS_PATH_USERDATA:
            if (path != NULL) {
                *path = turfs__strdup(USERDATA_PATH_STR);
            }
            if (len != NULL) {
                *len = strlen(USERDATA_PATH_STR);
            }
            break;
    }
    return TURFS_RET_OK;
}

TURFSAPI turfs_silph_scope(const char *vpath, char **path, size_t *len, turfs_path_type *type)
{
    char temp_path[2048];
    char *asset_vpath = NULL, *userdata_vpath = NULL;
    char *asset_rpath = NULL, *userdata_rpath = NULL;
    size_t vlen = strlen(vpath);
    size_t asset_len, userdata_len;
    if (vpath[0] != '/') {
        /* Absolute paths only --NO EDDIE, DIPSHIT. */
        //return TURFS_RET_INVALID_ARGS;
    }
    turfs_virtual_path(TURFS_PATH_ASSETS, &asset_vpath, &asset_len);
    if (asset_vpath != NULL && vlen > asset_len + 1) {
        if (!strncmp(vpath, asset_vpath, asset_len)) {
            if (vpath[asset_len] == '/') {
                memset(temp_path, 0, sizeof temp_path);
                turfs_resolve_path(TURFS_PATH_ASSETS, vpath, &asset_rpath, NULL);
                strcpy(temp_path, asset_rpath);
               // sprintf(temp_path, "%s/%s", asset_rpath, &vpath[asset_len + 1]);
                if (path != NULL) {
                    *path = turfs__strdup(temp_path);
                }
                if (len != NULL) {
                    *len = strlen(temp_path);
                }
                if (type != NULL) {
                    *type = TURFS_PATH_ASSETS;
                }
                return TURFS_RET_OK;
            }
        }
    }
    turfs_virtual_path(TURFS_PATH_USERDATA, &userdata_vpath, &userdata_len);
    if (userdata_vpath != NULL && vlen > userdata_len + 1) {
        if (!strncmp(vpath, userdata_vpath, userdata_len)) {
            if (vpath[userdata_len] == '/') {
                memset(temp_path, 0, sizeof temp_path);
                turfs_resolve_path(TURFS_PATH_USERDATA, vpath, &userdata_rpath, NULL);
                strcpy(temp_path, asset_rpath);
              //  sprintf(temp_path, "%s/%s", userdata_rpath, &vpath[userdata_len + 1]);
                if (path != NULL) {
                    *path = turfs__strdup(temp_path);
                }
                if (len != NULL) {
                    *len = strlen(temp_path);
                }
                if (type != NULL) {
                    *type = TURFS_PATH_USERDATA;
                }
                return TURFS_RET_OK;
            }
        }
    }

    //Path was not found, assume it's absolute, and don't shit your fucking pants!!!
    *path = turfs__strdup(vpath);
    *len = strlen(temp_path);
    return TURFS_RET_OK;

    //return TURFS_RET_NOT_FOUND;
}

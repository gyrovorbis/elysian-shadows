#include <stdlib.h>

#include <common/turfs.h>

#include <gyro_file_api.h>

int gyFileQuit(void)
{
	return turfs_quit() == TURFS_RET_OK;
}

int gyFileResolvePath(GYPathType path_type, char **path, size_t *len)
{
	if (path == NULL) {
		return 0;
	}
	switch (path_type) {
		case GY_PATH_USERDATA:
			return turfs_resolve_path(TURFS_PATH_USERDATA, path, len) == TURFS_RET_OK;
		case GY_PATH_ASSETS:
			return turfs_resolve_path(TURFS_PATH_ASSETS, path, len) == TURFS_RET_OK;
	}
	return 1;
}

int gyFileOpen(const char *file_path, const char *mode, GYFile **fp)
{
    if (fp == NULL) {
		return 0;
	}
    //*fp = malloc(sizeof(struct _GYFile));
    //if (*fp == NULL) {
    //	return 0;
    //}
    if (turfs_file_open(file_path, mode, (Turfs_file**)fp) != TURFS_RET_OK) {
        //free(*fp);
        //*fp = NULL;
		return 0;
	}
	return 1;
}

int gyFileRead(GYFile *fp, void *buffer, size_t count, size_t *bytes_read)
{
    return turfs_file_read((Turfs_file*)fp, buffer, count, bytes_read) == TURFS_RET_OK;
}

int gyFileSeek(GYFile *fp, off_t offset, int whence)
{
    return turfs_file_seek((Turfs_file*)fp, offset, whence) == TURFS_RET_OK;
}

int gyFileLength(GYFile *fp, size_t *len)
{
    return turfs_file_length((Turfs_file*)fp, len) == TURFS_RET_OK;
}

int gyFileTell(GYFile *fp, long int *pos)
{
    return turfs_file_tell((Turfs_file*)fp, pos) == TURFS_RET_OK;
}

int gyFileClose(GYFile **fp)
{
    turfs_ret ret = turfs_file_close((Turfs_file**)fp);
    //free(*fp);
    //*fp = NULL;
	return ret == TURFS_RET_OK;
}

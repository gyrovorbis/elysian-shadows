#include <stdlib.h>

#include <turfs/turfs.h>

#include <gyro_file_api.h>
#include <stdio.h>

struct _GYFile {
	turfs_fp fp;
};

int gyFileInit(const char *author, const char *name)
{
	if (author == NULL || name == NULL) {
		/* Require an app author & name to identify us */
		return 0;
	}
	return turfs_init(author, name, NULL) == TURFS_RET_OK;
}

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

#include <string.h>

int gyFileOpen(const char *file_path, const char *mode, GYFile **fp)
{
	char *buffer;
	turfs_path_type type;
	turfs_ret ret;
	printf(">> gyFileOpen(%s)\n", file_path);
	if (fp == NULL) {
		return 0;
	}
	*fp = malloc(sizeof(struct _GYFile));
	if (*fp == NULL) {
		return 0;
	}
	ret = turfs_virtual_path(TURFS_PATH_ASSETS, &buffer, 0);
	printf("VP ret: %d\n", ret);
	printf("Assets path: %s\n", buffer);
	free(buffer);
	buffer = NULL;
	ret = turfs_silph_scope(file_path, &buffer, 0, &type);
	printf("SILPH SAYS: %s\n", buffer);
	printf("  TYPE    : %d\n", type);
	printf("   RET    : %d\n", ret);
	free(buffer);
	if ((ret = turfs_file_open(file_path, mode, &(*fp)->fp)) != TURFS_RET_OK) {
		printf("  ORET    : %d\n", ret);
		free(*fp);
		*fp = NULL;
		return 0;
	}
	return 1;
}

int gyFileRead(GYFile *fp, void *buffer, size_t count, size_t *bytes_read)
{
	return turfs_file_read(fp->fp, buffer, count, bytes_read) == TURFS_RET_OK;
}

int gyFileSeek(GYFile *fp, off_t offset, int whence)
{
	return turfs_file_seek(fp->fp, offset, whence) == TURFS_RET_OK;
}

int gyFileLength(GYFile *fp, size_t *len)
{
	return turfs_file_length(fp->fp, len) == TURFS_RET_OK;
}

int gyFileTell(GYFile *fp, long int *pos)
{
	return turfs_file_tell(fp->fp, pos) == TURFS_RET_OK;
}

int gyFileClose(GYFile **fp)
{
	turfs_file_close(&(*fp)->fp);
	free(*fp);
	*fp = NULL;
}

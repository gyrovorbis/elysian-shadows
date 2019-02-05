#include <common/turfs.h>

#include <gyro_file_api.h>

int gyFileInit(const char *author, const char *name)
{
	if (author == NULL || name == NULL) {
		/* Require an app author & name to identify us */
		return 0;
	}
	return turfs_init(author, name, NULL) == TURFS_RET_OK;
}

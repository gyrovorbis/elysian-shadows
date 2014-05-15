#include <turfs/turfs.h>

#include <gyro_file_api.h>

#include "internal_android.h"

int gyFileInit(const char *author, const char *name)
{
	if (author == NULL || name == NULL) {
		/* Require an app author & name to identify us */
		return 0;
	}
	return turfs_init(author, name, gy_android_app->activity) == TURFS_RET_OK;
}

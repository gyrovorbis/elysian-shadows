#include <common/turfs.h>
#include <libGyro/gyro_file_api.h>
#include <libGyro/gyro_system_api.h>

extern "C" int gyFileQuit(void) {
	return turfs_quit() == TURFS_RET_OK;
}

extern "C" int gyFileInit(const GYFileInitParams*const params) {
  return turfs_init("Fuckface", "EWEB", NULL) == TURFS_RET_OK;
}

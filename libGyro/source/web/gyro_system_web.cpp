#include <gyro_system_api.h>
#include "gyro_defines.h"
#include "stdio.h"
#include "stdarg.h"

int gySysAppExec(int argc, char* argv[]) {}

int gySysInit(const GYSysInitParams*const params) {
	_gyLog(GY_DEBUG_VERBOSE, "Initializing libGyro_WEB v%s\n", GYRO_LIB_VERSION);
  return 1;
}

int gySysUninit(void) {
  return 1;
}

unsigned int gySysStatsVram() {
  return 1;
}

unsigned int gySysStatsRam() {
  return 1;
}

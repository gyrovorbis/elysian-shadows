#include <SDL2/SDL.h>
#include <gyro_system_api.h>
#include "gyro_defines.h"
#include "string.h"

static GYSysInitParams _sysParams;
static GYSystemInfo _systemInfo;

int gySysAppExec(int argc, char* argv[]) {}

int gySysInit(const GYSysInitParams*const params) {
  memcpy(&_sysParams, params, sizeof(GYSysInitParams));

  _gyLog(GY_DEBUG_VERBOSE, "Initializing libGyro_WEB v%s\n", GYRO_LIB_VERSION);


  _gyPop(1);
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

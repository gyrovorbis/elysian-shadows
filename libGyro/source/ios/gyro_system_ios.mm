#include <gyro_system_api.h>

extern "C" void _gySysPrintBuildInfo(const char*const platString);

int gySysInit(const GYSysParams * const params) {
    _gySysPrintBuildInfo("iOS");
    return 1;
}

#ifndef GYRO_SYSTEM_CALLBACKS_H
#define GYRO_SYSTEM_CALLBACKS_H

#include <gyro_system_api.h>

extern GYAppInitCb gy_appInitCb;
extern GYAppTickCb gy_appTickCb;
extern GYAppDrawCb gy_appDrawCb;
extern GYAppQuitCb gy_appQuitCb;

int gyro_entry_point(int argc, char **argv);

#endif /* GYRO_SYSTEM_CALLBACKS_H */

/*
 * Copyright (c) 2013-2026 Elysian Shadows Team
 * SPDX-License-Identifier: MIT
 * Additional permission for Elysian Shadows Kickstarter backers: see LICENSE.
 */

#ifndef GYRO_SYSTEM_CALLBACKS_H
#define GYRO_SYSTEM_CALLBACKS_H

#include <libGyro/gyro_system_api.h>

extern GYAppInitCb gy_appInitCb;
extern GYAppUpdateCb gy_appUpdateCb;
extern GYAppInputCb gy_appInputCb;
extern GYAppDrawCb gy_appDrawCb;
extern GYAppQuitCb gy_appQuitCb;

int gyro_entry_point(int argc, char **argv);

#endif /* GYRO_SYSTEM_CALLBACKS_H */

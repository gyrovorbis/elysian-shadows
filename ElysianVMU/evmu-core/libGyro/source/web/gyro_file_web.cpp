/*
 * Copyright (c) 2013-2026 Elysian Shadows Team
 * SPDX-License-Identifier: MIT
 * Additional permission for Elysian Shadows Kickstarter backers: see LICENSE.
 */

#include <common/turfs.h>
#include <libGyro/gyro_file_api.h>
#include <libGyro/gyro_system_api.h>

extern "C" int gyFileInit(const GYFileInitParams*const params) {
  return turfs_init("Fuckface", "EWEB", NULL) == TURFS_RET_OK;
}

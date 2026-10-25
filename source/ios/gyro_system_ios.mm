/*
 * Copyright (c) 2013-2026 Elysian Shadows Team
 * SPDX-License-Identifier: MIT
 * Additional permission for Elysian Shadows Kickstarter backers: see LICENSE.
 */

#include <gyro_system_api.h>

extern "C" void _gySysPrintBuildInfo(const char*const platString);

int gySysInit(const GYSysInitParams * const params) {
    _gySysPrintBuildInfo("iOS");
    return 1;
}

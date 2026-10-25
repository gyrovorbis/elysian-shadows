/*
 * Copyright (c) 2013-2026 Elysian Shadows Team
 * SPDX-License-Identifier: MIT
 * Additional permission for Elysian Shadows Kickstarter backers: see LICENSE.
 */

#ifndef GYRO_VMU_BATTERY_H
#define GYRO_VMU_BATTERY_H

#ifdef __cplusplus
extern "C" {
#endif

struct  VMUDevice;

int     gyVmuBatteryLow(const struct VMUDevice* dev);
void    gyVmuBatteryLowSet(struct VMUDevice* dev, int low);
int     gyVmuBatteryMonitorEnabled(const struct VMUDevice* dev);
void    gyVmuBatteryMonitorEnabledSet(struct VMUDevice* dev, int enabled);

#ifdef __cplusplus
}
#endif


#endif // GYRO_VMU_BATTERY_H


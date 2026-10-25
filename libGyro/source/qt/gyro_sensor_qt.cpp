/*
 * Copyright (c) 2013-2026 Elysian Shadows Team
 * SPDX-License-Identifier: MIT
 * Additional permission for Elysian Shadows Kickstarter backers: see LICENSE.
 */

#include <libGyro/gyro_sensor_api.h>

extern "C" {

int                     gySensorInit(const GYSensorInitParams* params) { return 1; }
int                     gySensorUninit(void) { return 1; }
GYSensorSupportFlags    gySensorSupportQuery(void) { return 0; }
int                     gySensorDevicePoll(const unsigned slot, struct GYSensorDevice** const sensorDevice) { return 0; }

int                     gyAccelerometerPoll(const unsigned slot, struct GYAccelerometer** const accelerometer) { return 0; }
int                     gyGyroscopePoll(const unsigned slot, struct GYGyroscope** const gyroscope) { return 0; }
int                     gyMagnetometerPoll(const unsigned slot, struct GYMagnetometer** const magnetometer) { return 0; }
int                     gyAltimeterPoll(const unsigned slot, struct GYAltimeter** const altimeter) { return 0; }
int                     gyPedometerPoll(const unsigned slot, struct GYPedometer** const pedometer) { return 0; }
int                     gyScalePoll(const unsigned slot, struct GYScale** const scale) { return 0; }


}


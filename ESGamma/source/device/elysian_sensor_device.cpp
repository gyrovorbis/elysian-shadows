/*
 * Copyright (c) 2013-2026 Elysian Shadows Team
 * SPDX-License-Identifier: MIT
 * Additional permission for Elysian Shadows Kickstarter backers: see LICENSE.
 */

#include <ESGamma/device/elysian_sensor_device.hpp>
#include <libGyro/gyro_sensor_api.h>

namespace elysian {

bool Accelerometer::    _isSupported = false;
bool Gyroscope::        _isSupported = false;
bool Magnetometer::     _isSupported = false;
bool Altimeter::        _isSupported = false;
bool Pedometer::        _isSupported = false;
bool Scale::            _isSupported = false;

bool Accelerometer::pollInput(void) {
    return gyAccelerometerPoll(getPlayerIndex(), &_accelerometer);
}

Vector3 Accelerometer::getAcceleration(void) const {
    return _accelerometer? _accelerometer->acceleration : Vector3();
}

bool Gyroscope::pollInput(void) {
    return gyGyroscopePoll(getPlayerIndex(), &_gyroscope);
}

Vector3 Gyroscope::getAngularVelocity(void) const {
    return _gyroscope? _gyroscope->angularVelocity : Vector3();
}

bool Magnetometer::pollInput(void) {
    return gyMagnetometerPoll(getPlayerIndex(), &_magnetometer);
}

Vector3 Magnetometer::getMagneticField(void) const {
    return _magnetometer? _magnetometer->magneticField : Vector3();
}

bool Altimeter::pollInput(void) {
    return gyAltimeterPoll(getPlayerIndex(), &_altimeter);
}

float Altimeter::getAltitude(void) const {
    return _altimeter? _altimeter->altitude : 0.0f;
}

bool Pedometer::pollInput(void) {
    return gyPedometerPoll(getPlayerIndex(), &_pedometer);
}

uint64_t Pedometer::getSteps(void) const {
    return _pedometer? _pedometer->steps : 0;
}

bool Scale::pollInput(void) {
    return gyScalePoll(getPlayerIndex(), &_scale);
}

float Scale::getWeight(void) const {
    return _scale? _scale->weight : 0.0f;
}


}

#include <device/elysian_rumble_device.hpp>


namespace elysian {

//RumbleEffect
RumbleEffect::RumbleEffect(void) {
    memset(&_gyEffect, 0, sizeof(GYRumbleEffect));
}

RumbleEffect::RumbleEffect(GY_RUMBLE_EFFECT_TYPE type):
    RumbleEffect()
{
    _gyEffect.effectType = type;
}

RumbleEffect::RumbleEffect(GY_RUMBLE_EFFECT_TYPE type, float duration):
    RumbleEffect(type)
{
    _gyEffect.duration = duration;
}

void RumbleEffect::setDuration(float secs) {
    _gyEffect.duration = secs;
}

//LeftRightRumbleEffect
LeftRightRumbleEffect::LeftRightRumbleEffect(void): RumbleEffect(GY_RUMBLE_EFFECT_LEFTRIGHT) {}

LeftRightRumbleEffect::LeftRightRumbleEffect(float duration, float largeIntensity, float smallIntensity):
    RumbleEffect(GY_RUMBLE_EFFECT_LEFTRIGHT, duration)
{
    _gyEffect.leftRight.largeIntensity = largeIntensity;
    _gyEffect.leftRight.smallIntensity = smallIntensity;
}

void LeftRightRumbleEffect::setLargeIntensity(float intensity) {
    _gyEffect.leftRight.largeIntensity = intensity;
}

void LeftRightRumbleEffect::setSmallIntensity(float intensity) {
    _gyEffect.leftRight.smallIntensity = intensity;
}

//DirectionalRumbleEffect
DirectionalRumbleEffect::DirectionalRumbleEffect(GY_RUMBLE_EFFECT_TYPE type):
    RumbleEffect(type) {}

void DirectionalRumbleEffect::setDirection(const Vector3 &dir) {
    switch(_gyEffect.effectType) {
    case GY_RUMBLE_EFFECT_CONSTANT:
        _gyEffect.constant.direction = dir;
        break;
    case GY_RUMBLE_EFFECT_RAMP:
        _gyEffect.ramp.direction = dir;
        break;
    case GY_RUMBLE_EFFECT_PERIODIC:
        _gyEffect.periodic.direction = dir;
        break;
    default:
        GY_ASSERT(false);
    }
}

void DirectionalRumbleEffect::setDelay(float secs) {
    switch(_gyEffect.effectType) {
    case GY_RUMBLE_EFFECT_CONSTANT:
        _gyEffect.constant.delay = secs;
        break;
    case GY_RUMBLE_EFFECT_RAMP:
        _gyEffect.ramp.delay = secs;
        break;
    case GY_RUMBLE_EFFECT_PERIODIC:
        _gyEffect.periodic.delay = secs;
        break;
    default:
        GY_ASSERT(false);
    }
}

void DirectionalRumbleEffect::setEnvelope(float startIntensity, float fadeInDuration, float endIntensity, float fadeOutDuration) {
    switch(_gyEffect.effectType) {
    case GY_RUMBLE_EFFECT_CONSTANT:
        _gyEffect.constant.startIntensity   = startIntensity;
        _gyEffect.constant.fadeInDuration   = fadeInDuration;
        _gyEffect.constant.endIntensity     = endIntensity;
        _gyEffect.constant.fadeOutDuration  = fadeOutDuration;
        break;
    case GY_RUMBLE_EFFECT_RAMP:
        _gyEffect.ramp.startIntensity   = startIntensity;
        _gyEffect.ramp.fadeInDuration   = fadeInDuration;
        _gyEffect.ramp.endIntensity     = endIntensity;
        _gyEffect.ramp.fadeOutDuration  = fadeOutDuration;
        break;
    case GY_RUMBLE_EFFECT_PERIODIC:
        _gyEffect.periodic.startIntensity   = startIntensity;
        _gyEffect.periodic.fadeInDuration   = fadeInDuration;
        _gyEffect.periodic.endIntensity     = endIntensity;
        _gyEffect.periodic.fadeOutDuration  = fadeOutDuration;
        break;
    default:
        GY_ASSERT(false);
    }
}

//ConstantRumbleEffect
ConstantRumbleEffect::ConstantRumbleEffect(void):
    DirectionalRumbleEffect(GY_RUMBLE_EFFECT_CONSTANT) {}

ConstantRumbleEffect::ConstantRumbleEffect(float duration, float intensity):
    ConstantRumbleEffect()
{
    _gyEffect.duration = duration;
    _gyEffect.constant.intensity = intensity;
}

void ConstantRumbleEffect::setIntensity(float intensity) {
    _gyEffect.constant.intensity = intensity;
}

//RampRumbleEffect
RampRumbleEffect::RampRumbleEffect(void):
    DirectionalRumbleEffect(GY_RUMBLE_EFFECT_RAMP) {}

RampRumbleEffect::RampRumbleEffect(float duration, float rampStartIntensity, float rampEndIntensity):
    RampRumbleEffect()
{
    _gyEffect.duration                  = duration;
    _gyEffect.ramp.rampStartIntensity   = rampStartIntensity;
    _gyEffect.ramp.rampEndIntensity     = rampEndIntensity;
}

void RampRumbleEffect::setRampStartIntensity(float intensity) {
    _gyEffect.ramp.rampStartIntensity   = intensity;
}

void RampRumbleEffect::setRampEndIntensity(float intensity) {
    _gyEffect.ramp.rampEndIntensity     = intensity;
}

//PeriodicRumbleEffect
PeriodicRumbleEffect::PeriodicRumbleEffect(GY_RUMBLE_EFFECT_PERIODIC_TYPE type):
    DirectionalRumbleEffect(GY_RUMBLE_EFFECT_PERIODIC)
{
    _gyEffect.periodic.periodicType = type;
}

PeriodicRumbleEffect::PeriodicRumbleEffect(GY_RUMBLE_EFFECT_PERIODIC_TYPE type, float duration, float period, float intensityPeak):
    PeriodicRumbleEffect(type)
{
    _gyEffect.duration                  = duration;
    _gyEffect.periodic.period           = period;
    _gyEffect.periodic.intensityPeak    = intensityPeak;
}

void PeriodicRumbleEffect::setPeriod(float secs) {
    _gyEffect.periodic.period = secs;
}

void PeriodicRumbleEffect::setPhaseShift(float degs) {
    _gyEffect.periodic.phaseShift = degs;
}

void PeriodicRumbleEffect::setIntensityPeak(float intensity) {
    _gyEffect.periodic.intensityPeak = intensity;
}

void PeriodicRumbleEffect::setIntensityOffset(float intensity) {
    _gyEffect.periodic.intensityOffset = intensity;
}

bool RumbleDevice::pollDevice(void) {
    return _attached = gyRumblePoll(_playerIndex, &_gyHandle);
}

bool RumbleDevice::play(float intensity, float duration) const {
    return gyRumblePlay(_gyHandle, intensity, duration*1000.0f);
}

bool RumbleDevice::playEffect(const RumbleEffect &effect, uint32_t iterations) const {
    return gyRumblePlayEffect(_gyHandle, &effect._gyEffect, iterations);
}

bool RumbleDevice::stop(void) const {
    return gyRumbleStop(_gyHandle);
}

}

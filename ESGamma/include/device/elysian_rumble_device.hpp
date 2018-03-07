#ifndef ELYSIAN_RUMBLE_DEVICE_HPP
#define ELYSIAN_RUMBLE_DEVICE_HPP

#include <device/elysian_player_io_device.hpp>
#include <level/elysian_event.hpp>
#include <primitive/elysian_instance_transform.hpp>
#include <gyro_rumble_api.h>

namespace elysian {
    class RumbleDevice;

    class RumbleEffect {
        friend class RumbleDevice;
    protected:
        GYRumbleEffect          _gyEffect;

                                RumbleEffect(void);
    public:
                                RumbleEffect(GY_RUMBLE_EFFECT_TYPE type);
                                RumbleEffect(GY_RUMBLE_EFFECT_TYPE type, float duration);
        GY_RUMBLE_EFFECT_TYPE   getEffectType(void) const;
        void                    setDuration(float val);
    };

    class LeftRightRumbleEffect: public RumbleEffect {
    public:
                                LeftRightRumbleEffect(void);
                                LeftRightRumbleEffect(float duration, float largeIntensity, float smallIntensity);

        void                    setLargeIntensity(float intensity);
        void                    setSmallIntensity(float intensity);
    };


    class DirectionalRumbleEffect: public RumbleEffect {
    public:
                                DirectionalRumbleEffect(GY_RUMBLE_EFFECT_TYPE type);
        void                    setDirection(const Vector3& dir);
        void                    setDelay(float secs);
        void                    setEnvelope(float startIntensity, float fadeInDuration, float endIntensity, float fadeOutDuration);

    };

    class ConstantRumbleEffect: public DirectionalRumbleEffect {
    public:
                                ConstantRumbleEffect(void);
                                ConstantRumbleEffect(float duration, float intensity);

        void                    setIntensity(float intensity);
    };

    class RampRumbleEffect: public DirectionalRumbleEffect {
    public:
                                RampRumbleEffect(void);
                                RampRumbleEffect(float duration, float rampStartIntensity, float rampEndIntensity);

        void                    setRampStartIntensity(float intensity);
        void                    setRampEndIntensity(float intensity);
    };

    class PeriodicRumbleEffect: public DirectionalRumbleEffect {
    public:
                                PeriodicRumbleEffect(GY_RUMBLE_EFFECT_PERIODIC_TYPE type);
                                PeriodicRumbleEffect(GY_RUMBLE_EFFECT_PERIODIC_TYPE type, float duration, float period, float intensityPeak);

        void                    setPeriod(float secs);
        void                    setPhaseShift(float degs);
        void                    setIntensityPeak(float intensity);
        void                    setIntensityOffset(float intensity);
    };



    class RumbleDeviceEvent: public Event {
    private:
        RumbleDevice* _dev;
    public:
                        RumbleDeviceEvent(EventType type, RumbleDevice* rumble);
        RumbleDevice*   getRumbleDevice(void) const;
    };


    class RumbleDevice: public PlayerIODevice {
    private:
        GYRumble*   _gyHandle = nullptr;
    public:

        bool        pollDevice(void);
        bool        play(float intensity, float duration) const;
        bool        playEffect(const RumbleEffect& effect, uint32_t iterations) const;
        bool        stop(void) const;
    };





//====== RUMBLE INLINEZ =======
inline RumbleDeviceEvent::RumbleDeviceEvent(EventType type, RumbleDevice *rumble):
    Event(type), _dev(rumble) {}
inline RumbleDevice* RumbleDeviceEvent::getRumbleDevice(void) const { return _dev; }

}

#endif // ELYSIAN_RUMBLE_DEVICE_HPP

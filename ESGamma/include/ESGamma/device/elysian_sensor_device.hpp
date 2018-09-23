#ifndef ELYSIAN_SENSOR_DEVICE_HPP
#define ELYSIAN_SENSOR_DEVICE_HPP

#include <ESGamma/device/elysian_player_io_device.hpp>
#include <AssetIO/primitive/elysian_instance_transform.hpp>

struct GYAccelerometer;
struct GYGyroscope;
struct GYPedometer;
struct GYMagnetometer;
struct GYAltimeter;
struct GYScale;

namespace elysian {

    class Accelerometer: public PlayerIODevice {
    private:
        static bool         _isSupported;
        GYAccelerometer*    _accelerometer = nullptr;
    public:
        bool                pollInput(void);
        Vector3             getAcceleration(void) const;
        static bool         isSupported(void);
        static void         setSupported(bool val=true);

    };

    class Gyroscope: public PlayerIODevice {
    private:
        static bool     _isSupported;
        GYGyroscope*    _gyroscope = nullptr;

    public:
        bool            pollInput(void);
        Vector3         getAngularVelocity(void) const;
        static bool     isSupported(void);
        static void     setSupported(bool val=true);

    };

    class Magnetometer: public PlayerIODevice {
    private:
        static bool     _isSupported;
        GYMagnetometer* _magnetometer = nullptr;

    public:
        bool            pollInput(void);
        Vector3         getMagneticField(void) const;
        static bool     isSupported(void);
        static void     setSupported(bool val=true);

    };

    class Altimeter: public PlayerIODevice {
    private:
        static bool     _isSupported;
        GYAltimeter*    _altimeter = nullptr;

    public:
        bool            pollInput(void);
        float           getAltitude(void) const;
        static bool     isSupported(void);
        static void     setSupported(bool val=true);

    };

    class Pedometer: public PlayerIODevice {
    private:
        static bool     _isSupported;
        GYPedometer*    _pedometer = nullptr;

    public:
        bool            pollInput(void);
        uint64_t        getSteps(void) const;
        static bool     isSupported(void);
        static void     setSupported(bool val=true);

    };

    class Scale: public PlayerIODevice {
    private:
        static bool _isSupported;
        GYScale*    _scale = nullptr;

    public:
        bool        pollInput(void);
        float       getWeight(void) const;
        static bool isSupported(void);
        static void setSupported(bool val=true);
    };

    inline bool Accelerometer::isSupported(void) { return _isSupported; }
    inline bool Gyroscope::isSupported(void) { return _isSupported; }
    inline bool Magnetometer::isSupported(void) { return _isSupported; }
    inline bool Altimeter::isSupported(void) { return _isSupported; }
    inline bool Pedometer::isSupported(void) { return _isSupported; }
    inline bool Scale::isSupported(void) { return _isSupported; }

    inline void Accelerometer::setSupported(bool val) { _isSupported = val; }
    inline void Gyroscope::setSupported(bool val) { _isSupported = val; }
    inline void Magnetometer::setSupported(bool val) { _isSupported = val; }
    inline void Altimeter::setSupported(bool val) { _isSupported = val; }
    inline void Pedometer::setSupported(bool val) { _isSupported = val; }
    inline void Scale::setSupported(bool val) { _isSupported = val; }
}



#endif // ELYSIAN_SENSOR_DEVICE_HPP

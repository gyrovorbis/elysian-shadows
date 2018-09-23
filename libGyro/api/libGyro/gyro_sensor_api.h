#ifndef GYRO_SENSOR_API_H
#define GYRO_SENSOR_API_H

#include <stdint.h>
#include <libGyro/gyro_matrix_api.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * EXAMPLE DEVICES:
 * iOS/Android Phones/Watches/Mobile
 * Wiimotes
 * Dreamcast fishing rods/maracas
 * PS3/PS4/3DS/Switch Tilt sensors
*/

typedef uint32_t GYSensorSupportFlags;

typedef enum GY_SENSOR_DEVICE_FEATURE_BITS {
    GY_SENSOR_DEVICE_SUPPORT_ACCELEROMETER  = 0x0,
    GY_SENSOR_DEVICE_SUPPORT_GYROSCOPE      = 0x1,
    GY_SENSOR_DEVICE_SUPPORT_PEDOMETER      = 0x2,
    GY_SENSOR_DEVICE_SUPPORT_MAGNETOMETER   = 0x4,
    GY_SENSOR_DEVICE_SUPPORT_ALTIMETER      = 0x8,
    GY_SENSOR_DEVICE_SUPPORT_SCALE          = 0x10
} GY_SENSOR_DEVICE_FEATURE_BITS;

typedef struct GYAccelerometer {
    GYVector3 acceleration;     //meters per second
} GYAccelerometer;

typedef struct GYGyroscope {
    GYVector3 angularVelocity;  //radians per second
} GYGyroscope;

    //pace, average pace, total steps, etc.
typedef struct GYPedometer {
    uint64_t    steps; //meters
} GYPedometer;

typedef struct GYMagnetometer {
    GYVector3   magneticField; //microteslas
} GYMagnetometer;

typedef struct GYAltimeter {
    float       altitude;       //meters
} GYAltimeter;

typedef struct GYScale {
    float       weight;         //kg, Wiiscale
} GYScale;

//Encapsulates all SENSOR-related subdevices and data
typedef struct GYSensorDevice {
    GYAccelerometer         accelerometer;
    GYGyroscope             gyroscope;
    GYPedometer             pedometer;
    GYMagnetometer          magnetometer;
    GYAltimeter             altimeter;
    GYScale                 scale;
    GYSensorSupportFlags    featureFlags;
} GYSENSORDevice;

typedef struct GYSensorInitParams {
    GYSensorSupportFlags    requestedFeatures;
    unsigned                maxDeviceCount;
} GYSENSORInitParams;


int                     gySensorInit(const GYSensorInitParams* params);
int                     gySensorUninit(void);
GYSensorSupportFlags    gySensorSupportQuery(void);
int                     gySensorDevicePoll(const unsigned slot, struct GYSensorDevice** const sensorDevice);

int                     gyAccelerometerPoll(const unsigned slot, struct GYAccelerometer** const accelerometer);
int                     gyGyroscopePoll(const unsigned slot, struct GYGyroscope** const gyroscope);
int                     gyMagnetometerPoll(const unsigned slot, struct GYMagnetometer** const magnetometer);
int                     gyAltimeterPoll(const unsigned slot, struct GYAltimeter** const altimeter);
int                     gyPedometerPoll(const unsigned slot, struct GYPedometer** const pedometer);
int                     gyScalePoll(const unsigned slot, struct GYScale** const scale);



#ifdef __cplusplus
}
#endif

#endif // GYRO_SENSOR_API_H

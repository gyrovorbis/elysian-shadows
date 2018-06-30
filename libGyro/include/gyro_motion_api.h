#ifndef GYRO_MOTION_API_H
#define GYRO_MOTION_API_H

#include <stdint.h>
#include <gyro_matrix_api.h>

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

typedef uint32_t GYMotionSupportFlags;

typedef enum GY_MOTION_DEVICE_FEATURE_BITS {
    GY_MOTION_DEVICE_SUPPORT_ACCELEROMETER  = 0x0,
    GY_MOTION_DEVICE_SUPPORT_GYROSCOPE      = 0x1,
    GY_MOTION_DEVICE_SUPPORT_PEDOMETER      = 0x2,
    GY_MOTION_DEVICE_SUPPORT_MAGNETOMETER   = 0x4,
    GY_MOTION_DEVICE_SUPPORT_ALTIMETER      = 0x8,
    GY_MOTION_DEVICE_SUPPORT_SCALE          = 0x10
} GY_MOTION_DEVICE_FEATURE_BITS;

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
    float       pressure;       //kilopascals
} GYAltimeter;

typedef struct GYScale {
    float       weight;         //kg, Wiiscale
} GYScale;

//Encapsulates all motion-related subdevices and data
typedef struct GYMotionDevice {
    GYAccelerometer         accelerometer;
    GYGyroscope             gyroscope;
    GYPedometer             pedometer;
    GYMagnetometer          magnetometer;
    GYAltimeter             altimeter;
    GYScale                 scale;
    GYMotionSupportFlags    featureFlags;
} GYMotionDevice;

typedef struct GYMotionInitParams {
    GYMotionSupportFlags    requestedFeatures;
    unsigned                maxDeviceCount;
} GYMotionInitParams;


int gyMotionInit(const GYMotionInitParams* params);
int gyMotionUninit(void);

int gyMotionDevicePoll(const unsigned slot, struct GYMotionDevice** const motionDevice);
int gyAccelerometerPoll(const unsigned slot, struct GYAccelerometer** const accelerometer);
int gyGyroscopePoll(const unsigned slot, struct GYGyroscope** const gyroscope);
int gyMagnetometerPoll(const unsigned slot, struct GYMagnetometer** const magnetometer);
int gyAltimeterPoll(const unsigned slot, struct GYAltimeter** const altimeter);
int gyScalePoll(const unsigned slot, struct GYScale** const scale);



#ifdef __cplusplus
}
#endif

#endif // GYRO_MOTION_API_H

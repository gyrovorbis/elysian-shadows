#include "gyro_vmu_device.h"
#include "gyro_vmu_cpu.h"
#include "gyro_vmu_serial.h"
#include <gyro_system_api.h>
#include <gyro_vmu_buzzer.h>
#include <gyro_vmu_gamepad.h>
#include <gyro_vmu_osc.h>
#include <string.h>
#include <stdlib.h>

VMUDevice* gyVmuDeviceCreate(void) {
    _gyLog(GY_DEBUG_VERBOSE, "Creating VMU Device.");
    VMUDevice* device = malloc(sizeof(VMUDevice));
    memset(device, 0, sizeof(VMUDevice));

    gyVmuBuzzerInit(device);

    return device;
}

void gyVmuDeviceDestroy(VMUDevice* dev) {
    _gyLog(GY_DEBUG_VERBOSE, "Destroying VMU Device.");
    gyVmuBuzzerUninit(dev);
    free(dev);
}

int gyVmuDeviceDump(VMUDevice* dev, const char *path) {
    (void)dev;
    (void)path;
    return 0;
}

int gyVmuDeviceRestore(VMUDevice* dev, const char *path) {
    (void)dev;
    (void)path;
    return 0;
}

int gyVmuDeviceUpdate(VMUDevice* device, float deltaTime) {
    if(deltaTime >= gyVmuOscSecPerCycle(device)) {
        gyVmuSerialUpdate(device);


#if 0
#ifdef VMU_TRIGGER_SPEED_FACTOR
        if(device->gamepad.lt) deltaTime /= VMU_TRIGGER_SPEED_FACTOR;
        if(device->gamepad.rt) deltaTime *= VMU_TRIGGER_SPEED_FACTOR;
#endif
#endif
        gyVmuCpuTick(device, deltaTime);

        return 1;

    } else {

        return 0;
    }
}

void gyVmuDeviceReset(VMUDevice* device) {
    gyVmuCpuReset(device);
    gyVmuBuzzerReset(device);
}

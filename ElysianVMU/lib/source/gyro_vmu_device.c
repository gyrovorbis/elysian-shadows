#include "gyro_vmu_device.h"
#include <gyro_system_api.h>
#include <string.h>
#include <stdlib.h>

VMUDevice* gyVmuDeviceCreate(void) {
    _gyLog(GY_DEBUG_VERBOSE, "Creating VMU Device.");
    VMUDevice* device = malloc(sizeof(VMUDevice));
    memset(device, 0, sizeof(VMUDevice));
    //gyVmuCpuReset(device);
    return device;
}

void gyVmuDeviceDestroy(VMUDevice* device) {
    _gyLog(GY_DEBUG_VERBOSE, "Destroying VMU Device.");
    free(device);
}

int gyVmuDeviceDump(VMUDevice *dev, const char *path) {
    (void)dev;
    (void)path;
    return 0;
}

int gyVmuDeviceRestore(VMUDevice *dev, const char *path) {
    (void)dev;
    (void)path;
    return 0;
}

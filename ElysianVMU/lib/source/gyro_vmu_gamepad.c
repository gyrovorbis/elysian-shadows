#include "gyro_vmu_gamepad.h"
#include "gyro_vmu_device.h"
#include "gyro_vmu_sfr.h"
#include <assert.h>

int gyVmuButtonStateGet(const struct VMUDevice* dev, VMU_GAMEPAD_BUTTON but) {
    assert(but >= 0 && but < VMU_GAMEPAD_MAX);
    return dev->sfr[SFR_OFFSET(SFR_ADDR_P3)]&(1u<<but);
}

void gyVmuButtonStateSet(struct VMUDevice* dev, VMU_GAMEPAD_BUTTON but, int down) {
    assert(but >= 0 && but < VMU_GAMEPAD_MAX);

    const unsigned char mask = 1u<<but;
    if(down)
        dev->sfr[SFR_OFFSET(SFR_ADDR_P3)] &= ~mask;
    else
        dev->sfr[SFR_OFFSET(SFR_ADDR_P3)] |= mask;

    if(dev->sfr[SFR_OFFSET(SFR_ADDR_P3INT)] & SFR_P3INT_P32INT_MASK) {
        dev->sfr[SFR_OFFSET(SFR_ADDR_P3INT)] |= SFR_P3INT_P31INT_MASK;
    }

}

#ifndef GYRO_VMU_BIOS_H
#define GYRO_VMU_BIOS_H

#ifdef __cplusplus
extern "C" {
#endif

#define BIOS_ADDR_FM_WRT_EX 0x100
#define BIOS_ADDR_FM_VRF_EX 0x110
#define BIOS_ADDR_FM_PRD_EX 0x120
#define BIOS_ADDR_TIMER_EX  0x130

struct VMUDevice;

int gyVmuBiosLoadImage(struct VMUDevice* device, const unsigned char* buffer, int size);
int gyVmuBiosHandleCall(struct VMUDevice* dev);

#ifdef __cplusplus
}
#endif

#endif // GYRO_VMU_BIOS_H


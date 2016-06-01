#ifndef GYRO_VMU_CPU_H
#define GYRO_VMU_CPU_H

#ifdef __cplusplus
extern "C" {
#endif

struct VMUDevice;

int gyVmuCpuTick(struct VMUDevice* dev, float deltaTime);
int gyVmuCpuReset(struct VMUDevice* dev);

#ifdef __cplusplus
}
#endif

#endif // GYRO_VMU_CPU_H


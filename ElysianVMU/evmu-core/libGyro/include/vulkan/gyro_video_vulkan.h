/*
 * Copyright (c) 2013-2026 Elysian Shadows Team
 * SPDX-License-Identifier: MIT
 * Additional permission for Elysian Shadows Kickstarter backers: see LICENSE.
 */

#ifndef GYRO_VIDEO_VULKAN_H
#define GYRO_VIDEO_VULKAN_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

struct VkPhysicalDeviceFeatures;

typedef struct GYVkVidInitParams {

    const struct VkPhysicalDeviceFeatures*  requiredPhysicalDeviceFeatures;
    const uint32_t*                         physicalDeviceIndex;
    uint32_t                                deviceQueueFamilyIndex;

} GYVkVidInitParams;


int vulkanInit(const GYVkVidInitParams* initParams);
int vulkanUninit(void);


#ifdef __cplusplus
}
#endif



#endif // GYRO_VIDEO_VULKAN_H

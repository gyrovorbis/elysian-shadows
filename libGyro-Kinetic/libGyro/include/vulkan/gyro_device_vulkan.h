/*
 * Copyright (c) 2013-2026 Elysian Shadows Team
 * SPDX-License-Identifier: MIT
 * Additional permission for Elysian Shadows Kickstarter backers: see LICENSE.
 */

#ifndef GYRO_DEVICE_VULKAN_H
#define GYRO_DEVICE_VULKAN_H

#include <stdint.h>
#include <vulkan/gyro_vulkan.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct GYVkPhysicalDevice {
    uint32_t index;
    VkPhysicalDevice                    device;
    VkPhysicalDeviceProperties          properties;
    VkPhysicalDeviceFeatures            features;
    VkPhysicalDeviceMemoryProperties    memoryProperties;
    VkQueueFamilyProperties*            queueFamilyProperties;
    uint32_t                            queueFamilyPropertiesCount;
    VkExtensionProperties*              extensionProperties;
    uint32_t                            extensionCount;
} GYVkPhysicalDevice;

int     gyVkPhysicalDevicesCreate(uint32_t* count, GYVkPhysicalDevice** devices);
void    gyVkPhysicalDevicesDestroy(uint32_t count, GYVkPhysicalDevice* devices);
int     gyVkPhysicalDevicesEnumerate(uint32_t* count, GYVkPhysicalDevice* devices);

void    gyVkPhysicalDeviceQueryAll(GYVkPhysicalDevice* device);
void    gyVkPhysicalDevicePrintAll(const GYVkPhysicalDevice* device);
void    gyVkPhysicalDevicePropertiesQuery(GYVkPhysicalDevice* device);
void    gyVkPhysicalDevicePropertiesPrint(const GYVkPhysicalDevice* device);
void    gyVkPhysicalDeviceFeaturesQuery(GYVkPhysicalDevice* device);
void    gyVkPhysicalDeviceFeaturesPrint(const GYVkPhysicalDevice* device);
void    gyVkPhysicalDeviceMemoryPropertiesQuery(GYVkPhysicalDevice* device);
void    gyVkPhysicalDeviceMemoryPropertiesPrint(const GYVkPhysicalDevice* device);
void    gyVkPhysicalDeviceQueueFamilyPropertiesQuery(GYVkPhysicalDevice* device);
void    gyVkPhysicalDeviceQueueFamilyPropertiesPrint(const GYVkPhysicalDevice* device);
void    gyVkPhysicalDeviceExtensionPropertiesQuery(GYVkPhysicalDevice* device);
void    gyVkPhysicalDeviceExtensionPropertiesPrint(const GYVkPhysicalDevice* device);

uint32_t gyVkPhysicalDeviceIndexFromFeatures(const VkPhysicalDeviceFeatures* features, VkBool32* found);
uint32_t gyVkPhysicalDeviceMemoryTypeIndexFromTypeProperties(const GYVkPhysicalDevice* device, uint32_t typeBits, VkMemoryPropertyFlags properties, VkBool32* memTypeFound);










#ifdef __cplusplus
}
#endif

#endif // GYRO_DEVICE_VULKAN_H

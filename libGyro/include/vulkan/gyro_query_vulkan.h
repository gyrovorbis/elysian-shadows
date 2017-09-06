#ifndef GYRO_QUERY_VULKAN_H
#define GYRO_QUERY_VULKAN_H

#include <vulkan/gyro_video_vulkan.h>

#ifdef __cplusplus
extern "C" {
#endif


void            vulkanInstanceExtensionPropertiesPrint(void);
void            vulkanInstanceLayerPropertiesPrint(void);
void            vulkanPhysicalDevicePrintAll(VkPhysicalDevice physicalDev);
inline uint32_t gyVkMemoryTypeIndexFromTypeProperties(uint32_t typeBits, VkMemoryPropertyFlags properties, VkBool32* memTypeFound);
uint32_t        gyVkPhysicalDeviceIndexFromFeatures(const VkPhysicalDeviceFeatures* features, VkBool32* found);



//===== INLINEZ ======


#ifdef __cplusplus
}
#endif

#endif // GYRO_QUERY_VULKAN_H

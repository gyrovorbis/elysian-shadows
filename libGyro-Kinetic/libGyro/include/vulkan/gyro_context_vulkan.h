#ifndef GYRO_CONTEXT_VULKAN_H
#define GYRO_CONTEXT_VULKAN_H

#include <vulkan/gyro_vulkan.h>

#ifdef __cplusplus
extern "C" {
#endif

struct GYVidInitParamsVulkan;
struct GYVkPhysicalDevice;

//Global context variables
extern VkInstance                           _vkInstance;
extern struct GYVkPhysicalDevice*           _gyVkPhysicalDevice;
extern struct GYVkPhysicalDevice*           _gyVkPhysicalDevices;
extern VkPhysicalDevice                     _vkPhysicalDevice;
extern uint32_t                             _vkPhysicalDeviceCount;
extern uint32_t                             _vkPhysicalDeviceIndex;
extern VkPhysicalDeviceFeatures*            _vkPhysicalDeviceFeatures;
extern VkDevice                             _vkDevice;
extern VkQueue                              _vkQueue;
extern VkCommandBuffer                      _vkCmdBuffer;
extern VkPhysicalDeviceMemoryProperties     _vkPhysicalDevMemProperties;
extern struct GYVkVidInitParams*            _vkInitParams;

#ifdef __cpluplus
}
#endif

#endif // GYRO_CONTEXT_VULKAN_H

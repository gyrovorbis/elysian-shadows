/*
 * Copyright (c) 2013-2026 Elysian Shadows Team
 * SPDX-License-Identifier: MIT
 * Additional permission for Elysian Shadows Kickstarter backers: see LICENSE.
 */

#include <vulkan/gyro_context_vulkan.h>
#include <vulkan/gyro_video_vulkan.h>

VkInstance                          _vkInstance;
struct GYVkPhysicalDevice*          _gyVkPhysicalDevice         = NULL;
VkPhysicalDevice                    _vkPhysicalDevice;
struct GYVkPhysicalDevice*          _gyVkPhysicalDevices        = NULL;
uint32_t                            _vkPhysicalDeviceCount      = 0;
uint32_t                            _vkPhysicalDeviceIndex      = 0;
VkPhysicalDeviceFeatures*           _vkPhysicalDeviceFeatures   = NULL;
VkDevice                            _vkDevice;
VkQueue                             _vkQueue;
VkCommandBuffer                     _vkCmdBuffer;
VkPhysicalDeviceMemoryProperties    _vkPhysicalDevMemProperties;
GYVkVidInitParams*                  _vkInitParams               = NULL;

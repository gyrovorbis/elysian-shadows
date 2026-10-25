/*
 * Copyright (c) 2013-2026 Elysian Shadows Team
 * SPDX-License-Identifier: MIT
 * Additional permission for Elysian Shadows Kickstarter backers: see LICENSE.
 */

#include <vulkan/gyro_vulkan.h>
#include <vulkan/gyro_context_vulkan.h>
#include <vulkan/gyro_video_vulkan.h>
#include <vulkan/gyro_util_vulkan.h>
#include <stdlib.h>
#include <assert.h>

extern inline uint32_t gyVkMemoryTypeIndexFromProperties(uint32_t typeBits, VkMemoryPropertyFlags properties, VkBool32* memTypeFound);


void vulkanInstanceExtensionPropertiesPrint(void) {
    VkResult result;
    VkExtensionProperties* properties;
    uint32_t extensionCount;

    _gyLog(GY_DEBUG_VERBOSE, "GYRO_VIDEO_API: Vulkan Instance Extension Properties");
    _gyPush();

    result = vkEnumerateInstanceExtensionProperties(NULL, &extensionCount, NULL);

    if(result == VK_SUCCESS) {
        if(extensionCount) {
            properties = malloc(sizeof(VkExtensionProperties)*extensionCount);

            result = vkEnumerateInstanceExtensionProperties(NULL, &extensionCount, properties);

            if(result == VK_SUCCESS) {
                for(unsigned i = 0; i < extensionCount; ++i) {
                    _gyLog(GY_DEBUG_VERBOSE, "Instance Extension[%u]: %s", i, properties[i].extensionName);
                    _gyPush();
                    _gyLog(GY_DEBUG_VERBOSE, "%-20s: %40u", "specVersion", properties[i].specVersion);
                    _gyPop(1);
                }
            } else _gyLog(GY_DEBUG_ERROR, "Failed to enumerate!");

            free(properties);

        } else _gyLog(GY_DEBUG_VERBOSE, "No Instance Extensions found!");

    } else _gyLog(GY_DEBUG_ERROR, "Query failed!");

    _gyPop(1);
}

void vulkanInstanceLayerPropertiesPrint(void) {
    VkResult result;
    VkLayerProperties* properties;
    uint32_t layerCount;

    _gyLog(GY_DEBUG_VERBOSE, "GYRO_VIDEO_API: Vulkan Instance Layer Properties");
    _gyPush();

    result = vkEnumerateInstanceLayerProperties(&layerCount, NULL);

    if(result == VK_SUCCESS) {
        if(layerCount > 0) {
            properties = malloc(sizeof(VkLayerProperties)*layerCount);

            result = vkEnumerateInstanceLayerProperties(&layerCount, properties);

            if(result == VK_SUCCESS) {

                for(unsigned i = 0; i < layerCount; ++i) {
                    _gyLog(GY_DEBUG_VERBOSE, "Property[%u]: %s", i, properties[i].layerName);
                    _gyPush();
                    _gyLog(GY_DEBUG_VERBOSE, "%-20s: %40u", "specVersion",              properties[i].specVersion);
                    _gyLog(GY_DEBUG_VERBOSE, "%-20s: %40u", "implementationVersion",    properties[i].implementationVersion);
                    _gyLog(GY_DEBUG_VERBOSE, "%-20s: %40s", "description",              properties[i].description);
                    _gyPop(1);
                }

            } else {
                _gyLog(GY_DEBUG_ERROR, "Problem with enumerating layers!");
            }

            free(properties);
        } else {
            _gyLog(GY_DEBUG_VERBOSE, "No Layers Found!");
        }
    } else {
        _gyLog(GY_DEBUG_ERROR, "Query failed!");
    }


    _gyPop(1);
}





//void vulkanPhysicalDeviceQueryQueueFamilyProperties(

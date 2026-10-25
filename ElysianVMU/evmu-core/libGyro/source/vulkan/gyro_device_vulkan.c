/*
 * Copyright (c) 2013-2026 Elysian Shadows Team
 * SPDX-License-Identifier: MIT
 * Additional permission for Elysian Shadows Kickstarter backers: see LICENSE.
 */

#include <vulkan/gyro_device_vulkan.h>
#include <vulkan/gyro_util_vulkan.h>
#include <vulkan/gyro_context_vulkan.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>


uint32_t gyVkPhysicalDeviceMemoryTypeIndexFromTypeProperties(const GYVkPhysicalDevice* device, uint32_t typeBits, VkMemoryPropertyFlags properties, VkBool32* memTypeFound) {
    assert(device);

    const VkPhysicalDeviceMemoryProperties* prop = &device->memoryProperties;
    for(uint32_t i = 0; i < prop->memoryTypeCount; i++) {
        if((typeBits & 1)) {
            if((prop->memoryTypes[i].propertyFlags & properties) == properties) {
                if(memTypeFound) {
                    *memTypeFound = 1;
                }
                return i;
            }
        }
        typeBits >>= 1;
    }

    if(memTypeFound) {
        *memTypeFound = 0;
    }

    _gyLog(GY_DEBUG_ERROR, "VULKAN API ERROR: Could not find physical device memory index from type and properties!");
    GY_VK_ASSERT(0);

    return 0;
}



uint32_t gyVkPhysicalDeviceIndexFromFeatures(const VkPhysicalDeviceFeatures* features, VkBool32* found) {
    assert(features);

    for(unsigned int i = 0; i < _vkPhysicalDeviceCount; ++i) {
        const VkBool32* ptr1 = (const VkBool32*)features;
        const VkBool32* ptr2 = (const VkBool32*)&_vkPhysicalDeviceFeatures[i];

        int matching = 1;

        for(unsigned int j = 0; j < sizeof(VkPhysicalDeviceFeatures)/sizeof(VkBool32); ++j) {
            if(*(ptr1 + j) && !(*(ptr2 + j))) {
                matching = 0;
                if(found) *found = 1;
                break;
            }
        }

        if(matching) {
            return i;
        }
    }

    if(found) *found = 0;
    return UINT32_MAX;
}

int gyVkPhysicalDevicesEnumerate(uint32_t* count, GYVkPhysicalDevice* devices) {
    int result;
    if(devices) {
        VkPhysicalDevice vkDevices[*count];
        result = GY_VK_VERIFY(vkEnumeratePhysicalDevices(_vkInstance, count, vkDevices));

        if(result && devices) {
            //Scatter from contiguous array back to gyro devices.
            for(unsigned i = 0; i < *count; ++i) {
                devices[i].index    = i;
                devices[i].device   = vkDevices[i];
            }
        }
    } else result = GY_VK_VERIFY(vkEnumeratePhysicalDevices(_vkInstance, count, NULL));

    return result;
}


int gyVkPhysicalDevicesCreate(uint32_t* count, GYVkPhysicalDevice** devices) {
    if(gyVkPhysicalDevicesEnumerate(count, NULL)) {
        *devices = malloc(sizeof(GYVkPhysicalDevice) * (*count));
        memset(*devices, 0, sizeof(GYVkPhysicalDevice) * (*count));

        if(gyVkPhysicalDevicesEnumerate(count, *devices)) {
            return 1;
        }
    }
    *devices = NULL;
    return 0;
}

void gyVkPhysicalDevicesDestroy(uint32_t count, GYVkPhysicalDevice* devices) {
    assert(devices);
    for(unsigned i = 0; i < count; ++i) {
        free(devices->queueFamilyProperties);
        free(devices->extensionProperties);
    }
    free(devices);
}


void gyVkPhysicalDeviceQueryAll(GYVkPhysicalDevice* device) {
    assert(device);
    gyVkPhysicalDevicePropertiesQuery(device);
    gyVkPhysicalDeviceFeaturesQuery(device);
    gyVkPhysicalDeviceMemoryPropertiesQuery(device);
    gyVkPhysicalDeviceQueueFamilyPropertiesQuery(device);
    gyVkPhysicalDeviceExtensionPropertiesQuery(device);
}

void gyVkPhysicalDevicePrintAll(const GYVkPhysicalDevice *device) {
    assert(device);
    gyVkPhysicalDevicePropertiesPrint(device);
    gyVkPhysicalDeviceFeaturesPrint(device);
    gyVkPhysicalDeviceMemoryPropertiesPrint(device);
    gyVkPhysicalDeviceQueueFamilyPropertiesPrint(device);
    gyVkPhysicalDeviceExtensionPropertiesPrint(device);
}

void gyVkPhysicalDevicePropertiesQuery(GYVkPhysicalDevice* device) {
    assert(device);
    vkGetPhysicalDeviceProperties(device->device, &device->properties);
}

inline static const char* gyVkPhysicalDeviceTypeString(VkPhysicalDeviceType type) {
    switch(type) {
    case VK_PHYSICAL_DEVICE_TYPE_OTHER:
        return "OTHER";
    case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU:
        return "INTEGRATED_GPU";
    case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:
        return "DISCRETE_GPU";
    case VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU:
        return "VIRTUAL_GPU";
    case VK_PHYSICAL_DEVICE_TYPE_CPU:
        return "CPU";
    default:
        return "ERRROR!";
    }
}

void gyVkPhysicalDevicePropertiesPrint(const GYVkPhysicalDevice* physicalDev) {
    assert(physicalDev);
    const VkPhysicalDeviceProperties* physicalDevProperties = &physicalDev->properties;
    _gyLog(GY_DEBUG_VERBOSE, "GYRO_VIDEO_API: Vulkan Physical Device Properties");
    _gyPush();

    _gyLog(GY_DEBUG_VERBOSE, "%-20s: %40u", "apiVersion",   physicalDevProperties->apiVersion);
    _gyLog(GY_DEBUG_VERBOSE, "%-20s: %40u", "driverVersion",physicalDevProperties->driverVersion);
    _gyLog(GY_DEBUG_VERBOSE, "%-20s: %40u", "vendorID",     physicalDevProperties->vendorID);
    _gyLog(GY_DEBUG_VERBOSE, "%-20s: %40u", "deviceID",     physicalDevProperties->deviceID);
    _gyLog(GY_DEBUG_VERBOSE, "%-20s: %40s", "deviceType",   gyVkPhysicalDeviceTypeString(physicalDevProperties->deviceType));
    _gyLog(GY_DEBUG_VERBOSE, "%-20s: %40s", "deviceName",   physicalDevProperties->deviceName);
    //TO BE CONTINUED: pipelineCacheUUID, limits, sparseProperties

    _gyPop(1);
}

void gyVkPhysicalDeviceFeaturesQuery(GYVkPhysicalDevice* device) {
    assert(device);
    vkGetPhysicalDeviceFeatures(device->device, &device->features);
}

void gyVkPhysicalDeviceFeaturesPrint(const GYVkPhysicalDevice* device) {
    assert(device);
#if 0
    _gyLog(GY_DEBUG_VERBOSE, "GYRO_VIDEO_API: Vulkan Physical Device Features");
    _gyPush();

    VkPhysicalDeviceFeatures physicalDevFeatures;
    vkGetPhysicalDeviceFeatures(physicalDev, &physicalDevFeatures);

    //PRINT SHIT HERE

    _gyPop(1);
#endif
}


void gyVkPhysicalDeviceMemoryPropertiesPrint(const GYVkPhysicalDevice* device) {
    assert(device);
    const VkPhysicalDeviceMemoryProperties* prop = &device->memoryProperties;

    _gyLog(GY_DEBUG_VERBOSE, "GYRO_VIDEO_API: Vulkan Physical Device Memory Properties");
    _gyPush();

    _gyLog(GY_DEBUG_VERBOSE, "Memory Types - %u", prop->memoryTypeCount);
    _gyPush();
    for(unsigned i = 0; i < prop->memoryTypeCount; ++i) {
        _gyLog(GY_DEBUG_VERBOSE, "Type[%u]", i);
        _gyPush();
        _gyLog(GY_DEBUG_VERBOSE, "%-20s: %40u", "propertyFlags",   prop->memoryTypes[i].propertyFlags);
        _gyLog(GY_DEBUG_VERBOSE, "%-20s: %40u", "heapIndex",       prop->memoryTypes[i].heapIndex);
        _gyPop(1);
    }
    _gyPop(1);

    _gyLog(GY_DEBUG_VERBOSE, "Heaps - %u", prop->memoryHeapCount);
    _gyPush();
    for(unsigned i = 0; i < prop->memoryHeapCount; ++i) {
        _gyLog(GY_DEBUG_VERBOSE, "Heap[%u]", i);
        _gyPush();
        _gyLog(GY_DEBUG_VERBOSE, "%-20s: %40u", "size",        prop->memoryHeaps[i].size);
        _gyLog(GY_DEBUG_VERBOSE, "%-20s: %40u", "flags",       prop->memoryHeaps[i].flags);
        _gyPop(1);
    }
    _gyPop(2);
}

void gyVkPhysicalDeviceMemoryPropertiesQuery(GYVkPhysicalDevice* device) {
    assert(device);
    vkGetPhysicalDeviceMemoryProperties(device->device, &device->memoryProperties);
}


void gyVkPhysicalDeviceQueueFamilyPropertiesPrint(const GYVkPhysicalDevice* device) {
    assert(device);

    const VkQueueFamilyProperties* props = device->queueFamilyProperties;
    uint32_t count = device->queueFamilyPropertiesCount;

    _gyLog(GY_DEBUG_VERBOSE, "GYRO_VIDEO_API: Vulkan Physical Queue Family Properties");
    _gyPush();

    for(unsigned int i = 0; i < count; ++i) {
        _gyLog(GY_DEBUG_VERBOSE, "QueueFamily[%u]", i);
        _gyPush();
        _gyLog(GY_DEBUG_VERBOSE, "%-20s: %40u", "queueFlags",           props[i].queueFlags);
        _gyLog(GY_DEBUG_VERBOSE, "%-20s: %40u", "queueCount",           props[i].queueCount);
        _gyLog(GY_DEBUG_VERBOSE, "%-20s: %40u", "timestampValidBits",   props[i].timestampValidBits);
        //minImageTransferGranularity! TODUZ!
        _gyPop(1);
    }

    _gyPop(1);
}

void gyVkPhysicalDeviceQueueFamilyPropertiesQuery(GYVkPhysicalDevice* device) {
    assert(device);
    vkGetPhysicalDeviceQueueFamilyProperties(device->device, &device->queueFamilyPropertiesCount, NULL);
    device->queueFamilyProperties = malloc(sizeof(VkQueueFamilyProperties) * device->queueFamilyPropertiesCount);
    vkGetPhysicalDeviceQueueFamilyProperties(device->device, &device->queueFamilyPropertiesCount, device->queueFamilyProperties);
}



void gyVkPhysicalDeviceExtensionPropertiesPrint(const GYVkPhysicalDevice* device) {
    assert(device);
    const VkExtensionProperties* properties = device->extensionProperties;

    if(device->extensionCount) {
        _gyLog(GY_DEBUG_VERBOSE, "GYRO_VIDEO_API: Vulkan Physical Device Extension Properties");
        _gyPush();

        for(unsigned i = 0; i < device->extensionCount; ++i) {
            _gyLog(GY_DEBUG_VERBOSE, "Device Extension[%u]: %s", i, properties[i].extensionName);
            _gyPush();
            _gyLog(GY_DEBUG_VERBOSE, "%-20s: %40u", "specVersion", properties[i].specVersion);
            _gyPop(1);
        }
        _gyPop(1);
    }
}


void gyVkPhysicalDeviceExtensionPropertiesQuery(GYVkPhysicalDevice* device) {
    assert(device);
    if(GY_VK_VERIFY(vkEnumerateDeviceExtensionProperties(device->device, NULL, &device->extensionCount, NULL))) {
        device->extensionProperties = malloc(sizeof(VkExtensionProperties) * device->extensionCount);
        vkEnumerateDeviceExtensionProperties(device->device, NULL, &device->extensionCount, device->extensionProperties);
    }
}


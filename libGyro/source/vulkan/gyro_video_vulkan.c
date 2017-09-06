#include <vulkan/gyro_video_vulkan.h>
#include <vulkan/gyro_device_vulkan.h>
#include <vulkan/gyro_context_vulkan.h>
#include <vulkan/gyro_vulkan.h>
#include <vulkan/gyro_util_vulkan.h>
#include <vulkan/gyro_query_vulkan.h>
#include <vulkan/gyro_descriptor_vulkan.h>
#include <gyro_system_api.h>
#include <stdint.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

const static GYVkVidInitParams  _defaultInitParams = {
    NULL,
    NULL,
    0
};


//Error handling cohesion
//exit vulkan init gracefully from all error paths
//uninit
//print all stats
//intelligently pick queues and devices
//iterate over everything
//optional init flags to manually pick device/queues
//malloc shit to keep track of memory usage for Vulkan? Probably a good idea
//optional debug mode shit for checking everything.



int vulkanInit(const GYVkVidInitParams* initParams) {
    int success = 1;
    VkResult result = VK_SUCCESS;

    _vkInitParams = malloc(sizeof(GYVkVidInitParams));
    if(initParams) {
        memcpy(_vkInitParams, initParams, sizeof(GYVkVidInitParams));
    } else {
        memcpy(_vkInitParams, &_defaultInitParams, sizeof(GYVkVidInitParams));
        initParams = &_defaultInitParams;
    }

    _gyLog(GY_DEBUG_VERBOSE, "GYRO_VIDEO_API: Initializing Vulkan.");
    _gyPush();

    VkApplicationInfo    appInfo;
    VkInstanceCreateInfo instanceCreateInfo;
    memset(&appInfo, 0, sizeof(VkApplicationInfo));
    memset(&instanceCreateInfo, 0, sizeof(VkInstanceCreateInfo));

    appInfo.sType                       = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName            = "LibGyro";
    appInfo.applicationVersion          = VK_MAKE_VERSION(GYRO_LIB_VERSION_MAJOR, GYRO_LIB_VERSION_MINOR, GYRO_LIB_VERSION_PATCH);
    appInfo.pEngineName                 = "LibGyro";
    appInfo.apiVersion                  = VK_API_VERSION_1_0;

    instanceCreateInfo.sType            = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    instanceCreateInfo.pNext            = NULL;
    instanceCreateInfo.pApplicationInfo = &appInfo;

    _gyLog(GY_DEBUG_VERBOSE, "Creating Instance.");
    _gyPush();
    if(!GY_VK_VERIFY(vkCreateInstance(&instanceCreateInfo, NULL, &_vkInstance))) {
        _gyLog(GY_DEBUG_ERROR, "Failure creating instance!");
        _gyPop(2);
        goto fail;
    }
    _gyPop(1);

    _gyLog(GY_DEBUG_VERBOSE, "Enumerating Physical Devices");
    _gyPush();
    if(gyVkPhysicalDevicesCreate(&_vkPhysicalDeviceCount, &_gyVkPhysicalDevices)) {
        for(unsigned int i = 0; i < _vkPhysicalDeviceCount; ++i) {
            _gyLog(GY_DEBUG_VERBOSE, "Querying Physical Device [%u]", i);
            _gyPush();
            gyVkPhysicalDeviceQueryAll(&_gyVkPhysicalDevices[i]);
            gyVkPhysicalDevicePrintAll(&_gyVkPhysicalDevices[i]);
            _gyPop(1);
        }
    } else {
        _gyPop(2);
        goto fail;
    }
    _gyPop(1);

    vulkanInstanceLayerPropertiesPrint();
    vulkanInstanceExtensionPropertiesPrint();

    if(initParams->physicalDeviceIndex) {
        if(*initParams->physicalDeviceIndex >= _vkPhysicalDeviceCount) {
           _gyLog(GY_DEBUG_ERROR, "Cannot select invalid physical device index: [%u]", initParams->physicalDeviceIndex);
           GY_VK_ASSERT(0);
           _gyPop(1);
           goto uninit_instance;
        } else {
            _vkPhysicalDeviceIndex = *initParams->physicalDeviceIndex;
        }
    } else if(initParams->requiredPhysicalDeviceFeatures) {
        VkBool32 validIndex;
        uint32_t index = gyVkPhysicalDeviceIndexFromFeatures(initParams->requiredPhysicalDeviceFeatures, &validIndex);
        if(validIndex) {
            _vkPhysicalDeviceIndex = index;
        } else {
            _gyLog(GY_DEBUG_ERROR, "Could not a PhysicalDevice with the required PhysicalDeviceFeatures!");
            GY_VK_ASSERT(0);
            _gyPop(1);
            goto uninit_instance;
        }
    }

    _gyVkPhysicalDevice = &_gyVkPhysicalDevices[_vkPhysicalDeviceIndex];
    _vkPhysicalDevice   = _gyVkPhysicalDevices->device;

    //Create a Logical Device
    _gyLog(GY_DEBUG_VERBOSE, "Creating Logical Device.");
    _gyPush();

    const float queuePriority = 1.0f;
    const VkDeviceQueueCreateInfo deviceQueueCreateInfo = {
        VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
        NULL,
        0,                  //flags
        initParams->deviceQueueFamilyIndex,   //queuefamilyIndex (NEEDS TO BE FUCKING VALIDATED!!!!)
        1,                  //queueCount
        &queuePriority      //pQueuePriorities
    };

    const VkDeviceCreateInfo deviceCreateInfo = {
        VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
        NULL,
        0,                              //flags
        1,                              //queueCreateInfoCount
        &deviceQueueCreateInfo,
        0,                              //enableedLayerCount
        NULL,                           //enabledLayerNames
        0,                              //enabledExtensionCount
        NULL,                           //enabledExtensionNames
        NULL//&requiredFeatures
    };

    result = vkCreateDevice(_vkPhysicalDevice, &deviceCreateInfo, NULL, &_vkDevice);
    if(result != VK_SUCCESS) {
        _gyLog(GY_DEBUG_ERROR, "Failure creating logical device!");
        _gyPop(2);
        goto uninit_instance;
    }
    _gyPop(1);

    vkGetDeviceQueue(_vkDevice,
                     0, //queue Family Index
                     0, //queueIndex
                     &_vkQueue);

    gyVkDescriptorPoolCreate();

    _gyPop(1);
    goto done;

uninit_instance:
    vkDestroyInstance(_vkInstance, NULL);
fail:
    success = 0;
done:
    return success;

}

int vulkanUninit(void) {
    int success = 1;
    _gyLog(GY_DEBUG_VERBOSE, "GYRO_VIDEO_API: Uninitializing Vulkan.");
    _gyPush();

    gyVkDescriptorPoolDestroy();

    if( (success &= GY_VK_VERIFY(vkDeviceWaitIdle(_vkDevice))) ) {
        gyVkPhysicalDevicesDestroy(_vkPhysicalDeviceCount, _gyVkPhysicalDevices);
        vkDestroyInstance(_vkInstance, NULL);
    }

    free(_vkInitParams);

    _gyPop(1);
    return success;
}

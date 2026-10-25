/*
 * Copyright (c) 2013-2026 Elysian Shadows Team
 * SPDX-License-Identifier: MIT
 * Additional permission for Elysian Shadows Kickstarter backers: see LICENSE.
 */

#ifndef GYRO_UTIL_VULKAN_H
#define GYRO_UTIL_VULKAN_H

#include <vulkan/gyro_vulkan.h>
#include <gyro_system_api.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifdef GY_VK_ASSERT_ON_ERROR

#define GY_VK_ASSERT(v) (assert(v))
#else
#define GY_VK_ASSERT(v)
#endif


#define GY_VK_VERIFY(func) \
    gyVkVerify((func), #func, __FILE__, __LINE__)

static inline const char* gyVkErrorString(VkResult errorCode) {
#define STRITIZE(e) case VK_ ##e: return #e
    switch(errorCode) {
        STRITIZE(NOT_READY);
        STRITIZE(TIMEOUT);
        STRITIZE(EVENT_SET);
        STRITIZE(EVENT_RESET);
        STRITIZE(INCOMPLETE);
        STRITIZE(ERROR_OUT_OF_HOST_MEMORY);
        STRITIZE(ERROR_OUT_OF_DEVICE_MEMORY);
        STRITIZE(ERROR_INITIALIZATION_FAILED);
        STRITIZE(ERROR_DEVICE_LOST);
        STRITIZE(ERROR_MEMORY_MAP_FAILED);
        STRITIZE(ERROR_LAYER_NOT_PRESENT);
        STRITIZE(ERROR_EXTENSION_NOT_PRESENT);
        STRITIZE(ERROR_FEATURE_NOT_PRESENT);
        STRITIZE(ERROR_INCOMPATIBLE_DRIVER);
        STRITIZE(ERROR_TOO_MANY_OBJECTS);
        STRITIZE(ERROR_FORMAT_NOT_SUPPORTED);
        STRITIZE(ERROR_SURFACE_LOST_KHR);
        STRITIZE(ERROR_NATIVE_WINDOW_IN_USE_KHR);
        STRITIZE(SUBOPTIMAL_KHR);
        STRITIZE(ERROR_OUT_OF_DATE_KHR);
        STRITIZE(ERROR_INCOMPATIBLE_DISPLAY_KHR);
        STRITIZE(ERROR_VALIDATION_FAILED_EXT);
        STRITIZE(ERROR_INVALID_SHADER_NV);
        default: return "UNKNOWN_ERROR";
    }
#undef STRITIZE
}

inline int gyVkVerify(VkResult result, const char* func, const char* file, int line) {
    if(result != VK_SUCCESS) {
        _gyLog(GY_DEBUG_ERROR, "VULKAN API ERROR: '%s' from [%s:%d] %s.", gyVkErrorString(result), file, line, func);
        GY_VK_ASSERT(0);
        return 0;
    }
    return 1;
}


#ifdef __cplusplus
}
#endif

#endif // GYRO_UTIL_VULKAN_H

/*
 * Copyright (c) 2013-2026 Elysian Shadows Team
 * SPDX-License-Identifier: MIT
 * Additional permission for Elysian Shadows Kickstarter backers: see LICENSE.
 */

#include <vulkan/gyro_util_vulkan.h>

//Generate symbols for inline functions with public external symbols
extern inline const char* gyVkErrorString(VkResult errorCode);
extern inline int gyVkVerify(VkResult result, const char* func, const char* file, int line);

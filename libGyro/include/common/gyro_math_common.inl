/*
 * Copyright (c) 2013-2026 Elysian Shadows Team
 * SPDX-License-Identifier: MIT
 * Additional permission for Elysian Shadows Kickstarter backers: see LICENSE.
 */

#include <math.h>

inline static void gySinCos(const float theta, float *const psin, float *const pcos) {
        *psin = sinf(theta); *pcos = cosf(theta);
}
    
inline static float gySqrt(const float val) {
    float retVal;
    retVal = sqrt(val);
    return retVal;
}

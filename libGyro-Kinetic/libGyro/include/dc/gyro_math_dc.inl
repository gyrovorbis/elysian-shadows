/*
 * Copyright (c) 2013-2026 Elysian Shadows Team
 * SPDX-License-Identifier: MIT
 * Additional permission for Elysian Shadows Kickstarter backers: see LICENSE.
 */

#include <kos.h>
#include <math.h>

inline static void gySinCos(const float theta, float *const psin, float *const pcos) {
    *psin = fsin(theta); *pcos = fcos(theta);
}

inline static float gySqrt(const float val) {
    return sqrt(val);
}

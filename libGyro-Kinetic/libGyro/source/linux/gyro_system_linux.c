/*
 * Copyright (c) 2013-2026 Elysian Shadows Team
 * SPDX-License-Identifier: MIT
 * Additional permission for Elysian Shadows Kickstarter backers: see LICENSE.
 */

#include <sys/utsname.h>
#include <string.h>
#include <stdio.h>

void _gyQueryOperatingSystemInfoString(char* buffer, int buffSize) {
    struct utsname uts;
    if(uname(&uts)) {
        buffer[0] = 0;
    } else {
        snprintf(buffer, buffSize, "%s %s", uts.sysname, uts.release, uts.version);
    }

}

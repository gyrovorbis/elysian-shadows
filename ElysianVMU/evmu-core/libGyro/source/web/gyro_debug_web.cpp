/*
 * Copyright (c) 2013-2026 Elysian Shadows Team
 * SPDX-License-Identifier: MIT
 * Additional permission for Elysian Shadows Kickstarter backers: see LICENSE.
 */

#include <stdio.h>
#include <stdarg.h>
#include <libGyro/gyro_system_api.h>
#include <libGyro/gyro_defines.h>

extern "C" int gyQtInit(void) {
    _gyLog(GY_DEBUG_VERBOSE, "Initializing Web");
    _gyPush();
    // _gyLog(GY_DEBUG_VERBOSE, "%-30s: %20s", "Qt Version (Compile-Time)",    QT_VERSION_STR);
    // _gyLog(GY_DEBUG_VERBOSE, "%-30s: %20s", "Qt Version (Run-Time)",        qVersion());
    _gyPop(1);
    return 1;
}

int gyDebugInit(void) {

    return 1;
}


void gyPrintf(const char *const str, ...) {
  va_list argptr;
  static char buffer[GYRO_PRINTF_BUFFER_SIZE];

  va_start(argptr, str);
  vsprintf(buffer, str, argptr);
  printf("\n");
  va_end(argptr);

  fprintf(stdout, "%s", buffer);
  fflush(stdout);
}

int gyDebugUninit(void) {

    return 1;
}

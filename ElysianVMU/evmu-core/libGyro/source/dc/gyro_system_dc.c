/*
 * Copyright (c) 2013-2026 Elysian Shadows Team
 * SPDX-License-Identifier: MIT
 * Additional permission for Elysian Shadows Kickstarter backers: see LICENSE.
 */

#include <gyro_system_api.h>
#include "gyro_defines.h"
#include <kos.h>
#include "stdio.h"
#include "stdarg.h"

int gySysAppExec(int argc, char* argv[]) {}

int gyDebugInit(void) {
    return 1;
}

int gySysInit(const GYSysInitParams*const params) {
    cont_btn_callback(0, CONT_START | CONT_A | CONT_B | CONT_X | CONT_Y, (void (*)(unsigned char, long  unsigned int))arch_exit);
    fs_chdir("/pc/");
    /* Quick test to make sure we've chrooted properly.
    file_t dir = fs_open("/pc/", O_RDONLY | O_DIR);
    dirent_t *file;
    file = fs_readdir(dir);
    while(file) {
        gyPrintf("FILE - %s\n", file->name);
        file = fs_readdir(dir);
    }

    fs_close(dir);*/

	_gyLog(GY_DEBUG_VERBOSE, "Initializing libGyro_DREAMCAST v%s\n", GYRO_LIB_VERSION);

    return 1;
}

int gySysUninit(void) {
    return 1;
}

void gyPrintf(const char *const str, ...) {
    va_list argptr;
    static char buffer[GYRO_PRINTF_BUFFER_SIZE];

    va_start(argptr, str);
    vsprintf(buffer, str, argptr);
    va_end(argptr);
    //printf(buffer);
    dbglog(DBG_DEBUG, buffer);
}

unsigned int gySysStatsVram() {
    //return pvr_mem_stats();
    return 1;
}

unsigned int gySysStatsRam() {
    mallinfo();
    return 1;
}


/*
 * Copyright (c) 2013-2026 Elysian Shadows Team
 * SPDX-License-Identifier: MIT
 * Additional permission for Elysian Shadows Kickstarter backers: see LICENSE.
 */

#include <stdio.h>
#include <sys/types.h>

#include <gyro_defines.h>
#include <gyro_system_api.h>

#include <android/log.h>

#include "../common/gyro_system_callbacks.h"

#include "android_native_app_glue.h"
#include "internal_android.h"

struct android_app *gy_android_app;


int gySysInit(const GYSysInitParams*const params)
{
	const char* platString = "ANDROID";
	_gyLog(GY_DEBUG_VERBOSE, "Initializing libGyro [%s] v%s", platString, GYRO_LIB_VERSION);
	return 1;
}

int gySysUninit(void)
{
	return 1;
}

void gyPrintf(const char *const str, ...)
{
	va_list argptr;
	va_start(argptr, str);
	__android_log_vprint(ANDROID_LOG_INFO, "libGyro", str, argptr);
	va_end(argptr);
}

unsigned int gySysStatsVram(void)
{
	return 0;
}

unsigned int gySysStatsRam(void)
{
	return 0;
}

static void _android_event_poller(void)
{
	int ident;
	int events;
	struct android_poll_source *source;
	while ((ident = ALooper_pollAll(-1, NULL, &events, (void**)&source)) >= 0) {
		if (source != NULL) {
			source->process(gy_android_app, source);
		}
		if (gy_android_app->destroyRequested != 0) {
			return;
		}
	}
}

int gyro_entry_point(int argc, char **argv)
{
#if 0
    gyro_main();
    if (gy_appInitCb != NULL) {
        if (!gy_appInitCb(argc, argv)) {
            return -1;
        }
    }
    do {
       // if (gy_appTickCb != NULL) {
        //    if (!gy_appTickCb()) {
        //        break;
        //    }
       // }
        if (gy_appDrawCb != NULL) {
            if (!gy_appDrawCb()) {
                break;
            }
        }
    } while (1);
    if (gy_appQuitCb != NULL) {
        if (!gy_appQuitCb()) {
            return -2;
        }
    }
    return 0;
#endif
}

static void _handle_android_cmd(struct android_app *app, int32_t cmd)
{
	switch (cmd) {
		case APP_CMD_SAVE_STATE:
			break;
		case APP_CMD_INIT_WINDOW:
			if (gy_android_app->window != NULL) {
				gyro_entry_point(0, 0);
			}
			break;
		case APP_CMD_TERM_WINDOW:
			break;
		case APP_CMD_GAINED_FOCUS:
			break;
		case APP_CMD_LOST_FOCUS:
			break;
	}
}

void android_main(struct android_app *app)
{
	app_dummy();
	gy_android_app = app;
    gy_android_app->onAppCmd = _handle_android_cmd;
    _android_event_poller();
}

int gyDebugInit(void) {
    return 1;
}

int gySysAppExec(int argc, char* argv[]) {}

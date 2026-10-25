/*
 * Copyright (c) 2013-2026 Elysian Shadows Team
 * SPDX-License-Identifier: MIT
 * Additional permission for Elysian Shadows Kickstarter backers: see LICENSE.
 */

#include <QDir>
#include <common/turfs.h>
#include <libGyro/gyro_file_api.h>
#include <libGyro/gyro_system_api.h>


extern "C" int gyFileInit(const GYFileInitParams*const params) {

    QString homeDirPath = params? params->workingDir : QDir::currentPath();

    if(!QDir::setCurrent(homeDirPath)) {
        _gyLog(GY_DEBUG_ERROR, "Could not cd into working directory directory: %s", homeDirPath.toStdString().c_str());
        return 0;
    }

    return turfs_init("ESTeam", "ESTk", NULL) == TURFS_RET_OK;
}

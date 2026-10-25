/*
 * Copyright (c) 2013-2026 Elysian Shadows Team
 * SPDX-License-Identifier: MIT
 * Additional permission for Elysian Shadows Kickstarter backers: see LICENSE.
 */

#include <QDir>
#include <common/turfs.h>
#include <libGyro/gyro_file_api.h>
#include <libGyro/gyro_system_api.h>
#include <libGyro/gyro_defines.h>


static QDir _assetPath;

extern "C" int gyFileInit(const GYFileInitParams*const params) {

    QString homeDirPath = params? params->workingDir : QDir::currentPath();

    if(!QDir::setCurrent(homeDirPath)) {
        _gyLog(GY_DEBUG_ERROR, "Could not cd into working directory directory: %s", homeDirPath.toStdString().c_str());
        return 0;
    } else {
        _assetPath = QDir::current();
    }

    return turfs_init("ESTeam", "ESTk", NULL) == TURFS_RET_OK;
}


extern "C" const char* gyFilePath(const char* relative) {

    if(!relative) return nullptr;

    _assetPath = QDir::current();
    if(!_assetPath.exists()) return relative;

    static char pathBuff[GYRO_PATH_MAX_SIZE];

    QString srcPath(relative);
    QString absPath;

    //It's an asset-relative path!!
    if(memcmp(relative, "/assets/", 8) == 0) {
        absPath = _assetPath.absoluteFilePath(&relative[8]);

        if(absPath.isEmpty()) {
            _gyLog(GY_DEBUG_WARNING, "Failed to find absolute asset path: %s", relative);
            return relative;
        }


    } else {
        absPath = _assetPath.absoluteFilePath(relative);

        if(absPath.isEmpty()) {
            _gyLog(GY_DEBUG_WARNING, "Failed to find absolute asset path: %s", relative);
            return relative;
        }


    }

    strncpy(pathBuff, absPath.toStdString().c_str(), GYRO_PATH_MAX_SIZE);

    return pathBuff;
}

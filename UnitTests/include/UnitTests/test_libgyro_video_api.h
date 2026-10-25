/*
 * Copyright (c) 2013-2026 Elysian Shadows Team
 * SPDX-License-Identifier: MIT
 * Additional permission for Elysian Shadows Kickstarter backers: see LICENSE.
 */

#ifndef TEST_LIBGYRO_VIDEO_API
#define TEST_LIBGYRO_VIDEO_API


#include <QObject>
#include <libGyro/gyro_video_api.h>


class TestLibGyroVideoApi: public QObject {

private Q_SLOTS:

    void initializeApi(void) {
        GYVidInitParams = {

        };

        gyVidInit();

    }

};



#endif // TEST_LIBGYRO_VIDEO_API


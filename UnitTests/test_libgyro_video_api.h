#ifndef TEST_LIBGYRO_VIDEO_API
#define TEST_LIBGYRO_VIDEO_API


#include <QObject>
#include <gyro_video_api.h>


class TestLibGyroVideoApi: public QObject {

private Q_SLOTS:

    void initializeApi(void) {
        GYVidInitParams = {

        };

        gyVidInit();

    }

};



#endif // TEST_LIBGYRO_VIDEO_API


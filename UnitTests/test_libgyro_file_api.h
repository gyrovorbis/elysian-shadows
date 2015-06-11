#ifndef TEST_LIBGYRO_FILE
#define TEST_LIBGYRO_FILE

#include <QtTest>
#include <gyro_file_api.h>
#include <test_estoolkit.h>
#include <QDir>

namespace tktest {

class LibGyroFileApiTest: public ESToolkitTest {
    Q_OBJECT
public:
    LibGyroFileApiTest() {
        QObject::setObjectName("RENAMING MY OBJECT N SHIT!!!!");
    }

private Q_SLOTS:

    void initializeApi(void) {
        QVERIFY(gyFileInit(nullptr));
    }



};





}


#endif // TEST_LIBGYRO_FILE


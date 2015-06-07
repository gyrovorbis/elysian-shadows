#ifndef TEST_LIBGYRO_FILE
#define TEST_LIBGYRO_FILE

#include <QTTest>
#include <gyro_file_api.h>
#include <test_estoolkit.h>
#include <QDir>

namespace tktest {

class BullshitTest: public QObject {
    Q_OBJECT
private Q_SLOTS:

void testzor(void) {
    qDebug() << "DYNAMICALLY BULLSHITTED";


}

};


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

    void bowsht(void) {
        qDebug() << "BOWSHT";
        enqueueNextText(new BullshitTest);
    }



};





}


#endif // TEST_LIBGYRO_FILE


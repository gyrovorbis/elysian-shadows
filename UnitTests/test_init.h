#ifndef TEST_INIT_H
#define TEST_INIT_H

#include <test_area.h>
#include <test_estk.h>
#include <QDebug>

namespace tktest {

class InitializationTest: public ESTkTest {
    Q_OBJECT
private:
    QString _assetsPath;

public:
    InitializationTest(QString assetsPath);

private Q_SLOTS:

    void createESToolkitMainWindow(void);
    void openProjectAssetsFolder(void);

};

inline InitializationTest::InitializationTest(QString assetsPath): _assetsPath(assetsPath) {}

inline void InitializationTest::createESToolkitMainWindow(void) {
    _estk = new ESToolkit;
    QVERIFY(_estk != nullptr);
}

inline void InitializationTest::openProjectAssetsFolder(void) {
    QVERIFY(_estk->openProjectDir(_assetsPath));
}





}

#endif // TEST_INIT_H


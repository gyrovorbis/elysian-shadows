#ifndef TEST_SEVERITY
#define TEST_SEVERITY

#include <tk_level.h>
#include <QObject>
#include <test_area.h>
#include <test_estk.h>
#include <QDebug>

namespace tktest {

class LevelTest: public ESTkTest {
    Q_OBJECT
private:
    TkLevel* _level;
public:
    LevelTest(TkLevel* level);

private Q_SLOTS:

    void initTestCase(void);
    void activateLevel(void);

};

inline LevelTest::LevelTest(TkLevel *level) { _level = level; }

inline void LevelTest::initTestCase() {
    qDebug() << QString("Verifying Level[") << QString(_level->path) << "]";
    QVERIFY(true);
}


inline void LevelTest::activateLevel(void) {
    getAssets()->setCurrentLevel(_level);
    for(unsigned i = 0 ; i < _level->getAreaAmount(); ++i) {
        ESTkTest::enqueueNextTest(new AreaTest(_level->getAreaByIndex(i)));
    }
    QVERIFY(true);
}


}


#endif // TEST_SEVERITY


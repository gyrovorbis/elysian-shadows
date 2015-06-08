#ifndef TEST_LEVEL
#define TEST_LEVEL

#include <tk_level.h>
#include <QObject>
#include <test_area.h>
#include <test_estoolkit.h>
#include <QDebug>

namespace tktest {

class LevelTest: public QObject, public AssetManager {
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
}


inline void LevelTest::activateLevel(void) {
    getAssets()->setCurrentLevel(_level);
    for(unsigned i = 0 ; i < _level->getAreaAmount(); ++i) {
        ESToolkitTest::enqueueNextText(new AreaTest(_level->getAreaByIndex(i)));
    }
}


}


#endif // TEST_LEVEL


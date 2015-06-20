#ifndef TEST_LEVEL_ASSETS_H
#define TEST_LEVEL_ASSETS_H


#include <tk_level.h>
#include <QObject>
#include <test_area_assets.h>
#include <test_estk.h>
#include <QDebug>
#include <tk_project.h>

namespace tktest {

class LevelAssetsTest: public ESTkTest {
    Q_OBJECT
private:

    TkLevel* _level;

public:
        LevelAssetsTest(Level*const lev): _level(lev) {}

private Q_SLOTS:




    inline void activateLevel(void) {
        getAssets()->setCurrentLevel(_level);
        for(unsigned i = 0 ; i < _level->getAreaAmount(); ++i) {
            ESTkTest::enqueueNextTest(new AreaAssetsTest(_level->getAreaByIndex(i)));
        }
        QVERIFY(true);
    }

};



}


#endif // TEST_LEVEL_ASSETS_H


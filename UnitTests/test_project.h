#ifndef TEST_OPEN_PROJECT
#define TEST_OPEN_PROJECT

#include <QTest>
#include <test_estk.h>
#include <tk_estoolkit.h>
#include <QDir>
#include <tk_assets.h>
#include <tk_project.h>
#include <test_level.h>

class Project;

namespace tktest {

class TestSmokeProject: public ESTkTest {
    Q_OBJECT


private:


private Q_SLOTS:
    void initTestCase();
    void cleanupTestCase();

    void openProject();
    //void closeProject();
    //void newLevel();
    //void deleteLevel();
    //void reloadProject();
    void iterateLevels();




};

inline void TestSmokeProject::initTestCase() {
    qDebug() << QString("Verifying Project[") << getAssets()->getCurrentProject()->getNodePath() << "]";
}

inline void TestSmokeProject::cleanupTestCase()
{
}

inline void TestSmokeProject::openProject()
{
  //  QVERIFY(getAssets()->getCurrentProject()- == _project);
}

inline void TestSmokeProject::iterateLevels() {
    QVERIFY(getAssets()->getCurrentProject());
    for(unsigned i = 0; i < getAssets()->getCurrentProject()->getLevelAmount(); ++i) {
      //  ESTkTest::enqueueNextTest(new LevelTest(getAssets()->getCurrentProject()->getLevelByIndex(i)));
    }
}

}
#endif // TEST_OPEN_PROJECT


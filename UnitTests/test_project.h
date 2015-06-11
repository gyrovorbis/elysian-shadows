#ifndef TEST_OPEN_PROJECT
#define TEST_OPEN_PROJECT

#include <QTest>
#include <test_estoolkit.h>
#include <tk_estoolkit.h>
#include <QDir>
#include <tk_assets.h>
#include <tk_project.h>
#include <test_level.h>

class Project;

namespace tktest {

class TestSmokeProject: public QObject, public ESTkTestCommon, public AssetManager {
    Q_OBJECT

public:
    TestSmokeProject(const QString project);

private:

    QString     _path;

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

inline TestSmokeProject::TestSmokeProject(const QString project): _path(project) {}

inline void TestSmokeProject::initTestCase() {
    qDebug() << QString("Verifying Project[") << _path << "]";
}

inline void TestSmokeProject::cleanupTestCase()
{
}

inline void TestSmokeProject::openProject()
{
    QVERIFY(_estk->openProjectDir(_path));
  //  QVERIFY(getAssets()->getCurrentProject()- == _project);
}

inline void TestSmokeProject::iterateLevels() {
    QVERIFY(getAssets()->getCurrentProject());
    for(unsigned i = 0; i < getAssets()->getCurrentProject()->getLevelAmount(); ++i) {
        ESToolkitTest::enqueueNextTest(new LevelTest(getAssets()->getCurrentProject()->getLevelByIndex(i)));
    }
}

}
#endif // TEST_OPEN_PROJECT


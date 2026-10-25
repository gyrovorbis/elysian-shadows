/*
 * Copyright (c) 2013-2026 Elysian Shadows Team
 * SPDX-License-Identifier: MIT
 * Additional permission for Elysian Shadows Kickstarter backers: see LICENSE.
 */

#ifndef TEST_OPEN_PROJECT
#define TEST_OPEN_PROJECT

#include <QTest>
#include <test_estk.h>
#include <ESToolkit/ui/tk_estoolkit.h>
#include <QDir>
#include <ESToolkit/core/tk_assets.h>
#include <ESToolkit/core/tk_project.h>
#include <test_level_assets.h>

class Project;

namespace tktest {

class TestProjectAssets: public ESTkTest {
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

inline void TestProjectAssets::initTestCase() {
    qDebug() << QString("Verifying Project[") << getAssets()->getCurrentProject()->getNodePath() << "]";
}

inline void TestProjectAssets::cleanupTestCase()
{
}

inline void TestProjectAssets::openProject()
{
  //  QVERIFY(getAssets()->getCurrentProject()- == _project);
}

inline void TestProjectAssets::iterateLevels() {
    QVERIFY(getAssets()->getCurrentProject());
    for(unsigned i = 0; i < getAssets()->getCurrentProject()->getLevelAmount(); ++i) {
        ESTkTest::enqueueNextTest(new LevelAssetsTest(getAssets()->getCurrentProject()->getLevelByIndex(i)));
    }
}

}
#endif // TEST_OPEN_PROJECT


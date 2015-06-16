#ifndef TEST_SEVERITY
#define TEST_SEVERITY

#include <tk_level.h>
#include <QObject>
#include <test_area.h>
#include <test_estk.h>
#include <QDebug>
#include <tk_tree_node.h>
#include <QTreeWidgetItem>
#include <tk_project.h>

#define TK_TEST_LEVEL_DIR   "_tkSystemTests"

namespace tktest {

class LevelTest: public ESTkTest {
    Q_OBJECT
private:
    TreeNode*   curNodeParent   = nullptr;
    Level*      _newLevel       = nullptr;
    Level*      _loadSaveLevel  = nullptr;

protected:


    Level*      _findByName(const QString name) const;
    QString     _nameToPath(const QString name) const;
    void        _indexLevelAtDepth(const QString name);
    void        _indexSubDirAtDepth(const QString name);

private Q_SLOTS:

    void initTestCase(void);
    void cleanupTestCase(void);
   // void activateLevel(void);

    void indexSubDirAtTopLevel();
    void indexLevelAtDepth1();
    void indexSubDirAtDepth1();
    void indexLevelAtDepth2();
    void flagInvalidLevelAtDepth2();
    void indexSubDirAtDepth2();
    void indexLevelAtDepth3();

    void addNewLevelAtTopWithSameName();
    void removeSameNameLevelFromTop();
    void reloadLevelAtDepth3();
    void duplicateLevelAtDepth1();
    void saveLevel();
    void loadLevel();
   /*

    void flagEmptySubDirAtDepth3();
    void save();
    void load();
    void duplicateTopLevel();       ADD AREA TAB BAR SHIT TO TESTS
    void duplicateSubDir();
    void reloadTopLevel();
    void renameLevel();
    void reloadSubDir();
    void removeTopLevel();
    void removeSubDir();
*/
};

inline QString LevelTest::_nameToPath(const QString name) const {
    return getCurrentProject()->getNodePath() + QString("/level/") + name;
}

inline Level* LevelTest::_findByName(const QString name) const {
    return getCurrentProject()->getLevel(_nameToPath(name));
}

inline void LevelTest::_indexLevelAtDepth(const QString name) {
    Level* level = _findByName(name);
    QVERIFY(level != nullptr);

    getAssets()->setCurrentLevel(level);
    QVERIFY(getCurrentLevel() != nullptr);

    TreeNode* node = getCurrentProject()->getTreeNode(_nameToPath(name));

    QVERIFY(node->getTreeWidgetItemType() == TreeNode::ITEM_LEVEL);

    QVERIFY(node->parent() == curNodeParent);
    QVERIFY(static_cast<TreeNode*>(getCurrentLevel()) == node);
}

inline void LevelTest::_indexSubDirAtDepth(const QString name) {
    auto* node = getCurrentProject()->getTreeNode(_nameToPath(name));

    QVERIFY(node != nullptr);

    QVERIFY(node->getTreeWidgetItemType() == TreeNode::ITEM_SUBDIR);

    QVERIFY(node->parent() == curNodeParent);
    curNodeParent = node;
}

inline void LevelTest::initTestCase(void) {
    QVERIFY(getCurrentProject() != nullptr);
    curNodeParent = getCurrentProject()->getTreeNode(getCurrentProject()->getNodePath() + QString("/level"));
}


inline void LevelTest::indexSubDirAtTopLevel(void) {
    _indexSubDirAtDepth("_tkSystemTests");
}


inline void LevelTest::indexLevelAtDepth1(void) {
    _indexLevelAtDepth("_tkSystemTests/level1");
}

inline void LevelTest::indexSubDirAtDepth1(void) {
    _indexSubDirAtDepth("_tkSystemTests/subdir1");

}


inline void LevelTest::indexLevelAtDepth2(void) {
    _indexLevelAtDepth("_tkSystemTests/subdir1/level2");
}

inline void LevelTest::flagInvalidLevelAtDepth2(void) {
    //QEXPECT_FAIL("", "Working as intended.", Continue);
    _indexLevelAtDepth("_tkSystemTests/subdir1/reestedLevel");
    auto* lev = _findByName("_tkSystemTests/subdir1/reestedLevel");
    QVERIFY(!lev->validSheets());

}

inline void LevelTest::indexSubDirAtDepth2(void) {
    _indexSubDirAtDepth("_tkSystemTests/subdir1/subdir2");

}


inline void LevelTest::indexLevelAtDepth3(void) {
    _indexLevelAtDepth("_tkSystemTests/subdir1/subdir2/level3");
}



inline void LevelTest::addNewLevelAtTopWithSameName(void) {
    TkLevel* level = getCurrentProject()->newLevel("_tkSystemTests");
    _newLevel = level;
    getAssets()->setCurrentArea(nullptr);
    getAssets()->setCurrentLevel(level);
    QVERIFY(getCurrentLevel() == level);
    QDir dir(level->getNodePath());
    QVERIFY(dir.exists("tile"));
    QVERIFY(dir.exists("area"));
    QVERIFY(dir.exists("estk"));
    QVERIFY(dir.cd("tile"));
    QVERIFY(dir.exists("tilesheet.png"));

    QVERIFY(getCurrentProject()->getLevel(level->getNodePath()));
    QTreeWidgetItem* node = static_cast<QTreeWidgetItem*>(level)->parent();
    QVERIFY(node);
    QTreeWidgetItem* root = static_cast<QTreeWidgetItem*>(getCurrentProject()->getTreeNode(getCurrentProject()->getNodePath() + QString("/level")));
    QVERIFY(root);

    QVERIFY(node == root);
}


inline void LevelTest::removeSameNameLevelFromTop(void) {
    const unsigned levelCount = getCurrentProject()->getLevelAmount();
    const QString path = _newLevel->getNodePath();
    QVERIFY(getCurrentProject()->removeLevel(_newLevel->getNodePath(), true));
    QVERIFY(getCurrentProject()->getLevelAmount() == levelCount-1);
    QVERIFY(!QDir(path).exists());

}

inline void LevelTest::reloadLevelAtDepth3(void) {
    auto* level = _findByName("_tkSystemTests/subdir1/subdir2/level3");
    QVERIFY(level);
    getAssets()->setCurrentLevel(level);
    QVERIFY(getCurrentLevel() == level);

    QVERIFY(level->getAreaAmount() == 1);
    QString nodeName = level->getAreaByIndex(0)->getNodeName();

    QString newNodeName = (nodeName == "area1")? "area2" : "area1";
    QTreeWidgetItem* parent = static_cast<QTreeWidgetItem*>(level)->parent();
    const int childCount = parent->childCount();

    QDir dir(level->getNodePath());
    QVERIFY(dir.cd("area"));
    QVERIFY(dir.rename(nodeName, newNodeName));
    QVERIFY(dir.mkdir("area0"));

    getAssets()->setCurrentLevel(nullptr);
    QVERIFY((level = getCurrentProject()->reloadLevel(level)));

    QVERIFY(level->getAreaAmount() == 2);
    QVERIFY(!level->getAreaByName(nodeName));
    QVERIFY(level->getAreaByName("area0"));
    QVERIFY(level->getAreaByName(newNodeName));
    QVERIFY(static_cast<QTreeWidgetItem*>(level)->parent() == parent);
    QVERIFY(parent->childCount() == childCount);

    QVERIFY(dir.rmdir("area0"));
}



inline void LevelTest::duplicateLevelAtDepth1() {
    Level* level = _findByName("_tkSystemTests/level1");
    QVERIFY(level);
    getAssets()->setCurrentLevel(level);
    QVERIFY(getCurrentLevel() == level);

    const unsigned levels = getCurrentProject()->getLevelAmount();

    QVERIFY(level->duplicate());

    QVERIFY(getCurrentProject()->getLevelAmount() == levels+1);

    Level* newLevel = getCurrentProject()->getLevelByIndex(getCurrentProject()->getLevelAmount()-1);
    _loadSaveLevel = newLevel;

    QVERIFY(level != newLevel);
    QVERIFY(static_cast<QTreeWidgetItem*>(level)->parent() == static_cast<QTreeWidgetItem*>(newLevel)->parent());
    QVERIFY(level->getAreaAmount() == newLevel->getAreaAmount());
    QVERIFY(newLevel->getNodeName() == "level11");
    QVERIFY(QDir(newLevel->getNodePath()).exists());

}

inline void LevelTest::saveLevel(void)  {
    QVERIFY(_loadSaveLevel);
    _loadSaveLevel->getTile(0)->animated        = true;
    _loadSaveLevel->getTile(1)->animTime        = 2.0f;
    _loadSaveLevel->getTile(2)->colliderType    = (elysian::Tile::COLLIDER_TYPE)3;
    _loadSaveLevel->getTile(3)->damping         = 8.0f;
    _loadSaveLevel->getTile(4)->nextIndex       = 12;
    _loadSaveLevel->getTile(5)->standOn         = true;
    _loadSaveLevel->getTile(6)->transparency    = true;
    QVERIFY(_loadSaveLevel->save());
    _loadSaveLevel->getTile(0)->animated        = false;
    _loadSaveLevel->getTile(1)->animTime        = -2.0f;
    _loadSaveLevel->getTile(2)->colliderType    = (elysian::Tile::COLLIDER_TYPE)-3;
    _loadSaveLevel->getTile(3)->damping         = -0.8f;
    _loadSaveLevel->getTile(4)->nextIndex       = 33;
    _loadSaveLevel->getTile(5)->standOn         = false;
    _loadSaveLevel->getTile(6)->transparency    = false;
}

inline void LevelTest::loadLevel(void) {
    QVERIFY(_loadSaveLevel);
    QVERIFY((_loadSaveLevel = getCurrentProject()->reloadLevel(_loadSaveLevel)));
    //LEVEL ISN'T TECHNICALLY "OPEN"/LOADED UNTIL IT HAS BEEN ACTIVATED!!!1111
    getAssets()->setCurrentLevel(_loadSaveLevel);
    QVERIFY(getCurrentLevel() == _loadSaveLevel);
    //QVERIFY(_loadSaveLevel->getTile(0)->animated);
    QVERIFY(_loadSaveLevel->getTile(1)->animTime == 2.0f);
    QVERIFY(_loadSaveLevel->getTile(2)->colliderType == (elysian::Tile::COLLIDER_TYPE)3);
    QVERIFY(_loadSaveLevel->getTile(3)->damping == 8.0f);
    QVERIFY(_loadSaveLevel->getTile(4)->nextIndex == 12);
    QVERIFY(_loadSaveLevel->getTile(5)->standOn == true);
    QVERIFY(_loadSaveLevel->getTile(6)->transparency == true);
}


inline void LevelTest::cleanupTestCase() {
    auto* level = _findByName("_tkSystemTests/level11");
    QVERIFY(level);
    QString path = level->getNodePath();
    QVERIFY(level);
    QVERIFY(getCurrentProject()->removeLevel(path, true));
    QVERIFY(!QDir(path).exists());

}




}













#if 0
inline void LevelTest::activateLevel(void) {
    getAssets()->setCurrentLevel(_level);
    for(unsigned i = 0 ; i < _level->getAreaAmount(); ++i) {
        ESTkTest::enqueueNextTest(new AreaTest(_level->getAreaByIndex(i)));
    }
    QVERIFY(true);
}
#endif


#endif // TEST_SEVERITY


#ifndef TEST_ESTOOLKIT_H
#define TEST_ESTOOLKIT_H

#include <test_estk.h>
#include <QtTest>
#include <ESToolkit/core/tk_asset_manager.h>
#include <ESToolkit/ui/tk_estoolkit.h>
#include <ESToolkit/AssetIO/tk_level.h>
#include <ESToolkit/AssetIO/tk_area.h>
#include <ESToolkit/core/tk_project.h>
#include <QTabBar>

namespace tktest {

class ESToolkitTest: public ESTkTest {
    Q_OBJECT
private:


    //test reload shaders
    //switch area via tabs
    //switch area via asset view
    //layer test (switch layers, lay tiles)

    void    _verifyCurrentTabsText() const;
    void    _verifyCurrentTabSelection() const;
    Level*  _findNextLevel(void) const;

private Q_SLOTS:

    void initTestCase(void);

    //Tab Bar
    void tabTextCorrectInitially(void);
    void tabSelectionCorrectInitially(void);
    void tabsTextUpdateOnLevelChange(void);
    void tabSelectionUpdatesOnLevelChange(void);
    void tabSelectionUpdatesOnAreaChange(void);
    void tabSelectionChangeTriggersAreaChange(void);
};


inline void ESToolkitTest::_verifyCurrentTabsText(void) const {
    QVERIFY(_estk->_areaTabBar->count() == (int)getCurrentLevel()->getAreaAmount());

    for(unsigned i = 0; i < getCurrentLevel()->getAreaAmount(); ++i) {
        QVERIFY(_estk->_areaTabBar->tabText(i) == getCurrentLevel()->getAreaByIndex(i)->getNodeName());
        qDebug() << "TAB TEXT" << _estk->_areaTabBar->tabText(i) << "AREA NAME" << getCurrentLevel()->getAreaByIndex(i)->getNodeName();
    }
}

inline void ESToolkitTest::_verifyCurrentTabSelection() const {
    QVERIFY(_estk->_areaTabBar->tabText(_estk->_areaTabBar->currentIndex()) == getCurrentArea()->getNodeName());
    qDebug() << "SELECTED" << _estk->_areaTabBar->tabText(_estk->_areaTabBar->currentIndex()) << "AREA" << getCurrentArea()->getNodeName();
}

inline Level* ESToolkitTest::_findNextLevel(void) const {
    TkLevel* nextLevel = nullptr;
    for(unsigned i = 0; i < getCurrentProject()->getLevelAmount(); ++i) {
        if(getCurrentProject()->getLevelByIndex(i) != getCurrentLevel()) {
            nextLevel = getCurrentProject()->getLevelByIndex(i);
            if(getCurrentProject()->getLevelByIndex(i)->getAreaAmount() >= 2) {
                break;  //prefer level with multiple areas
            }
        }
    }
    return nextLevel;
}


inline void ESToolkitTest::initTestCase(void) {
    QVERIFY(currentProjectLevelAreaExist());
}

inline void ESToolkitTest::tabTextCorrectInitially(void) {
    _verifyCurrentTabsText();
}

inline void ESToolkitTest::tabSelectionCorrectInitially(void) {
    _verifyCurrentTabSelection();
}

inline void ESToolkitTest::tabsTextUpdateOnLevelChange(void) {
    Level* next = _findNextLevel();
    qDebug() << "CURRENT" << getCurrentLevel()->getNodeName() << "NEXT" << next->getNodeName();
    if(!next) QSKIP("No other levels are loaded in the project!!!");
    getAssets()->setCurrentSelection(next);
    _verifyCurrentTabsText();
}

inline void ESToolkitTest::tabSelectionUpdatesOnLevelChange(void) {
    _verifyCurrentTabSelection();
}

inline void ESToolkitTest::tabSelectionUpdatesOnAreaChange(void) {
    if(getCurrentLevel()->getAreaAmount() < 2) QSKIP("Couldn't find a new level with more than one area for test!");

    for(unsigned i = 0; i < getCurrentLevel()->getAreaAmount(); ++i) {
        if(_estk->_areaTabBar->tabText(_estk->_areaTabBar->currentIndex()) != getCurrentLevel()->getAreaByIndex(i)->getNodeName()) {
            getAssets()->setCurrentSelection(getCurrentLevel()->getAreaByIndex(i));
            break;
        }
    }

    _verifyCurrentTabSelection();
}


inline void ESToolkitTest::tabSelectionChangeTriggersAreaChange(void) {
    if(getCurrentLevel()->getAreaAmount() < 2) QSKIP("Current level only has one area!");

    for(int i = 0; i < _estk->_areaTabBar->count(); ++i) {
        expectError(true);
        _estk->_areaTabBar->setCurrentIndex(i);
        expectError(false);
        QVERIFY(_estk->_areaTabBar->tabText(i) == getCurrentArea()->getNodeName());
    }
}

}


#endif // TEST_ESTOOLKIT_H


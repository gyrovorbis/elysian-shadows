/*
 * Copyright (c) 2013-2026 Elysian Shadows Team
 * SPDX-License-Identifier: MIT
 * Additional permission for Elysian Shadows Kickstarter backers: see LICENSE.
 */

#ifndef TEST_AREA
#define TEST_AREA

#include <ESToolkit/AssetIO/tk_area.h>
#include <QObject>
#include <QDebug>
#include <QtTest>

namespace tktest {

class AreaTest: public QObject, public AssetManager {
    Q_OBJECT
private:
    TkArea* _area;
public:
    AreaTest(TkArea* area);

private Q_SLOTS:

    void initTestCase(void);
    void activateArea(void);

};

inline AreaTest::AreaTest(TkArea *area): _area(area) {}

inline void AreaTest::initTestCase() {
    QVERIFY(true);
    qDebug() << QString("Verifying Area[") << QString(_area->path) << "]";
}


inline void AreaTest::activateArea(void) {
    QVERIFY(true);
    getAssets()->setCurrentArea(_area);
}


}

#endif // TEST_AREA


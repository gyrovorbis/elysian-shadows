#ifndef TEST_AREA
#define TEST_AREA

#include <tk_area.h>
#include <QObject>
#include <QDebug>

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
    qDebug() << QString("Verifying Area[") << QString(_area->path) << "]";
}


inline void AreaTest::activateArea(void) {
    getAssets()->setCurrentArea(_area);
}


}

#endif // TEST_AREA


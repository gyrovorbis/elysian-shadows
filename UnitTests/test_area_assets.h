#ifndef TEST_AREA_ASSETS_H
#define TEST_AREA_ASSETS_H


#include <tk_level.h>
#include <QObject>
#include <test_area.h>
#include <test_estk.h>
#include <QDebug>
#include <tk_project.h>

namespace tktest {

class AreaAssetsTest: public ESTkTest {
    Q_OBJECT
private:

    TkArea* _area;

public:
        AreaAssetsTest(Area*const ar): _area(ar) {}

private Q_SLOTS:




    inline void activateArea(void) {
        _area->activate();
    }

};



}



#endif // TEST_AREA_ASSETS_H


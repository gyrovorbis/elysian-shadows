#ifndef TEST_LUA_FIELD_REF_H
#define TEST_LUA_FIELD_REF_H

#include <QtTest>
#include <crtp/elysian_lua_variant.hpp>
#include <proxy/elysian_instance_transform_proxy.hpp>
#include <QDebug>

namespace tktest {


    class LuaFieldRefTest: public QObject, public LuaManager {
        Q_OBJECT

    private Q_SLOTS:

        void initTestCase(void);

    };


inline void LuaFieldRefTest::initTestCase(void) {}

}

#endif // TEST_LUA_FIELD_REF_H

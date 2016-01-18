#ifndef TEST_INSTANCE_TRANSFORM_H
#define TEST_INSTANCE_TRANSFORM_H

#include <test_estk.h>
#include <elysian_instance_transform.hpp>
#include <crtp/elysian_lua_variant.hpp>
#include <proxy/elysian_instance_transform_proxy.hpp>

namespace tktest {

    class Vector2Test: public ESTkTest {
        Q_OBJECT

    private Q_SLOTS:

        //C++
        void constructDefault(void);
        void constructFromFloats(void);
        void constructFromGYVector2(void);
        void constructFromVector2(void);

        void modifyGYVector2(void);

        void equalOperator(void);
        void notEqualOperator(void);

        //Lua wrappers
        void luaGetX(void);
        void luaSetX(void);
        void luaGetY(void);
        void luaSetY(void);
        void luaXProperty(void);
        void luaYProperty(void);
        void luaConstructFromTable(void);

        //overloaded Lua operators


    };


    // ======= IMPLEMENTATION ========

    inline void Vector2Test::constructDefault(void) {
        elysian::Vector2 vec;
        QVERIFY(vec.getX() == 0.0f);
        QVERIFY(vec.getY() == 0.0f);
    }

    inline void Vector2Test::constructFromFloats(void) {
        elysian::Vector2 vec(1.0f, 2.0f);
        QVERIFY(vec.getX() == 1.0f);
        QVERIFY(vec.getY() == 2.0f);
    }

    inline void Vector2Test::constructFromGYVector2(void) {
        GYVector2 gyVec = { 1.0f, 2.0f };
        elysian::Vector2 vec(&gyVec);
        QVERIFY(vec.getX() == 1.0f);
        QVERIFY(vec.getY() == 2.0f);
    }

    inline void Vector2Test::constructFromVector2(void) {
        elysian::Vector2 vec1(1.0f, 2.0f);
        elysian::Vector2 vec2(vec1);

        QVERIFY(vec2.getX() == vec1.getX());
        QVERIFY(vec2.getY() == vec2.getY());
    }

    inline void Vector2Test::modifyGYVector2(void) {
        GYVector2 vex = { 1.0f, 2.0f };
        elysian::Vector2 vec(&vex);

        vec.setX(-1.0f);
        vec.setY(-2.0f);

        QVERIFY(vex.x == vec.getX());
        QVERIFY(vex.y == vec.getY());

    }

    inline void Vector2Test::equalOperator(void) {
        elysian::Vector2 vec1, vec2;
        QVERIFY(vec1 == vec2);

    }

    inline void Vector2Test::notEqualOperator(void) {
        elysian::Vector2 vec1, vec2(-1.0f, -1.0f);
        QVERIFY(vec1 != vec2);
    }

    inline void Vector2Test::luaGetX(void) {
        elysian::Vector2 vec;
        elysian::LuaVariant ref(&vec);


    }

    inline void Vector2Test::luaSetX(void) {

    }

    inline void Vector2Test::luaGetY(void) {

    }

    inline void Vector2Test::luaSetY(void) {

    }

    inline void Vector2Test::luaXProperty(void) {

    }

    inline void Vector2Test::luaYProperty(void) {

    }

    inline void Vector2Test::luaConstructFromTable(void) {

    }

}


#endif // TEST_INSTANCE_TRANSFORM_H

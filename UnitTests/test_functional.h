#ifndef TEST_FUNCTIONAL_H
#define TEST_FUNCTIONAL_H

#include <QtTest>
#include <utility/elysian_functional.hpp>
#include <crtp/elysian_lua_manager.hpp>
#include <elysian_instance_transform.hpp>
#include <proxy/elysian_instance_transform_proxy.hpp>
#include <QDebug>

namespace tktest {


    class FunctionalTest: public QObject, public LuaManager {
        Q_OBJECT

    private Q_SLOTS:

        void initTestCase(void);
        void forwardTupleToLambda(void);
        void forwardTupleToStdFunction(void);
        void forwardTupleToMemberFunctionPtr(void);
        void forwardTupleToConstMemberFunctionPtr(void);
        void forwardTupleToCFuncPtr(void);
        void foreachInTuple(void);
        void foreachInTupleReverse(void);

    };


inline void FunctionalTest::initTestCase(void) {}

inline void FunctionalTest::forwardTupleToLambda(void) {

    auto lambda = [&](float, int, const char* s) {

        return s;
    };

    std::tuple<float, int, const char*> args = {
        17.7f,
        33,
        "Fuck you!"
    };

    //QVERIFY(strcmp(forward_tuple_to_func_as_args(lambda, args), "Fuck you!") == 0);
    QVERIFY(strcmp(forward_tuple(lambda, args), "Fuck you!") == 0);
}


inline void FunctionalTest::forwardTupleToStdFunction(void) {


}

inline void FunctionalTest::forwardTupleToMemberFunctionPtr(void) {
    elysian::InstanceTransform transform;

    std::tuple<elysian::InstanceTransform*, float> args = {
        &transform,
        78.6f
    };

    //forward_tuple_to_func_as_args(&elysian::InstanceTransform::setSizeX, args);
    forward_tuple(&elysian::InstanceTransform::setSizeX, args);

    QVERIFY(transform.getSizeX() == 78.6f);
}


inline void FunctionalTest::forwardTupleToConstMemberFunctionPtr(void) {
    const elysian::Vector2 vex { 33.0f, 192.8f };

    std::tuple<const elysian::Vector2*, float> args = {
        &vex,
        -14.4f
    };

    //forward_tuple_to_func_as_args(&elysian::Vector2::setX, args);
    forward_tuple(&elysian::Vector2::setX, args);

    QVERIFY(vex.getX() == -14.4f);

}

inline void FunctionalTest::forwardTupleToCFuncPtr(void) {
    const char*(*cFunc)(float, int, const char*) =  [](float, int, const char* s) {

        return s;
    };

    std::tuple<float, int, const char*> args = {
        17.7f,
        33,
        "WHOOO!"
    };

    QVERIFY(strcmp(forward_tuple(cFunc, args), "WHOOO!") == 0);
}

inline void FunctionalTest::foreachInTuple(void) {
    elysian::InstanceTransform xform;
    lua_pushboolean(*_mainState, true);
    lua_pushnumber(*_mainState, 12.0f);
    OOLUA::push(*_mainState, &xform);

    std::tuple<elysian::InstanceTransform*, float, bool> tuplett;

    foreach_in_tuple(tuplett, [](auto&& element) {
        OOLUA::pull(*_mainState, element);
    });


    QVERIFY(std::get<0>(tuplett) == &xform);
    QVERIFY(std::get<1>(tuplett) == 12.0f);
    QVERIFY(std::get<2>(tuplett) == true);
}

inline void FunctionalTest::foreachInTupleReverse(void) {
    elysian::InstanceTransform transform;
    lua_pushboolean(*_mainState, true);
    lua_pushnumber(*_mainState, -12.0f);
    OOLUA::push(*_mainState, &transform);

    std::tuple<bool, float, elysian::InstanceTransform*> tuple;

    foreach_in_tuple_reverse(tuple, [](auto&& element) {
        OOLUA::pull(*_mainState, element);
    });

    QVERIFY(std::get<0>(tuple) == true);
    QVERIFY(std::get<1>(tuple) == -12.0f);
    QVERIFY(std::get<2>(tuple) == &transform);

}

}


#endif // TEST_FUNCTIONAL_H

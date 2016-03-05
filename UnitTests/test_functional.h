#ifndef TEST_FUNCTIONAL_H
#define TEST_FUNCTIONAL_H

#include <QtTest>
#include <utility/elysian_functional.hpp>
#include <crtp/elysian_lua_manager.hpp>
#include <elysian_instance_transform.hpp>
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

    //QVERIFY(strcmp(forward_tuple_to_func_as_args(cFunc, args), "WHOOO!") == 0);
    QVERIFY(strcmp(forward_tuple(cFunc, args), "WHOOO!") == 0);
}

}


#endif // TEST_FUNCTIONAL_H

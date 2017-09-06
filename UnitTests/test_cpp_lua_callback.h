#ifndef TEST_CPP_LUA_CALLBACK_H
#define TEST_CPP_LUA_CALLBACK_H

#include <utility/elysian_cpp_lua_callback.hpp>
#include <utility/elysian_generic.hpp>
#include <QDebug>
#include <QtTest>

namespace tktest {

class CppLuaCallbackTest: public QObject {
    Q_OBJECT

public:
    CppLuaCallbackTest(void) = default;

private Q_SLOTS:

    void createEmptyIsNullTest();
    void createEmptyTypeTest();
    void createEmptyBoolTest();
    void createEmptyComparisonTest();

    void createCFunctionTypeTest();
    void createCFunctionBoolTest();
    void createCFunctionCallTest();

    void createLuaFunctionTypeTest();
    void createLuaFunctionBoolTest();
    void createLuaFunctionComparisonTest();
    void createLuaFunctionCallTest();


};

inline void CppLuaCallbackTest::createEmptyIsNullTest(void) {
    elysian::CppLuaCallback<void> cb;
    QVERIFY(cb.isNull());
}

inline void CppLuaCallbackTest::createEmptyTypeTest(void) {
    elysian::CppLuaCallback<void> cb;
    QVERIFY(cb.getType() == elysian::CppLuaCallback<void>::Type::Null);
}

inline void CppLuaCallbackTest::createEmptyBoolTest(void) {
    elysian::CppLuaCallback<void> cb;
    QVERIFY(!(bool)cb);
}

inline void CppLuaCallbackTest::createEmptyComparisonTest(void) {
    elysian::CppLuaCallback<void> cb;
    QVERIFY(cb == nullptr);
}


static int CFunction(int a, int b) {
    return a + b;
}


inline void CppLuaCallbackTest::createCFunctionTypeTest(void) {
    std::function<int(int, int)> func = &CFunction;
    elysian::CppLuaCallback<int, int, int> cb(func);

    QVERIFY(cb.getType() == elysian::CppLuaCallback<int ELYSIAN_COMMA int ELYSIAN_COMMA int>::Type::Cpp);
}

inline void CppLuaCallbackTest::createCFunctionBoolTest(void) {
    std::function<int(int, int)> func = &CFunction;
    elysian::CppLuaCallback<int, int, int> cb(func);
    QVERIFY((bool)cb);
}

inline void CppLuaCallbackTest::createCFunctionCallTest(void) {
    std::function<int(int, int)> func = &CFunction;
    elysian::CppLuaCallback<int, int, int> cb(func);
    int retVal = cb(3, 4);
    QVERIFY(retVal == 7);
}

inline void CppLuaCallbackTest::createLuaFunctionTypeTest(void) {
    std::function<int(int, int)> func = &CFunction;
    LuaVariant v(func);
    elysian::CppLuaCallback<int, int, int> cb(v);

    QVERIFY(cb.getType() == elysian::CppLuaCallback<int ELYSIAN_COMMA int ELYSIAN_COMMA int>::Type::Lua);
}

inline void CppLuaCallbackTest::createLuaFunctionBoolTest(void) {
    std::function<int(int, int)> func = &CFunction;
    LuaVariant v(func);
    elysian::CppLuaCallback<int, int, int> cb(v);
    QVERIFY((bool)cb);
}

inline void CppLuaCallbackTest::createLuaFunctionCallTest(void) {
    std::function<int(int, int)> func = &CFunction;
    LuaVariant v(func);
    elysian::CppLuaCallback<int, int, int> cb(v);
    int retVal = cb(3, 4);
    QVERIFY(retVal == 7);
}

inline void CppLuaCallbackTest::createLuaFunctionComparisonTest(void) {
    std::function<int(int, int)> func = &CFunction;
    LuaVariant v(func);
    elysian::CppLuaCallback<int, int, int> cb(v);
    QVERIFY(cb == v);
}



}

#endif // TEST_CPP_LUA_CALLBACK_H

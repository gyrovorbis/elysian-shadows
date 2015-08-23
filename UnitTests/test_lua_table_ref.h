#ifndef TEST_LUA_TABLE_REF
#define TEST_LUA_TABLE_REF


#include <QtTest>
#include <utility/elysian_lua_table_ref.hpp>
#include <QDebug>

namespace tktest {

class LuaTableRefTest: public QObject, public LuaManager {
    Q_OBJECT

private:

    elysian::LuaTableRef _ref;

    constexpr static const char*const _testTable = "return {"
                                                   "    1, 2, 3, 4,"
                                                   "    a = \"aa\", "
                                                   "    b = \"bb\", "
                                                   "    c = {"
                                                   "        1, 2, 3,"
                                                   "        d = \"dd\","
                                                   "        e = function() return 44 end"
                                                   "    }"
                                                   "}";

public:

private Q_SLOTS:

    void initTestCase(void);

    //void createNew(void);
    //void createFromReference(void);
    void defaultstoNilRef(void);
    void createFromString(void);
    //void getIntValueFromStringKey(void);
    //void getIntValueFromIntKey(void);


    //void getValueNil(void);
    //void getValueBool(void);
    //void getValueNumber(void);
    //void getValueString(void);


    //void arrayFromStack(void);
    //void arrayToStack(void);


    //void deserialize(void);
    //void serialize(void);


};


inline void LuaTableRefTest::initTestCase() {
    QVERIFY(LuaManager::initialize());
}

inline void LuaTableRefTest::defaultstoNilRef(void) {
    QVERIFY(_ref.isNilRef());
}

inline void LuaTableRefTest::createFromString() {
    _ref = elysian::LuaTableRef::createFromString(_testTable);
    QVERIFY(!_ref.isNilRef());
}



}

#endif // TEST_LUA_TABLE_REF


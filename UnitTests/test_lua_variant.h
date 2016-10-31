#ifndef TEST_LUA_VARIANT_H
#define TEST_LUA_VARIANT_H


#include <QtTest>
#include <crtp/elysian_lua_variant.hpp>
#include <proxy/elysian_instance_transform_proxy.hpp>
#include <QDebug>
#include <initializer_list>

namespace tktest {

//1 - convert to/from OOLUA::Lua_table_refc


class LuaVariantTest: public QObject, public LuaManager {
    Q_OBJECT

    constexpr static const char*const _luaFunc =
                                        "function addShit(a, b)\n"
                                        "     return a+b\n"
                                        "end\n"
                                        "return addShit";

private Q_SLOTS:

    void initTestCase(void);

    //void createNew(void);
    //void createFromReference(void);
    //constructors
    void createWithNilDefaultType(void);
    void createWithNullPtrType(void);
    void createWithBoolType(void);
    void createWithIntType(void);
    void createWithFloatType(void);
    void createWithDoubleType(void);
    void createWithStringType(void);
    void createWithVoidPtrType(void);
    void createWithUserdataType(void);
    void createFromStdInitializerList(void);
    void createFromPairsStdInitializerList(void);
    void createFromStack(void);
    void createFromArray(void);
    void createNewTable(void);
    //void createFromRef(void);

    //fetching values
    void isNil(void);
    void getBool(void);
    void getInt(void);
    void getFloat(void);
    void getDouble(void);
    void getString(void);
    void getVoidPtr(void);
    void getUserdata(void);

    void compareNullPtr(void);
    void compareVariant(void);
    void compareBool(void);
    void compareInt(void);
    void compareFloat(void);
    void compareDouble(void);
    void compareString(void);
    void compareVoidPtr(void);
    void compareUserdata(void);

    void setNil(void);
    void setVariant(void);
    void setBool(void);
    void setInt(void);
    void setFloat(void);
    void setDouble(void);
    void setString(void);
    void setVoidPtr(void);
    void setUserdata(void);

    void assignNullPtr(void);
    void assignBool(void);
    void assignInt(void);
    void assignFloat(void);
    void assignDouble(void);
    void assignString(void);
    void assignVoidPtr(void);
    void assignUserdata(void);

    void pushNil(void);
    void pullNil(void);
    void pushBool(void);
    void pullBool(void);
    void pushNumber(void);
    void pullNumber(void);
    void pushString(void);
    void pullString(void);
    void pushVoidPtr(void);
    void pullVoidPtr(void);
    void pushUserdata(void);
    void pullUserdata(void);
    void pushLuaFunction(void);
    void pullLuaFunction(void);

    //boolean conversions
    void nilAsBool(void);
    void boolAsBool(void);
    void intAsBool(void);
    void floatAsBool(void);
    void doubleAsBool(void);
    void stringAsBool(void);
    void voidPtrAsBool(void);

    //integer conversions
    void nilAsNumber(void);
    void boolAsNumber(void);
    void numberAsNumber(void);
    void stringAsNumber(void);
    void voidPtrAsNumber(void);

    //string conversions
    void nilAsString(void);
    void boolAsString(void);
    void numberAsString(void);
    void stringAsString(void);
    void voidPtrAsString(void);

    void voidPtrAsVoidPtr(void);
    void otherAsVoidPtr(void);

    //function operations
    void callLuaFunctionIntReturn(void);
    void callLuaFunctionVariadicArgsVoidReturn(void);
    void callCppLambdaClosure(void);

    void makeWeakRef(void);
    void getLength(void);

    //metatables
    //void setMetatable(void);
    //void getMetatable(void);
    //getTypeName() userdata
    //fromString()

    //table iteration
    void iterateTable(void);
    void iterateTableConst(void);
    void iterateTableCConst(void);
    void iterateTableRangedBasedFor(void);
    void iterateTableModifyFields(void);
};


inline void LuaVariantTest::initTestCase(void) {}


inline void LuaVariantTest::createWithNilDefaultType(void) {
    LuaVariant variant;
    QVERIFY(variant.getType() == LUA_TNIL);
}

inline void LuaVariantTest::createWithNullPtrType(void) {
    LuaVariant variant(nullptr);
    QVERIFY(variant.getType() == LUA_TNIL);
}

inline void LuaVariantTest::createWithBoolType(void) {
    LuaVariant variant(true);
    QVERIFY(variant.getType() == LUA_TBOOLEAN);
}

inline void LuaVariantTest::createWithIntType(void) {
    LuaVariant variant(31);
    QVERIFY(variant.getType() == LUA_TNUMBER);
}

inline void LuaVariantTest::createWithFloatType(void) {
    LuaVariant variant(33.0f);
    QVERIFY(variant.getType() == LUA_TNUMBER);
}

inline void LuaVariantTest::createWithDoubleType(void) {
    LuaVariant variant(33.0);
    QVERIFY(variant.getType() == LUA_TNUMBER);
}

inline void LuaVariantTest::createWithStringType(void) {
    LuaVariant variant("stringTest");
    QVERIFY(variant.getType() == LUA_TSTRING);
}

inline void LuaVariantTest::createWithVoidPtrType(void) {
    int i = 33;
    LuaVariant variant((void*)&i);
    QVERIFY(variant.getType() == LUA_TLIGHTUSERDATA);
}

inline void LuaVariantTest::createWithUserdataType(void) {
    elysian::InstanceTransform transform;
    LuaVariant variant(&transform);
    QVERIFY(variant.getType() == LUA_TUSERDATA);
}

inline void LuaVariantTest::createFromStdInitializerList(void) {
    LuaVariant variant({{1, 1}, {2, 2}, {3, 3}, {4, 4}, {5, 5}});
    QVERIFY(variant.getType() == LUA_TTABLE);
    QVERIFY(variant.getLength() == 5);
}

inline void LuaVariantTest::createFromPairsStdInitializerList(void) {
    elysian::Vector2 testUd;

    LuaVariant v = {
        {"field1", 3},
        {"field2", false},
        {1, "Testzy"},
        {"LOLOLOL", {
            { 0, 33.333f },
            {"tragler", true },
            {"twoLayersDeepTable", {
                {"randomAssUserdata", &testUd},
                {"randomAssLambdaClosure", [&](const float val) {
                     testUd.setX(val);
                     return val;
                 }}
             }}
        }},
        {"array", LuaVariant::fromArray({
            1,
            22.0f,
            "fuck",
            false,
            LuaManager::exec("return function(s) return '%s: '..s end", "DYNAMIC_ASS_STRING")
        })},
#if 0 //NEEDS TO BE FUCKING FIXED!!!!
        {"constCharArg", [&](char*string) {
            return string;
        }},
#endif
        { 1, "Weeeeell!!"},
        { 2, 8 },
        { &testUd, "Arbitrary userdata as an index... because I can!"},
        {"So fucking epic", false},
        {22.8, (void*)this}
    };

    QVERIFY(v.getType() == LUA_TTABLE);
    QVERIFY((*v["LOLOLOL"]["twoLayersDeepTable"]["randomAssLambdaClosure"])(-17.77f).getValue<float>() == -17.77f);
    QVERIFY(v["LOLOLOL"]["twoLayersDeepTable"]["randomAssUserdata"]["x"]->getValue<float>() == -17.77f);
    QVERIFY((*v["array"][5])("randomString") == "DYNAMIC_ASS_STRING: randomString");
#if 0
    QVERIFY((*v["constCharArg"])("textz0r") == "textz0r");
#endif
}


inline void LuaVariantTest::createFromStack(void) {
    lua_pushinteger(*_mainState, 3);
    QVERIFY(LuaVariant::fromStack(-1) == 3);
}

inline void LuaVariantTest::createFromArray(void) {
   elysian::Vector2 vex;

    LuaVariant v = LuaVariant::fromArray({
        1,
        false,
        2.2f,
        "fuckme",
        (void*)this,
        OOLUA::Lua_table_ref(),
        OOLUA::Lua_func_ref(),
        &vex,
        [&](const int value) {
            return value;
        },
        static_cast<lua_CFunction>([](lua_State*) {
            return 0;
        })
    });

    QVERIFY(v.getLength() == 10);

}

inline void LuaVariantTest::createNewTable(void) {
    LuaVariant variant = LuaVariant::newTable();
    QVERIFY(variant.getType() == LUA_TTABLE);
    QVERIFY(variant.getRef() != LUA_REFNIL);
    QVERIFY(variant.getRef() != LUA_NOREF);
}

inline void LuaVariantTest::isNil(void) {
    LuaVariant variant(nullptr);
    QVERIFY(variant.isNil());
}

inline void LuaVariantTest::getBool(void) {
    LuaVariant variant(true);
    QVERIFY(variant.getValue<bool>() == true);
}

inline void LuaVariantTest::getInt(void) {
    LuaVariant variant(3);
    QVERIFY(variant.getValue<int>() == 3);
}

inline void LuaVariantTest::getFloat(void) {
    LuaVariant variant(13.0f);
    QVERIFY(variant.getValue<float>() == 13.0f);
}

inline void LuaVariantTest::getDouble(void) {
    LuaVariant variant(14.0);
    QVERIFY(variant.getValue<double>() == 14.0);
}

inline void LuaVariantTest::getString(void) {
    LuaVariant variant("StringTest");
    QVERIFY((strcmp(variant.getValue<const char*>(), "StringTest") == 0));
}

inline void LuaVariantTest::getVoidPtr(void) {
    int i = 3;
    LuaVariant variant((void*)&i);
    QVERIFY(variant.getValue<void*>() == &i);
}

inline void LuaVariantTest::getUserdata(void) {
    elysian::InstanceTransform transform;
    LuaVariant variant(&transform);
    QVERIFY(variant.getValue<elysian::InstanceTransform*>() == &transform);
}

inline void LuaVariantTest::compareNullPtr(void) {
    QVERIFY(LuaVariant(nullptr) == nullptr);
}

inline void LuaVariantTest::compareVariant(void) {
    elysian::Vector2 vec;
    QVERIFY(LuaVariant(nullptr) == LuaVariant(nullptr));
    QVERIFY(LuaVariant(true) == LuaVariant(true));
    QVERIFY(LuaVariant(0) == LuaVariant(0));
    QVERIFY(LuaVariant(0.3f) == LuaVariant(0.3f));
    QVERIFY(LuaVariant(0.4123) == LuaVariant(0.4123));
    QVERIFY(LuaVariant("FuckYou") == LuaVariant("FuckYou"));
    QVERIFY(LuaVariant(&vec) == LuaVariant(&vec));
}

inline void LuaVariantTest::compareBool(void) {
    QVERIFY(LuaVariant(true) == true);
}

inline void LuaVariantTest::compareInt(void) {
    QVERIFY(LuaVariant(3) == 3);
}

inline void LuaVariantTest::compareFloat(void) {
    QVERIFY(LuaVariant(14.4f) == 14.4f);
}

inline void LuaVariantTest::compareDouble(void) {
    QVERIFY(LuaVariant(11.11) == 11.11);
}

inline void LuaVariantTest::compareString(void) {
    QVERIFY(LuaVariant("stringitator") == "stringitator");
}

inline void LuaVariantTest::compareVoidPtr(void) {
    int i = 33;
    QVERIFY(LuaVariant((void*)&i) == (void*)&i);
}

inline void LuaVariantTest::compareUserdata(void) {
    elysian::InstanceTransform transform;
    QVERIFY(LuaVariant(&transform) == &transform);
}

inline void LuaVariantTest::setNil(void) {
    LuaVariant variant1(4);
    LuaVariant variant2(4);
    variant1.setValue(nullptr);
    variant2.setNil();
    QVERIFY(variant1 == LuaVariant());
    QVERIFY(variant2 == LuaVariant());
    QVERIFY(variant1 == variant2);
}

inline void LuaVariantTest::setVariant(void) {
    elysian::Vector2 vec;
    LuaVariant nil;
    LuaVariant boolean(true);
    LuaVariant intNum(3);
    LuaVariant floatNum(3.33f);
    LuaVariant doubleNum(4.12345);
    LuaVariant str("fucktwats");
    LuaVariant userdata(&vec);

    LuaVariant v;
    v = nil;
    QVERIFY(v == nil);
    v = boolean;
    QVERIFY(v == boolean);
    v = intNum;
    QVERIFY(v == intNum);
    v = floatNum;
    QVERIFY(v == floatNum);
    v = doubleNum;
    QVERIFY(v == doubleNum);
    v = str;
    QVERIFY(v == str);
    v = userdata;
    QVERIFY(v == userdata);
}

inline void LuaVariantTest::setBool(void) {
    LuaVariant nil;
    nil.setValue(true);
    QVERIFY(nil == LuaVariant(true));
    nil.setValue(false);
    QVERIFY(nil == LuaVariant(false));
}

inline void LuaVariantTest::setInt(void) {
    LuaVariant v;
    v.setValue(44);
    QVERIFY(v == LuaVariant(44));
}

inline void LuaVariantTest::setFloat(void) {
    LuaVariant v;
    v.setValue(44.2f);
    QVERIFY(v == LuaVariant(44.2f));
}

inline void LuaVariantTest::setDouble(void) {
    LuaVariant v;
    v.setValue(44.12345);
    QVERIFY(v == LuaVariant(44.12345));
}

inline void LuaVariantTest::setString(void) {
    LuaVariant v;
    v.setValue("LOLSTR");
    QVERIFY(v == LuaVariant("LOLSTR"));
}

inline void LuaVariantTest::setVoidPtr(void) {
    LuaVariant v;
    v.setValue((void*)this);
    QVERIFY(v == LuaVariant((void*)this));
}

inline void LuaVariantTest::setUserdata(void) {
    elysian::InstanceTransform transform;
    LuaVariant v;
    v.setValue(&transform);
    QVERIFY(v == LuaVariant(&transform));
}


inline void LuaVariantTest::assignNullPtr(void) {
    LuaVariant variant(33.0f);
    variant = nullptr;
    QVERIFY(variant == nullptr);
}

inline void LuaVariantTest::assignBool(void) {
    LuaVariant variant;
    variant = true;
    QVERIFY(variant == true);
}

inline void LuaVariantTest::assignInt(void) {
    LuaVariant variant;
    variant = 12;
    QVERIFY(variant == 12);
}

inline void LuaVariantTest::assignFloat(void) {
    LuaVariant variant;
    variant = 44.0f;
    QVERIFY(variant == 44.0f);
}

inline void LuaVariantTest::assignDouble(void) {
    LuaVariant variant;
    variant = 11.11;
    QVERIFY(variant == 11.11);
}

inline void LuaVariantTest::assignString(void) {
    LuaVariant variant;
    variant = "Fuck You";
    QVERIFY(variant == "Fuck You");
}

inline void LuaVariantTest::assignVoidPtr(void) {
    int i = 3;
    LuaVariant variant;
    variant = (void*)&i;
    QVERIFY(variant == (void*)&i);
}

inline void LuaVariantTest::assignUserdata(void) {
    elysian::InstanceTransform transform;
    LuaVariant variant;
    variant = &transform;
    QVERIFY(variant == &transform);
}


inline void LuaVariantTest::pushNil(void) {
    LuaVariant variant;
    variant.push();
    const int luaType = lua_type(*_mainState, -1);
    lua_pop(*_mainState, 1);
    QVERIFY(luaType == LUA_TNIL);
}

inline void LuaVariantTest::pullNil(void) {
    LuaVariant variant;
    lua_pushnil(*_mainState);
    variant.pull();
    QVERIFY(variant.isNil());
}

inline void LuaVariantTest::pushBool(void) {
    LuaVariant variant(true);
    variant.push();
    const int luaType = lua_type(*_mainState, -1);
    const bool val = lua_toboolean(*_mainState, -1);
    lua_pop(*_mainState, 1);
    QVERIFY(luaType == LUA_TBOOLEAN);
    QVERIFY(val == true);
}

inline void LuaVariantTest::pullBool(void) {
    LuaVariant variant;
    lua_pushboolean(*_mainState, true);
    variant.pull();
    QVERIFY(variant.getType() == LUA_TBOOLEAN);
    QVERIFY(variant.getValue<bool>() == true);
}

inline void LuaVariantTest::pushNumber(void) {
    LuaVariant variant(33.0f);
    variant.push();
    const int luaType = lua_type(*_mainState, -1);
    const float val = lua_tonumber(*_mainState, -1);
    lua_pop(*_mainState, 1);
    QVERIFY(luaType == LUA_TNUMBER);
    QVERIFY(val == 33.0f);
}

inline void LuaVariantTest::pullNumber(void) {
    LuaVariant variant;
    lua_pushnumber(*_mainState, 12.0f);
    variant.pull();
    QVERIFY(variant.getType() == LUA_TNUMBER);
    QVERIFY(variant.getValue<float>() == 12.0f);
}

inline void LuaVariantTest::pushString(void) {
    LuaVariant variant("FUCK YOU!!!");
    variant.push();
    const int luaType = lua_type(*_mainState, -1);
    const char*const val = lua_tostring(*_mainState, -1);
    lua_pop(*_mainState, 1);
    QVERIFY(luaType == LUA_TSTRING);
    QVERIFY(strcmp(val, "FUCK YOU!!!") == 0);
}

inline void LuaVariantTest::pullString(void) {
    LuaVariant variant;
    lua_pushstring(*_mainState, "biiiitches!!!");
    variant.pull();
    QVERIFY(variant.getType() == LUA_TSTRING);
    QVERIFY(strcmp(variant.getValue<const char*>(), "biiiitches!!!") == 0);
}

inline void LuaVariantTest::pushVoidPtr(void) {
    int i = 22;
    LuaVariant variant((void*)&i);
    variant.push();
    const int luaType = lua_type(*_mainState, -1);
    void*const val = lua_touserdata(*_mainState, -1);
    lua_pop(*_mainState, 1);
    QVERIFY(luaType == LUA_TLIGHTUSERDATA);
    QVERIFY(val == &i);
}

inline void LuaVariantTest::pullVoidPtr(void) {
    int i = 22;
    LuaVariant variant;
    lua_pushlightuserdata(*_mainState, &i);
    variant.pull();
    QVERIFY(variant.getType() == LUA_TLIGHTUSERDATA);
    QVERIFY(variant.getValue<void*>() == &i);
}

inline void LuaVariantTest::pushUserdata(void) {
    elysian::InstanceTransform transform;
    LuaVariant variant(&transform);
    variant.push();
    const int luaType = lua_type(*_mainState, -1);
    elysian::InstanceTransform* ptr = nullptr;
    OOLUA::pull(*_mainState, ptr);
    QVERIFY(luaType == LUA_TUSERDATA);
    QVERIFY(ptr == &transform);
}

inline void LuaVariantTest::pullUserdata(void) {
    elysian::InstanceTransform transform;
    LuaVariant variant;
    OOLUA::push(*_mainState, &transform);
    variant.pull();
    QVERIFY(variant.getType() == LUA_TUSERDATA);
    QVERIFY(variant.getValue<elysian::InstanceTransform*>() == &transform);
}

inline void LuaVariantTest::pullLuaFunction(void) {
    LuaVariant variant;
    QVERIFY(LuaManager::runString(_luaFunc));
    QVERIFY(variant.pull());
    QVERIFY(variant.getType() == LUA_TFUNCTION);
}

inline void LuaVariantTest::pushLuaFunction(void) {
    LuaManager::pushGlobalField("print");
    int ref = luaL_ref(*_mainState, LUA_REGISTRYINDEX);
    QVERIFY(ref != LUA_REFNIL);
    QVERIFY(ref != LUA_NOREF);
    LuaVariant variant;
    variant.setRef(ref);
    QVERIFY(variant.push());
    QVERIFY(lua_isfunction(*_mainState, -1));
    lua_rawgeti(*_mainState, LUA_REGISTRYINDEX, ref);
    QVERIFY(lua_compare(*_mainState, -1, -2, LUA_OPEQ));
    lua_pop(*_mainState, 2);
    luaL_unref(*_mainState, LUA_REGISTRYINDEX, ref);
}


inline void LuaVariantTest::nilAsBool(void) {
    LuaVariant v;
    QVERIFY(v.asBool() == false);
}

inline void LuaVariantTest::boolAsBool(void) {
    LuaVariant v(false);
    QVERIFY(v.asBool() == false);
    v = true;
    QVERIFY(v.asBool() == true);
}

inline void LuaVariantTest::intAsBool(void) {
    LuaVariant v(0);
    QVERIFY(v.asBool() == false);
    v = 1;
    QVERIFY(v.asBool() == true);
    v = 2;
    QVERIFY(v.asBool() == true);

}

inline void LuaVariantTest::floatAsBool(void) {
    LuaVariant v(0.0f);
    QVERIFY(v.asBool() == false);
    v = 1.0f;
    QVERIFY(v.asBool() == true);
    v = 2.0f;
    QVERIFY(v.asBool() == true);
}

inline void LuaVariantTest::doubleAsBool(void) {
    LuaVariant v(0.0);
    QVERIFY(v.asBool() == false);
    v = 1.0;
    QVERIFY(v.asBool() == true);
    v = 2.0;
    QVERIFY(v.asBool() == true);
}

inline void LuaVariantTest::stringAsBool(void) {
    LuaVariant v("false");
    QVERIFY(v.asBool() == false);
    v = "true";
    QVERIFY(v.asBool() == true);
    v = "fuckface";
    QVERIFY(v.asBool() == false);
}

inline void LuaVariantTest::voidPtrAsBool(void) {
    LuaVariant v((void*)this);
    QVERIFY(v.asBool() == true);

    v = (void*)nullptr;
    QVERIFY(v.asBool() == false);
}

inline void LuaVariantTest::nilAsNumber(void) {
    LuaVariant v;
    QVERIFY(v.asNumber() == 0);
}

inline void LuaVariantTest::boolAsNumber(void) {
    LuaVariant v(false);
    QVERIFY(v.asNumber() == 0);
    v = true;
    QVERIFY(v.asNumber() == 1);
}

inline void LuaVariantTest::numberAsNumber(void) {
    LuaVariant v(3.31f);
    QVERIFY(v.asNumber() == 3.31f);
    v = 14;
    QVERIFY(v.asNumber() == 14.0f);
}

inline void LuaVariantTest::stringAsNumber(void) {
    LuaVariant v("3.2");
    QVERIFY(v.asNumber() == 3.2f);
    v = "12";
    QVERIFY(v.asNumber() == 12.0f);
}

inline void LuaVariantTest::voidPtrAsNumber(void) {
    LuaVariant v((void*)this);
    QVERIFY(v.asNumber() == 1.0f);
    v = (void*)nullptr;
    QVERIFY(v.asNumber() == 0.0f);
}

inline void LuaVariantTest::nilAsString(void) {
    LuaVariant v;
    QVERIFY(v.asString() == "nil");
}

inline void LuaVariantTest::boolAsString(void) {
    LuaVariant v(false);
    QVERIFY(v.asString() == "false");
    v = true;
    QVERIFY(v.asString() == "true");
}

inline void LuaVariantTest::numberAsString(void) {
    LuaVariant v(12.4f);
    QVERIFY(QVariant(v.asString().getValue<const char*>()).value<float>() == 12.4f); //helps with inaccuracies and different precisions of strings!!!
    v = 46;
    QVERIFY(v.asString() == "46");
}

inline void LuaVariantTest::stringAsString(void) {
    LuaVariant v("Jesus");
    QVERIFY(v.asString() == "Jesus");
}

inline void LuaVariantTest::voidPtrAsString(void) {
    LuaVariant v((void*)this);

    v.push();

    QVERIFY(strcmp(v.asString().getValue<const char*>(), luaL_tolstring(*_mainState, -1, nullptr)) == 0);

    lua_pop(*_mainState, 2);
}

inline void LuaVariantTest::voidPtrAsVoidPtr(void) {
    LuaVariant v((void*)this);
    QVERIFY(v.asUserdata() == (void*)this);
}

inline void LuaVariantTest::otherAsVoidPtr(void) {
    LuaVariant v;
    QVERIFY(v.asUserdata() == nullptr);
}

inline void LuaVariantTest::callCppLambdaClosure(void) {
    int a = 3;
    LuaVariant v;

    v.setValue(std::function<int(int)>([=](int b) {
        return a*b;
    }));

    int val = v(3).getValue<int>();
    QVERIFY(val == 9);
}

inline void LuaVariantTest::makeWeakRef(void) {
    elysian::InstanceTransform transform;
    LuaVariant v1 = &transform;
    LuaVariant v2 = LuaVariant::makeWeakRef(v1);

    QVERIFY(v1 == v2);
    QVERIFY(v1.getValue<elysian::InstanceTransform*>() == v2.getValue<elysian::InstanceTransform*>());
    QVERIFY(v1.getRef() == v2.getRef());
}

inline void LuaVariantTest::getLength(void) {
    LuaVariant v1 = LuaVariant::fromArray( {1, 2, 3, 4, 5 });
    LuaVariant v2 = {
        { 1, 1 },
        { 2, 2 },
        { 3, 3 },
        { 6, 6 },
        {"LOLOL", "TEST" }
    };

    QVERIFY(v1.getLength() == 5);
    QVERIFY(v2.getLength() == 3);
}

inline void LuaVariantTest::callLuaFunctionIntReturn(void) {
    LuaVariant variant;
    QVERIFY(LuaManager::runString(_luaFunc));
    QVERIFY(variant.pull());
    int i = variant(3, 4).getValue<int>();
    QVERIFY(i == 7);
}


inline void LuaVariantTest::callLuaFunctionVariadicArgsVoidReturn(void) {
    LuaVariant v;
    LuaManager::pushGlobalField("print");
    QVERIFY(v.pull());
    QVERIFY(v("FUCK", "YOU", 3, 45.0f, "LOLZ").isNil());
}


inline void LuaVariantTest::iterateTable(void) {
    LuaVariant v({{1,1}, {2,2}, {3,3}, {4,4}, {5,5}});
    int startVal = 1;
    int values = 0;

    for(auto it = v.begin(); it != v.end(); ++it) {
        QVERIFY(it.first() == values+1);
        QVERIFY(it.second() == startVal++);
        ++values;
    }
    QVERIFY(values == v.getLength());

}

inline void LuaVariantTest::iterateTableConst(void) {
    LuaVariant o;
    const LuaVariant v({{1,1}});
    int startVal = 1;
    int values = 0;

    for(auto it = v.begin(); it != v.end(); it++) {
        QVERIFY(it.first() == values+1);
        it.first().push();
        o.pull();
        QVERIFY(it.second() == startVal++);
        ++values;
    }
    QVERIFY(values == v.getLength());


}

inline void LuaVariantTest::iterateTableCConst(void) {
    const LuaVariant v({{1,1}, {2,2}, {3,3}, {4,4}, {5,5}});
    int startVal = 1;
    int values = 0;

    for(auto it = v.cbegin(); it != v.cend(); ++it) {
        QVERIFY(it.first() == values+1);
        QVERIFY(it.second() == startVal++);
        ++values;
    }
    QVERIFY(values == v.getLength());
}

inline void LuaVariantTest::iterateTableRangedBasedFor(void) {
    std::initializer_list<elysian::LuaPair> array = { {1,2}, {2,3}, {3,5}, {4,7}, {5,8}, {6,10}, {7,12}, {8,33}};
    const LuaVariant v(array);
    size_t values = 0;

    auto listIt = array.begin();
    for(auto it : v) {
        QVERIFY(*it == (*listIt).second);
        ++values;
        ++listIt;
    }
    QVERIFY(values == array.size());
}

inline void LuaVariantTest::iterateTableModifyFields(void) {
    LuaVariant v({{1,1}, {2,2}, {3,3}, {4,4}, {5,5}});
    std::initializer_list<elysian::LuaPair> array = { {1,2}, {2,3}, {3,5}, {4,7}, {5,8}};

    auto listIt = array.begin();
    for(auto it = v.begin(); it != v.end(); ++it) {
        it->setVariant((*listIt).second);
        ++listIt;
    }

    listIt = array.begin();
    for(auto it : v) {
        QVERIFY(*it == (*listIt).second);
        ++listIt;
    }

}

}


#endif // TEST_LUA_VARIANT_H


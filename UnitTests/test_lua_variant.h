#ifndef TEST_LUA_VARIANT_H
#define TEST_LUA_VARIANT_H


#include <QtTest>
#include <utility/elysian_lua_variant.hpp>
#include <QDebug>

namespace tktest {

class LuaVariantTest: public QObject, public LuaManager {
    Q_OBJECT


    typedef elysian::LuaVariant LuaVariant;

public:

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
    void createFromStack(void);

    //fetching values
    void isNil(void);
    void getBool(void);
    void getInt(void);
    void getFloat(void);
    void getDouble(void);
    void getString(void);
    void getVoidPtr(void);

    void compareNullPtr(void);
    void compareVariant(void);
    void compareBool(void);
    void compareInt(void);
    void compareFloat(void);
    void compareDouble(void);
    void compareString(void);
    void compareVoidPtr(void);

    void setNil(void);
    void setVariant(void);
    void setBool(void);
    void setInt(void);
    void setFloat(void);
    void setDouble(void);
    void setString(void);
    void setVoidPtr(void);

    void assignNullPtr(void);
    void assignBool(void);
    void assignInt(void);
    void assignFloat(void);
    void assignDouble(void);
    void assignString(void);
    void assignVoidPtr(void);

    void pushNil(void);
    void popNil(void);
    void pushBool(void);
    void popBool(void);
    void pushNumber(void);
    void popNumber(void);
    void pushString(void);
    void popString(void);
    void pushUserdata(void);
    void popUserdata(void);

    //boolean conversions
    void nilToBool(void);
    void boolToBool(void);
    void intToBool(void);
    void floatToBool(void);
    void doubleToBool(void);
    void stringToBool(void);
    void voidPtrToBool(void);

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

    void closure(void);
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
    LuaVariant variant(&i);
    QVERIFY(variant.getType() == LUA_TLIGHTUSERDATA);
}

inline void LuaVariantTest::createFromStack(void) {
    lua_pushinteger(*_mainState, 3);
    QVERIFY(LuaVariant::fromStack(-1) == 3);
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
    LuaVariant variant(&i);
    QVERIFY(variant.getValue<void*>() == &i);
}

inline void LuaVariantTest::compareNullPtr(void) {
    QVERIFY(LuaVariant(nullptr) == nullptr);
}

inline void LuaVariantTest::compareVariant(void) {
    QVERIFY(LuaVariant(nullptr) == LuaVariant(nullptr));
    QVERIFY(LuaVariant(true) == LuaVariant(true));
    QVERIFY(LuaVariant(0) == LuaVariant(0));
    QVERIFY(LuaVariant(0.3f) == LuaVariant(0.3f));
    QVERIFY(LuaVariant(0.4123) == LuaVariant(0.4123));
    QVERIFY(LuaVariant("FuckYou") == LuaVariant("FuckYou"));
    QVERIFY(LuaVariant(this) == LuaVariant(this));
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
    QVERIFY(LuaVariant(&i) == &i);
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
    LuaVariant nil;
    LuaVariant boolean(true);
    LuaVariant intNum(3);
    LuaVariant floatNum(3.33f);
    LuaVariant doubleNum(4.12345);
    LuaVariant str("fucktwats");
    LuaVariant userdata(this);

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
    v.setValue(this);
    QVERIFY(v == LuaVariant(this));
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
    variant = &i;
    QVERIFY(variant == &i);
}


inline void LuaVariantTest::pushNil(void) {
    LuaVariant variant;
    variant.push();
    const int luaType = lua_type(*_mainState, -1);
    lua_pop(*_mainState, 1);
    QVERIFY(luaType == LUA_TNIL);
}

inline void LuaVariantTest::popNil(void) {
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

inline void LuaVariantTest::popBool(void) {
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

inline void LuaVariantTest::popNumber(void) {
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

inline void LuaVariantTest::popString(void) {
    LuaVariant variant;
    lua_pushstring(*_mainState, "biiiitches!!!");
    variant.pull();
    QVERIFY(variant.getType() == LUA_TSTRING);
    QVERIFY(strcmp(variant.getValue<const char*>(), "biiiitches!!!") == 0);
}

inline void LuaVariantTest::pushUserdata(void) {
    int i = 22;
    LuaVariant variant(&i);
    variant.push();
    const int luaType = lua_type(*_mainState, -1);
    void*const val = lua_touserdata(*_mainState, -1);
    lua_pop(*_mainState, 1);
    QVERIFY(luaType == LUA_TLIGHTUSERDATA);
    QVERIFY(val == &i);
}

inline void LuaVariantTest::popUserdata(void) {
    int i = 22;
    LuaVariant variant;
    lua_pushlightuserdata(*_mainState, &i);
    variant.pull();
    QVERIFY(variant.getType() == LUA_TLIGHTUSERDATA);
    QVERIFY(variant.getValue<void*>() == &i);
}




inline void LuaVariantTest::nilToBool(void) {
    LuaVariant v;
    QVERIFY(v.asBool() == false);
    QVERIFY(v.toBool() == false);
    QVERIFY((bool)v == false);
}

inline void LuaVariantTest::boolToBool(void) {
    LuaVariant v(false);
    QVERIFY(v.asBool() == false);
    QVERIFY(v.toBool() == false);
    QVERIFY((bool)v == false);
    v = true;
    QVERIFY(v.asBool() == true);
    QVERIFY(v.toBool() == true);
    QVERIFY((bool)v == true);
}

inline void LuaVariantTest::intToBool(void) {
    LuaVariant v(0);
    QVERIFY(v.asBool() == false);
    QVERIFY(v.toBool() == false);
    QVERIFY((bool)v == false);
    v = 1;
    QVERIFY(v.asBool() == true);
    QVERIFY(v.toBool() == true);
    QVERIFY((bool)v == true);
    v = 2;
    QVERIFY(v.asBool() == true);
    QVERIFY(v.toBool() == true);
    QVERIFY((bool)v == true);

}

inline void LuaVariantTest::floatToBool(void) {
    LuaVariant v(0.0f);
    QVERIFY(v.asBool() == false);
    QVERIFY(v.toBool() == false);
    QVERIFY((bool)v == false);
    v = 1.0f;
    QVERIFY(v.asBool() == true);
    QVERIFY(v.toBool() == true);
    QVERIFY((bool)v == true);
    v = 2.0f;
    QVERIFY(v.asBool() == true);
    QVERIFY(v.toBool() == true);
    QVERIFY((bool)v == true);
}

inline void LuaVariantTest::doubleToBool(void) {
    LuaVariant v(0.0);
    QVERIFY(v.asBool() == false);
    QVERIFY(v.toBool() == false);
    QVERIFY((bool)v == false);
    v = 1.0;
    QVERIFY(v.asBool() == true);
    QVERIFY(v.toBool() == true);
    QVERIFY((bool)v == true);
    v = 2.0;
    QVERIFY(v.asBool() == true);
    QVERIFY(v.toBool() == true);
    QVERIFY((bool)v == true);
}

inline void LuaVariantTest::stringToBool(void) {
    LuaVariant v("false");
    QVERIFY(v.asBool() == false);
    QVERIFY(v.toBool() == false);
    QVERIFY((bool)v == false);
    v = "true";
    QVERIFY(v.asBool() == true);
    QVERIFY(v.toBool() == true);
    QVERIFY((bool)v == true);
    v = "fuckface";
    QVERIFY(v.asBool() == false);
    QVERIFY(v.toBool() == false);
    QVERIFY((bool)v == false);
}

inline void LuaVariantTest::voidPtrToBool(void) {
    LuaVariant v(this);
    QVERIFY(v.asBool() == true);
    QVERIFY(v.toBool() == true);
    QVERIFY((bool)v == true);

    v = (void*)nullptr;
    QVERIFY(v.asBool() == false);
    QVERIFY(v.toBool() == false);
    QVERIFY((bool)v == false);
}

inline void LuaVariantTest::nilAsNumber(void) {
    LuaVariant v;
    QVERIFY(v.asNumber() == 0);
    QVERIFY(v.toNumber() == 0);
    QVERIFY((int)v == 0);
    QVERIFY((float)v == 0.0f);
    QVERIFY((double)v == 0.0);
}

inline void LuaVariantTest::boolAsNumber(void) {
    LuaVariant v(false);
    QVERIFY(v.asNumber() == 0);
    QVERIFY(v.toNumber() == 0);
    QVERIFY((int)v == 0);
    QVERIFY((float)v == 0.0f);
    QVERIFY((double)v == 0.0);
    v = true;
    QVERIFY(v.asNumber() == 1);
    QVERIFY(v.toNumber() == 1);
    QVERIFY((int)v == 1);
    QVERIFY((float)v == 1.0f);
    QVERIFY((double)v == 1.0);
}

inline void LuaVariantTest::numberAsNumber(void) {
    LuaVariant v(3.31f);
    QVERIFY(v.asNumber() == 3.31f);
    QVERIFY(v.toNumber() == 3.31f);
    QVERIFY((int)v == 3);
    QVERIFY((float)v == 3.31f);
    QVERIFY((double)v == 3.31f);
    v = 14;
    QVERIFY(v.asNumber() == 14.0f);
    QVERIFY(v.toNumber() == 14.0);
    QVERIFY((int)v == 14);
    QVERIFY((float)v == 14.0f);
    QVERIFY((double)v == 14.0);
}

inline void LuaVariantTest::stringAsNumber(void) {
    LuaVariant v("3.2");
    QVERIFY(v.asNumber() == 3.2f);
    QVERIFY(v.toNumber() == 3.2f);
    QVERIFY((int)v == 3);
    QVERIFY((float)v == 3.2f);
    QVERIFY((double)v == 3.2f);
    v = "12";
    QVERIFY(v.asNumber() == 12.0f);
    QVERIFY(v.toNumber() == 12.0);
    QVERIFY((int)v == 12);
    QVERIFY((float)v == 12.0f);
    QVERIFY((double)v == 12.0);
}

inline void LuaVariantTest::voidPtrAsNumber(void) {
    LuaVariant v(this);
    QVERIFY(v.asNumber() == 1.0f);
    QVERIFY(v.toNumber() == 1.0);
    QVERIFY((int)v == 1);
    QVERIFY((float)v == 1.0f);
    QVERIFY((double)v == 1.0);
    v = (void*)nullptr;
    QVERIFY(v.asNumber() == 0.0f);
    QVERIFY(v.toNumber() == 0);
    QVERIFY((int)v == 0);
    QVERIFY((float)v == 0.0f);
    QVERIFY((double)v == 0.0);
}

inline void LuaVariantTest::nilAsString(void) {
    LuaVariant v;
    QVERIFY(v.asString() == "nil");
    QVERIFY(strcmp(v.toString(), "nil") == 0);
    QVERIFY(strcmp((const char*)v, "nil") == 0);
}

inline void LuaVariantTest::boolAsString(void) {
    LuaVariant v(false);
    QVERIFY(v.asString() == "false");
    QVERIFY(strcmp(v.toString(), "false") == 0);
    QVERIFY(strcmp((const char*)v, "false") == 0);
    v = true;
    QVERIFY(v.asString() == "true");
    QVERIFY(strcmp(v.toString(), "true") == 0);
    QVERIFY(strcmp((const char*)v, "true") == 0);
}

inline void LuaVariantTest::numberAsString(void) {
    LuaVariant v(12.4f);
    QVERIFY(QVariant(v.asString().getValue<const char*>()).value<float>() == 12.4f); //helps with inaccuracies and different precisions of strings!!!
    QVERIFY(strcmp((const char*)v, "number") == 0);
    QVERIFY(QVariant(v.toString()).value<float>() == 12.4f);
    v = 46;
    QVERIFY(v.asString() == "46");
    QVERIFY(strcmp((const char*)v, "number") == 0);
    QVERIFY(strcmp(v.toString(), "46") == 0);
    QVERIFY(strcmp((const char*)v, "46") == 0);
}

inline void LuaVariantTest::stringAsString(void) {
    LuaVariant v("Jesus");
    QVERIFY(v.asString() == "Jesus");
    QVERIFY(strcmp(v.toString(), "Jesus") == 0);
    QVERIFY(strcmp((const char*)v, "Jesus") == 0);
}

inline void LuaVariantTest::voidPtrAsString(void) {
    LuaVariant v(this);
    //QVERIFY(v.asString() == "userdata");
    //QVERIFY(strcmp(v.toString(), "userdata") == 0);
    QVERIFY(strcmp((const char*)v, "userdata") == 0);
    v = (void*)nullptr;
    //QVERIFY(v.asString() == "userdata");
    //QVERIFY(strcmp(v.toString(), "userdata") == 0);
    QVERIFY(strcmp((const char*)v, "nil") == 0);

}

inline void LuaVariantTest::voidPtrAsVoidPtr(void) {
    LuaVariant v(this);
    QVERIFY(v.asUserdata() == (void*)this);
    QVERIFY(v.toUserdata() == (void*)this);
    QVERIFY((void*)v == (void*)this);
}

inline void LuaVariantTest::otherAsVoidPtr(void) {
    LuaVariant v;
    QVERIFY(v.asUserdata() == nullptr);
    QVERIFY(v.toUserdata() == nullptr);
    QVERIFY((void*)v == nullptr);
}

inline void LuaVariantTest::closure(void) {
    int a = 3;
    LuaVariant v;
#if 0
    v.setClosure<int>([=](int b) {
        qWarning() << "HOLY FUCKING SHIT! " << a << b;
        return 2;
    });

    int val = v<int, int>(38);
    qWarning() << "RETURN VALUE - " << val;
#endif
}


}


#endif // TEST_LUA_VARIANT_H


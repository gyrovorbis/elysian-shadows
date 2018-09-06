#ifndef TEST_DIALOG_H
#define TEST_DIALOG_H

#include <QtTest>
#include <QDebug>
#include <ESToolkit/AssetIO/tk_dialog.h>
#include <AssetIO/crtp/elysian_lua_variant.hpp>
#include <AssetIO/proxy/elysian_entity_proxy.hpp>
#include <AssetIO/proxy/elysian_dialog_proxy.hpp>

namespace tktest {

class DialogLuaTest: public QObject, public LuaManager {
    Q_OBJECT

private Q_SLOTS:

    //void initTestCase(void);

    //void createNew(void);
    //void createFromReference(void);

    //void create(void);
    void screenTextProperty(void);
    //void screenTriggerProperty(void);
    //void screenSourceProperty(void);
};


inline void DialogLuaTest::screenTextProperty(void) {
    DialogText text;
    elysian::LuaVariant reff(&text);
#if 0
    lua_pushnumber(*_mainState, 33.0f);
    int ref = luaL_ref(*_mainState, LUA_REGISTRYINDEX);
    lua_rawgeti(*_mainState, LUA_REGISTRYINDEX, ref);
    elysian::LuaVariant v;
    v.pull();
    QVERIFY(v == 33.0f);

    lua_pushstring(*_mainState, "FUUUUUCK");
    ref = luaL_ref(*_mainState, LUA_REGISTRYINDEX);
    lua_rawgeti(*_mainState, LUA_REGISTRYINDEX, ref);
    v.pull();
    QVERIFY(v == "FUUUUUCK");

    LuaManager::runString("return { a = 3, b = \"FUCKFUCK\" }");
    QVERIFY(v.pull());
    QVERIFY(v.push());
    lua_getfield(*_mainState, -1, "a");
    ref = luaL_ref(*_mainState, LUA_REGISTRYINDEX);
    lua_rawgeti(*_mainState, LUA_REGISTRYINDEX, ref);
    elysian::LuaVariant w;
    w.pull();
    QVERIFY(w == 3);

    lua_pushnumber(*_mainState, 2);
    lua_rawseti(*_mainState, LUA_REGISTRYINDEX, ref);
    lua_rawgeti(*_mainState, LUA_REGISTRYINDEX, ref);
    w.pull();
    qWarning() << "W " << w.toString();



    v.push();
    lua_getfield(*_mainState, -1, "a");
    w.pull();
    qWarning() << " VAL " << w.toString();
#endif

    reff["setText"].getVariant()(&text, "TESTFUCKFUCK!");


    QVERIFY(reff["getText"].getVariant()(&text) == "TESTFUCKFUCK!");
    QVERIFY(*reff["text"] == "TESTFUCKFUCK!");

    QVERIFY((*reff["getText"])(&text) == "TESTFUCKFUCK!");
    QVERIFY(*reff["text"] == "TESTFUCKFUCK!");
}
/*
void DialogLuaTest::screenTriggerProperty(void) {

}

void DialogLuaTest::screenSourceProperty(void) {

}*/

}

#endif // TEST_DIALOG_H


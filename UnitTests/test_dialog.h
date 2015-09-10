#ifndef TEST_DIALOG_H
#define TEST_DIALOG_H

#include <QtTest>
#include <QDebug>
#include <asset/tk_dialog.h>
#include <utility/elysian_lua_variant.hpp>
#include <proxy/elysian_entity_proxy.hpp>
#include <proxy/elysian_primitive_proxy.hpp>
#include <proxy/elysian_collidable_proxy.hpp>
#include <proxy/elysian_event_monitor_proxy.hpp>
#include <proxy/elysian_renderable_proxy.hpp>
#include <proxy/elysian_animation_proxy.hpp>
#include <proxy/elysian_warp_proxy.hpp>
#include <proxy/elysian_rigid_body_proxy.hpp>
#include <proxy/elysian_character_proxy.hpp>
#include <proxy/elysian_audio_source_proxy.hpp>
#include <proxy/elysian_light_source_proxy.hpp>
#include <proxy/elysian_examinable_proxy.hpp>
#include <proxy/elysian_particle_emitter_proxy.hpp>
#include <proxy/elysian_combat_proxy.hpp>
#include <proxy/elysian_skeleton_proxy.hpp>
#include <proxy/elysian_lua_behavior_proxy.hpp>
#include <proxy/elysian_dialog_proxy.hpp>

#if 0
ELYSIAN_LUA_PROXY_FIELDS_BEGIN()
    ELYSIAN_LUA_PROXY_FIELD(text,       getText,    setText)
    ELYSIAN_LUA_PROXY_FIELD(trigger,    isTrigger,  setTrigger)
    ELYSIAN_LUA_PROXY_FIELD(srcEntity,  getEntity,  addSourceEntity)
ELYSIAN_LUA_PROXY_FIELDS_END()
ELYSIAN_LUA_PROXY_FIELDS_BEGIN()
    ELYSIAN_LUA_PROXY_FIELD(nextIndex,  getNextIndex,   setNextIndex)
ELYSIAN_LUA_PROXY_FIELDS_END()
ELYSIAN_LUA_PROXY_FIELDS_BEGIN()
    ELYSIAN_LUA_PROXY_FIELD(text,       getText,        setText)
    ELYSIAN_LUA_PROXY_FIELD(nextIndex,  getNextIndex,   setNextIndex)
ELYSIAN_LUA_PROXY_FIELDS_END()
ELYSIAN_LUA_PROXY_FIELDS_BEGIN()
    ELYSIAN_LUA_PROXY_ARRAY_FIELD(options,      getOption,          setOption,          getOptionCount)
ELYSIAN_LUA_PROXY_FIELDS_END()
ELYSIAN_LUA_PROXY_FIELDS_BEGIN()
    ELYSIAN_LUA_PROXY_FIELD(startScreen,        getStartScreen,     setStartScreen)
    ELYSIAN_LUA_PROXY_FIELD(triggerEntity,      getTriggerEntity,   setTriggerEntity)
    ELYSIAN_LUA_PROXY_ARRAY_FIELD(screens,      getScreen,          setScreen,          getScreenCount)
ELYSIAN_LUA_PROXY_FIELDS_END()
#endif

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

    reff["setText"].getValue()(&text, "TESTFUCKFUCK!");


    QVERIFY(reff["getText"].getValue()(&text) == "TESTFUCKFUCK!");
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


#ifndef ELYSIAN_LUA_WRAPPERS_HPP
#define ELYSIAN_LUA_WRAPPERS_HPP

#include <oolua.h>
#include <utility/elysian_event_monitor.hpp>
#include <system/elysian_gui_system_base.hpp>
#include <lua/elysian_lua_singleton.hpp>
#include <crtp/elysian_collidable.hpp>
#include <crtp/elysian_entity.hpp>
#include <crtp/elysian_debug.hpp>
#include <level/elysian_tile.hpp>
#include <proxy/elysian_entity_proxy.hpp>


/* IT'S NOT SAFE TO GO ALONE! TAKE THIS HEADER FILE. IT WILL HELP
 * YOU ON YOUR JOURNEY!
 *
 * I have taken special care to create a SINGLE header file serving
 * as a registry for all Gamma/libGyro functionality exposed to Lua.
 * I have tried to keep this as clean as possible so that it can be
 * used as an index without scaring scripters away. This is everything
 * you need. It is organized as follows:
 * 1) C and C++ Class and Struct Wrappers
 * 2) C and C++ Constant and Enumeration Wrappers
 * 3) C Global and C++ Static Member Function Wrappers
 *
 *
 * Now go forth unto the world with the power of Lua, my children.
 *				--Falco
 */


// ====== C/++ CLASS/STRUCT WRAPPERS ======

namespace OOLUA
{
    typedef elysian::Engine     Engine;
}




OOLUA_PROXY(Engine)
	OOLUA_TAGS (
		No_public_constructors,
		No_public_destructor
	)
OOLUA_PROXY_END

//dummy class for encapsulating libGyro within its own namespace
class gyro {};
OOLUA_PROXY(gyro)
	OOLUA_TAGS (
		No_public_constructors,
		No_public_destructor
	)
OOLUA_PROXY_END



// ======= C GLOBAL AND C++ STATIC FUNCTION WRAPPERS ======

static inline int dbgPush(lua_State* const /*l*/) {
    elysian::Debug::push();
    return 0;
}

static inline int dbgPop(lua_State* const /*l*/) {
    elysian::Debug::pop();
    return 0;
}

static inline const char* luaGetVariableString(lua_State* const l) {
    static char string[GYRO_PRINTF_BUFFER_SIZE] = {0};
    unsigned n = lua_gettop(l);
    const char *curString;

    string[0] = 0;
    for(unsigned i = 1; i <= n; ++i) {

        int t = lua_type(l, i);

        switch(t) {
            case LUA_TSTRING:
            case LUA_TNUMBER:
            //case LUA_TBOOLEAN:
                curString = lua_tostring(l, i);
                strcat(string, curString);
                break;
            case LUA_TBOOLEAN:
                curString = (lua_toboolean(l, i))? "true" : "false";
                strcat(string, curString);
                break;
            default: {
                elysian::LuaVariant v = elysian::LuaVariant::fromStack(i, false);
                //curString = lua_typename(l, t);
                strcat(string, v.asString().getValue<const char*>());
            }
        }

        strcat(string, " ");

    }

    return string;

}

static inline int print(lua_State* const l) {


    elysian::LuaManager::luaPrint(luaGetVariableString(l));

    return 0;
}

static inline int printAdvLog(lua_State* const l) {
    elysian::Singletons::getGuiSys()->adventureLog.addLine(elysian::AdventureLog::TEXT_NORMAL, luaGetVariableString(l));
    return 0;
}

static inline int Singletons_getLevelSystem(lua_State* const vm) {
	OOLUA_C_FUNCTION(elysian::LevelSystem*, elysian::Singletons::getLevelSys)
}

static inline int Singletons_getCurArea(lua_State* const vm) {
    OOLUA_C_FUNCTION(elysian::Area*, elysian::Singletons::getCurrentArea)
}

static inline int Singletons_getCurLevel(lua_State* const vm) {
    OOLUA_C_FUNCTION_0(elysian::Level*, elysian::Singletons::getCurrentLevel)
}

static inline int Singletons_setProcessInput(lua_State* const vm) {
	OOLUA_C_FUNCTION(void, elysian::Singletons::setProcessInput, bool)
}

static inline int Singletons_getPartySystem(lua_State* const vm) {
	OOLUA_C_FUNCTION(elysian::PartySystem*, elysian::Singletons::getPartySys)
}

static inline int Singletons_getInputSystem(lua_State* const vm) {
    OOLUA_C_FUNCTION(elysian::InputSystem*, elysian::Singletons::getInputSys)
}

static inline int Singletons_getAudioSystem(lua_State* const vm) {
	OOLUA_C_FUNCTION(elysian::AudioSystem*, elysian::Singletons::getAudioSys)
}

static inline int Singletons_getLightSystem(lua_State* const vm) {
	OOLUA_C_FUNCTION(elysian::LightSystem*, elysian::Singletons::getLightSys)
}

static inline int Singletons_getParticleSystem(lua_State* const vm) {
	OOLUA_C_FUNCTION(elysian::ParticleSystem*, elysian::Singletons::getPartSys)
}

static inline int Singletons_getDaySystem(lua_State* const vm) {
	OOLUA_C_FUNCTION(elysian::DaySystem*, elysian::Singletons::getDaySys)
}

static inline int Singletons_getDialogSystem(lua_State* const vm) {
	OOLUA_C_FUNCTION(elysian::DialogSystem*, elysian::Singletons::getDialogSys)
}

static inline int Singletons_getGuiSystem(lua_State* const vm) {
	OOLUA_C_FUNCTION(elysian::GuiSystem*, elysian::Singletons::getGuiSys)
}

static inline int Singletons_getTimerSystem(lua_State* const vm) {
	OOLUA_C_FUNCTION(elysian::TimerSystem*, elysian::Singletons::getTimerSys)
}

static inline int Singletons_getCollisionSystem(lua_State* const vm) {
    OOLUA_C_FUNCTION(elysian::CollisionSystem*, elysian::Singletons::getCollisionSys)
}

static inline int Singletons_getRigidBodySystem(lua_State* const vm) {
	OOLUA_C_FUNCTION(elysian::RigidBodySystem*, elysian::Singletons::getRigidBodySys)
}

static inline int Singletons_getCombatSystem(lua_State* const vm) {
	OOLUA_C_FUNCTION(elysian::CombatSystem*, elysian::Singletons::getCombatSys)
}

static inline int Singletons_getCameraSystem(lua_State* const vm) {
	OOLUA_C_FUNCTION(elysian::CameraSystem*, elysian::Singletons::getCameraSys)
}

static inline int Singletons_getCharacterSystem(lua_State* const vm) {
	OOLUA_C_FUNCTION(elysian::CharacterSystem*, elysian::Singletons::getCharSys)
}

static inline int Singletons_getExaminableSystem(lua_State* const vm) {
    OOLUA_C_FUNCTION(elysian::ExaminableSystem*, elysian::Singletons::getExamSys)
}

static inline int Singletons_getEventSystem(lua_State* const vm) {
    OOLUA_C_FUNCTION(elysian::EventSystem*, elysian::Singletons::getEventSys)
}

static inline int Direction_fromVector2(lua_State* const vm) {
    OOLUA_C_FUNCTION(elysian::Direction::DIR4, elysian::Direction::fromVector2, const GYVector2&)
}

static inline int Direction_toVector2(lua_State* const vm) {
    OOLUA_C_FUNCTION(GYVector2, elysian::Direction::toVector2, elysian::Direction::DIR4)
}

static inline int Direction_toVector3(lua_State* const vm) {
    OOLUA_C_FUNCTION(GYVector3, elysian::Direction::toVector3, elysian::Direction::DIR4, float)
}

static inline int EventMonitor_null(lua_State *const vm) {
	OOLUA_C_FUNCTION(elysian::EventMonitor*, elysian::EventMonitor::null)
}

static inline int gyroWavCreate(lua_State *const vm) {
	OOLUA_C_FUNCTION(GYWav*, gyAudWavCreate, const char*)
}

static inline int gyroWavDestroy(lua_State *const vm) {
	OOLUA_C_FUNCTION(int, gyAudWavDestroy, GYWav*)
}

static inline int gyroMatPush(lua_State *const vm) {
    OOLUA_C_FUNCTION(void, gyMatPush)
}

//===== GYRO MATRIX STACK ====

static inline int gyroMatPop(lua_State*const vm) {
    OOLUA_C_FUNCTION(void, gyMatPop)
}

static inline int gyroMatLoadIdentity(lua_State*const vm) {
    OOLUA_C_FUNCTION(void, gyMatLoadIdentity)
}

static inline int gyroMatTranslate(lua_State*const vm) {
    OOLUA_C_FUNCTION(void, gyMatTranslate, const float, const float, const float)
}

static inline int gyroMatRotateZ(lua_State*const vm) {
    OOLUA_C_FUNCTION(void, gyMatRotateZ, const float)
}

static inline int gyroMatScale(lua_State*const vm) {
    OOLUA_C_FUNCTION(void, gyMatScale, const float, const float, const float)
}

static inline int gyroMatTranslateScale(lua_State*const vm) {
    OOLUA_C_FUNCTION(void, gyMatTranslateScale, const float, const float, const float, const float, const float, const float)
}

/*
static inline int gyroMatInstance(lua_State*const vm) {
    OOLUA_C_FUNCTION(void, gyMatInstance, const GYInstMat *const)
}
*/

static inline int gyroMatRotate(lua_State*const vm) {
    OOLUA_C_FUNCTION(void, gyMatRotate, const float, const float, const float, const float)
}

static inline int gyroMatShearZ(lua_State*const vm) {
    OOLUA_C_FUNCTION(void, gyMatShearZ, const float, const float)
}

//==== GYRO VECTOR MATH ====
/*
static inline int gyroMatMultMat(lua_State*const vm) {
    OOLUA_C_FUNCTION(void, gyMatMultMat, GYMatrix4, const GYMatrix4, const GYMatrix4)
}*/
/*
static inline int gyroMatMultVec(lua_State*const vm) {
    OOLUA_C_FUNCTION(void, gyMatMultVec, const GYVector4*const, GYVector4*, const unsigned)
}

static inline int gyroVec4Add(lua_State*const vm) {
    OOLUA_C_FUNCTION(void, gyVec4Add, void*, const void* const, const void* const)
}*/
/*
static inline int gyroVec4Mult(lua_State*const vm) {
    OOLUA_C_FUNCTION(void, gyVec4Mult, const void*const, const void* const)
}

static inline int gyroVec4Add(lua_State*const vm) {
    OOLUA_C_FUNCTION(void, gyVec4Add, void*, const void* const, const void* const)
}*/

#endif

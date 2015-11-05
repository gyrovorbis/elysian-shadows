#ifndef ELYSIAN_CHARACTER_PROXY_HPP
#define ELYSIAN_CHARACTER_PROXY_HPP

#include <crtp/elysian_character.hpp>
#include <proxy/elysian_character_proxy.hpp>
#include <proxy/elysian_component_proxy.hpp>

namespace OOLUA {
    typedef elysian::Direction      Direction;
    typedef elysian::Character      Character;

}

OOLUA_PROXY(Direction)
    OOLUA_TAGS (
        No_public_constructors,
        No_public_destructor
    )
OOLUA_PROXY_END

OOLUA_PROXY(Character, Component)
    OOLUA_TAGS (
        No_public_constructors,
        No_public_destructor
    )
    OOLUA_MEM_FUNC(void, faceEntity, Entity&)
    OOLUA_MEM_FUNC(void, faceDir, Direction::DIR4)
    OOLUA_MEM_FUNC(void, moveByForce, const GYVector2&)
    OOLUA_MEM_FUNC(void, setAnimate, bool)
    OOLUA_MEM_FUNC_CONST(bool, isFacing, const Entity&)
    OOLUA_MEM_FUNC_CONST(Direction::DIR4, getFaceDir)
    OOLUA_MEM_FUNC(void, setWalkForce, float)
    OOLUA_MEM_FUNC(void, setDashMultiplier, float)
    OOLUA_MEM_FUNC(void, setDashing, bool)
    OOLUA_MEM_FUNC_CONST(maybe_null<Entity*>, getHeldEntity)
    OOLUA_MEM_FUNC_CONST(const GYVector3&, getLastMoveDir)
    OOLUA_MEM_FUNC_CONST(float, getWalkForce)
    OOLUA_MEM_FUNC_CONST(float, getDashMultiplier)
    OOLUA_MEM_FUNC(float, getEntityForceDampFactor)
    OOLUA_MEM_FUNC(void, addState, const Character::ANIM_STATE)
    OOLUA_MEM_FUNC(void, addTempState, const Character::ANIM_STATE, const int, OOLUA::Lua_func_ref)
    OOLUA_MEM_FUNC(void, removeState, const Character::ANIM_STATE)
    OOLUA_MEM_FUNC(void, jump)
    OOLUA_MEM_FUNC(void, applyKnockbackForce, const GYVector3&)
    OOLUA_MEM_FUNC_CONST(bool, hasState, const Character::ANIM_STATE)
    OOLUA_MEM_FUNC_CONST(const char*, getAnimNameFromStates, Direction::DIR4)
OOLUA_PROXY_END

#endif // ELYSIAN_CHARACTER_PROXY_HPP


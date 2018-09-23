#include <game/behavior/elysian_playable_behavior.hpp>
#include <ESGamma/lua/elysian_lua_singleton.hpp>
#include <ESGamma/crtp/elysian_timer_system.hpp>
#include <ESGamma/system/elysian_party_system.hpp>
#include <ESGamma/crtp/elysian_input_system.hpp>
#include <ESGamma/crtp/elysian_particle_system.hpp>
#include <ESGamma/system/elysian_audio_system.hpp>
#include <AssetIO/crtp/elysian_entity.hpp>
#include <libGyro/gyro_input_api.h>
#include <ESGamma/system/elysian_engine.hpp>
#include <AssetIO/crtp/elysian_combat.hpp>
#include <ESGamma/device/elysian_rumble_device.hpp>
#include <AssetIO/proxy/elysian_entity_proxy.hpp>

namespace elysian {

namespace game {

void PlayableBehavior::onUpdate(void) {
    if(_playerIndex == -1) return;
    auto* partyMember       = getEntity();
    auto* partySys          = Singletons::getPartySys();
    auto* cont              = Singletons::getInputSys()->getController(_playerIndex);
    const float deltaTime   = Singletons::getTimerSys()->getDeltaTime();

        if(System::getEngineState() == Engine::ENGINE_WARPING) return;
        if(!System::getProcessInput()) return;
       // if(partySys->getPartyLeader() != partyMember) return;

    if(System::getEngineState() != Engine::ENGINE_WARPING) {
        _processMovement(deltaTime, cont, partyMember, partySys);
        _processJump(deltaTime, cont, partyMember);
        _processExamine(deltaTime, cont, partyMember, partySys);
        _processPickup(deltaTime, cont, partyMember, partySys);
      //  _processSwapPartyMember(deltaTime, cont, partyMember, partySys);
    }
}

void PlayableBehavior::_processMovement(const float /*deltaTime*/, const Controller*const cont, Entity*const entity, PartySystem*const partySys) {
    auto angle = Vector2(cont->getAnalogDir(GY_CONTROLLER_ANALOG_LEFT)).toAngle();
    auto mag = cont->getAnalogMag(GY_CONTROLLER_ANALOG_LEFT);

    _tapTimer += partySys->getTimerSys()->getDeltaTime();
    _dashPartTimer += partySys->getTimerSys()->getDeltaTime();

//    auto endDash = [&](void) {
//        entity->getCharacter()->setSpeedMultiplier(1.0f);
//        partySys->getAudioSys()->setSfxChannelPitch(0, 1.0f);
//        entity->getCharacter()->setDashing(false);
//        _tapTimer = 0.0f;
//        _lastAngle = 0.0f;
//    };

    auto emitDashParticles = [&](void) {
        //partySys->getPartSys()->getAttrib(0)->setAcceleration(GYVector3{ 0.0f, 8.0, 7.0f/*7.0f*//*6.5f*/ });
        partySys->getPartSys()->getAttrib(0)->setForceMask(0);

        partySys->getPartSys()->getAttrib(0)->setStartSize(1);
        partySys->getPartSys()->getAttrib(0)->setEndSize(6);
        partySys->getPartSys()->getAttrib(0)->setEndColor(Color4(255, 255, 255, 0));
        partySys->getPartSys()->getAttrib(0)->setColor(255, 255, 255, 100);
        partySys->getPartSys()->getAttrib(0)->setIntensityFactor(10.0f);
        partySys->getPartSys()->getAttrib(0)->setMaxLifeTime(0.3f);
        partySys->getPartSys()->getAttrib(0)->setStick(false);
        GYVector3 pos = entity->getCollidable()->getWorldPos();
        pos.y += 0.5f * entity->getCollidable()->getWorldSize().y;
        pos.z += 5.0f;
        GYVector2 dir = cont->getAnalogDir(GY_CONTROLLER_ANALOG_LEFT);
        GYVector3 dirNorm = { -dir.x, -dir.y, 1.0f };
        gyVec3Normalize(&dirNorm);
        //partySys->getPartSys()->emitFromPoint(0, pos, angle, 500);
        partySys->getPartSys()->emitFromPointInDir(0, pos, dirNorm, 100, angle);
        _dashPartTimer = 0.0f;
    };

    _tapTimer += partySys->getTimerSys()->getDeltaTime();
    switch(_dashState) {
        case DASH_NONE:
        entity->getCharacter()->setDashing(false);
        if(mag < 0.3f) {
            ++_dashState;
                 //   dbgprintf("DASH_FIRST_TAP");
        }
        break;
    case DASH_FIRST_TAP:
        if(mag >= 0.5f) {
            _tapTimer = 0.0f;
            _lastAngle = angle;
            ++_dashState;
                 //   dbgprintf("DASH_RELEASE");
        }
        break;
    case DASH_RELEASE:
        //if(fabs(_lastAngle-angle) > 0.5f*M_PI) {
            //_dashState = DASH_NONE;
        //} else {
            if(_tapTimer >= _dashTapTimeWindow) {
                _dashState = DASH_NONE;
                    //    dbgprintf("DASH_NONE");
            } else if(mag < 0.5f) {
          //      _lastAngle = angle;
                ++_dashState;
                 //       dbgprintf("DASH_SECOND_TAP");
            }
        //}
        break;
    case DASH_SECOND_TAP:
        //if(fabs(_lastAngle-angle) > 0.5f*M_PI) {
          //  _dashState = DASH_NONE;
        //} else {
            if(_tapTimer >= _dashTapTimeWindow) {
                _dashState = DASH_NONE;
                    //    dbgprintf("DASH_NONE: TIME");
            } else if(mag >= 0.5f) {
                //if(fabs(_lastAngle-angle) > M_PI) {
                //    _dashState = DASH_NONE;
                //            dbgprintf("DASH_NONE: ANGLE[%f]", fabs(_lastAngle-angle));
                //} else {
                      //      dbgprintf("DASH_ACTIVE");
                    _lastAngle = angle;
                    ++_dashState;


                    RampRumbleEffect effect(_dashDuration, ELYSIAN_PLAYABLE_BEHAVIOR_RUMBLE_MAG_DASH, 0.0);
                    GYVector2 analogDir = cont->getAnalogDir(GY_CONTROLLER_ANALOG_LEFT);
                    Vector3 rumbleDir = { analogDir.x, analogDir.y, 0.0f };
                    effect.setDirection(rumbleDir);
                    System::getInputSys()->getRumbleDevice(_playerIndex)->playEffect(effect, 1);


                    entity->getCharacter()->setDashing(true);
                    emitDashParticles();
                    _tapTimer = 0.0f;
                //}
            }
        //}
        break;
    case DASH_ACTIVE:
        default:
        if(!entity->getCharacter()->isDashing() || _tapTimer >= _dashDuration) {
            _dashState = DASH_NONE;
                  //  dbgprintf("DASH_NONE");
        }
        break;
    }

        //Compensate for inaccuracy and over-sensitivity with dead-zones on either side



        const bool move = (mag < 0.3f)? false : true;
        mag -= 0.3f;
        mag /= 0.7f;

        GYVector2 moveVec = cont->getAnalogDir(GY_CONTROLLER_ANALOG_LEFT);

        if(!cont->isPressed(GY_CONTROLLER_BUT_LSHOULDER)/* && !entity->getCharacter()->getPullEntity()*/) {
            if(move) {
                auto dir = Direction::fromVector2(moveVec);
                entity->getCharacter()->setFaceDir(dir);
            }
            entity->getCharacter()->removeState(Character::STATE_STRAFE);
        } else {
            entity->getCharacter()->addState(Character::STATE_STRAFE);
            if(mag >= 0.7f && !entity->getCharacter()->isDashing()) mag = 0.699999f;
        }

        if(move){
            gyVec2ScaleAccum(&moveVec, mag);

            entity->getCharacter()->walk(moveVec);
        } else entity->getCharacter()->removeState(Character::STATE_STRAFE);




}

void PlayableBehavior::_processJump(const float /*deltaTime*/, const Controller*const cont, Entity*const entity) {
    if(cont->isTapped(GY_CONTROLLER_BUT_X)) {
        entity->getCharacter()->jump();
        RampRumbleEffect effect(0.2f, ELYSIAN_PLAYABLE_BEHAVIOR_RUMBLE_MAG_JUMP, 0.0f);
        GYVector2 analogDir = cont->getAnalogDir(GY_CONTROLLER_ANALOG_LEFT);
        Vector3 dir = { analogDir.x, analogDir.y, -1.0f };
        dir.normalize();
        effect.setDirection(dir);
        //System::getInputSys()->getRumbleDevice(_playerIndex)->playEffect(effect, 1);
    }
}



void PlayableBehavior::_processExamine(const float /*deltaTime*/, const Controller*const cont, Entity*const entity, PartySystem* partySys) {
    if(cont->isTapped(GY_CONTROLLER_BUT_CIRCLE)) {
        partySys->checkExamine(entity);
    }
}


void PlayableBehavior::_processIdle(const float /*deltaTime*/, const Controller*const /*cont*/, Entity*const /*entity*/) { }
void PlayableBehavior::_processPickup(const float /*deltaTime*/, const Controller*const /*cont*/, Entity*const entity, PartySystem* partySys) {



   // if(cont->isTapped(GY_CONTROLLER_BUT_RSHOULDER)) {
        partySys->checkPickup(entity);
   // }

}

void PlayableBehavior::_processCarry(const float /*deltaTime*/, const Controller*const /*cont*/, Entity*const /*entity*/) {
}

void PlayableBehavior::_processPushPull(const float /*deltaTime*/, const Controller*const /*cont*/, Entity*const /*entity*/) {
}

int PlayableBehavior::getPartySlot(void) const {
    return Singletons::getPartySys()->getPartySlot(_entity);
}

bool PlayableBehavior::isAlive(void) const {
    return (getEntity()->getCombat()->getStatus().getHp() > 0);
}

void PlayableBehavior::_playerIndexChanged(int oldIndex, int newIndex) {
#if 0
    LuaBehavior* behav = getEntity()->getLuaBehavior("/assets/scripts/combat/_player_combat_behav.lua");
    if(behav) {
        LuaEvent event({
            { "luaType", "EVENT_PLAYER_INDEX_CHANGED" },
            { "entity",   getEntity() },
            { "oldIndex", oldIndex },
            { "newIndex", newIndex }
        });
        System::getEventSys()->sendEvent(&event, behav);
    }
#endif
}

}

}

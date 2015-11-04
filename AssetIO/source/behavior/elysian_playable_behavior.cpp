#include <behavior/elysian_playable_behavior.hpp>
#include <lua/elysian_lua_singleton.hpp>
#include <crtp/elysian_timer_system.hpp>
#include <system/elysian_party_system.hpp>
#include <crtp/elysian_input_system.hpp>
#include <system/elysian_particle_system.hpp>
#include <system/elysian_audio_system.hpp>
#include <crtp/elysian_entity.hpp>
#include <gyro_input_api.h>


namespace elysian {

void PlayableBehavior::onUpdate(void) {
    auto* partyMember       = getEntity();
    auto* partySys          = Singletons::getPartySys();
    auto* cont              = Singletons::getInputSys()->getController(_playerNum);
    const float deltaTime   = Singletons::getTimerSys()->getDeltaTime();


    _processMovement(deltaTime, cont, partyMember, partySys);
    _processJump(deltaTime, cont, partyMember);
    _processExamine(deltaTime, cont, partyMember, partySys);
    _processPickup(deltaTime, cont, partyMember, partySys);
}

void PlayableBehavior::_processMovement(const float deltaTime, const Controller*const cont, Entity*const entity, PartySystem*const partySys) {
    auto angle = cont->getAnalogAngle(GY_CONTROLLER_ANALOG_LEFT);
    auto mag = cont->getAnalogMag(GY_CONTROLLER_ANALOG_LEFT);

    _tapTimer += partySys->getTimerSys()->getDeltaTime();
    _dashPartTimer += partySys->getTimerSys()->getDeltaTime();

    auto endDash = [&](void) {
        entity->getCharacter()->setSpeedMultiplier(1.0f);
        partySys->getAudioSys()->setSfxChannelPitch(0, 1.0f);
        entity->getCharacter()->setDashing(false);
        _tapTimer = 0.0f;
        _lastAngle = 0.0f;
    };

    auto emitDashParticles = [&](void) {
        partySys->getPartSys()->getAttrib(0)->setAcceleration(GYVector3{ 0.0f, -70.0f, 7.0f/*7.0f*//*6.5f*/ });
        partySys->getPartSys()->getAttrib(0)->setForceMask(0);
        partySys->getPartSys()->getAttrib(0)->setEndColor(GYColor4 { 255, 255, 255, 0});
        partySys->getPartSys()->getAttrib(0)->setColor(255, 255, 255, 100);
        partySys->getPartSys()->getAttrib(0)->setMaxLife(30);
        partySys->getPartSys()->getAttrib(0)->setStick(false);
        GYVector3 pos = entity->_transform._pos;
        pos.z += 5.0f;
        pos.y = entity->getDepthBaseY();
        //partySys->getPartSys()->emitFromPoint(0, pos, angle, 500);
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
        }




}

void PlayableBehavior::_processJump(const float deltaTime, const Controller*const cont, Entity*const entity) {
    if(cont->isTapped(GY_CONTROLLER_BUT_X)) {
        entity->getCharacter()->jump();
    }
}



void PlayableBehavior::_processExamine(const float deltaTime, const Controller*const cont, Entity*const entity, PartySystem* partySys) {
    if(cont->isTapped(GY_CONTROLLER_BUT_CIRCLE)) {
        partySys->checkExamine(entity);
    }
}


void PlayableBehavior::_processIdle(const float deltaTime, const Controller*const cont, Entity*const entity) { }
void PlayableBehavior::_processPickup(const float deltaTime, const Controller*const cont, Entity*const entity, PartySystem* partySys) {



   // if(cont->isTapped(GY_CONTROLLER_BUT_RSHOULDER)) {
        partySys->checkPickup(entity);
   // }

}

void PlayableBehavior::_processCarry(const float deltaTime, const Controller*const cont, Entity*const entity) {
}

void PlayableBehavior::_processPushPull(const float deltaTime, const Controller*const cont, Entity*const entity) {
}



}

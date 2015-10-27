#ifndef ELYSIAN_PLAYABLE_BEHAVIOR_HPP
#define ELYSIAN_PLAYABLE_BEHAVIOR_HPP

#include <crtp/elysian_behavior.hpp>

#define ELYSIAN_PLAYABLE_BEHAVIOR_NAME  "PlayableBehavior"
#define ELYSIAN_PLAYABLE_BEHAVIOR_DASH_PART_TIME_INTERVAL    0.5f

namespace elysian {

    class Controller;
    class PartySystem;

    class PlayableBehavior: public Behavior {
    private:
        enum DASH_STATE         { DASH_NONE, DASH_FIRST_TAP, DASH_RELEASE, DASH_SECOND_TAP, DASH_ACTIVE };

        unsigned                _playerNum      = 0;
        bool                    _aiControlled   = false;
        unsigned                _dashState                  = DASH_NONE;
        bool                    _dashing                    = false;
        float                   _lastAngle                  = 0.0f;
        float                   _dashPartInterval           = ELYSIAN_PLAYABLE_BEHAVIOR_DASH_PART_TIME_INTERVAL;
        float                   _dashDuration               = 0.25f;
        float                   _tapTimer                   = 0.0f;
        float                   _dashTapTimeWindow          = 1.0f;
        float                   _dashPartTimer              = 0.0f;
        float                   _dashPartAngle              = 0.0f;

        void                    _processMovement(const float deltaTime, const Controller* cont, Entity*const entity, PartySystem*const partySys);
        void                    _processJump(const float deltaTime, const Controller* cont, Entity*const entity);
        void                    _processExamine(const float deltaTime, const Controller* cont, Entity*const entity, PartySystem*const partySys);
        void                    _processIdle(const float deltaTime, const Controller*const cont, Entity*const entity);
        void                    _processPickup(const float deltaTime, const Controller*const cont, Entity*const entity, PartySystem*const partySys);
        void                    _processCarry(const float deltaTime, const Controller*const cont, Entity*const entity);
        void                    _processPushPull(const float deltaTime, const Controller*const cont, Entity*const entity);
        void                    _processSwapPartyMember(const float deltaTime, const Controller*const cont, PartySystem*const partySys);

    public:

                                PlayableBehavior(Entity& entity);

        unsigned                getPlayerNum(void) const;
        void                    setPlayerNum(const unsigned num);

        //overridden from Behavior
        virtual const char*     getBehavName(void) const override;
        virtual void            onUpdate(void) override;
    };






    // ==== INLINEZ ======

    inline unsigned PlayableBehavior::getPlayerNum(void) const { return _playerNum; }
    inline void PlayableBehavior::setPlayerNum(const unsigned num) { _playerNum = num; }
    inline const char* PlayableBehavior::getBehavName(void) const { return ELYSIAN_PLAYABLE_BEHAVIOR_NAME; }
    inline PlayableBehavior::PlayableBehavior(Entity &entity): Behavior(entity) {
        addTrigger(Behavior::ON_UPDATE);

    }
}

#endif // ELYSIAN_PLAYABLE_BEHAVIOR_HPP


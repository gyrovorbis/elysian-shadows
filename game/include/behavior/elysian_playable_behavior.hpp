#ifndef ELYSIAN_PLAYABLE_BEHAVIOR_HPP
#define ELYSIAN_PLAYABLE_BEHAVIOR_HPP

#include <crtp/elysian_behavior.hpp>

#define ELYSIAN_PLAYABLE_BEHAVIOR_NAME                          "PlayableBehavior"
#define ELYSIAN_PLAYABLE_BEHAVIOR_DASH_PART_TIME_INTERVAL       0.3f
#define ELYSIAN_PLAYABLE_BEHAVIOR_RUMBLE_MAG_DASH               0.5f
#define ELYSIAN_PLAYABLE_BEHAVIOR_RUMBLE_MAG_JUMP               0.2f

ELYSIAN_ASSET_TYPEDEF(LuaBehavior)

namespace elysian {

    class Controller;
    class PartySystem;

    namespace game {

        class PlayableBehavior: public Behavior {
        private:
            enum DASH_STATE         { DASH_NONE, DASH_FIRST_TAP, DASH_RELEASE, DASH_SECOND_TAP, DASH_ACTIVE };

            int                     _playerIndex                = -1;
            bool                    _aiControlled               = false;
            unsigned                _dashState                  = DASH_NONE;
            bool                    _dashing                    = false;
            float                   _lastAngle                  = 0.0f;
            float                   _dashPartInterval           = ELYSIAN_PLAYABLE_BEHAVIOR_DASH_PART_TIME_INTERVAL;
            float                   _dashDuration               = 0.25f;
            float                   _tapTimer                   = 0.0f;
            float                   _dashTapTimeWindow          = 0.7f;
            float                   _dashPartTimer              = 0.0f;
            float                   _dashPartAngle              = 0.0f;

            void                    _processMovement(const float deltaTime, const Controller* cont, Entity*const entity, PartySystem*const partySys);
            void                    _processJump(const float deltaTime, const Controller* cont, Entity*const entity);
            void                    _processExamine(const float deltaTime, const Controller* cont, Entity*const entity, PartySystem*const partySys);
            void                    _processIdle(const float deltaTime, const Controller*const cont, Entity*const entity);
            void                    _processPickup(const float deltaTime, const Controller*const cont, Entity*const entity, PartySystem*const partySys);
            void                    _processCarry(const float deltaTime, const Controller*const cont, Entity*const entity);
            void                    _processPushPull(const float deltaTime, const Controller*const cont, Entity*const entity);
            void                    _processSwapPartyMember(const float deltaTime, const Controller*const cont, Entity*const entity, PartySystem*const partySys);

            void                    _playerIndexChanged(int oldIndex, int newIndex);

        public:

                                    PlayableBehavior(Entity& entity);

            int                     getPlayerIndex(void) const;
            void                    setPlayerIndex(const int num);
            int                     getPartySlot(void) const;
            bool                    isAlive(void) const;

            //overridden from Behavior
            virtual const char*     getBehavName(void) const override;
            virtual void            onUpdate(void) override;
        };







        // ==== INLINEZ ======

        inline int PlayableBehavior::getPlayerIndex(void) const { return _playerIndex; }
        inline void PlayableBehavior::setPlayerIndex(const int index)  {
            if(_playerIndex != index) {
                int oldIndex = _playerIndex;
                _playerIndex = index;
                _playerIndexChanged(oldIndex, index);
            }
        }
        inline const char* PlayableBehavior::getBehavName(void) const { return ELYSIAN_PLAYABLE_BEHAVIOR_NAME; }
        inline PlayableBehavior::PlayableBehavior(Entity &entity): Behavior(entity) {
            addTrigger(Behavior::ON_UPDATE);

        }

    }
}

#endif // ELYSIAN_PLAYABLE_BEHAVIOR_HPP


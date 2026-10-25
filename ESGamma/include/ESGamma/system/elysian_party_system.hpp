/*
 * Copyright (c) 2013-2026 Elysian Shadows Team
 * SPDX-License-Identifier: MIT
 * Additional permission for Elysian Shadows Kickstarter backers: see LICENSE.
 */

#ifndef ELYSIAN_PARTY_SYSTEM_HPP
#define ELYSIAN_PARTY_SYSTEM_HPP

#include <vector>
#include <libGyro/gyro_matrix_api.h>
#include <libGyro/gyro_wav_api.h>
#include <ESGamma/system/elysian_system.hpp>
#include <AssetIO/crtp/elysian_character.hpp>
#include <AssetIO/level/elysian_event.hpp>

#define ELYSIAN_PARTY_SYSTEM_FOOT_STEP_SFX_TIME         0.3f
#define ELYSIAN_PARTY_SYSTEM_DASH_PART_TIME_INTERVAL    0.5f
#define ELYSIAN_PARTY_SLOTS_MAX                         5
#define ELYSIAN_PARTY_GUEST_SLOTS_MAX                   10

#define ELYSIAN_PARTY_SYSTEM_PLAYER_JOIN_SFX          "/assets/audio/sfx/UI/player_connect.wav"
#define ELYSIAN_PARTY_SYSTEM_PLAYER_SLOT_SWAP_SFX     "/assets/audio/sfx/UI/menu_select_02.wav"

ELYSIAN_ASSET_TYPEDEF(Entity)

/* Needs the concept of active vs inactive party members
 * figure out about how to treat guests and if you can play as them?
 * FOR MENUS
 * 1 - swap slots
 * 2 - activate/deactivate
 */


namespace elysian {

    class Controller;
    class PartySystem;

    namespace game {
        class PlayableBehavior;
    }

    class Player {
    private:

        enum DASH_STATE { DASH_NONE, DASH_FIRST_TAP, DASH_RELEASE, DASH_SECOND_TAP, DASH_ACTIVE };

        //dash state variables
        unsigned                _dashState                  = DASH_NONE;
        bool                    _dashing                    = false;
        float                   _lastAngle                  = 0.0f;
        float                   _dashPartInterval           = ELYSIAN_PARTY_SYSTEM_DASH_PART_TIME_INTERVAL;
        float                   _dashDuration               = 0.25f;
        float                   _tapTimer                   = 0.0f;
        float                   _dashTapTimeWindow          = 1.0f;
        float                   _dashPartTimer              = 0.0f;
        float                   _dashPartAngle              = 0.0f;

        unsigned    _playerNum      = 0;
        bool        _aiControlled   = true;

        //Entity*     _partyMember    = nullptr;

        void        _processMovement(const float deltaTime, const Controller* cont, Entity*const entity, PartySystem*const partySys);
        void        _processJump(const float deltaTime, const Controller* cont, Entity*const entity);
        void        _processExamine(const float deltaTime, const Controller* cont, Entity*const entity, PartySystem*const partySys);
        void        _processIdle(const float deltaTime, const Controller*const cont, Entity*const entity);
        void        _processPickup(const float deltaTime, const Controller*const cont, Entity*const entity, PartySystem*const partySys);
        void        _processCarry(const float deltaTime, const Controller*const cont, Entity*const entity);
        void        _processPushPull(const float deltaTime, const Controller*const cont, Entity*const entity);
        void        _processSwapPartyMember(const float deltaTime, const Controller*const cont, PartySystem*const partySys);

    public:

        void        processInput(const Controller*const cont, PartySystem*const partySys);
        unsigned    getPlayerNum(void) const;
        void        setPlayerNum(const unsigned num);

    };


    class PartySystem: public System, public EventObserver {
    public:
        static const int UnassignedPlayerIndex  = -1;
        static const int InvalidPlayerIndex     = -2;
    private:

        struct PartyMember {
            Entity* entity          = nullptr;
            bool    active          = true;
            bool    followLogic      = true;
            int     playerIndex     = UnassignedPlayerIndex;
            game::PlayableBehavior* playable  = nullptr;
            PartyMember(Entity* e);
             operator Entity*() const { return entity; }
            bool operator ==(const Entity* e) const { return e == entity; }
            Entity* operator->() const { return entity; }
            void setPlayerIndex(int index);
        };

        Player  _players[ELYSIAN_PARTY_SLOTS_MAX];

        int     _unassignedPlayerIndices[ELYSIAN_PARTY_SLOTS_MAX]   = { UnassignedPlayerIndex };
        int     _partySlotFromPlayerIndex[ELYSIAN_PARTY_SLOTS_MAX]  = { UnassignedPlayerIndex };

        bool    addUnassignedPlayerIndex(int index);
        bool    removeUnassignedPlayerIndex(int index);
        bool    removePlayerIndex(int index); //controller disconnected, player totally gone
        bool    assignPlayerIndex(int index);
        bool    unassignPlayerIndex(int index); //remove player from party member, put back in unassigned state
        int     getSlotFromPlayerIndex(int index) const;
        bool    setPlayerIndexBySlot(int slot, int playerIndex);
        void    swapPlayerIndexSlots(int playerIndex, int oldSlot, int newSlot);


        void    pollUnassignedPlayers(void);
        void    _processInputDeviceEvent(Event* event);
        void    _checkDpadPartyMemberSwap(void);


		unsigned int _activePlayablePartySize	= 3;
//		unsigned int _characterPartySize		= 3;
		unsigned int _playableAmount			= 0;
		
		//Array holding every potential playable character
        std::vector<Entity*> _playables;

		//< _activePartySize is active
        std::vector<PartyMember> _playableParty;
		//ONLY active and can only contain up to _characterPartySize
        std::vector<PartyMember> _characterParty;
		
		std::vector<GYVector2> _partyPathPoint;
        static const unsigned int _pointsPerCharacter = 30;
		
		void _resizePartyPathPoints(void);
        void _resetPartyPathPoints(void);

		//footstep SFX
        float                   _footStepTimer              = 0.0f;
        float                   _dashAnimSpeedMultiplier    = 5.0f;
        bool                    _footStepSfxLooping         = false;
        GYWav*                  _curWalkSfx                 = nullptr;
        GYWav*                  _playerJoinSfx              = nullptr;
        GYWav*                  _playerSwapSfx              = nullptr;
        bool                    _wasOnGround                = true;
        bool                    _partyActive                = false;
        bool                    _followLogic                = true;

        void _updateCamera(void) const;

	public:
            PartySystem(void);
            ~PartySystem(void);

        virtual void onObserveEvent(Event* event, EventTarget* target);
		
		void update(void);
		void updatePartyPaths(void);
        void checkExamine(Entity* const leader) const;
        void checkPickup(Entity* const leader) const;
		
        void addPlayable(Entity* entity);
        unsigned getPlayableCount(void) const;
        Entity* getPlayable(const char* name) const;
		//bool loadParty(std::ifstream &stream);
		//bool saveParty(std::ofstream &stream);
		
        void addPlayableToParty(const char *name);
        void addCharacterToParty(Entity *entity);
		
		bool removeCharacterFromParty(const char *name);
		bool removeCharacterFromParty(const Entity *entity);
		bool removeCharacterFromParty(const unsigned int slot);
		
        Entity *getPartyLeader(void) const;
        void    setPartyLeader(Entity* entity);
        Entity *getPlayablePartyMember(const char *name) const;
        Entity *getPartyMember(const unsigned int slot) const;
        void    swapPartyMemberSlots(unsigned int slot1, unsigned int slot2);
        Entity *getCharacterPartyMember(const char *name) const;
        Entity *getCharacterPartyMember(const unsigned int slot) const;
        bool    isPartyMemberActive(unsigned slot) const;
        void    setPartyMemberActive(unsigned slot, bool active=true);

        unsigned getActivePartyMemberCount(void) const;

        unsigned getPartySize(void) const;

        int    getPartySlot(const Entity* entity) const;
        //void    swapBySlot(unsigned slot1, unsigned slot2);
		
	//	void DisablePartyFollow(); Probably going to want these pretty soon... 
	//	void EnablePartyFollow();
		
		void removePartyFromArea(void);
		void addPartyToArea(void);

        void setDashTime(const float time);
        void setDashForceMagnitude(const float mag);
        void setDashAnimSpeedMultiplier(const float mult);
        void setFollowLogicEnabled(bool val=true);
        void setSlotFollowLogicEnabled(const unsigned int slot, bool enabled=true);
        bool isSlotFollowLogicEnabled(const unsigned int slot) const;
        void processInput(void);

        int     getPlayerIndexFromSlot(int slot) const;
        int     getPlayerIndexFromPartyMember(const Entity* entity) const;

		
		//bool SwapPlayablePartyMembersBySlot();
		//bool SwapLeader();
		
	};




    //============= INLINEZ ===============

     inline unsigned PartySystem::getPartySize(void) const { return _characterParty.size(); }
    inline unsigned PartySystem::getPlayableCount(void) const { return _playables.size(); }
    inline void PartySystem::setFollowLogicEnabled(bool val) { _followLogic = val; }
    inline bool PartySystem::isPartyMemberActive(unsigned slot) const { return (slot < getPartySize())? _characterParty[slot].active : false; }
    inline bool PartySystem::isSlotFollowLogicEnabled(const unsigned int slot) const { return (slot < getPartySize())? _characterParty[slot].followLogic : false; }

    inline int PartySystem::getPlayerIndexFromPartyMember(const Entity* entity) const {
        return getPlayerIndexFromSlot(getPartySlot(entity));
    }



     inline unsigned Player::getPlayerNum(void) const { return _playerNum; }
     inline void Player::setPlayerNum(const unsigned num) { _playerNum = num; }


}

#endif

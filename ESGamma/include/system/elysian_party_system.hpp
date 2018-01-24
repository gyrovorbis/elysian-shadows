#ifndef ELYSIAN_PARTY_SYSTEM_HPP
#define ELYSIAN_PARTY_SYSTEM_HPP

#include <vector>
#include <gyro_matrix_api.h>
#include <gyro_wav_api.h>
#include <system/elysian_system.hpp>
#include <crtp/elysian_character.hpp>

#define ELYSIAN_PARTY_SYSTEM_FOOT_STEP_SFX_TIME         0.3f
#define ELYSIAN_PARTY_SYSTEM_DASH_PART_TIME_INTERVAL    0.5f
#define ELYSIAN_PARTY_SLOTS_MAX                         5
#define ELYSIAN_PARTY_GUEST_SLOTS_MAX                   10

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


	class PartySystem: public System { 
    private:
        Player _players[ELYSIAN_PARTY_SLOTS_MAX];

		unsigned int _activePlayablePartySize	= 3;
//		unsigned int _characterPartySize		= 3;
		unsigned int _playableAmount			= 0;
		
		//Array holding every potential playable character
        std::vector<Entity*> _playables;

		//< _activePartySize is active
        std::vector<Entity*> _playableParty;
		//ONLY active and can only contain up to _characterPartySize
        std::vector<Entity*> _characterParty;
		
		std::vector<GYVector2> _partyPathPoint;
        static const unsigned int _pointsPerCharacter = 30;
		
		void _resizePartyPathPoints(void);

		//footstep SFX
        float                   _footStepTimer              = 0.0f;
        float                   _dashAnimSpeedMultiplier    = 5.0f;
        bool                    _footStepSfxLooping         = false;
        GYWav*                  _curWalkSfx                 = nullptr;
        bool                    _wasOnGround                = true;
        bool                    _partyActive                = false;
        bool                    _followLogic                = true;

	public:
            PartySystem(void);
            ~PartySystem(void);
		
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
        Entity *getCharacterPartyMember(const char *name) const;
        Entity *getCharacterPartyMember(const unsigned int slot) const;

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
		
		//bool SwapPlayablePartyMembersBySlot();
		//bool SwapLeader();
		
	};




    //============= INLINEZ ===============

     inline unsigned PartySystem::getPartySize(void) const { return _characterParty.size(); }
    inline unsigned PartySystem::getPlayableCount(void) const { return _playables.size(); }
    inline void PartySystem::setFollowLogicEnabled(bool val) { _followLogic = val; }


     inline unsigned Player::getPlayerNum(void) const { return _playerNum; }
     inline void Player::setPlayerNum(const unsigned num) { _playerNum = num; }


}

#endif

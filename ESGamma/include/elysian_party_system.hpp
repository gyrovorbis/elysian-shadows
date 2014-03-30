#ifndef ELYSIAN_PARTY_SYSTEM_HPP
#define ELYSIAN_PARTY_SYSTEM_HPP

#include <vector>
#include "elysian_system.hpp"

#include <gyro.h>

#define ELYSIAN_PARTY_PATH	"party/party.txt"

namespace elysian {

	class GameInput;
	class Entity;
	
	class PartySystem: public System { 
	private:
		unsigned int _activePlayablePartySize	= 3;
//		unsigned int _characterPartySize		= 3;
		unsigned int _playableAmount			= 0;
		
		//Array holding every potential playable character
		Entity **_playable;
		
		//< _activePartySize is active
		std::vector<Entity *> _playableParty;
		//ONLY active and can only contain up to _characterPartySize
		std::vector<Entity *> _characterParty;
		
		std::vector<GYVector2> _partyPathPoint;
		static const unsigned int _pointsPerCharacter = 15;
		
		void _resizePartyPathPoints(void);
		void _checkExamine(Entity* const leader) const;

	public:
		PartySystem(void);
		~PartySystem(void); 
		
		void update(void);
		void render(void);
		void processInput(const GameInput &input);
		void updatePartyPaths(void);
		
		bool loadPlayables();
		//bool loadParty(std::ifstream &stream);
		//bool saveParty(std::ofstream &stream);
		
		bool addPlayableToParty(const char *name);
		bool addPlayableToParty(const Entity &entity);
		bool addCharacterToParty(Entity *entity);
		
		bool removePlayableFromParty(const char *name);
		bool removePlayableFromParty(const Entity &entity);
		bool removePlayableFromParty(const unsigned int slot);
		bool removeCharacterFromParty(const char *name);
		bool removeCharacterFromParty(const Entity *entity);
		bool removeCharacterFromParty(const unsigned int slot);
		
		Entity *getPartyLeader(void) const;
		Entity *getPlayablePartyMember(const char *name) const;
		Entity *getPlayablePartyMember(const unsigned int slot) const;
		Entity *getCharacterPartyMember(const char *name) const;
		Entity *getCharacterPartyMember(const unsigned int slot) const;
		
	//	void DisablePartyFollow(); Probably going to want these pretty soon... 
	//	void EnablePartyFollow();
		
		void removePartyFromArea(void);
		void addPartyToArea(void);
		
		//bool SwapPlayablePartyMembersBySlot();
		//bool SwapLeader();
		
	};

}

#endif

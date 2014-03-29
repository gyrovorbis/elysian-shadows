#ifndef ELYSIAN_COLLISION_SYSTEM_HPP
#define ELYSIAN_COLLISION_SYSTEM_HPP

#include <gyro.h>
#include <gyro_kinetic.h>
#include "elysian_system.hpp"
#include "elysian_tile.hpp"
#include "elysian_collidable.hpp"

#define ELYSIAN_COLLISION_SYSTEM_PROTECTED					true
#define ELYSIAN_COLLISION_CONTACT_LIST_SIZE					100
#define ELYSIAN_COLLISION_PREV_CONTACT_LIST_SIZE			100
#define ELYSIAN_COLLISION_CONTACT_RESOLUTION_MAX_ITERATIONS 3000
#define ELYSIAN_COLLISION_CONTACT_PENETRATION_EPSILON		0.5f

namespace elysian {
	
	class CollisionSystem: public System { 
	public:
		enum RENDER_LAYER {
			RENDER_DISABLED,
			RENDER_LAYER_1,
			RENDER_LAYER_2,
			RENDER_LAYER_3,
			RENDER_COMBINED,
			RENDER_MAX
        };
	private:

		CollisionContact _contactPool[ELYSIAN_COLLISION_CONTACT_LIST_SIZE];
		
		/* Stores minimal collision information from the previous frame ONLY for
		 * entities that have collision triggers */
		struct PrevContact {
			Collidable* collidable[2];
		} _prevContactPool[ELYSIAN_COLLISION_PREV_CONTACT_LIST_SIZE];

		unsigned		_contactAmount		= 0;
		unsigned		_prevContactAmount	= 0;
		unsigned		_conResIter			= 0;

		RENDER_LAYER	_renderLayer		= RENDER_DISABLED;
		bool			_enabled			= true;

		void renderEntities(void);
		void detectEntityCollision(const unsigned i);
		void detectTerrainCollision(Collidable &col);
		bool resolvePenetration(void); //true if resolved, false if ran out of attempts
		bool resolveVelocities(void);

		void _prepareGeometry(void);
		void _checkTriggers(void);
		void _detectCollisions(void);
        void _applyTileColliderTransform(const Tile::COLLIDER_TYPE colType) const;

		void _detectCollidableColliderCollision(Collidable &collidable, GYCollider &collider);
		void _detectEntityTileCollisionFine(Collidable &collidable, const unsigned x, const unsigned y);
		void _detectEntityTileCollisionCoarse(Collidable &collidable);

	public:

		void update(void);
		void render(void); 
        void renderTileColliders(void) const;
		//void PrepareGeometry(void);
		//void DetectCollisions(void);
		//void ResolveContacts(void);

		void clearContacts(void);
		unsigned getContactCount(void) const;
		unsigned getPenResIterUsed(void) const;
		CollisionContact* getContact(const unsigned index);

		void toggleCollision(void);
		RENDER_LAYER cycleRenderLayer(void);

		bool checkCollision(const Entity &entity1, const Entity &entity2) const;
		bool checkCollision(const Collidable &col1, const Collidable &col2) const;
		bool previouslyColliding(const Collidable &col1, const Collidable &col2) const;
		
	};


	//==== INLINEZ ====
	inline void CollisionSystem::clearContacts(void) { _contactAmount = 0; _prevContactAmount = 0; }
	inline void CollisionSystem::toggleCollision(void) { _enabled = !_enabled; }

	inline bool CollisionSystem::previouslyColliding(const Collidable& col1, const Collidable& col2) const {
		for(unsigned i = 0; i < _prevContactAmount; ++i) {
			const PrevContact &con = _prevContactPool[i];
			if((con.collidable[0] == &col1 && con.collidable[1] == &col2) ||
				(con.collidable[0] == &col2 && con.collidable[1] == &col1)) 
				return true;
		}
		
		return false;
	}

	inline unsigned CollisionSystem::getContactCount(void) const { return _contactAmount; }
	inline CollisionContact* CollisionSystem::getContact(const unsigned index) { return &_contactPool[index]; }
	inline unsigned CollisionSystem::getPenResIterUsed(void) const { return _conResIter; }

}

#endif

#ifndef ELYSIAN_COLLISION_SYSTEM_HPP
#define ELYSIAN_COLLISION_SYSTEM_HPP

#include <map>
#include <gyro.h>
#include <gyro_kinetic.h>
#include "elysian_system.hpp"
#include "elysian_tile.hpp"
#include "elysian_collidable.hpp"

#define ELYSIAN_COLLISION_SYSTEM_PROTECTED					true
#define ELYSIAN_COLLISION_CONTACT_LIST_SIZE					100
#define ELYSIAN_COLLISION_PREV_CONTACT_LIST_SIZE			100
#define ELYSIAN_COLLISION_CONTACT_RESOLUTION_MAX_ITERATIONS 1000
#define ELYSIAN_COLLISION_CONTACT_PENETRATION_EPSILON		0.8f

namespace elysian {

	class RigidBodySystem;

	class CollisionContactEventMonitor {
	private:
		uint32_t _eventMask = 0;

	public:

		virtual void onCollisionEnter(Collidable*const bodyA, Collidable*const bodyB, CollisionContact* contact) = 0;
		virtual void onCollide(Collidable*const bodyA, Collidable*const bodyB, CollisionContact* contact) = 0;
		virtual void onCollisionExit(Collidable*const bodyA, Collidable*const bodyB) = 0;
		uint32_t getEventMask(void) const;
		void setEventMask(const uint32_t mask);
	};
	
	class CollisionSystem: public System { 
		friend class RigidBodySystem;
	public:
		enum RENDER_LAYER {
			RENDER_DISABLED,
			RENDER_LAYER_1,
			RENDER_LAYER_2,
			RENDER_LAYER_3,
			RENDER_COMBINED,
			RENDER_MAX
        };
		enum ENTITY_TILE_CHECK {
			ENTITY_TILE_CHECK_BOTTOM,
			ENTITY_TILE_CHECK_TOP,
			ENTITY_TILE_CHECK_VOLUME
		};
	private:
		std::map<uint32_t, CollisionContactEventMonitor*> _contactMonitors;
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


		void _checkTriggers(void);
        void _applyTileColliderTransform(const Tile::COLLIDER_TYPE colType) const;

		void _detectCollidableColliderCollision(Collidable &collidable, GYCollider &collider);
		void _detectEntityTileCollisionFine(Collidable &collidable, const unsigned x, const unsigned y, const unsigned char elev);
		void _detectEntityTileCollisionCoarse(Collidable &collidable, const ENTITY_TILE_CHECK checkFlag=ENTITY_TILE_CHECK_VOLUME);

	public:

		void prepareGeometry(void);
		void detectCollisions(void);
		void update(void);
		void render(void); 
        void renderTileColliders(void) const;
		//void PrepareGeometry(void);
		//void DetectCollisions(void);
		//void ResolveContacts(void);

		void registerContactEventMonitor(const uint32_t mask, CollisionContactEventMonitor*const monitor);
		void clearContacts(void);
		unsigned getContactCount(void) const;
		unsigned getPenResIterUsed(void) const;
		CollisionContact* getContact(const unsigned index);

		void setEnabled(const bool enabled);
		RENDER_LAYER cycleRenderLayer(void);

		bool checkCollision(const Entity &entity1, const Entity &entity2) const;
		bool checkCollision(const Collidable &col1, const Collidable &col2) const;
		bool previouslyColliding(const Collidable &col1, const Collidable &col2) const;
		
	};


	//==== INLINEZ ====
	inline uint32_t CollisionContactEventMonitor::getEventMask(void) const { return _eventMask; }
	inline void CollisionContactEventMonitor::setEventMask(const uint32_t mask) { _eventMask = mask; }

	inline void CollisionSystem::registerContactEventMonitor(const uint32_t mask, CollisionContactEventMonitor * const monitor) {
		_contactMonitors[mask] = monitor;
	}
	inline void CollisionSystem::clearContacts(void) { _contactAmount = 0; _prevContactAmount = 0; }
	inline void CollisionSystem::setEnabled(const bool enabled) { _enabled = enabled; }

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

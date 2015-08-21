#ifndef ELYSIAN_COLLISION_SYSTEM_HPP
#define ELYSIAN_COLLISION_SYSTEM_HPP

#include <map>
#include <gyro_kinetic.h>
#include <utility/elysian_mem_pool.hpp>
#include "elysian_system.hpp"
#include <level/elysian_tile.hpp>
#include <crtp/elysian_collidable.hpp>
#include <utility/elysian_sort_sweep.hpp>
#include <elysian_task_monitor.hpp>

#define ELYSIAN_COLLISION_SYSTEM_PROTECTED					true
#define ELYSIAN_COLLISION_BROAD_PHASE                       true
#define ELYSIAN_COLLISION_CONTACT_LIST_SIZE					10000
#define ELYSIAN_COLLISION_PREV_CONTACT_LIST_SIZE			1000
#define ELYSIAN_COLLISION_CONTACT_RESOLUTION_MAX_ITERATIONS 1000
#define ELYSIAN_COLLISION_CONTACT_PENETRATION_EPSILON		0.8f
#define ELYSIAN_COLLISION_CONTACT_Z_DEPTH_EPSILON           1.0f
#define ELYSIAN_COLLISION_CONTACT_Z_DEPTH_MAX               16.0f

namespace elysian {

	class RigidBodySystem;

	class CollisionContactEventMonitor {
	private:
		uint32_t _eventMask = 0;

	public:

        virtual void    onCollisionEnter(Collidable*const bodyA, Collidable*const bodyB, CollisionContact* contact) = 0;
        virtual void    onCollide(Collidable*const bodyA, Collidable*const bodyB, CollisionContact* contact) = 0;
        virtual void    onCollisionExit(Collidable*const bodyA, Collidable*const bodyB) = 0;
        uint32_t        getEventMask(void) const;
        void            setEventMask(const uint32_t mask);
	};
	

	class CollisionSystem: public System { 
		friend class RigidBodySystem;
        friend class CollidableBase;
        friend class DebugBase;
        friend Debug;
	public:
		enum RENDER_LAYER {
			RENDER_LAYER_1,
			RENDER_LAYER_2,
			RENDER_LAYER_3,
			RENDER_COMBINED,
            RENDER_DISABLED,
			RENDER_MAX
        };
		enum ENTITY_TILE_CHECK {
			ENTITY_TILE_CHECK_BOTTOM,
			ENTITY_TILE_CHECK_TOP,
			ENTITY_TILE_CHECK_VOLUME
		};
	private:
        std::map<uint32_t, CollisionContactEventMonitor*>
                                _contactMonitors;
        SortSweepPool           _sortSweepPool;
        CollisionContact        _contactPool[ELYSIAN_COLLISION_CONTACT_LIST_SIZE];
		
		/* Stores minimal collision information from the previous frame ONLY for
		 * entities that have collision triggers */
		struct PrevContact {
                                Collidable* collidable[2];
        }                       _prevContactPool[ELYSIAN_COLLISION_PREV_CONTACT_LIST_SIZE];

        unsigned                _contactAmount		= 0;
        unsigned                _prevContactAmount	= 0;
        unsigned                _conResIter			= 0;
        unsigned                _entityEntityChecks = 0;
        bool                    _broadPhase         = ELYSIAN_COLLISION_BROAD_PHASE;

        TaskMonitor             _detectionMonitor;
        TaskMonitor             _tileDetectMonitor;
        TaskMonitor             _entityDetectMonitor;

        RENDER_LAYER            _renderLayer		= RENDER_DISABLED;
        bool                    _enabled			= true;

        void                    renderEntities(void);
        void                    detectEntityCollision(const unsigned i);
        void                    detectTerrainCollision(Collidable &col);
        bool                    resolvePenetration(void); //true if resolved, false if ran out of attempts

        void                    _resolveZPenetration(CollisionContact*const contact);
        void                    _resolveXYPenetration(CollisionContact*const contact);

        void                    _checkTriggers(void);
        void                    _applyTileColliderTransform(const Tile::COLLIDER_TYPE colType) const;

        bool                    _detectEntityEntityCollision(Entity*const entity1, Entity*const entity2);
        void                    _detectCollidableColliderCollision(Collidable &collidable, GYCollider &collider);
        void                    _detectEntityTileCollisionFine(Collidable &collidable, const unsigned x, const unsigned y, const unsigned char elev);
        void                    _detectEntityTileZCollisionCoarse(Collidable &collidable);
        void                    _detectEntityTileCollisionCoarse(Collidable &collidable, const ENTITY_TILE_CHECK checkFlag=ENTITY_TILE_CHECK_VOLUME);


	public:
                                CollisionSystem(void);
        void                    prepareGeometry(void);
        void                    detectCollisions(void);
        void                    update(void);
        void                    render(void);
        void                    renderTileColliders(void) const;

        void                    onAreaEnter(void);
        void                    onAreaExit(void);

        void                    registerContactEventMonitor(const uint32_t mask, CollisionContactEventMonitor*const monitor);
        void                    unregisterContactEventMonitor(const uint32_t mask);
        void                    clearContacts(void);
        unsigned                getContactCount(void) const;
        unsigned                getPenResIterUsed(void) const;
        CollisionContact*       getContact(const unsigned index);
        unsigned                getEntityEntityChecks(void) const;
        double                  getDetectionTime(void) const;

        void                    setEnabled(const bool enabled);
        void                    setBroadPhase(const bool val=true);
        bool                    getBroadPhase(void) const;
        RENDER_LAYER            cycleRenderLayer(void);
        void                    setRenderLayer(const RENDER_LAYER layer);

        bool                    checkCollision(const Entity &entity1, const Entity &entity2) const;
        bool                    checkCollision(const Collidable &col1, const Collidable &col2) const;
        bool                    previouslyColliding(const Collidable &col1, const Collidable &col2) const;

        SortSweepCollidable*    addSortSweepCollidable(void);
        void                    removeSortSweepCollidable(SortSweepCollidable* body);
        bool                    detectCollidableCollidableCollision(Collidable* col1, Collidable* col2);
	};











	//==== INLINEZ ====
	inline uint32_t CollisionContactEventMonitor::getEventMask(void) const { return _eventMask; }
	inline void CollisionContactEventMonitor::setEventMask(const uint32_t mask) { _eventMask = mask; }

	inline void CollisionSystem::registerContactEventMonitor(const uint32_t mask, CollisionContactEventMonitor * const monitor) {
		_contactMonitors[mask] = monitor;
	}
    inline void CollisionSystem::unregisterContactEventMonitor(const uint32_t mask) {
        _contactMonitors.erase(mask);
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

    inline bool CollisionSystem::getBroadPhase(void) const { return _broadPhase; }
    inline void CollisionSystem::setBroadPhase(const bool val) { _broadPhase = val; }
    inline double CollisionSystem::getDetectionTime(void) const { return _entityDetectMonitor.getAvgTime(); }
	inline unsigned CollisionSystem::getContactCount(void) const { return _contactAmount; }
    inline unsigned CollisionSystem::getEntityEntityChecks(void) const { return _entityEntityChecks; }
	inline CollisionContact* CollisionSystem::getContact(const unsigned index) { return &_contactPool[index]; }
	inline unsigned CollisionSystem::getPenResIterUsed(void) const { return _conResIter; }

    inline SortSweepCollidable* CollisionSystem::addSortSweepCollidable(void) {
        return &_sortSweepPool.claimFreeEntry();
    }

    inline void CollisionSystem::removeSortSweepCollidable(SortSweepCollidable* body) {
        _sortSweepPool.removeAABB(*body);
    }

    inline void CollisionSystem::setRenderLayer(const RENDER_LAYER layer) {
        _renderLayer = layer;
    }

}

#endif

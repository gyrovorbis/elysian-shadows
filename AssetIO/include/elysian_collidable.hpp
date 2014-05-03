#ifndef ELYSIAN_COLLIDABLE_HPP
#define ELYSIAN_COLLIDABLE_HPP

#include <stdint.h>
#include <gyro.h>
#include <cstring>
#include <kinetic/gyro_kinetic.h>

#include "elysian_component.hpp"

#define ELYSIAN_COLLISION_CONTACT_EVENT_MONITOR_BEHAVIOR_MASK	(1<<0)
#define ELYSIAN_COLLISION_CONTACT_EVENT_MONITOR_WARP_MASK		(1<<1)
#define ELYSIAN_COLLISION_CONTACT_EVENT_MONITOR_COMBAT_MASK		(1<<2)

namespace elysian {

	struct CollisionFlags { 
		inline CollisionFlags(void): checkAgainst(~0x0), type(~0x0) {}

		enum TYPE {
			TILE		= 1,
			RENDERABLE	= 1<<1,
			COLLIDABLE	= 1<<2,
			CHARACTER	= 1<<3,
			WARP		= 1<<4,
			RIGID_BODY	= 1<<5,
			ITEM		= 1<<6,
			FRIENDLY	= 1<<7,
			ENEMY		= 1<<8,
			PLAYER		= 1<<9,
		//MOAR BITS
			ALL			= ~0x0
		};
		
		uint32_t checkAgainst;
		uint32_t type;
		inline void resetType(void) { type = 0; }
		inline void resetCheckAgainst(void) { checkAgainst = 0; }
		inline void addType(const uint32_t flag) { type |= flag; }
		inline void addCheckAgainst(const uint32_t flag) { checkAgainst |= flag; }
	};


class CollidableAttrib {
friend class Collidable;
public:

	virtual ~CollidableAttrib(void) { printf("~CollidabAttrib - YOU STILL NEED TO FUCKING FIX ME!!!!\n"); }
	
	CollisionFlags flags;
	
	CollidableAttrib(void);
	inline GY_COLLIDER_TYPE getColliderType(void) const { return _type; }
	inline void setColliderType(const GY_COLLIDER_TYPE type) { _type = type; }
	inline void setRelPos(const float x, const float y) { _relPos.x = x; _relPos.y = y; }
	inline void setRelSize(const float x, const float y) { _relSize.x = x; _relSize.y = y; }
	inline const GYVector3& getRelSize(void) const { return _relSize; }
	inline GYStatic *getStaticAttrib(void) { return &_staticAttrib; }
	inline CollisionFlags *getFlags(void) { return &flags; }
	
private:
        GYStatic _staticAttrib;
		GYVector3 _relPos		= { 0.0f, 0.0f, 0.0f };
		GYVector3 _relSize		= { 1.0f, 1.0f, 1.0f };
		GY_COLLIDER_TYPE _type	= GY_COLLIDER_AABB;

};
	
	class Collidable: public Component {
        friend class Entity;
#ifdef ELYSIAN_ENGINE
		friend class CollisionSystem;
		friend class RigidBodySystem;
#endif
	public:
		enum ZTRAN_STATE { ZTRAN_INACTIVE, ZTRAN_ACTIVE };
	protected:
		unsigned			_contactCount		= 0;
		CollidableAttrib*	_attrib				= nullptr;
		unsigned			_zTranTileX			= 0;
		unsigned			_zTranTileY			= 0;
		ZTRAN_STATE			_zTranState			= ZTRAN_INACTIVE;
		bool				_onZTran			= false;
		bool				_resolvePen			= true;
		bool				_active				= true;
		uint32_t			_eventMonitorMask	= 0;

		union { 
			GYCollider col;
			GYBoxCollider box;
			GYTriCollider tri;
			GYCirCollider cir;
		} _collider;

		inline Collidable(Entity &parent): 
			Component(parent)
		{}

		virtual ~Collidable(void) {}
		
	public:        
		virtual bool load(FILE* file);
		virtual bool save(FILE* file);
        virtual bool canAddTo(Entity& /*entity*/) { return true; }
		virtual bool addToLevel(void);
		virtual bool removeFromLevel(void);
		virtual Component::TYPE getType(void) const { return Component::COLLIDABLE; }

		void update(void);
        void render(void);

		inline unsigned getContactCount(void) const { return _contactCount; }
		inline void setAttrib(CollidableAttrib *attrib) { _attrib = attrib; }
		inline CollidableAttrib *getAttrib(void) const { return _attrib; }
		inline ZTRAN_STATE getZTranState(void) const { return _zTranState; }
		inline void setZTranState(const ZTRAN_STATE tran) { _zTranState = tran; }
		inline const GYVector3 &getCenterWorldPos(void) const { return _collider.col.worldPos; }
		inline void setZTranTile(unsigned x, unsigned y) { _zTranTileX = x; _zTranTileY = y; }
		inline void getZTranTile(unsigned &x, unsigned &y) const { x = _zTranTileX; y = _zTranTileY; }
		inline void setResolvePenetration(const bool v) { _resolvePen = v; }
		inline uint32_t getContactEventMonitorMask(void) const { return _eventMonitorMask; }
		inline void setContactEventMonitorMask(const uint32_t t) { _eventMonitorMask = t; }
		inline GYCollider* getCollider(void) { return &_collider.col; }
		const GYVector3& getWorldPos(void) const;
		inline void setActive(const bool val) { _active = val; }
	};

	/*
	Extend libKineticGyro's contact structure to maintain an association
	between the two GYColliders involved in the contact AND the 
	elysian::Collidables encapsulating them.
	*/

	class CollisionContact: private GYContact {
#ifdef ELYSIAN_ENGINE
		friend class CollisionSystem;
		friend class RigidBodySystem;
		Collidable* _bodyA			= nullptr;
		Collidable* _bodyB			= nullptr;
		bool _resolvePen			= true;
		uint32_t _eventMonitorMask	= 0;
#endif

		void _calculatePenetrationInternals(void);
		void _calculateVelocityInternals(const float deltaTime);

	public:
		Collidable*		getCollidable(const unsigned index) const;
		const GYStatic*	getStaticAttrib(const unsigned index) const;
		void			getWorldPos(GYVector2& pos) const;
		void			getNormal(GYVector2& _normal) const;
		void			getObjPos(const unsigned index, GYVector2& pos) const;
		float			getSeparatingVel(void) const;
		float			getClosingVel(void) const;
		float			getDeltaVel(void) const;
		float			getDepth(void) const;


	};


	// ======== INLINEZ ========

	inline const GYVector3& Collidable::getWorldPos(void) const { return _collider.col.worldPos; }

	inline CollidableAttrib::CollidableAttrib(void) {
		_staticAttrib.inverseMass = 1.0f/3000.0f;
		_staticAttrib.angDamping = 1.0f;
		_staticAttrib.damping = 1.0f;
		_staticAttrib.coeffOfRest = 0.0f;
		_staticAttrib.invMomOfInertia = 0.0f;
	}

	inline Collidable* CollisionContact::getCollidable(const unsigned index) const { return (index == 0)? _bodyA : _bodyB; }
	inline const GYStatic* CollisionContact::getStaticAttrib(const unsigned index) const { return staticAttrib[index]; }
	inline void CollisionContact::getWorldPos(GYVector2 &pos) const { pos = worldPos; }
	inline void CollisionContact::getNormal(GYVector2 &_normal) const { _normal = normal; }
	inline void CollisionContact::getObjPos(const unsigned index, GYVector2 &pos) const { pos = objPos[index]; }
	inline float CollisionContact::getSeparatingVel(void) const { return sepVel; }
	inline float CollisionContact::getClosingVel(void) const { return closingVel; }
	inline float CollisionContact::getDeltaVel(void) const { return deltaVel; }
	inline float CollisionContact::getDepth(void) const { return depth; }


}




#endif

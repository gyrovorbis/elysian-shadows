#ifndef ELYSIAN_COLLIDABLE_HPP
#define ELYSIAN_COLLIDABLE_HPP

#include <stdint.h>
#include <gyro.h>
#include <cstring>
#include <kinetic/gyro_kinetic.h>

#include "elysian_component.hpp"

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
	inline GYStatic *getStaticAttrib(void) { return &_staticAttrib; }
	inline CollisionFlags *getFlags(void) { return &flags; }
	
private:
        GYStatic _staticAttrib;
        GYVector2 _relPos;
		GYVector2 _relSize;
        GY_COLLIDER_TYPE _type;

};
	
	class Collidable: public Component {
        friend class Entity;
#ifdef ELYSIAN_ENGINE
		friend class CollisionSystem;
#endif
	public:
		enum ZTRAN_STATE { ZTRAN_INACTIVE, ZTRAN_ACTIVE };
	protected:
		GYVector4 _centerWorldPos;
		unsigned _contactCount;
		CollidableAttrib *_attrib;
		unsigned _zTranTileX, _zTranTileY;
		ZTRAN_STATE _zTranState;
		bool _onZTran;
		bool _resolvePen, _trigger;

		union { 
			GYCollider col;
			GYBoxCollider box;
			GYTriCollider tri;
			GYCirCollider cir;
		} _collider;

		inline Collidable(Entity &parent): 
			Component(parent), 
			_contactCount(0), 
			_attrib(NULL),
			_zTranTileX(0), _zTranTileY(0),
			_zTranState(ZTRAN_INACTIVE), 
			_onZTran(false),
			_resolvePen(true),
			_trigger(false)
		{
			memset(&_centerWorldPos, 0, sizeof(_centerWorldPos));
		}

		virtual ~Collidable(void) {}
		
	public:        
		virtual bool load(std::ifstream& stream);
		virtual bool save(std::ofstream& stream);
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
		inline const GYVector4 &getCenterWorldPos(void) const { return _centerWorldPos; }
		inline void setZTranTile(unsigned x, unsigned y) { _zTranTileX = x; _zTranTileY = y; }
		inline void getZTranTile(unsigned &x, unsigned &y) const { x = _zTranTileX; y = _zTranTileY; }
		inline void setResolvePenetration(const bool v) { _resolvePen = v; }
		inline bool getTrigger(void) const { return _trigger; }
		inline void setTrigger(const bool t) { _trigger = t; }
	};

	/*
	Extend libKineticGyro's contact structure to maintain an association
	between the two GYColliders involved in the contact AND the 
	elysian::Collidables encapsulating them.
	*/
	class CollisionContact: private GYContact {
#ifdef ELYSIAN_ENGINE
		friend class CollisionSystem;
#endif
		Collidable *_bodyA, *_bodyB;
		bool _resolvePen, _trigger;

		inline CollisionContact(void): 
			_bodyA(NULL), 
			_bodyB(NULL),
			_resolvePen(true),
			_trigger(false)
		{}
	};

	inline CollidableAttrib::CollidableAttrib(void): 
		_type(GY_COLLIDER_AABB) { 
		_relSize.x = _relSize.y = 1.0f; _relPos.x = _relPos.y = 0.0f; 
		_staticAttrib.inverseMass = 1.0f/3000.0f;
		_staticAttrib.angDamping = 1.0f;
		_staticAttrib.damping = 1.0f;
		_staticAttrib.coeffOfRest = 0.0f;
		_staticAttrib.invMomOfInertia = 0.0f;
	}



}




#endif

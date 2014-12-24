#ifndef ELYSIAN_COLLIDABLE_HPP
#define ELYSIAN_COLLIDABLE_HPP

#include <stdint.h>
#include <gyro.h>
#include <cstring>
#include <kinetic/gyro_kinetic.h>
#include "elysian_component.hpp"
#include "elysian_sort_sweep.hpp"

#define ELYSIAN_COLLISION_CONTACT_EVENT_MONITOR_BEHAVIOR_MASK	(1<<0)
#define ELYSIAN_COLLISION_CONTACT_EVENT_MONITOR_WARP_MASK		(1<<1)
#define ELYSIAN_COLLISION_CONTACT_EVENT_MONITOR_COMBAT_MASK		(1<<2)

namespace elysian {

    class CollisionSystem;

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
            CURSOR      = 1<<10,
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

    class CollidableStaticAttrib: public GYStatic {
    public:
        CollidableStaticAttrib(void);
        void setDamping(const float xy);
        void setZDamping(const float z);
        void setDamping3f(const float x, const float y, const float z);
    };


class CollidableAttrib {
friend class Collidable;
public:

	virtual ~CollidableAttrib(void) { printf("~CollidabAttrib - YOU STILL NEED TO FUCKING FIX ME!!!!\n"); }
	
	CollisionFlags flags;
	
    CollidableAttrib(void) = default;
	inline GY_COLLIDER_TYPE getColliderType(void) const { return _type; }
	inline void setColliderType(const GY_COLLIDER_TYPE type) { _type = type; }
    inline void setRelPos(const float x, const float y, const float z) { _relPos.x = x; _relPos.y = y; _relPos.z = z; /* was fucked? No setting of Z?*/ }
    inline void setRelSize(const float x, const float y, const float z) { _relSize.x = x; _relSize.y = y; _relSize.z = z; }
	inline const GYVector3& getRelSize(void) const { return _relSize; }
    inline CollidableStaticAttrib *getStaticAttrib(void) { return &_staticAttrib; }
	inline CollisionFlags *getFlags(void) { return &flags; }
	
private:
        CollidableStaticAttrib  _staticAttrib;
		GYVector3 _relPos		= { 0.0f, 0.0f, 0.0f };
		GYVector3 _relSize		= { 1.0f, 1.0f, 1.0f };
		GY_COLLIDER_TYPE _type	= GY_COLLIDER_AABB;

};
	
	class Collidable: public Component {
        friend Entity;
        friend class EntityBase;
		friend class CollisionSystem;
		friend class RigidBodySystem;

	public:
		enum ZTRAN_STATE { ZTRAN_INACTIVE, ZTRAN_ACTIVE };
	protected:
        static CollisionSystem* _colSys;

        GYVector3           _worldSize          = { 1.0f, 1.0f, 1.0f, };
		CollidableAttrib*	_attrib				= nullptr;
        SortSweepCollidable* _sortSweepAABB     = nullptr;

        unsigned			_zTranTileX			= 0;
        unsigned			_zTranTileY			= 0;
        unsigned			_contactCount		= 0;
        ZTRAN_STATE			_zTranState			= ZTRAN_INACTIVE;
        bool                _stackedOnGround    = false;
        bool                _restingOnBody      = false;
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
        void    _updateSortSweepAABB(void);
		
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
        const GYVector3& getWorldSize(void) const { return _worldSize; }
		inline void setActive(const bool val) { _active = val; }
        bool            getRestingOnBody(void) const;
        void            setRestingOnBody(const bool v);
        bool            getStackedOnGround(void) const;
        void            setStackedOnGround(const bool v);

        void            setSortSweepAABB(SortSweepCollidable* sweep);
        bool            checkAABBCollision(Collidable* col);
        bool            checkFullCollision(Collidable* col);

	};

	/*
	Extend libKineticGyro's contact structure to maintain an association
	between the two GYColliders involved in the contact AND the 
	elysian::Collidables encapsulating them.
	*/

	class CollisionContact: private GYContact {
		friend class CollisionSystem;
		friend class RigidBodySystem;
       // Collidable* _body[2]            = nullptr;
        Collidable* _bodyA = nullptr;
        Collidable* _bodyB = nullptr;
        float       _zDepth             = 0.0f;
        uint32_t    _eventMonitorMask	= 0;
        bool        _resolvePen			= true;
        bool        _resolveZ           = false;

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
        void            setZDepth(const float depth);
        float           getZDepth(void) const;
        void            setResolveZ(const bool val=true);
        bool            getResolveZ(void) const;
        void            updateRelativeZDepth(const unsigned body, const float zDelta);
	};


	// ======== INLINEZ ========

    inline bool Collidable::getStackedOnGround(void) const { return _stackedOnGround; }
    inline bool Collidable::getRestingOnBody(void) const { return _restingOnBody; }
    inline void Collidable::setStackedOnGround(const bool v) { _stackedOnGround = v; }
    inline void Collidable::setRestingOnBody(const bool v) { _restingOnBody = v; }

    inline CollidableStaticAttrib::CollidableStaticAttrib(void):
        GYStatic {
            { 0.5f, 0.5f, 0.5f },   //damping
            1.0f/3000.0f,           //invMass
            0.1f,                   //angDamping
            0.0f,                   //coeffOfRest
            0.0f,                   //invMomOfInertia
            0.0f                    //zCoeffOfRest
        } {}
    inline void CollidableStaticAttrib::setDamping(const float xy) { damping.x = damping.y = xy; }
    inline void CollidableStaticAttrib::setDamping3f(const float x, const float y, const float z) { damping.x = x; damping.y = y; damping.z = z; }
    inline void CollidableStaticAttrib::setZDamping(const float z) { damping.z = z; }


	inline const GYVector3& Collidable::getWorldPos(void) const { return _collider.col.worldPos; }

	inline Collidable* CollisionContact::getCollidable(const unsigned index) const { return (index == 0)? _bodyA : _bodyB; }
	inline const GYStatic* CollisionContact::getStaticAttrib(const unsigned index) const { return staticAttrib[index]; }
	inline void CollisionContact::getWorldPos(GYVector2 &pos) const { pos = worldPos; }
	inline void CollisionContact::getNormal(GYVector2 &_normal) const { _normal = normal; }
	inline void CollisionContact::getObjPos(const unsigned index, GYVector2 &pos) const { pos = objPos[index]; }
	inline float CollisionContact::getSeparatingVel(void) const { return sepVel; }
	inline float CollisionContact::getClosingVel(void) const { return closingVel; }
	inline float CollisionContact::getDeltaVel(void) const { return deltaVel; }
	inline float CollisionContact::getDepth(void) const { return depth; }
    inline void CollisionContact::setZDepth(const float depth) { _zDepth = depth; }
    inline float CollisionContact::getZDepth(void) const { return _zDepth; }
    inline void CollisionContact::setResolveZ(const bool val) { _resolveZ = val; }
    inline bool CollisionContact::getResolveZ(void) const { return _resolveZ; }

    inline void CollisionContact::updateRelativeZDepth(const unsigned /*body*/, const float /*zDelta*/) {
   /*     Collidable* col[2];

        float coeff;

        if(body == _body[0]) coeff = 1.0f;
        else coeff = -1.0f;

*/

    }

    inline bool Collidable::checkAABBCollision(Collidable* col) {
        SortSweepCollidable* b = col->_sortSweepAABB;
        auto* a = _sortSweepAABB;

        return (((a->min[0] > b->min[0] && a->min[0] < b->max[0]) || (b->min[0] > a->min[0] && b->min[0] < a->max[0])) &&
                ((a->min[1] > b->min[1] && a->min[1] < b->max[1]) || (b->min[1] > a->min[1] && b->min[1] < a->max[1])));// &&
               // ((a->min[2] > b->min[2] && a->min[2] < b->max[2]) || (b->min[2] > a->min[2] && b->min[2] < a->max[2])));
    }
\

    inline void Collidable::setSortSweepAABB(SortSweepCollidable* sweep) {
        _sortSweepAABB = sweep;
    }

}




#endif

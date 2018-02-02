#ifndef ELYSIAN_COLLIDABLE_BASE_HPP
#define ELYSIAN_COLLIDABLE_BASE_HPP


#include <stdint.h>
#include <cstring>
#include <cstdint>
#include <limits>
#include <kinetic/gyro_kinetic_collider.h>
#include <kinetic/gyro_kinetic_static.h>
#include <kinetic/gyro_kinetic_contact.h>
#include <crtp/elysian_component.hpp>
#include <utility/elysian_sort_sweep.hpp>
#include <primitive/elysian_instance_transform.hpp>
#include <crtp/elysian_lua_table_proxy.hpp>
#include <utility/elysian_bit_mask.hpp>
#include <memory>

#define ELYSIAN_COLLISION_CONTACT_EVENT_MONITOR_BEHAVIOR_MASK	(1u<<0)
#define ELYSIAN_COLLISION_CONTACT_EVENT_MONITOR_WARP_MASK		(1u<<1)
#define ELYSIAN_COLLISION_CONTACT_EVENT_MONITOR_COMBAT_MASK		(1u<<2)

ELYSIAN_ASSET_TYPEDEF(CollidableAttrib)

namespace elysian {

    class CollisionSystem;

    struct CollisionFlags: public LuaTableProxy<CollisionFlags> {

        ELYSIAN_LUA_PROXY_FIELDS_BEGIN()
            ELYSIAN_LUA_PROXY_FIELD(type,           getType,        setType)
            ELYSIAN_LUA_PROXY_FIELD(typeAgainst,    getTypeAgainst, setTypeAgainst)
        ELYSIAN_LUA_PROXY_FIELDS_END()

        enum TYPE {
            TILE		= 1u,
            RENDERABLE	= 1u<<1,
            COLLIDABLE	= 1u<<2,
            CHARACTER	= 1u<<3,
            WARP		= 1u<<4,
            RIGID_BODY	= 1u<<5,
            ITEM		= 1u<<6,
            FRIENDLY	= 1u<<7,
            ENEMY		= 1u<<8,
            PLAYER		= 1u<<9,
            CURSOR      = 1u<<10,
        //MOAR BITS
            ALL			= ~0x0
        };

        BitMask32       _typeAgainst;
        BitMask32       _type;

                            CollisionFlags(void) = default;
        BitMask32&          getType(void);
        BitMask32&          getTypeAgainst(void);
        void                setType(const BitMask32 flags);
        void                setTypeAgainst(const BitMask32 flags);
    };


    class CollidableStaticAttrib:
            public GYStatic,
            public LuaTableProxy<CollidableStaticAttrib>
    {
    public:
        ELYSIAN_LUA_PROXY_FIELDS_BEGIN()
            ELYSIAN_LUA_PROXY_FIELD(invMass,            getInverseMass,         setInverseMass)
            ELYSIAN_LUA_PROXY_FIELD(coeffOfRest,        getCoeffOfRest,         setCoeffOfRest)
            ELYSIAN_LUA_PROXY_FIELD(invMomOfInertia,    getInvMomOfInertia,     setInvMomOfInertia)
            ELYSIAN_LUA_PROXY_FIELD(angDamping,         getAngDamping,          setAngDamping)
            ELYSIAN_LUA_PROXY_FIELD(zCoeffOfRest,       getZCoeffOfRest,        setZCoeffOfRest)
            ELYSIAN_LUA_PROXY_FIELD(damping,            getDampingVec,          setDampingVec)
        ELYSIAN_LUA_PROXY_FIELDS_END()

                    CollidableStaticAttrib(void);
        void        setDamping(const float xy);
        void        setZDamping(const float z);
        void        setDamping3f(const float x, const float y, const float z);

        Vector3     getDampingVec(void);
        void        setDampingVec(const Vector3& vec);
        float       getInverseMass(void) const;
        void        setInverseMass(const float mass);
        float       getCoeffOfRest(void) const;
        void        setCoeffOfRest(const float coeff);
        float       getInvMomOfInertia(void) const;
        void        setInvMomOfInertia(const float inertia);
        float       getAngDamping(void) const;
        void        setAngDamping(const float damping);
        float       getZCoeffOfRest(void) const;
        void        setZCoeffOfRest(const float coeff);
    };



    class CollidableAttribBase: public LuaTableProxy<CollidableAttribBase, CollidableAttrib> {
    friend Collidable;
    friend class CollidableBase;
    public:

    ELYSIAN_LUA_PROXY_FIELDS_BEGIN()
        ELYSIAN_LUA_PROXY_FIELD(colliderType,       getColliderType,       setColliderType)
        ELYSIAN_LUA_PROXY_FIELD(relPos,             getRelPosVec,          setRelPosVec)
        ELYSIAN_LUA_PROXY_FIELD(relSize,            getRelSizeVec,         setRelSizeVec)
        ELYSIAN_LUA_PROXY_FIELD(staticAttrib,       getStaticAttrib,       setStaticAttrib)
        ELYSIAN_LUA_PROXY_FIELD(flags,              getFlags,              setFlags)
    ELYSIAN_LUA_PROXY_FIELDS_END()

        CollisionFlags          flags;

                                CollidableAttribBase(void) = default;
        GY_COLLIDER_TYPE        getColliderType(void) const;
        void                    setColliderType(const GY_COLLIDER_TYPE type);
        void                    setRelPos(const float x, const float y, const float z);
        void                    setRelSize(const float x, const float y, const float z);
        const GYVector3&        getRelSize(void) const;
        Vector3                 getRelSizeVec(void);
        Vector3                 getRelPosVec(void);
        void                    setRelSizeVec(const Vector3& size);
        void                    setRelPosVec(const Vector3& pos);
        CollidableStaticAttrib* getStaticAttrib(void);
        void                    setStaticAttrib(const OOLUA::Lua_table_ref valTable);
        CollisionFlags*         getFlags(void);
        void                    setFlags(const OOLUA::Lua_table_ref valTable);

    private:
        CollidableStaticAttrib  _staticAttrib;
        GYVector3               _relPos         = { 0.0f, 0.0f, 0.0f };
        GYVector3               _relSize		= { 1.0f, 1.0f, 1.0f };
        GY_COLLIDER_TYPE        _type           = GY_COLLIDER_AABB;

    };


    class CollidableBase:
        public Component,
        public LuaTableProxy<CollidableBase, Collidable>
    {
        friend Entity;
        friend class EntityBase;
        friend class CollisionSystem;
        friend class RigidBodySystem;

    public:
        ELYSIAN_LUA_PROXY_FIELDS_BEGIN()
            ELYSIAN_LUA_PROXY_FIELD(active,             isActive,               setActive)
            ELYSIAN_LUA_PROXY_FIELD(contactCount,       getContactCount)
            ELYSIAN_LUA_PROXY_FIELD(stackedOnGround,    getStackedOnGround)
            ELYSIAN_LUA_PROXY_FIELD(restingOnBody,      getRestingOnBody)
            ELYSIAN_LUA_PROXY_FIELD(worldSpace,         getWorldSpace,          setWorldSpace)
            ELYSIAN_LUA_PROXY_FIELD(resolvePen,         getResolvePenetration,  setResolvePenetration)
            ELYSIAN_LUA_PROXY_FIELD(attrib,             getAttrib,              setAttrib)
        ELYSIAN_LUA_PROXY_FIELDS_END()
    protected:
        static CollisionSystem* _colSys;

        GYVector3                           _worldSize          = { 1.0f, 1.0f, 1.0f, };
        std::shared_ptr<CollidableAttrib>   _attrib;
        SortSweepCollidable*                _sortSweepAABB      = nullptr;

        unsigned                            _contactCount		= 0;
        bool                                _stackedOnGround    = false;
        bool                                _restingOnBody      = false;
        bool                                _resolvePen			= true;
        bool                                _active				= true;
        bool                                _worldSpace         = true;
        uint32_t                            _eventMonitorMask	= 0;

        union {
            GYCollider          col;
            GYBoxCollider       box;
            GYTriCollider       tri;
            GYCirCollider       cir;
        }                       _collider;

                                CollidableBase(Entity &parent, CollidableBase*const prefab=nullptr);
                                ~CollidableBase(void) = default;
        void                    _updateSortSweepAABB(void);

    public:
        //Overridden from Component
                                CollidableBase(void) = default;

        static constexpr const TYPE componentType = COLLIDABLE;


        void                    update(void);
        void                    render(void);

        void                    activate(void);
        void                    deactivate(void);

        unsigned                getContactCount(void) const;
        void                    setAttrib(std::shared_ptr<CollidableAttrib> attrib);
        auto                    getAttrib(void) const -> std::shared_ptr<CollidableAttrib>;
        const GYVector3&        getCenterWorldPos(void) const;
        void                    setResolvePenetration(const bool v);
        bool                    getResolvePenetration(void) const;
        uint32_t                getContactEventMonitorMask(void) const;
        void                    setContactEventMonitorMask(const uint32_t t);
        GYCollider*             getCollider(void);
        const GYVector3&        getWorldPos(void) const;
        const GYVector3&        getWorldSize(void) const;
        void                    setWorldSize(const Vector3& size);
        void                    setWorldPos(const Vector3& pos);
        void                    setActive(const bool val);
        bool                    isActive(void) const;
        bool                    getRestingOnBody(void) const;
        void                    setRestingOnBody(const bool v);
        bool                    getStackedOnGround(void) const;
        void                    setStackedOnGround(const bool v);
        bool                    getWorldSpace(void) const;
        void                    setWorldSpace(const bool val);

        void                    setSortSweepAABB(SortSweepCollidable* sweep);
        SortSweepCollidable*    getSortSweepAABB(void) const;
        bool                    checkAABBCollision(Collidable* col);
        bool                    checkFullCollision(Collidable* col);

        CollidableBase&         operator= (CollidableBase& other);

    };



    /*
    Extend libKineticGyro's contact structure to maintain an association
    between the two GYColliders involved in the contact AND the
    elysian::Collidables encapsulating them.
    */

    class CollisionContact: private GYContact {
        friend class CollisionSystem;
        friend class RigidBodySystem;
        Collidable*     _bodyA              = nullptr;
        Collidable*     _bodyB              = nullptr;
        float           _zDepth             = 0.0f;
        uint32_t        _eventMonitorMask	= 0;
        bool            _resolvePen			= true;
        bool            _resolveZ           = false;

        void            _calculatePenetrationInternals(void);
        void            _calculateVelocityInternals(const float deltaTime);

    public:
        Collidable*		getCollidable(const unsigned index) const;
        int             getBodyIndex(const Collidable*const body) const;
        const GYStatic*	getStaticAttrib(const unsigned index) const;
        void			getWorldPos(GYVector2& pos) const;
        void			getNormal(GYVector2& _normal) const;
        void            getNormal(const unsigned body, GYVector2& _normal) const;
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
    inline BitMask32& CollisionFlags::getType(void){ return _type; }
    inline BitMask32& CollisionFlags::getTypeAgainst(void) { return _typeAgainst; }
    inline void CollisionFlags::setType(const BitMask32 flags) { _type = flags; }
    inline void CollisionFlags::setTypeAgainst(const BitMask32 flags) { _typeAgainst = flags; }

    inline GY_COLLIDER_TYPE CollidableAttribBase::getColliderType(void) const { return _type; }
    inline void CollidableAttribBase::setColliderType(const GY_COLLIDER_TYPE type) { _type = type; }
    inline void CollidableAttribBase::setRelPos(const float x, const float y, const float z) { _relPos.x = x; _relPos.y = y; _relPos.z = z; /* was fucked? No setting of Z?*/ }
    inline void CollidableAttribBase::setRelSize(const float x, const float y, const float z) { _relSize.x = x; _relSize.y = y; _relSize.z = z; }
    inline const GYVector3& CollidableAttribBase::getRelSize(void) const { return _relSize; }
    inline Vector3 CollidableAttribBase::getRelSizeVec(void) { return Vector3(&_relSize); }
    inline Vector3 CollidableAttribBase::getRelPosVec(void) { return Vector3(&_relPos); }
    inline CollidableStaticAttrib* CollidableAttribBase::getStaticAttrib(void) { return &_staticAttrib; }
    inline CollisionFlags* CollidableAttribBase::getFlags(void) { return &flags; }
    inline void CollidableAttribBase::setRelSizeVec(const Vector3& size) { _relSize = size; }
    inline void CollidableAttribBase::setRelPosVec(const Vector3& pos) { _relPos = pos; }
    inline CollidableBase::CollidableBase(Entity &parent, CollidableBase*const prefab): Component(parent) {
        if(prefab) *this = *prefab;
    }
    inline unsigned CollidableBase::getContactCount(void) const { return _contactCount; }
    inline void CollidableBase::setAttrib(std::shared_ptr<CollidableAttrib> attrib) { _attrib = attrib; }
    inline std::shared_ptr<CollidableAttrib> CollidableBase::getAttrib(void) const { return _attrib; }
    inline const GYVector3& CollidableBase::getCenterWorldPos(void) const { return _collider.col.worldPos; }
    inline void CollidableBase::setResolvePenetration(const bool v) { _resolvePen = v; }
    inline bool CollidableBase::getResolvePenetration(void) const { return _resolvePen; }
    inline uint32_t CollidableBase::getContactEventMonitorMask(void) const { return _eventMonitorMask; }
    inline void CollidableBase::setContactEventMonitorMask(const uint32_t t) { _eventMonitorMask = t; }
    inline GYCollider* CollidableBase::getCollider(void) { return &_collider.col; }
    inline const GYVector3& CollidableBase::getWorldPos(void) const { return _collider.col.worldPos; }
    inline const GYVector3& CollidableBase::getWorldSize(void) const { return _worldSize; }
    inline void CollidableBase::setActive(const bool val) { _active = val; }
    inline bool CollidableBase::isActive(void) const { return _active; }
    inline bool CollidableBase::getStackedOnGround(void) const { return _stackedOnGround; }
    inline bool CollidableBase::getRestingOnBody(void) const { return _restingOnBody; }
    inline void CollidableBase::setStackedOnGround(const bool v) { _stackedOnGround = v; }
    inline void CollidableBase::setRestingOnBody(const bool v) { _restingOnBody = v; }
    inline bool CollidableBase::getWorldSpace(void) const { return _worldSpace; }
    inline void CollidableBase::setWorldSpace(const bool val) { _worldSpace = val; }

    inline CollidableStaticAttrib::CollidableStaticAttrib(void):
        GYStatic {
            { 0.1f, 0.1f, 0.1f },   //damping
            1.0f/3000.0f,           //invMass
            0.0f,                   //angDamping
            0.0f,                   //coeffOfRest
            0.0f,                   //invMomOfInertia
            0.0f                    //zCoeffOfRest
        } {}
    inline void CollidableStaticAttrib::setDamping(const float xy) { damping.x = damping.y = xy; }
    inline void CollidableStaticAttrib::setDamping3f(const float x, const float y, const float z) { damping.x = x; damping.y = y; damping.z = z; }
    inline void CollidableStaticAttrib::setZDamping(const float z) { damping.z = z; }
    inline Vector3 CollidableStaticAttrib::getDampingVec(void) { return Vector3(&damping); }
    inline void CollidableStaticAttrib::setDampingVec(const Vector3& vec) { damping = vec; }
    inline float CollidableStaticAttrib::getInverseMass(void) const { return inverseMass; }
    inline void CollidableStaticAttrib::setInverseMass(const float mass) { inverseMass = mass; }
    inline float CollidableStaticAttrib::getCoeffOfRest(void) const { return coeffOfRest; }
    inline void CollidableStaticAttrib::setCoeffOfRest(const float coeff) { coeffOfRest = coeff; }
    inline float CollidableStaticAttrib::getInvMomOfInertia(void) const { return invMomOfInertia; }
    inline void CollidableStaticAttrib::setInvMomOfInertia(const float inertia) { invMomOfInertia = inertia; }
    inline float CollidableStaticAttrib::getAngDamping(void) const { return angDamping; }
    inline void  CollidableStaticAttrib::setAngDamping(const float damping) { angDamping = damping; }
    inline float CollidableStaticAttrib::getZCoeffOfRest(void) const { return zCoeffOfRest; }
    inline void CollidableStaticAttrib::setZCoeffOfRest(const float coeff) { zCoeffOfRest = coeff; }



    inline int CollisionContact::getBodyIndex(const Collidable*const body) const {
        if(body == _bodyA) return 0;
        else if(body == _bodyB) return 1;
        else return -1;
    }
    inline Collidable* CollisionContact::getCollidable(const unsigned index) const { return (index == 0)? _bodyA : _bodyB; }
    inline const GYStatic* CollisionContact::getStaticAttrib(const unsigned index) const { return staticAttrib[index]; }
    inline void CollisionContact::getWorldPos(GYVector2 &pos) const { pos = worldPos; }
    inline void CollisionContact::getNormal(GYVector2 &_normal) const { _normal = normal; }
    inline void CollisionContact::getNormal(const unsigned body, GYVector2 &_normal) const { gyVec2Scale(&_normal, &normal, body? -1.0f : 1.0f); }
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
    inline CollidableBase& CollidableBase::operator=(CollidableBase& rhs) {
        _worldSize          = rhs._worldSize;
        _attrib             = rhs._attrib;
        _resolvePen         = rhs._resolvePen;
        _active             = rhs._active;
        _worldSpace         = rhs._worldSpace;
        _eventMonitorMask   = rhs._eventMonitorMask;
        _collider           = rhs._collider;
        return *this;
    }

}

#endif // ELYSIAN_COLLIDABLE_BASE_HPP


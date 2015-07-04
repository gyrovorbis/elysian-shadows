#ifndef ELYSIAN_RIGID_BODY_BASE_HPP
#define ELYSIAN_RIGID_BODY_BASE_HPP

#include <stdint.h>
#include <kinetic/gyro_kinetic_kinetic.h>
#include <kinetic/gyro_kinetic_static.h>
#include <elysian_component.hpp>
#include <elysian_lua_table_proxy.hpp>
#include <elysian_instance_transform.hpp>

namespace elysian {

    class CollisionContact;

    class RigidBodyAttrib: public LuaTableProxy<RigidBodyAttrib> {	//later separate GYStatic attrib
    private:
        uint32_t	_forceMask		= ~0x0;
        bool		_breakable		= false;
        float		_baseDurability = -1;
        bool        _canHold        = true;

    public:
        ELYSIAN_LUA_PROXY_FIELDS_BEGIN()
            ELYSIAN_LUA_PROXY_FIELD(canHold,    getCanHold,         setCanHold)
            ELYSIAN_LUA_PROXY_FIELD(breakable,  isBreakable,        setBreakable)
            ELYSIAN_LUA_PROXY_FIELD(durability, getBaseDurability,  setBaseDurability)
            ELYSIAN_LUA_PROXY_FIELD(forceMask,  getForceMask,       setForceMask)
        ELYSIAN_LUA_PROXY_FIELDS_END()

        uint32_t	getForceMask(void) const;
        void		setForceMask(const uint32_t mask);
        void		setBreakable(const bool val);
        bool		isBreakable(void) const;
        float		getBaseDurability(void) const;
        void		setBaseDurability(const float val);
        bool        getCanHold(void) const;
        void        setCanHold(const bool val=true);
    };

    //Abstraction for particles and rigidBodies to share force generators
    class ForceTarget:
            protected GYKinetic,
            public LuaTableProxy<ForceTarget>
    {
	public:

        ELYSIAN_LUA_PROXY_FIELDS_BEGIN()
            ELYSIAN_LUA_PROXY_FIELD(vel,            getVel)
            ELYSIAN_LUA_PROXY_FIELD(accel,          getAccel)
            ELYSIAN_LUA_PROXY_FIELD(forceAccum,     getForceAccum)
            ELYSIAN_LUA_PROXY_FIELD(rotVel,         getRotation,    setRotation)
            ELYSIAN_LUA_PROXY_FIELD(angAccel,       getAngAccel,    setAngAccel)
            ELYSIAN_LUA_PROXY_FIELD(torqueAccum,    getTorqueAccum, setTorqueAccum)
        ELYSIAN_LUA_PROXY_FIELDS_END()

		enum TARGET_TYPE { TARGET_PARTICLE, TARGET_RIGID_BODY };

                                    ForceTarget(void);
        virtual TARGET_TYPE			getTargetType(void) const			= 0;
        virtual float				getOrientation(void) const			= 0;
        virtual const GYVector3&	getPosition(void) const				= 0;
        virtual const GYStatic*		getStaticAttrib(void) const			= 0;
        virtual bool               	getRestingOnBody(void) const        = 0;

        void                        applyZForce(const GYVector3& force);
        void						applyForce(const GYVector3& force);
        void						applyForceToPoint(const GYVector2& loc, const GYVector2& force);
        GYVector3       			getVelocity(void) const;
        void						setVelocity(const GYVector3& _vel);
        void						clearKinetic(void);

        void						setRotation(const float rotation);
        float                       getRotation(void) const;
        void                        setAngAccel(const float accel);
        float                       getAngAccel(void) const;
        void                        setTorqueAccum(const float torque);
        float                       getTorqueAccum(void) const;
        Vector3                     getVel(void);
        Vector3                     getAccel(void);
        Vector3                     getForceAccum(void);
    };

    class CollisionSystem;

    class RigidBodyBase:
        public Component,
        public ForceTarget,
        public LuaTableProxy<RigidBodyBase, RigidBody, ForceTarget>
    {
        friend class RigidBodySystem;
        friend class CollisionSystem;
        friend RigidBody;
	private:
		float				_tileDamping	= 1.0f;
		bool				_active			= true;

        RigidBodyAttrib*	_attrib			= nullptr;
        float				_durability		= -1.0f;
        CollisionContact*	_breakContact	= nullptr;
    public:

        ELYSIAN_LUA_PROXY_FIELDS_BEGIN()
            ELYSIAN_LUA_PROXY_FIELD(tileDamping,    getTileDamping)
            ELYSIAN_LUA_PROXY_FIELD(durability,     getDurability,  setDurability)
            ELYSIAN_LUA_PROXY_FIELD(attrib,         getAttrib,      setAttrib)
        ELYSIAN_LUA_PROXY_FIELDS_END()

        using                       LuaTableProxy<RigidBodyBase, RigidBody, ForceTarget>::registerProxyMetaTable;
        using                       LuaTableProxy<RigidBodyBase, RigidBody, ForceTarget>::fromTable;
                                    RigidBodyBase(Entity& entity);
                                    RigidBodyBase(void) = default;
        virtual                     ~RigidBodyBase(void) override = default;

		//reimplemented from Component
        static constexpr const Component::TYPE componentType = Component::RIGID_BODY;

        //reimplemented from ForceTarget
        virtual TARGET_TYPE			getTargetType(void)	const override;
        virtual float				getOrientation(void) const override;
        virtual const GYVector3&	getPosition(void) const override;
        virtual const GYStatic*		getStaticAttrib(void) const override;
        virtual bool                getRestingOnBody(void) const override;

        void                        setActive(const bool val);
        void                        setTileDamping(const float factor);
        float                       getTileDamping(void) const;
        void                        setAttrib(RigidBodyAttrib*const attrib);
        RigidBodyAttrib*            getAttrib(void) const;
        float                       getDurability(void) const;
        void                        setDurability(const float val);
        CollisionContact*           getBreakContact(void) const;
        void                        setBreakContact(CollisionContact*const contact);

        //GYKinetic rigid bodies internal
        void                        integrate(const float deltaTime);

        //GYKinetic rigid bodies external
        //void applyForceMask(const uint32_t mask, const GYVector2& force);

    };













    // ========= INLINEZ ==========
    inline GYVector3 ForceTarget::getVelocity(void) const { return vel; }
    inline void	ForceTarget::setVelocity(const GYVector3& _vel) { vel = _vel; }
#if 0
    inline bool ForceTarget::getOnGround(void) const { return (bool)resting; }
    inline void ForceTarget::setOnGround(const bool val) { resting = (int)val; }
#endif
    inline void ForceTarget::clearKinetic(void) {
        GYKinetic* kinetic = static_cast<GYKinetic*>(this);
        memset(kinetic, 0, sizeof(GYKinetic));
    }
    inline void	ForceTarget::setRotation(const float rotation) { rot = rotation; }
    inline float ForceTarget::getRotation(void) const { return rot; }
    inline void ForceTarget::setAngAccel(const float accel) { angAccel = accel; }
    inline float ForceTarget::getAngAccel(void) const { return angAccel; }
    inline void ForceTarget::setTorqueAccum(const float torque) { torqueAccum = torque; }
    inline float ForceTarget::getTorqueAccum(void) const { return torqueAccum; }
    inline Vector3 ForceTarget::getVel(void) { return Vector3(&vel); }
    inline Vector3 ForceTarget::getAccel(void) { return Vector3(&accel); }
    inline Vector3 ForceTarget::getForceAccum(void) { return Vector3(&forceAccum); }


    inline RigidBodyBase::RigidBodyBase(Entity& entity): Component(entity) {}
    inline float RigidBodyBase::getTileDamping(void) const { return _tileDamping; }
    inline void RigidBodyBase::setTileDamping(const float factor) {_tileDamping = factor; }
    inline void RigidBodyBase::setAttrib(RigidBodyAttrib*const attrib) { /*delete _attrib;*/ _attrib = attrib; _durability = _attrib->getBaseDurability();}
    inline void RigidBodyBase::setActive(const bool val) { _active = val; }
    inline float RigidBodyBase::getDurability(void) const { return _durability; }
    inline void RigidBodyBase::setDurability(const float val) { _durability = val; }
    inline CollisionContact* RigidBodyBase::getBreakContact(void) const { return _breakContact; }
    inline void	RigidBodyBase::setBreakContact(CollisionContact*const contact) { _breakContact = contact; }
    inline RigidBodyAttrib* RigidBodyBase::getAttrib(void) const { return _attrib; }

	//inline void RigidBody::applyForceMask(const uint32_t mask, const GYVector2& force) { if(_attrib->getForceMask() & mask) applyForce(force); }
	inline uint32_t RigidBodyAttrib::getForceMask(void) const { return _forceMask; }
	inline void RigidBodyAttrib::setForceMask(const uint32_t mask) { _forceMask = mask; }
	inline bool RigidBodyAttrib::isBreakable(void) const { return _breakable; }
	inline void RigidBodyAttrib::setBreakable(const bool val) { _breakable = val; }
	inline float RigidBodyAttrib::getBaseDurability(void) const { return _baseDurability; }
	inline void RigidBodyAttrib::setBaseDurability(const float val) { _baseDurability = val; }
    inline bool RigidBodyAttrib::getCanHold(void) const { return _canHold; }
    inline void RigidBodyAttrib::setCanHold(const bool val) { _canHold = val; }



}

#endif // ELYSIAN_RIGID_BODY_H

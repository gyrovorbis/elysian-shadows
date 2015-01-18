#ifndef ELYSIAN_RIGID_BODY_HPP
#define ELYSIAN_RIGID_BODY_HPP

#include <stdint.h>
#include <kinetic/gyro_kinetic.h>
#include "elysian_component.hpp"
#include "elysian_lua_table_proxy.hpp"

namespace elysian {

	class CollisionContact;

	class RigidBodyAttrib {	//later separate GYStatic attrib
	private:
		uint32_t	_forceMask		= ~0x0;
		bool		_breakable		= false;
		float		_baseDurability = -1;
        bool        _canHold        = true;

	public:
		uint32_t	getForceMask(void) const;
		void		setForceMask(const uint32_t mask);
		void		setBreakable(const bool val);
		bool		isBreakable(void) const;
		float		getBaseDurability(void) const;
		void		setBaseDurability(const float val);
        bool        canHold(void) const;
        void        setCanHold(const bool val=true);
	};

	//Abstraction for particles and rigidBodies to share force generators
	class ForceTarget: protected GYKinetic {
    protected:

	public:
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
		void						setRotation(const float rotation);
		void						clearKinetic(void);
	};

//DELETE ME
    class CollisionSystem;

    class RigidBody:
        public Component,
        public ForceTarget,
        public LuaTableProxy<RigidBody>
    {
		friend class RigidBodySystem;
        friend class CollisionSystem;
	private:
		float				_tileDamping	= 1.0f;
		bool				_active			= true;

		RigidBodyAttrib*	_attrib			= nullptr;
		float				_durability		= -1.0f;
		CollisionContact*	_breakContact	= nullptr;
	public:

        ELYSIAN_LUA_PROXY_FIELDS_BEGIN()
        ELYSIAN_LUA_PROXY_FIELDS_END()

                RigidBody(Entity& entity);
		virtual ~RigidBody(void) override = default;

		//reimplemented from Component
		virtual bool canAddTo(Entity& entity) override;
		virtual bool load(FILE* file) override;
		virtual bool save(FILE* file) override;
		virtual bool addToLevel(void) override;
		virtual bool removeFromLevel(void) override;
		virtual TYPE getType(void) const override;

		//reimplemented from ForceTarget
		virtual TARGET_TYPE			getTargetType(void)	const override;
		virtual float				getOrientation(void) const override;
		virtual const GYVector3&	getPosition(void) const override;
		virtual const GYStatic*		getStaticAttrib(void) const override;
        virtual bool                getRestingOnBody(void) const override;

		void				setActive(const bool val);
		void				setTileDamping(const float factor);
		float				getTileDamping(void) const;
		void				setAttrib(RigidBodyAttrib*const attrib);
		RigidBodyAttrib*	getAttrib(void) const;
		float				getDurability(void) const;
		void				setDurability(const float val);
		CollisionContact*	getBreakContact(void) const;
		void				setBreakContact(CollisionContact*const contact);

		//GYKinetic rigid bodies internal
		void integrate(const float deltaTime);

		//GYKinetic rigid bodies external
		//void applyForceMask(const uint32_t mask, const GYVector2& force);

	};


	// ========= INLINEZ ==========
    inline GYVector3 ForceTarget::getVelocity(void) const { return vel; }
	inline void	ForceTarget::setVelocity(const GYVector3& _vel) { vel = _vel; }
	inline void ForceTarget::setRotation(const float rotation) { rot = rotation; }
#if 0
    inline bool ForceTarget::getOnGround(void) const { return (bool)resting; }
    inline void ForceTarget::setOnGround(const bool val) { resting = (int)val; }
#endif
    inline void ForceTarget::clearKinetic(void) {
		GYKinetic* kinetic = static_cast<GYKinetic*>(this);
		memset(kinetic, 0, sizeof(GYKinetic));
	}

	inline float RigidBody::getTileDamping(void) const { return _tileDamping; }
	inline void RigidBody::setTileDamping(const float factor) {_tileDamping = factor; }
	inline void RigidBody::setAttrib(RigidBodyAttrib*const attrib) { _attrib = attrib; _durability = _attrib->getBaseDurability();}
	inline void RigidBody::setActive(const bool val) { _active = val; }
	inline float RigidBody::getDurability(void) const { return _durability; }
    inline void RigidBody::setDurability(const float val) { _durability = val; }
	inline CollisionContact* RigidBody::getBreakContact(void) const { return _breakContact; }
	inline void	RigidBody::setBreakContact(CollisionContact*const contact) { _breakContact = contact; }

	inline RigidBodyAttrib* RigidBody::getAttrib(void) const { return _attrib; }
	//inline void RigidBody::applyForceMask(const uint32_t mask, const GYVector2& force) { if(_attrib->getForceMask() & mask) applyForce(force); }
	inline uint32_t RigidBodyAttrib::getForceMask(void) const { return _forceMask; }
	inline void RigidBodyAttrib::setForceMask(const uint32_t mask) { _forceMask = mask; }
	inline bool RigidBodyAttrib::isBreakable(void) const { return _breakable; }
	inline void RigidBodyAttrib::setBreakable(const bool val) { _breakable = val; }
	inline float RigidBodyAttrib::getBaseDurability(void) const { return _baseDurability; }
	inline void RigidBodyAttrib::setBaseDurability(const float val) { _baseDurability = val; }
    inline bool RigidBodyAttrib::canHold(void) const { return _canHold; }
    inline void RigidBodyAttrib::setCanHold(const bool val) { _canHold = val; }



}

#endif // ELYSIAN_RIGID_BODY_H

#ifndef ELYSIAN_RIGID_BODY_HPP
#define ELYSIAN_RIGID_BODY_HPP

#include <stdint.h>
#include <kinetic/gyro_kinetic.h>
#include "elysian_component.hpp"

namespace elysian {

	class RigidBodyAttrib {	//later separate GYStatic attrib
	private:
		uint32_t	_forceMask		= ~0x0;
		bool		_breakable		= false;
		float		_baseDurability = -1;

	public:
		uint32_t	getForceMask(void) const;
		void		setForceMask(const uint32_t mask);
		void		setBreakable(const bool val);
		bool		isBreakable(void) const;
		float		getBaseDurability(void) const;
		void		setBaseDurability(const float val);

	};

	//Abstraction for particles and rigidBodies to share force generators
	class ForceTarget: protected GYKinetic {
	public:
		enum TARGET_TYPE { TARGET_PARTICLE, TARGET_RIGID_BODY };

									ForceTarget(void);
		virtual TARGET_TYPE			getTargetType(void) const			= 0;
		virtual float				getOrientation(void) const			= 0;
		virtual const GYVector3&	getPosition(void) const				= 0;
		virtual const GYStatic*		getStaticAttrib(void) const			= 0;
		//virtual void		integrate(const float deltaTime)	= 0; nevermind, fuck it.

		void						applyForce(const GYVector3& force);
		void						applyForceToPoint(const GYVector2& loc, const GYVector2& force);
		const GYVector3&			getVelocity(void) const;
		void						setVelocity(const GYVector3& _vel);
		void						setRotation(const float rotation);
		void						clearKinetic(void);
	};


	class RigidBody: public Component, public ForceTarget {
		friend class RigidBodySystem;
	private:
		float				_tileDamping = 1.0f;
		bool				_active	= true;
		bool				_onGround = true;
		RigidBodyAttrib*	_attrib = nullptr;
		float				_durability = -1.0f;
	public:

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

		void				setActive(const bool val);
		void				setTileDamping(const float factor);
		float				getTileDamping(void) const;
		void				setAttrib(RigidBodyAttrib*const attrib);
		RigidBodyAttrib*	getAttrib(void) const;
		bool				getOnGround(void) const;
		void				setOnGround(const bool val);
		float				getDurability(void) const;
		void				setDurability(const float val);

		//GYKinetic rigid bodies internal
		void integrate(const float deltaTime);

		//GYKinetic rigid bodies external
		//void applyForceMask(const uint32_t mask, const GYVector2& force);

	};


	// ========= INLINEZ ==========
	inline const GYVector3&	ForceTarget::getVelocity(void) const { return vel; }
	inline void	ForceTarget::setVelocity(const GYVector3& _vel) { vel = _vel; }
	inline void ForceTarget::setRotation(const float rotation) { rot = rotation; }
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
	inline bool RigidBody::getOnGround(void) const { return _onGround; }
	inline void RigidBody::setOnGround(const bool val) { _onGround = val; }

	inline RigidBodyAttrib* RigidBody::getAttrib(void) const { return _attrib; }
	//inline void RigidBody::applyForceMask(const uint32_t mask, const GYVector2& force) { if(_attrib->getForceMask() & mask) applyForce(force); }
	inline uint32_t RigidBodyAttrib::getForceMask(void) const { return _forceMask; }
	inline void RigidBodyAttrib::setForceMask(const uint32_t mask) { _forceMask = mask; }
	inline bool RigidBodyAttrib::isBreakable(void) const { return _breakable; }
	inline void RigidBodyAttrib::setBreakable(const bool val) { _breakable = val; }
	inline float RigidBodyAttrib::getBaseDurability(void) const { return _baseDurability; }
	inline void RigidBodyAttrib::setBaseDurability(const float val) { _baseDurability = val; }



}

#endif // ELYSIAN_RIGID_BODY_H

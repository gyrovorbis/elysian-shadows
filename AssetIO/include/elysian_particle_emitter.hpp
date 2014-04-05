#ifndef ELYSIAN_PARTICLE_EMITTER_HPP
#define ELYSIAN_PARTICLE_EMITTER_HPP

#include "elysian_component.hpp"
#include "elysian_mem_pool.hpp"
#include "elysian_sprite_attrib.hpp"
#include "elysian_rigid_body.hpp"

#define ELYSIAN_PARTICLE_SYSTEM_INTERNAL_EMITTER_POOL_SIZE	5000

namespace elysian {

	class CameraState;
	class Material;
	class ForceGenerator;

	class ParticleEmitterAttrib: public SpriteAttrib, public GYStatic {
		friend class ParticleEmitterInternals;
	private:
		GYVector3	_acceleration	= GYVector3 { 0.0f, 0.7f, 0.0f };
		Material*	_material		= nullptr;
		GYTexture*	_specularMap	= nullptr;
		//float		_angularDamping	= 0.99f;
		unsigned	_maxLife		= 100;
		unsigned	_size			= 3;
		//unsigned	_intensity		= 100; //for periodically generated particles, dunno if it should be here or not
		bool		_fadeOut		= true;
		unsigned	_minFrame		= 0;
		unsigned	_maxFrame		= 0;
		uint32_t	_forceMask		= ~0x0;
	public:

				ParticleEmitterAttrib(void);
		virtual ~ParticleEmitterAttrib(void) override;

		void setAcceleration(const GYVector3& accel);
		//void setAngularDamping(const float factor);
		void setMaxLife(const unsigned life); //number of frames since birth that it lives
		void setSize(const unsigned newSize); // 1 = 1x1 square
		void setFadeOut(const bool val);
		void setFrameRange(const unsigned min, const unsigned max);
		void setMaterial(Material*const material);
		void loadSpecularMap(const char*const path, const unsigned width, const unsigned height);
		uint32_t getForceMask(void) const;
		void setForceMask(const uint32_t mask);
	};

	class Particle: public ForceTarget {
	public:
		ParticleEmitterAttrib*	attrib;
		GYVector3				position;
		float					orientation;
		float					rotation;
		int						life;
		unsigned				size;
		unsigned				frame;
		GYColor4				color;

		//bool		active = false;

		//reimplemented from ForceTarget
		virtual TARGET_TYPE			getTargetType(void)	const override;
		virtual float				getOrientation(void) const override;
		virtual const GYVector3&	getPosition(void) const override;
		virtual const GYStatic*		getStaticAttrib(void) const override;

		void integrate(const float deltaTime);
	};

	class ParticleEmitterInternals {
	private:
		//Particle*				_pool			= nullptr;
		Pool<Particle>          _pool           = Pool<Particle>(ELYSIAN_PARTICLE_SYSTEM_INTERNAL_EMITTER_POOL_SIZE);
		ParticleEmitterAttrib*	_attrib			= nullptr;

	public:
		~ParticleEmitterInternals(void);

		void allocatePool(const unsigned size);
		void setAttrib(ParticleEmitterAttrib*const attrib);
		ParticleEmitterAttrib* getAttrib(void) const;

		void applyForce(ForceGenerator*const force, const float deltaTime);
		void update(const CameraState*const state, const float deltaTime);
		void render(void);

		//emitter functions
		void emitFromPoint(const GYVector3& pos, const float angle, const unsigned intensity);
	//	void fallFromSky(void);
	};



	class ParticleEmitter: public Component, public ParticleEmitterInternals {
	public:
		ParticleEmitter(Entity& entity);
		virtual ~ParticleEmitter(void) override = default;

		//reimplemented from component
		virtual bool canAddTo(Entity& entity) override;
		virtual bool load(FILE* file) override;
		virtual bool save(FILE* file) override;
		virtual bool addToLevel(void) override;
		virtual bool removeFromLevel(void) override;
		virtual TYPE getType(void) const override;

		void emitFromEntity(const GYVector3& pos, const float angle, const unsigned intensity);
	};


	// ====== INLINEZ =====

	inline uint32_t ParticleEmitterAttrib::getForceMask(void) const { return _forceMask; }
	inline void ParticleEmitterAttrib::setForceMask(const uint32_t mask) { _forceMask = mask; }

	inline void ParticleEmitterAttrib::setAcceleration(const GYVector3& accel) {
		_acceleration = accel;
	}

	inline void ParticleEmitterAttrib::setMaxLife(const unsigned life) {
		_maxLife = life;
	}

	inline void ParticleEmitterAttrib::setSize(const unsigned newSize) {
		_size = newSize;
	}

	inline void ParticleEmitterAttrib::setFadeOut(const bool val) {
		_fadeOut = val;
	}

	inline void ParticleEmitterAttrib::setFrameRange(const unsigned min, const unsigned max) {
		_minFrame = min; _maxFrame = max;
	}

	inline void ParticleEmitterAttrib::setMaterial(Material * const material) { _material = material; }

	inline void ParticleEmitterAttrib::loadSpecularMap(const char*const path, const unsigned width, const unsigned height) {
		if(_specularMap) gyVidTexDestroy(_specularMap);
		_specularMap = gyVidTexCreate(path, width, height);
	}

}

#endif // ELYSIAN_PARTICLE_EMITTER_HPP

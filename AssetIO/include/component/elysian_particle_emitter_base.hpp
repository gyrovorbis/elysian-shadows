#ifndef ELYSIAN_PARTICLE_EMITTER_BASE_HPP
#define ELYSIAN_PARTICLE_EMITTER_BASE_HPP

#include <cassert>
#include <elysian_component_base.hpp>
#include <elysian_mem_pool.hpp>
#include <elysian_sprite_attrib.hpp>
#include <elysian_collidable.hpp>
#include <elysian_lua_table_proxy.hpp>
#include <elysian_rigid_body.hpp>

#define ELYSIAN_PARTICLE_SYSTEM_INTERNAL_EMITTER_POOL_SIZE	5000
#define ELYSIAN_PARTICLE_EMITTER_ANIM_GRID_WIDTH            10
#define ELYSIAN_PARTICLE_EMITTER_ANIM_GRID_HEIGHT           10

namespace elysian {

    struct CameraState;
    class Material;
    class ForceGenerator;

    class ParticleEmitterAttrib: public SpriteAttrib, public CollidableStaticAttrib {
        friend class ParticleEmitterInternals;
    private:
        int         _animGrid[ELYSIAN_PARTICLE_EMITTER_ANIM_GRID_HEIGHT][ELYSIAN_PARTICLE_EMITTER_ANIM_GRID_WIDTH] = { {-1} };
        unsigned    _animGridWidth      = 0;
        unsigned    _animGridHeight     = 0;
        unsigned    _animFrameDuration  = 0;

        unsigned    _startSize          = 3;
        unsigned    _endSize            = 3;

        GYColor4    _endColor           = { 255, 255, 255, 255 };

        GYVector3	_acceleration		= { 0.0f, 0.0f, 0.0f };
        Material*	_material			= nullptr;
        GYTexture*	_specularMap		= nullptr;
        GYTexture*  _normalMap          = nullptr;
        float		_intensityFactor	= 1.0f;
        unsigned	_maxLife			= 100;
        uint32_t	_forceMask			= ~0x0;
        bool		_stick				= true;
        bool        _randomStartFrame   = false;
        bool        _shadowVolume       = true;

    public:

                    ParticleEmitterAttrib(void);
        virtual     ~ParticleEmitterAttrib(void) override;

        void        setAcceleration(const GYVector3& accel);
        void        setMaxLife(const unsigned life); //number of frames since birth that it lives
        void        setStartSize(const unsigned size); // 1 = 1x1 square
        void        setEndSize(const unsigned size);
        void        setEndColor(const GYColor4 color);
        void        setAnimGridSize(const unsigned w, const unsigned h);
        void        setAnimGridIndex(const unsigned row, const unsigned col, int index);
        void        setAnimFrameDuration(const unsigned frame);
        void        setMaterial(Material*const material);
        void        loadSpecularMap(const char*const path, const unsigned width, const unsigned height);
        void        loadNormalMap(const char*const path, const unsigned width, const unsigned height);
        uint32_t    getForceMask(void) const;
        void        setForceMask(const uint32_t mask);
        void        setStick(const bool val);
        void        setIntensityFactor(const float factor);
        void        setRandomStartFrame(const bool val);
        void        setShadowVolume(const bool val);
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
        unsigned                animRow;
        int						zIndex;
        bool					flat;
        bool                    resting;

        //reimplemented from ForceTarget
        virtual TARGET_TYPE         getTargetType(void)	const override;
        virtual float               getOrientation(void) const override;
        virtual const GYVector3&    getPosition(void) const override;
        virtual const GYStatic*     getStaticAttrib(void) const override;
        virtual bool                getRestingOnBody(void) const override;

        void                        integrate(const float deltaTime);
    };

	class ParticleEmitterInternals {
        friend ParticleEmitter;
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
        void renderAtElev(const unsigned char elev);

        //Emitter functions
        void emitFromPoint(const GYVector3& pos, const float angle, const unsigned intensity);
        void emitFromPointInDir(const GYVector3& pos, const GYVector3& dir, const unsigned intensity, const float angle);
    //	void fallFromSky(void);
    };



    class ParticleEmitterBase:
        public Component,
        public ParticleEmitterInternals,
        public LuaTableProxy<ParticleEmitterBase, ParticleEmitter>
    {
	public:
        friend ParticleEmitter;

        ELYSIAN_LUA_PROXY_FIELDS_BEGIN()
        ELYSIAN_LUA_PROXY_FIELDS_END()

                        ParticleEmitterBase(void) = default;
                        ParticleEmitterBase(Entity& entity);
        virtual         ~ParticleEmitterBase(void) override = default;

		//reimplemented from component
        static constexpr const Component::TYPE componentType = Component::PARTICLE_EMITTER;

        void            emitFromEntity(const GYVector3& pos, const float angle, const unsigned intensity);

    };


    // ====== INLINEZ =====

    inline uint32_t ParticleEmitterAttrib::getForceMask(void) const { return _forceMask; }
    inline void ParticleEmitterAttrib::setShadowVolume(const bool val) { _shadowVolume = val; }
    inline void ParticleEmitterAttrib::setForceMask(const uint32_t mask) { _forceMask = mask; }
    inline void ParticleEmitterAttrib::setStartSize(const unsigned size) { _startSize = size; }
    inline void ParticleEmitterAttrib::setEndSize(const unsigned size) { _endSize = size; }
    inline void ParticleEmitterAttrib::setEndColor(const GYColor4 color) { _endColor = color; }
    inline void ParticleEmitterAttrib::setAnimGridSize(const unsigned w, const unsigned h) {
        _animGridWidth = w; _animGridHeight = h;
        assert(w <= ELYSIAN_PARTICLE_EMITTER_ANIM_GRID_WIDTH);
        assert(h <= ELYSIAN_PARTICLE_EMITTER_ANIM_GRID_HEIGHT);
    }
    inline void ParticleEmitterAttrib::setAnimGridIndex(const unsigned row, const unsigned col, int index) {
        _animGrid[row][col] = index;
    }
    inline void ParticleEmitterAttrib::setAnimFrameDuration(const unsigned duration) { _animFrameDuration = duration; }

    inline void ParticleEmitterAttrib::setAcceleration(const GYVector3& accel) {
        _acceleration = accel;
    }

    inline void ParticleEmitterAttrib::setIntensityFactor(const float factor) { _intensityFactor = factor; }

    inline void ParticleEmitterAttrib::setMaxLife(const unsigned life) {
        _maxLife = life;
    }

    inline void ParticleEmitterAttrib::setMaterial(Material * const material) { _material = material; }

    inline void ParticleEmitterAttrib::loadSpecularMap(const char*const path, const unsigned /*width*/, const unsigned /*height*/) {
        if(_specularMap) gyVidTexDestroy(_specularMap);
        _specularMap = gyVidTexCreate(path);
    }

    inline void ParticleEmitterAttrib::loadNormalMap(const char*const path, const unsigned /*width*/, const unsigned /*height*/) {
        if(_normalMap) gyVidTexDestroy(_normalMap);
        _normalMap = gyVidTexCreate(path);
    }

    inline void ParticleEmitterAttrib::setStick(const bool val) { _stick = val; }

    inline void ParticleEmitterAttrib::setRandomStartFrame(const bool val) { _randomStartFrame = val; }
}

#endif // ELYSIAN_PARTICLE_EMITTER_HPP

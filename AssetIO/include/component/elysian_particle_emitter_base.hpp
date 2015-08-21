#ifndef ELYSIAN_PARTICLE_EMITTER_BASE_HPP
#define ELYSIAN_PARTICLE_EMITTER_BASE_HPP

#include <cassert>
#include <crtp/elysian_component.hpp>
#include <utility/elysian_mem_pool.hpp>
#include <crtp/elysian_sprite_attrib.hpp>
#include <crtp/elysian_collidable.hpp>
#include <crtp/elysian_lua_table_proxy.hpp>
#include <crtp/elysian_rigid_body.hpp>

#define ELYSIAN_PARTICLE_SYSTEM_INTERNAL_EMITTER_POOL_SIZE	5000
#define ELYSIAN_PARTICLE_EMITTER_ANIM_GRID_WIDTH            10
#define ELYSIAN_PARTICLE_EMITTER_ANIM_GRID_HEIGHT           10

namespace elysian {

    struct Camera;
    class Material;
    class ForceGenerator;

    class ParticleEmitterAttrib:
            public SpriteAttrib,
            public CollidableStaticAttrib,
            public LuaTableProxy<ParticleEmitterAttrib, ParticleEmitterAttrib, CollidableStaticAttrib, SpriteAttrib>
    {
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
        float		_intensityFactor	= 1.0f;
        unsigned	_maxLife			= 100;
        BitMask32	_forceMask			= ~0x0;
        bool		_stick				= true;
        bool        _randomStartFrame   = false;
        bool        _shadowVolume       = true;

    public:

        ELYSIAN_LUA_PROXY_FIELDS_BEGIN()
            ELYSIAN_LUA_PROXY_FIELD(maxLife,            getMaxLife,             setMaxLife)
            ELYSIAN_LUA_PROXY_FIELD(startSize,          getStartSize,           setStartSize)
            ELYSIAN_LUA_PROXY_FIELD(endSize,            getEndSize,             setEndSize)
            ELYSIAN_LUA_PROXY_FIELD(endColor,           getEndColor,            setEndColor)
            ELYSIAN_LUA_PROXY_FIELD(intensityFactor,    getIntensityFactor,     setIntensityFactor)
            ELYSIAN_LUA_PROXY_FIELD(randomStartFrame,   getRandomStartFrame,    setRandomStartFrame)
            ELYSIAN_LUA_PROXY_FIELD(forceMask,          getForceMask,           setForceMask)
            ELYSIAN_LUA_PROXY_FIELD(animFrameDuration,  getAnimFrameDuration,   setAnimFrameDuration)
            ELYSIAN_LUA_PROXY_FIELD(shadowVolume,       getShadowVolume,        setShadowVolume)
            ELYSIAN_LUA_PROXY_FIELD(stick,              getStick,               setStick)
        ELYSIAN_LUA_PROXY_FIELDS_END()

        using       LuaTableProxy<ParticleEmitterAttrib, ParticleEmitterAttrib, CollidableStaticAttrib, SpriteAttrib>::registerProxyMetaTable;
        using       LuaTableProxy<ParticleEmitterAttrib, ParticleEmitterAttrib, CollidableStaticAttrib, SpriteAttrib>::fromTable;
        using       LuaTableProxy<ParticleEmitterAttrib, ParticleEmitterAttrib, CollidableStaticAttrib, SpriteAttrib>::initializeDelegates;

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
        BitMask32&  getForceMask(void);
        void        setForceMask(const BitMask32 mask);
        void        setStick(const bool val);
        void        setIntensityFactor(const float factor);
        void        setRandomStartFrame(const bool val);
        void        setShadowVolume(const bool val);

        unsigned    getMaxLife(void) const;
        unsigned    getStartSize(void) const;
        unsigned    getEndSize(void) const;
        Color4      getEndColor(void);
        float       getIntensityFactor(void) const;
        bool        getRandomStartFrame(void) const;
        unsigned    getAnimFrameDuration(void) const;
        bool        getShadowVolume(void) const;
        bool        getStick(void) const;
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

        ELYSIAN_LUA_PROXY_FIELDS_BEGIN()
            ELYSIAN_LUA_PROXY_FIELD(attrib,     getAttrib,      setAttrib)
        ELYSIAN_LUA_PROXY_FIELDS_END()

        ~ParticleEmitterInternals(void);

        void allocatePool(const unsigned size);
        void setAttrib(ParticleEmitterAttrib*const attrib);
        ParticleEmitterAttrib* getAttrib(void) const;

        void applyForce(ForceGenerator*const force, const float deltaTime);
        void update(const Camera*const state, const float deltaTime);
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
            ELYSIAN_LUA_PROXY_FIELD(attrib,     getAttrib,      setAttrib)
        ELYSIAN_LUA_PROXY_FIELDS_END()

                        ParticleEmitterBase(void) = default;
                        ParticleEmitterBase(Entity& entity);
                        ParticleEmitterBase(const ParticleEmitterBase& other);
                        ~ParticleEmitterBase(void) = default;

		//reimplemented from component
        static constexpr const Component::TYPE componentType = Component::PARTICLE_EMITTER;

        void            emitFromEntity(const GYVector3& pos, const float angle, const unsigned intensity);

    };


    // ====== INLINEZ =====

    inline BitMask32& ParticleEmitterAttrib::getForceMask(void) { return _forceMask; }

    inline void ParticleEmitterAttrib::setShadowVolume(const bool val) { _shadowVolume = val; }
    inline void ParticleEmitterAttrib::setForceMask(const BitMask32 mask) { _forceMask = mask; }
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

    inline void ParticleEmitterAttrib::setStick(const bool val) { _stick = val; }

    inline void ParticleEmitterAttrib::setRandomStartFrame(const bool val) { _randomStartFrame = val; }

    inline unsigned ParticleEmitterAttrib::getMaxLife(void) const { return _maxLife; }
    inline unsigned ParticleEmitterAttrib::getStartSize(void) const { return _startSize; }
    inline unsigned ParticleEmitterAttrib::getEndSize(void) const { return _endSize; }
    inline Color4 ParticleEmitterAttrib::getEndColor(void) { return Color4(&_endColor); }
    inline float ParticleEmitterAttrib::getIntensityFactor(void) const { return _intensityFactor; }
    inline bool ParticleEmitterAttrib::getRandomStartFrame(void) const { return _randomStartFrame; }
    inline unsigned ParticleEmitterAttrib::getAnimFrameDuration(void) const { return _animFrameDuration; }
    inline bool ParticleEmitterAttrib::getShadowVolume(void) const { return _shadowVolume; }
    inline bool ParticleEmitterAttrib::getStick(void) const { return _stick; }

}

#endif // ELYSIAN_PARTICLE_EMITTER_HPP

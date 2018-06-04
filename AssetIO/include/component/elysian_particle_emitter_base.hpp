#ifndef ELYSIAN_PARTICLE_EMITTER_BASE_HPP
#define ELYSIAN_PARTICLE_EMITTER_BASE_HPP

#include <cassert>
#include <crtp/elysian_component.hpp>
#include <utility/elysian_mem_pool.hpp>
#include <crtp/elysian_sprite_attrib.hpp>
#include <crtp/elysian_collidable.hpp>
#include <crtp/elysian_lua_table_proxy.hpp>
#include <crtp/elysian_rigid_body.hpp>

#define ELYSIAN_PARTICLE_SYSTEM_INTERNAL_EMITTER_POOL_SIZE	50000
#define ELYSIAN_PARTICLE_EMITTER_ANIM_GRID_WIDTH            10
#define ELYSIAN_PARTICLE_EMITTER_ANIM_GRID_HEIGHT           10

ELYSIAN_ASSET_TYPEDEF(ParticleEmitterAttrib)

namespace elysian {

    struct Camera;
    class Material;
    class ForceGenerator;

    class ParticleEmitterAttribBase:
            public SpriteAttrib,
            public CollidableStaticAttrib,
            public LuaTableProxy<ParticleEmitterAttribBase, ParticleEmitterAttrib, CollidableStaticAttrib, SpriteAttrib>
    {
        friend class ParticleEmitterInternals;
    private:
        int         _animGrid[ELYSIAN_PARTICLE_EMITTER_ANIM_GRID_HEIGHT][ELYSIAN_PARTICLE_EMITTER_ANIM_GRID_WIDTH] = { {-1} };
        unsigned    _animGridWidth      = 0;
        unsigned    _animGridHeight     = 0;
        float       _animFrameDuration  = 0.0f;

        unsigned    _startSize          = 3;
        unsigned    _endSize            = 3;

        GYColor4    _endColor           = { 255, 255, 255, 255 };

        GYVector3	_acceleration		= { 0.0f, 0.0f, 0.0f };
        float		_intensityFactor	= 1.0f;
        float       _maxLifeTime        = 1.5f;
        BitMask32	_forceMask			= ~0x0;
        bool		_stick				= true;
        bool        _randomStartFrame   = false;
        bool        _shadowVolume       = true;

    public:

        ELYSIAN_LUA_PROXY_FIELDS_BEGIN()
            ELYSIAN_LUA_PROXY_FIELD(maxLifeTime,        getMaxLifeTime,         setMaxLifeTime)
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

        using       LuaTableProxy<ParticleEmitterAttribBase, ParticleEmitterAttrib, CollidableStaticAttrib, SpriteAttrib>::registerProxyMetaTable;
        using       LuaTableProxy<ParticleEmitterAttribBase, ParticleEmitterAttrib, CollidableStaticAttrib, SpriteAttrib>::fromTable;
        using       LuaTableProxy<ParticleEmitterAttribBase, ParticleEmitterAttrib, CollidableStaticAttrib, SpriteAttrib>::lua_downCast;

                    ParticleEmitterAttribBase(void);
        virtual     ~ParticleEmitterAttribBase(void) override;

        void        setAcceleration(const GYVector3& accel);
        void        setMaxLifeTime(const float secs); //number of frames since birth that it lives
        void        setStartSize(const unsigned size); // 1 = 1x1 square
        void        setEndSize(const unsigned size);
        void        setEndColor(const Color4& color);
        void        setAnimGridSize(const unsigned w, const unsigned h);
        void        setAnimGridIndex(const unsigned row, const unsigned col, int index);
        void        setAnimFrameDuration(const float secs);
        BitMask32&  getForceMask(void);
        void        setForceMask(const BitMask32 mask);
        void        setStick(const bool val);
        void        setIntensityFactor(const float factor);
        void        setRandomStartFrame(const bool val);
        void        setShadowVolume(const bool val);

        float       getMaxLifeTime(void) const;
        unsigned    getStartSize(void) const;
        unsigned    getEndSize(void) const;
        Color4      getEndColor(void);
        float       getIntensityFactor(void) const;
        bool        getRandomStartFrame(void) const;
        float       getAnimFrameDuration(void) const;
        bool        getShadowVolume(void) const;
        bool        getStick(void) const;
    };

    class Particle: public ForceTarget {
    public:
        ParticleEmitterAttrib*	attrib;
        GYVector3				position;
        float					orientation;
        float					rotation;
        float                   lifeTime;
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
    public:
		//Particle*				_pool			= nullptr;
        Pool<Particle>                          _pool   = Pool<Particle>(ELYSIAN_PARTICLE_SYSTEM_INTERNAL_EMITTER_POOL_SIZE);
        std::shared_ptr<ParticleEmitterAttrib>	_attrib;

    public:

        ELYSIAN_LUA_PROXY_FIELDS_BEGIN()
            ELYSIAN_LUA_PROXY_FIELD(attrib,     getAttrib,      setAttrib)
        ELYSIAN_LUA_PROXY_FIELDS_END()

        ParticleEmitterInternals&   operator=(ParticleEmitterInternals& rhs);

        void                        allocatePool(const unsigned size);
        unsigned                    getPoolSize(void) const;
        unsigned                    getActiveCount(void) const;
        void                        resizePool(unsigned size);
        void                        setAttrib(std::shared_ptr<ParticleEmitterAttrib> attrib);
        auto                        getAttrib(void) const -> std::shared_ptr<ParticleEmitterAttrib>;

        void                        applyForce(ForceGenerator*const force, const float deltaTime);
        void                        update(const Camera*const state, const float deltaTime);
        void                        render(void);
        void                        renderAtElev(const unsigned char elev);

        //Emitter functions
        void                        emitFromPoint(const GYVector3& pos, const float angle, const unsigned intensity);
        void                        emitFromPointInDir(const GYVector3& pos, const GYVector3& dir, const unsigned intensity, const float angle);
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
                                ParticleEmitterBase(Entity& entity, ParticleEmitterBase*const prefab=nullptr);
                                ~ParticleEmitterBase(void) = default;

        ParticleEmitterBase&    operator=(ParticleEmitterBase& rhs);

		//reimplemented from component
        static constexpr const Component::TYPE componentType = Component::PARTICLE_EMITTER;

        void                    emitFromEntity(const GYVector3& localPos, const float angle, const unsigned intensity);

    };





    // ====== INLINEZ =====
    inline ParticleEmitterAttribBase::ParticleEmitterAttribBase(void) {}
    inline BitMask32& ParticleEmitterAttribBase::getForceMask(void) { return _forceMask; }
    inline void ParticleEmitterAttribBase::setShadowVolume(const bool val) { _shadowVolume = val; }
    inline void ParticleEmitterAttribBase::setForceMask(const BitMask32 mask) { _forceMask = mask; }
    inline void ParticleEmitterAttribBase::setStartSize(const unsigned size) { _startSize = size; }
    inline void ParticleEmitterAttribBase::setEndSize(const unsigned size) { _endSize = size; }
    inline void ParticleEmitterAttribBase::setEndColor(const Color4& color) { _endColor = GYColor4 { color.getR(), color.getG(), color.getB(), color.getA() }; }

    inline void ParticleEmitterAttribBase::setAnimGridIndex(const unsigned row, const unsigned col, int index) {
        _animGrid[row][col] = index;
    }
    inline void ParticleEmitterAttribBase::setAnimFrameDuration(const float secs) { _animFrameDuration = secs; }

    inline void ParticleEmitterAttribBase::setAcceleration(const GYVector3& accel) {
        _acceleration = accel;
    }

    inline void ParticleEmitterAttribBase::setIntensityFactor(const float factor) { _intensityFactor = factor; }

    inline void ParticleEmitterAttribBase::setMaxLifeTime(const float secs) {
        _maxLifeTime = secs;
    }
    inline void ParticleEmitterAttribBase::setStick(const bool val) { _stick = val; }
    inline void ParticleEmitterAttribBase::setRandomStartFrame(const bool val) { _randomStartFrame = val; }
    inline float ParticleEmitterAttribBase::getMaxLifeTime(void) const { return _maxLifeTime; }
    inline unsigned ParticleEmitterAttribBase::getStartSize(void) const { return _startSize; }
    inline unsigned ParticleEmitterAttribBase::getEndSize(void) const { return _endSize; }
    inline Color4 ParticleEmitterAttribBase::getEndColor(void) { return Color4(&_endColor); }
    inline float ParticleEmitterAttribBase::getIntensityFactor(void) const { return _intensityFactor; }
    inline bool ParticleEmitterAttribBase::getRandomStartFrame(void) const { return _randomStartFrame; }
    inline float ParticleEmitterAttribBase::getAnimFrameDuration(void) const { return _animFrameDuration; }
    inline bool ParticleEmitterAttribBase::getShadowVolume(void) const { return _shadowVolume; }
    inline bool ParticleEmitterAttribBase::getStick(void) const { return _stick; }


    inline ParticleEmitterInternals& ParticleEmitterInternals::operator=(ParticleEmitterInternals& rhs) {
        _attrib = rhs._attrib;
        return *this;
    }
    inline unsigned ParticleEmitterInternals::getPoolSize(void) const { return _pool.getMaxSize(); }
    inline void ParticleEmitterInternals::resizePool(unsigned size) { _pool.resize(size); }
    inline unsigned ParticleEmitterInternals::getActiveCount(void) const { return _pool.size(); }

    inline ParticleEmitterBase::ParticleEmitterBase(Entity& entity, ParticleEmitterBase*const prefab): Component(entity) {
        if(prefab) *this = *prefab;
    }
    inline ParticleEmitterBase& ParticleEmitterBase::operator=(ParticleEmitterBase& rhs) {
        ParticleEmitterInternals::operator=(rhs);
        return *this;
    }

    inline std::shared_ptr<ParticleEmitterAttrib> ParticleEmitterInternals::getAttrib(void) const {
        return _attrib;
    }

    inline void ParticleEmitterInternals::setAttrib(std::shared_ptr<ParticleEmitterAttrib> attrib) {
        _attrib = attrib;
    }

}

#endif // ELYSIAN_PARTICLE_EMITTER_HPP

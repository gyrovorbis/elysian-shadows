#ifndef ELYSIAN_ENTITY_BASE_HPP
#define ELYSIAN_ENTITY_BASE_HPP

#include <vector>
#include <map>

#include <oolua.h>

#include <primitive/elysian_renderable_object.hpp>
#include <utility/elysian_stl_algorithm.hpp>
#include <crtp/elysian_component.hpp>
#include <crtp/elysian_lua_table_proxy.hpp>
#include <gyro_video_api.h>

#define ELYSIAN_ENTITY_NAME_SIZE	100
#define ELYSIAN_ENTITY_NAME_DEFAULT	"Untitled"
#define ELYSIAN_ENTITY_DESC_DEFAULT "No Description"

ELYSIAN_ASSET_TYPEDEF(Entity)
ELYSIAN_ASSET_TYPEDEF(Area)
ELYSIAN_ASSET_TYPEDEF(LuaBehavior)

namespace elysian {

	class EntityBase:
			public RenderableObject,
            public LuaTableProxy<EntityBase, Entity>
	{
	public:

        ELYSIAN_LUA_PROXY_FIELDS_BEGIN()
            ELYSIAN_LUA_PROXY_FIELD(name,               getName,                setName)
            ELYSIAN_LUA_PROXY_FIELD(dispName,           getDispName,            setDispName)
            ELYSIAN_LUA_PROXY_FIELD(elev,               getElev,                setElev)
            ELYSIAN_LUA_PROXY_FIELD(planeType,          getPlaneType,           setPlaneType)
            ELYSIAN_LUA_PROXY_FIELD(shadowVolume,       isShadowVolume,         setShadowVolume)
            ELYSIAN_LUA_PROXY_FIELD(persistent,         isPersistent,           setPersistent)
            ELYSIAN_LUA_PROXY_FIELD(transform,          getTransform,           setTransform)
            ELYSIAN_LUA_PROXY_FIELD(renderable,         getRenderable,          setRenderable)
            ELYSIAN_LUA_PROXY_FIELD(skeleton,           getSkeleton,            setSkeleton)
            //ELYSIAN_LUA_PROXY_FIELD(animation,          getAnimation,           setAnimation)
            ELYSIAN_LUA_PROXY_FIELD(collidable,         getCollidable,          setCollidable)
            ELYSIAN_LUA_PROXY_FIELD(rigidBody,          getRigidBody,           setRigidBody)
            ELYSIAN_LUA_PROXY_FIELD(examinable,         getExaminable,          setExaminable)
            ELYSIAN_LUA_PROXY_FIELD(warp,               getWarp,                setWarp)
            ELYSIAN_LUA_PROXY_FIELD(character,          getCharacter,           setCharacter)
            ELYSIAN_LUA_PROXY_FIELD(lightSource,        getLightSource,         setLightSource)
            ELYSIAN_LUA_PROXY_FIELD(audioSource,        getAudioSource,         setAudioSource)
            ELYSIAN_LUA_PROXY_FIELD(particleEmitter,    getParticleEmitter,     setParticleEmitter)
            ELYSIAN_LUA_PROXY_FIELD(combat,             getCombat,              setCombat)
            ELYSIAN_LUA_PROXY_ARRAY_FIELD(luaBehaviors,  getLuaBehaviorByIndex,  setLuaBehaviorByIndex, getLuaBehaviorCount)
		ELYSIAN_LUA_PROXY_FIELDS_END()



                            EntityBase(void);
                            EntityBase(const OOLUA::Lua_table_ref valTable);
                            EntityBase(EntityBase* other);
        virtual             ~EntityBase(void);

        bool                load(FILE* file);
        bool                save(FILE* file);

        virtual void        render(void) override;
        void                renderAtZ(const float z); //reested?
        void                renderShadowVolume(void);

        bool                activate(void);
        bool                deactivate(void);

        void                move(const GYVector2& dist);

        void                setName(const char* const newName);
        const char*         getName(void) const;
        void                setDispName(const char* const name);
        const char*         getDispName(void) const;

        void                setTransform(const OOLUA::Lua_table_ref valTable);

        void                setElev(const unsigned char e);
        unsigned char       getElev(void) const;
        GY_RECT_PLANE_TYPE  getPlaneType(void) const;
        void                setPlaneType(const GY_RECT_PLANE_TYPE plane);
        void                setShadowVolume(const bool v);
        bool                isShadowVolume(void) const;
        void                setShadowDepth(const float depth);
        float               getShadowDepth(void) const;
        bool                isActive(void) const;
        
		Renderable*			addRenderable(void);
		Collidable*			addCollidable(void);
		RigidBody*			addRigidBody(void);
		Character*			addCharacter(void);
		Warp*				addWarp(void);
		Examinable*			addExaminable(void);
		AudioSource*		addAudioSource(void);
		LightSource*		addLightSource(void);
		ParticleEmitter*	addParticleEmitter(void);
		Combat*				addCombat(void);
        Skeleton*           addSkeleton(void);

        void                setRenderable(const OOLUA::Lua_table_ref valTable);
        void                setCollidable(const OOLUA::Lua_table_ref valTable);
        void                setRigidBody(const OOLUA::Lua_table_ref valTable);
        void                setCharacter(const OOLUA::Lua_table_ref valTable);
        void                setWarp(const OOLUA::Lua_table_ref valTable);
        void                setExaminable(const OOLUA::Lua_table_ref valTable);
        void                setAudioSource(const OOLUA::Lua_table_ref valTable);
        void                setLightSource(const OOLUA::Lua_table_ref valTable);
        void                setParticleEmitter(const OOLUA::Lua_table_ref valTable);
        void                setCombat(const OOLUA::Lua_table_ref valTable);
        void                setSkeleton(const OOLUA::Lua_table_ref valTable);

        Renderable*			getRenderable(void) const;
        Collidable*			getCollidable(void) const;
        RigidBody*			getRigidBody(void) const;
        Character*			getCharacter(void) const;
        Warp*				getWarp(void) const;
        Examinable*			getExaminable(void) const;
        AudioSource*		getAudioSource(void) const;
        LightSource*		getLightSource(void) const;
        ParticleEmitter*	getParticleEmitter(void) const;
        Combat*				getCombat(void) const;
        Skeleton*           getSkeleton(void) const;

        void                removeRenderable(void);
        void                removeCollidable(void);
        void                removeRigidBody(void);
        void                removeCharacter(void);
        void                removeWarp(void);
        void                removeExaminable(void);
        void                removeAudioSource(void);
        void                removeLightSource(void);
        void                removeParticleEmitter(void);
        void                removeCombat(void);
        void                removeSkeleton(void);

        Behavior*           addBehavior(char const* behaviorName);
        Behavior*           getBehavior(char const* constbehaviorName) const;

        LuaBehavior*        addLuaBehavior(char const* scriptName, const OOLUA::Lua_table_ref paramTable);

        LuaBehavior*        getLuaBehavior(char const* scriptName) const;
        std::vector<Behavior*>& getBehaviors(void);
        LuaBehavior*        getLuaBehaviorByIndex(const unsigned index) const;
        void                setLuaBehaviorByIndex(const unsigned index, const OOLUA::Lua_table_ref valueTable);
        unsigned            getLuaBehaviorCount(void) const;

        void                setParentEntity(Entity*const parent);
        Entity*             getParentEntity(void) const;
        Entity*             getTopParentEntity(void);

        void                removeBehavior(char const* behaviorName);
        void                removeLuaBehavior(char const* scriptName);

        float               getDepthBaseY(void) const;
        void                setDepthBaseY(const float base);

        bool                isPersistent(void) const;
        void                setPersistent(const bool val);

        Area*               getArea(void) const;
        void                setArea(Area*const area);

		OOLUA::Lua_table_ref&	getSelfTable(void);
		void					setSelfTable(const OOLUA::Lua_table_ref self);
        void                    pushViewTransform(void);
        void                    popViewTransform(void);
        
	protected:
		
        char _name[ELYSIAN_ENTITY_NAME_SIZE];
        char _dispName[ELYSIAN_ENTITY_NAME_SIZE];
        bool _componentActive[Component::MAX_TYPE] { false };
        
        std::vector<Behavior*> _behaviors;

        Entity*             _parent             = nullptr;
        Area*               _area               = nullptr;
		unsigned char		_elev				= 0;
		bool				_shadowVolume		= false;
		float				_shadowDepth		= 16.0f;
        GY_RECT_PLANE_TYPE  _planeType			= GY_RECT_PLANE_FRONT;
        bool				_active				= false;
        bool                _persistent         = false;

        Renderable*			_renderable			= nullptr;
        Collidable*			_collidable			= nullptr;
        RigidBody*			_rigidBody			= nullptr;
        Character*			_character			= nullptr;
        Warp*				_warp				= nullptr;
        Examinable*			_examinable			= nullptr;
        AudioSource*		_audioSource		= nullptr;
        LightSource*		_lightSource		= nullptr;
        ParticleEmitter*	_particleEmitter	= nullptr;
        Combat*				_combat				= nullptr;
        Skeleton*           _skeleton           = nullptr;

        OOLUA::Lua_table_ref	_selfTable;
        static float            _oldZ;


        template<typename T>
        T* _addComponent(T*& comp, std::vector<T*>* container = nullptr, T* prefab=nullptr);
        template<typename T>
        void _removeComponent(T*& comp, std::vector<T*>* container=nullptr);
        template<typename T>
        void _setComponent(T*& comp, const OOLUA::Lua_table_ref valTable, std::vector<T*>* container=nullptr);

        void _renderRelative(void);

		Behavior*	attachBehavior(Behavior *const behav);
		void		reested(void); //default for failed deserialization attempts
    }; 






    //=========== INLINEZ =========

    inline Renderable*      EntityBase::getRenderable(void) const { return _renderable; }
    inline Collidable*		EntityBase::getCollidable(void) const { return _collidable; }
    inline RigidBody*		EntityBase::getRigidBody(void) const { return _rigidBody; }
    inline Character*		EntityBase::getCharacter(void) const { return _character; }
    inline Warp*			EntityBase::getWarp(void) const { return _warp; }
    inline Examinable*		EntityBase::getExaminable(void) const { return _examinable; }
    inline AudioSource*		EntityBase::getAudioSource(void) const { return _audioSource; }
    inline LightSource*		EntityBase::getLightSource(void) const { return _lightSource; }
    inline ParticleEmitter*	EntityBase::getParticleEmitter(void) const { return _particleEmitter; }
    inline Combat*			EntityBase::getCombat(void) const { return _combat; }
    inline Skeleton*        EntityBase::getSkeleton(void) const { return _skeleton; }

    inline void EntityBase::setName(const char* const newName) { strcpy(_name, newName); }
    inline const char* EntityBase::getName(void) const { return _name; }
    inline void EntityBase::setDispName(const char* const name) { strcpy(_dispName, name); }
    inline const char* EntityBase::getDispName(void) const { return _dispName; }

    inline OOLUA::Lua_table_ref& EntityBase::getSelfTable(void) {
        return _selfTable;
    }

    inline void EntityBase::setSelfTable(const OOLUA::Lua_table_ref self) {
        _selfTable = self;
    }

    inline void EntityBase::setElev(const unsigned char e) { _transform._pos.z = (float)e*32.0f; }
    inline unsigned char EntityBase::getElev(void) const { return _transform._pos.z/32.0f; }
    inline GY_RECT_PLANE_TYPE EntityBase::getPlaneType(void) const  { return _planeType; }
    inline void EntityBase::setPlaneType(const GY_RECT_PLANE_TYPE plane) { _planeType = plane; }
    inline float EntityBase::getDepthBaseY(void) const {
        switch(_planeType) {
        case GY_RECT_PLANE_LEFT:
        case GY_RECT_PLANE_RIGHT:
            return _transform._pos.y - 32.0f;

        default:
           return  getSkeleton()? _transform._pos.y : (_transform._pos.y  + 0.5f * _transform._size.y * _transform._scale.y + 16.0f);
        }
    }

     //   return (transform._pos.y); /* + 0.5f * transform._size.y * transform._scale.y);*/ }
   // inline void Entity::setDepthBaseY(const float base) { transform._pos.y = base - 0.5f * transform._size.y * transform._scale.y; }
    inline std::vector<Behavior*>& EntityBase::getBehaviors(void) { return _behaviors; }
    inline void EntityBase::setShadowVolume(const bool v) { _shadowVolume = v; }
    inline bool EntityBase::isShadowVolume(void) const { return _shadowVolume; }
    inline void EntityBase::setShadowDepth(const float depth) { _shadowDepth = depth; }
    inline float EntityBase::getShadowDepth(void) const { return _shadowDepth; }
    inline void EntityBase::setParentEntity(Entity * const parent) { _parent = parent; }
    inline Entity* EntityBase::getParentEntity(void) const { return _parent; }
    inline bool EntityBase::isActive(void) const { return _active; }
    inline void EntityBase::setPersistent(const bool val) { _persistent = val; }
    inline bool EntityBase::isPersistent(void) const { return _persistent; }
    inline unsigned EntityBase::getLuaBehaviorCount(void) const { return _behaviors.size(); }
    inline Area* EntityBase::getArea(void) const { return _area; }
    inline void EntityBase::setArea(Area*const area) { _area = area; }

}


#endif

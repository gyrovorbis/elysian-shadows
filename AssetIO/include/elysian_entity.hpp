#ifndef ELYSIAN_ENTITY_HPP
#define ELYSIAN_ENTITY_HPP

#include <vector>
#include <map>

#include "elysian_renderable_object.hpp"
#include "elysian_asset.hpp"
#include "elysian_stl_algorithm.hpp"
#include "elysian_component.hpp"
#include "elysian_area.hpp"
#include "elysian_map_depth_buffer.hpp"
#ifdef ELYSIAN_ENGINE
#	include <oolua.h>
#endif

#define ELYSIAN_ENTITY_NAME_SIZE	100
#define ELYSIAN_ENTITY_NAME_DEFAULT	"Untitled"

namespace elysian {

	class Renderable;
    class Animation;
    class Collidable;
	class RigidBody;
	class Character;
	class Warp;
	class Examinable;
	class AudioSource;
	class LightSource;
	class ParticleEmitter;
	class Combat;
	class Behavior;
	class LuaBehavior;
	
    class Entity: public RenderableObject, public Asset {
	public:

				Entity(void)	= default;
		virtual ~Entity(void);
		
		bool load(FILE* file);
		bool save(FILE* file);

		virtual void render(void) override;
        void renderAtZ(const float z); //reested?
		void renderShadowVolume(void);

		bool activate(void);
		bool deactivate(void);
        
        void move(const GYVector2& dist);
        
        void setName(char const* const newName) { strcpy(_name, newName); }
        const char *getName(void) const { return _name; }

		void setElev(const unsigned char e);
		unsigned char getElev(void) const;
        GY_RECT_PLANE_TYPE getPlaneType(void) const;
        void setPlaneType(const GY_RECT_PLANE_TYPE plane);
		void setShadowVolume(const bool v);
		bool isShadowVolume(void) const;
		void setShadowDepth(const float depth);
        float getShadowDepth(void) const;
        
		Renderable*			addRenderable(void);
		Animation*			addAnimation(void);
		Collidable*			addCollidable(void);
		RigidBody*			addRigidBody(void);
		Character*			addCharacter(void);
		Warp*				addWarp(void);
		Examinable*			addExaminable(void);
		AudioSource*		addAudioSource(void);
		LightSource*		addLightSource(void);
		ParticleEmitter*	addParticleEmitter(void);
		Combat*				addCombat(void);
		
		Renderable*			getRenderable(void) const { return _renderable; }
		Animation*			getAnimation(void) const { return _animation; }
		Collidable*			getCollidable(void) const { return _collidable; }
		RigidBody*			getRigidBody(void) const { return _rigidBody; }
		Character*			getCharacter(void) const { return _character; }
		Warp*				getWarp(void) const { return _warp; }
		Examinable*			getExaminable(void) const { return _examinable; }
		AudioSource*		getAudioSource(void) const { return _audioSource; }
		LightSource*		getLightSource(void) const { return _lightSource; }
		ParticleEmitter*	getParticleEmitter(void) const { return _particleEmitter; }
		Combat*				getCombat(void) const { return _combat; }

		void removeRenderable(void);
		void removeAnimation(void);
		void removeCollidable(void);
		void removeRigidBody(void);
		void removeCharacter(void);
		void removeWarp(void);
		void removeExaminable(void);
		void removeAudioSource(void);
		void removeLightSource(void);
		void removeParticleEmitter(void);
		void removeCombat(void);
 
		Behavior* addBehavior(char const* behaviorName);
        Behavior* getBehavior(char const* constbehaviorName) const;
		LuaBehavior* addLuaBehavior(char const* scriptName, const OOLUA::Lua_table_ref paramTable);
		LuaBehavior* getLuaBehavior(char const* scriptName) const;
		std::map<char const *, Behavior*, ltstr>& getBehaviors(void);

		void removeBehavior(char const* behaviorName);
		void removeLuaBehavior(char const* scriptName);
        
        float getDepthBaseY(void) const;
        void setDepthBaseY(const float base);

#ifdef ELYSIAN_ENGINE
		OOLUA::Lua_table_ref& getSelfTable(void);
		void setSelfTable(const OOLUA::Lua_table_ref self);
#endif
        
	protected:
		
		char _name[ELYSIAN_ENTITY_NAME_SIZE]		= ELYSIAN_ENTITY_NAME_DEFAULT;
		bool _componentActive[Component::MAX_TYPE]	= { false };
        
        std::map<char const *, Behavior*, ltstr> _behavior;

		unsigned char		_elev				= 0;
		bool				_shadowVolume		= false;
		float				_shadowDepth		= 16.0f;
        GY_RECT_PLANE_TYPE  _planeType			= GY_RECT_PLANE_FRONT;
		bool				_active				= false;
		Renderable*			_renderable			= nullptr;
		Animation*			_animation			= nullptr;
		Collidable*			_collidable			= nullptr;
		RigidBody*			_rigidBody			= nullptr;
		Character*			_character			= nullptr;
		Warp*				_warp				= nullptr;
		Examinable*			_examinable			= nullptr;
		AudioSource*		_audioSource		= nullptr;
		LightSource*		_lightSource		= nullptr;
		ParticleEmitter*	_particleEmitter	= nullptr;
		Combat*				_combat				= nullptr;
#ifdef ELYSIAN_ENGINE
		OOLUA::Lua_table_ref	_selfTable;
#endif
		
		bool		attachComponent(Component* const comp);
		Behavior*	attachBehavior(Behavior *const behav);
		void		reested(void); //default for failed deserialization attempts
    }; 

#ifdef ELYSIAN_ENGINE
		inline OOLUA::Lua_table_ref& Entity::getSelfTable(void) {
			return _selfTable;
		}

		inline void Entity::setSelfTable(const OOLUA::Lua_table_ref self) {
			_selfTable = self;
		}
#endif


	inline void Entity::setElev(const unsigned char e) { transform._pos.z = (float)e*32.0f; }
	inline unsigned char Entity::getElev(void) const { return transform._pos.z/32.0f; }
    inline GY_RECT_PLANE_TYPE Entity::getPlaneType(void) const  { return _planeType; }
    inline void Entity::setPlaneType(const GY_RECT_PLANE_TYPE plane) { _planeType = plane; }
    inline float Entity::getDepthBaseY(void) const {
        switch(_planeType) {
        case GY_RECT_PLANE_LEFT:
        case GY_RECT_PLANE_RIGHT:
            return transform._pos.y - 32.0f;

        default:
           return  (transform._pos.y  + 0.5f * transform._size.y * transform._scale.y + 16.0f);
        }
    }

     //   return (transform._pos.y); /* + 0.5f * transform._size.y * transform._scale.y);*/ }
   // inline void Entity::setDepthBaseY(const float base) { transform._pos.y = base - 0.5f * transform._size.y * transform._scale.y; }
	inline std::map<char const *, Behavior*, ltstr>& Entity::getBehaviors(void) { return _behavior; }
	inline void Entity::setShadowVolume(const bool v) { _shadowVolume = v; }
	inline bool Entity::isShadowVolume(void) const { return _shadowVolume; }
	inline void Entity::setShadowDepth(const float depth) { _shadowDepth = depth; }
    inline float Entity::getShadowDepth(void) const { return _shadowDepth; }

}

// ======== OOLUA WRAPPER =========


// ======= /OOLUA WRAPPER =======

#endif

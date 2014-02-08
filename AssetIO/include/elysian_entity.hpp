#ifndef ELYSIAN_ENTITY_HPP
#define ELYSIAN_ENTITY_HPP

#include <fstream>
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
	class Character;
	class Warp;
	class Item;
	class Friendly;
	class Enemy;
	class Examinable;
	class AudioSource;
	class LightSource;
	class Behavior;
	class LuaBehavior;
	
    class Entity: public RenderableObject, public Asset {
	public:
		Entity(void)	= default;
		virtual ~Entity(void);
		
		bool load(std::ifstream &stream);
		bool save(std::ofstream &stream);

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
		DEPTH_TYPE getDepthType(void) const;
		void setDepthType(const DEPTH_TYPE depth);
		void setShadowVolume(const bool v);
		bool isShadowVolume(void) const;
        
		Renderable*		addRenderable(void);
		Animation*      addAnimation(void);
		Collidable*     addCollidable(void);
		Character*      addCharacter(void);
		Warp*           addWarp(void);
		Item*           addItem(void);
		Friendly*       addFriendly(void);
		Enemy*          addEnemy(void);
		Examinable*		addExaminable(void);
		AudioSource*	addAudioSource(void);
		LightSource*	addLightSource(void);
		
		Renderable*		getRenderable(void) const { return _renderable; }
		Animation*      getAnimation(void) const { return _animation; }
		Collidable*     getCollidable(void) const{ return _collidable; }
		Character*      getCharacter(void) const { return _character; }
		Warp*           getWarp(void) const { return _warp; }
		Item*           getItem(void) const { return _item; }
		Friendly*       getFriendly(void) const { return _friendly; }
		Enemy*          getEnemy(void) const { return _enemy; }
		Examinable*		getExaminable(void) const { return _examinable; }
		AudioSource*	getAudioSource(void) const { return _audioSource; }
		LightSource*	getLightSource(void) const { return _lightSource; }
		
		void removeRenderable(void);
		void removeAnimation(void);
		void removeCollidable(void);
		void removeCharacter(void);
		void removeWarp(void);
		void removeItem(void);
		void removeFriendly(void);
		void removeEnemy(void);
		void removeExaminable(void);
		void removeAudioSource(void);
		void removeLightSource(void);
 
		Behavior* addBehavior(char const* behaviorName);
        Behavior* getBehavior(char const* constbehaviorName) const;
		LuaBehavior* addLuaBehavior(char const* scriptName);
		LuaBehavior* getLuaBehavior(char const* scriptName) const;
		std::map<char const *, Behavior*, ltstr>& getBehaviors(void);

		void removeBehavior(char const* behaviorName);
		void removeLuaBehavior(char const* scriptName);
        
        float getDepthBaseY(void) const;

#ifdef ELYSIAN_ENGINE
		OOLUA::Lua_table_ref& getSelfTable(void);
		void setSelfTable(const OOLUA::Lua_table_ref self);
#endif
        
	protected:
		
		char _name[ELYSIAN_ENTITY_NAME_SIZE]		= ELYSIAN_ENTITY_NAME_DEFAULT;
		bool _componentActive[Component::MAX_TYPE]	= { false };
        
        std::map<char const *, Behavior*, ltstr> _behavior;

		unsigned char	_elev			= 0;
		bool			_shadowVolume	= true;
		float			_shadowDepth	= 32.0f;
		DEPTH_TYPE		_depthType		= DEPTH_NONE;
		bool			_active			= false;
		Renderable*		_renderable		= nullptr;
		Animation*      _animation		= nullptr;
		Collidable*     _collidable		= nullptr;
		Character*      _character		= nullptr;
		Warp*           _warp			= nullptr;
		Item*           _item			= nullptr;
		Friendly*       _friendly		= nullptr;
		Enemy*          _enemy			= nullptr;
		Examinable*		_examinable		= nullptr;
		AudioSource*	_audioSource	= nullptr;
		LightSource*	_lightSource	= nullptr;
		//RigidBody*    _rigidBody;
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


	inline void Entity::setElev(const unsigned char e) { _elev = e; }
	inline unsigned char Entity::getElev(void) const { return _elev; }
	inline DEPTH_TYPE Entity::getDepthType(void) const { return _depthType; }
	inline void Entity::setDepthType(const DEPTH_TYPE depth) { _depthType = depth; }
    inline float Entity::getDepthBaseY(void) const { return (transform._pos.y + 0.5f * transform._size.y + 16.0f); }
	inline std::map<char const *, Behavior*, ltstr>& Entity::getBehaviors(void) { return _behavior; }
	inline void Entity::setShadowVolume(const bool v) { _shadowVolume = v; }
	inline bool Entity::isShadowVolume(void) const { return _shadowVolume; }

}

// ======== OOLUA WRAPPER =========


// ======= /OOLUA WRAPPER =======

#endif

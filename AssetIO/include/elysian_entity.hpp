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

#define ELYSIAN_ENTITY_NAME_SIZE 100

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
	class Behavior;
	class LuaBehavior;
	
    class Entity: public RenderableObject, public Asset {
	public:
        Entity();
        ~Entity(void);
		
		bool load(std::ifstream &stream);
		bool save(std::ofstream &stream);

		void render(void);
        void renderAtZ(const float z); //reested?

		bool activate(void);
		bool deactivate(void);
        
        void move(const GYVector2& dist);
        
        void setName(char const* const newName) { strcpy(_name, newName); }
        const char *getName(void) const { return _name; }

		void setElev(const unsigned char e);
		unsigned char getElev(void) const;
		DEPTH_TYPE getDepthType(void) const;
		void setDepthType(const DEPTH_TYPE depth);
        
		Renderable*		addRenderable(void);
		Animation*      addAnimation(void);
		Collidable*     addCollidable(void);
		Character*      addCharacter(void);
		Warp*           addWarp(void);
		Item*           addItem(void);
		Friendly*       addFriendly(void);
		Enemy*          addEnemy(void);
		Examinable*		addExaminable(void);
		
		Renderable*		getRenderable(void) const { return _renderable; }
		Animation*      getAnimation(void) const { return _animation; }
		Collidable*     getCollidable(void) const{ return _collidable; }
		Character*      getCharacter(void) const { return _character; }
		Warp*           getWarp(void) const { return _warp; }
		Item*           getItem(void) const { return _item; }
		Friendly*       getFriendly(void) const { return _friendly; }
		Enemy*          getEnemy(void) const { return _enemy; }
		Examinable*		getExaminable(void) const { return _examinable; }
		
		void removeRenderable(void);
		void removeAnimation(void);
		void removeCollidable(void);
		void removeCharacter(void);
		void removeWarp(void);
		void removeItem(void);
		void removeFriendly(void);
		void removeEnemy(void);
		void removeExaminable(void);
 
		Behavior* addBehavior(char const* behaviorName);
        Behavior* getBehavior(char const* constbehaviorName) const;
		LuaBehavior* addLuaBehavior(char const* scriptName);
		LuaBehavior* getLuaBehavior(char const* scriptName) const;
		std::map<char const *, Behavior*, ltstr>& getBehaviors(void);

		void removeBehavior(char const* behaviorName);
		void removeLuaBehavior(char const* scriptName);
        
        float getDepthBaseY(void) const;
        
    protected:
		
        char _name[ELYSIAN_ENTITY_NAME_SIZE];
        bool _active;
		
		Renderable*		_renderable;
		Animation*      _animation;
		Collidable*     _collidable;
		Character*      _character;
		Warp*           _warp;
		Item*           _item;
		Friendly*       _friendly;
		Enemy*          _enemy;
		Examinable*		_examinable;
        //RigidBody*    _rigidBody;
		
        bool _componentActive[Component::MAX_TYPE];
        
        std::map<char const *, Behavior*, ltstr> _behavior;

		unsigned char _elev;
		DEPTH_TYPE _depthType;
		
		bool attachComponent(Component* const comp);
		Behavior* attachBehavior(Behavior *const behav);
		void reested(void); //default for failed deserialization attempts
    }; 


	inline void Entity::setElev(const unsigned char e) { _elev = e; }
	inline unsigned char Entity::getElev(void) const { return _elev; }
	inline DEPTH_TYPE Entity::getDepthType(void) const { return _depthType; }
	inline void Entity::setDepthType(const DEPTH_TYPE depth) { _depthType = depth; }
    inline float Entity::getDepthBaseY(void) const { return (transform._pos.y + 0.5f * transform._size.y + 16.0f); }
	inline std::map<char const *, Behavior*, ltstr>& Entity::getBehaviors(void) { return _behavior; }

}

// ======== OOLUA WRAPPER =========


// ======= /OOLUA WRAPPER =======

#endif

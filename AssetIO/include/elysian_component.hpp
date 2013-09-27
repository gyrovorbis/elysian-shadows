#ifndef ELYSIAN_COMPONENT_HPP
#define ELYSIAN_COMPONENT_HPP

#include <fstream>
#include "elysian_asset.hpp"

namespace elysian {
	class Entity;
	
	class Component: public Asset { 
		friend class Entity;
	public:
		
		enum TYPE { 
			RENDERABLE, 
			ANIMATION, 
			COLLIDABLE, 
			CHARACTER, 
			WARP, 
			EXAMINABLE,
			RIGID_BODY,
			AUDIO_SOURCE,
			ITEM, 
			FRIENDLY, 
			ENEMY, 
			MAX_TYPE, 
			BEHAVIOR 
		};

		inline static bool isValidType(const TYPE type) { 	return (type >= 0 && type < Component::MAX_TYPE); }
		
		virtual bool canAddTo(Entity& entity) = 0;
		virtual bool load(std::ifstream& stream) = 0;
		virtual bool save(std::ofstream& stream) = 0;
		virtual bool addToLevel(void) = 0;
		virtual bool removeFromLevel(void) = 0;
		virtual TYPE getType(void) const = 0;

		inline Entity* getEntity(void) const { return _entity; }
		
	protected:
		virtual ~Component(void) { }
		inline Component(Entity& parent): _entity(&parent) { }
		inline Component(void) { }
		Entity* _entity;
		
	}; 
}


#endif

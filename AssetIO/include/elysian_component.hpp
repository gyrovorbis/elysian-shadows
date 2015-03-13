#ifndef ELYSIAN_COMPONENT_HPP
#define ELYSIAN_COMPONENT_HPP

#include "elysian_asset_typedefs.hpp"
#include "elysian_asset.hpp"

ELYSIAN_ASSET_TYPEDEF(Entity)

namespace elysian {

	class Component: public Asset { 
        friend Entity;
        friend class EntityBase;
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
			LIGHT_SOURCE,
			PARTICLE_EMITTER,
			COMBAT,
            SKELETON,
			MAX_TYPE, 
			BEHAVIOR 
		};

		inline static bool isValidType(const TYPE type) { 	return (type >= 0 && type < Component::MAX_TYPE); }
		
		virtual bool canAddTo(Entity& entity) = 0;
		virtual bool load(FILE* file) = 0;
		virtual bool save(FILE* file) = 0;
		virtual bool addToLevel(void) = 0;
		virtual bool removeFromLevel(void) = 0;
		virtual TYPE getType(void) const = 0;

		inline Entity* getEntity(void) const { return _entity; }
		
	protected:
        virtual ~Component(void) = default;
        inline Component(Entity& parent): _entity(&parent) { }
        inline Component(void) = default;
		Entity* _entity;
		
	}; 
}


#endif

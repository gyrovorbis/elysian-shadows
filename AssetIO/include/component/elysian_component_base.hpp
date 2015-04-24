#ifndef ELYSIAN_COMPONENT_BASE_HPP
#define ELYSIAN_COMPONENT_BASE_HPP

#include <../utility/elysian_asset_typedefs.hpp>
#include <../utility/elysian_generic.hpp>

#define ELYSIAN_COMPONENT_CONTAINER(C) std::vector<C*>


#define ELYSIAN_COMPONENT_TYPES_LIST                    Renderable, \
                                                        Animation, \
                                                        Collidable, \
                                                        Character, \
                                                        Warp, \
                                                        Examinable, \
                                                        RigidBody, \
                                                        AudioSource, \
                                                        LightSource, \
                                                        ParticleEmitter, \
                                                        Combat, \
                                                        Skeleton, \
                                                        Behavior


//Generate forward declarations for every component type
ELYSIAN_VA_EXPAND(ELYSIAN_ASSET_TYPEDEF, ELYSIAN_COMPONENT_TYPES_LIST)

ELYSIAN_ASSET_TYPEDEF(Entity)

namespace elysian {

    class ComponentBase:
        public GenericMemberData<ComponentBase, ELYSIAN_VA_EXPAND_LIST(ELYSIAN_COMPONENT_CONTAINER, ELYSIAN_COMPONENT_TYPES_LIST) >
    {
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

        static bool     isValidType(const TYPE type);
        Entity*         getEntity(void) const;
		
	protected:
                        ~ComponentBase(void) = default;
                        ComponentBase(Entity& parent);
                        ComponentBase(void) = default;

        Entity*         _entity = nullptr;
	}; 








    //============== INLINEZ =============
    inline bool ComponentBase::isValidType(const TYPE type) { 	return (type >= 0 && type < ComponentBase::MAX_TYPE); }
    inline Entity* ComponentBase::getEntity(void) const { return _entity; }
    inline ComponentBase::ComponentBase(Entity& parent): _entity(&parent) { }
}


#endif

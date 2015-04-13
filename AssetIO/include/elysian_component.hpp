#ifndef ELYSIAN_COMPONENT_HPP
#define ELYSIAN_COMPONENT_HPP

#include <elysian_asset.hpp>

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

        //Implemented by child components
        virtual bool    canAddTo(Entity& entity) const = 0;
        virtual bool    load(FILE* file) = 0;
        virtual bool    save(FILE* file) = 0;
        virtual bool    addToLevel(void) = 0;
        virtual bool    removeFromLevel(void) = 0;
        virtual TYPE    getType(void) const = 0;

        static bool     isValidType(const TYPE type);
        Entity*         getEntity(void) const;

    protected:
        virtual         ~Component(void) = default;
                        Component(Entity& parent);
                        Component(void) = default;

        Entity* _entity;
    };








    //============== INLINEZ =============
    inline bool Component::isValidType(const TYPE type) { 	return (type >= 0 && type < Component::MAX_TYPE); }
    inline Entity* Component::getEntity(void) const { return _entity; }
    inline Component::Component(Entity& parent): _entity(&parent) { }
}


#endif

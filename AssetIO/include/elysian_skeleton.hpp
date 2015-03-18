#ifndef ELYSIAN_SKELETON_HPP
#define ELYSIAN_SKELETON_HPP

#include "elysian_component.hpp"
#include "elysian_lua_table_proxy.hpp"
#include "elysian_asset_typedefs.hpp"

struct spAtlas;
struct spSkeleton;
struct spSkeletonData;
struct spAnimationStateData;
struct spAnimationState;

ELYSIAN_ASSET_TYPEDEF(Entity);

namespace elysian {

    class Skeleton;

    class SkeletonAttrib {
    friend class Skeleton;
        spAtlas*                _atlas         = nullptr;
        spSkeletonData*         _skeletonData  = nullptr;
        spAnimationStateData*   _animStateData = nullptr;

    public:
        ~SkeletonAttrib(void);

        bool loadAssets(const char*const atlas, const char*const json);
    };

    class Skeleton:
            public Component,
            public LuaTableProxy<Skeleton>
    {
        SkeletonAttrib*     _attrib         = nullptr;
        spSkeleton*         _skeleton       = nullptr;
        spAnimationState*   _animState      = nullptr;

        float               _animTimeScale  = 1.0f;

    public:
                        Skeleton(Entity& entity);
        virtual         ~Skeleton(void) override;

       //reimplemented from component
        virtual bool    canAddTo(Entity& entity) const override;
        virtual bool    load(FILE* file) override;
        virtual bool    save(FILE* file) override;
        virtual bool    addToLevel(void) override;
        virtual bool    removeFromLevel(void) override;
        virtual TYPE    getType(void) const override;

        void            update(const float deltaTime) const;
        void            render(void) const;

        void            setAttrib(SkeletonAttrib*const attrib);
        SkeletonAttrib* getAttrib(void) const;

        void            setAnimTimeScale(const float scale);
        float           getAnimTimeScale(void) const;

    };





    // ========= INLINEZ =============
    //inline SkeletonAttrib::SkeletonAttrib(void) {}

    inline SkeletonAttrib* Skeleton::getAttrib(void) const { return _attrib; }

    inline Skeleton::Skeleton(Entity& entity): Component(entity) {}
    inline void Skeleton::setAnimTimeScale(const float scale) { _animTimeScale = scale; }
    inline float Skeleton::getAnimTimeScale(void) const { return _animTimeScale; }


}

#endif // ELYSIAN_SKELETON_HPP

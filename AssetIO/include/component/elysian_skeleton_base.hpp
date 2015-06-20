#ifndef ELYSIAN_SKELETON_BASE_HPP
#define ELYSIAN_SKELETON_BASE_HPP

#include <elysian_component.hpp>
#include <elysian_lua_table_proxy.hpp>

struct spAtlas;
struct spSkeleton;
struct spSkeletonData;
struct spAnimationStateData;
struct spAnimationState;

namespace elysian {

    class SkeletonAttrib {
    friend class SkeletonBase;
    friend Skeleton;
        spAtlas*                _atlas         = nullptr;
        spSkeletonData*         _skeletonData  = nullptr;
        spAnimationStateData*   _animStateData = nullptr;

    public:
        ~SkeletonAttrib(void);

        bool loadAssets(const char*const atlas, const char*const json);
    };

    class SkeletonBase:
            public Component,
            public LuaTableProxy<SkeletonBase, Skeleton>
    {
        SkeletonAttrib*     _attrib         = nullptr;
        spSkeleton*         _skeleton       = nullptr;
        spAnimationState*   _animState      = nullptr;

        float               _animTimeScale  = 1.0f;

    public:
                        SkeletonBase(Entity& entity);
        virtual         ~SkeletonBase(void) override;

       //reimplemented from component
        static constexpr const Component::TYPE componentType = Component::SKELETON;

        void            update(const float deltaTime) const;
        void            render(void) const;

        void            setAttrib(SkeletonAttrib*const attrib);
        SkeletonAttrib* getAttrib(void) const;

        void            setAnimTimeScale(const float scale);
        float           getAnimTimeScale(void) const;

        void            setAnimByName(const int trackIndex, const char*const animationName, const bool loop) const;

    };





    // ========= INLINEZ =============
    //inline SkeletonAttrib::SkeletonAttrib(void) {}

    inline SkeletonAttrib* SkeletonBase::getAttrib(void) const { return _attrib; }

    inline SkeletonBase::SkeletonBase(Entity& entity): Component(entity) {}
    inline void SkeletonBase::setAnimTimeScale(const float scale) { _animTimeScale = scale; }
    inline float SkeletonBase::getAnimTimeScale(void) const { return _animTimeScale; }

}

#endif // ELYSIAN_SKELETON_HPP

#ifndef ELYSIAN_SKELETON_BASE_HPP
#define ELYSIAN_SKELETON_BASE_HPP

#include <elysian_component.hpp>
#include <elysian_lua_table_proxy.hpp>

#define ELYSIAN_SKELETON_ATLAS_PATH_SIZE    300
#define ELYSIAN_SKELETON_JSON_PATH_SIZE     300

ELYSIAN_ASSET_TYPEDEF(SkeletonAttrib)
ELYSIAN_ASSET_TYPEDEF(Skeleton)

struct spAtlas;
struct spSkeleton;
struct spSkeletonData;
struct spAnimationStateData;
struct spAnimationState;

namespace elysian {

    class SkeletonAttribBase:
        public LuaTableProxy<SkeletonAttrib>
    {
    friend class SkeletonBase;
    friend Skeleton;

    ELYSIAN_LUA_PROXY_FIELDS_BEGIN()
        ELYSIAN_LUA_PROXY_FIELD(atlasPath,               getAtlasPath,      setAtlasPath)
        ELYSIAN_LUA_PROXY_FIELD(jsonPath,                getJsonPath,       setJsonPath)
    ELYSIAN_LUA_PROXY_FIELDS_END()

        char                    _atlasPath[ELYSIAN_SKELETON_ATLAS_PATH_SIZE]    = { 0 };
        char                    _jsonPath[ELYSIAN_SKELETON_JSON_PATH_SIZE]      = { 0 };
        spAtlas*                _atlas          = nullptr;
        spSkeletonData*         _skeletonData   = nullptr;
        spAnimationStateData*   _animStateData  = nullptr;

    protected:
        void                    _reloadAtlas(void);
        void                    _reloadJson(void);

    public:
                                ~SkeletonAttribBase(void);

        void                    loadAssets(const char*const atlas, const char*const json);
        void                    setAtlasPath(const char*const file);
        const char*             getAtlasPath(void) const;
        void                    setJsonPath(const char*const json);
        const char*             getJsonPath(void) const;

        bool                    isReested(void) const;
    };

    class SkeletonBase:
            public Component,
            public LuaTableProxy<SkeletonBase, Skeleton>
    {
        ELYSIAN_LUA_PROXY_FIELDS_BEGIN()
            ELYSIAN_LUA_PROXY_FIELD(attrib,     getAttrib,      setAttrib)
        ELYSIAN_LUA_PROXY_FIELDS_END()

        SkeletonAttrib*     _attrib         = nullptr;
        spSkeleton*         _skeleton       = nullptr;
        spAnimationState*   _animState      = nullptr;

        float               _animTimeScale  = 1.0f;

    public:
                        SkeletonBase(Entity& entity);
                        SkeletonBase(void) = default;
        virtual         ~SkeletonBase(void) override;

       //reimplemented from component
        static constexpr const Component::TYPE componentType = Component::SKELETON;

        void            update(const float deltaTime) const;
        void            render(void) const;

        void            setAttrib(SkeletonAttrib*const attrib);
        SkeletonAttrib* getAttrib(void) const;

        void            setAnimTimeScale(const float scale);
        float           getAnimTimeScale(void) const;

        void            setAnimByName(const int trackIndex, const char*const animName, const bool loop=true) const;
        //void            addAnimByName(const int trackIndex, const char*const animName, const bool loop, const float delay) const;
        //void            mixAnimByName(const int trackIndex, const char*const animName, const bool loop, const float alpha) const;
        //void            clearAnim(const int trackIndex) const;
        //bool            isAnimComplete(const int trackIndex) const;

    };





    // ========= INLINEZ =============
    //inline SkeletonAttrib::SkeletonAttrib(void) {}

    inline SkeletonAttrib* SkeletonBase::getAttrib(void) const { return _attrib; }
    inline const char* SkeletonAttribBase::getAtlasPath(void) const { return _atlasPath; }
    inline const char* SkeletonAttribBase::getJsonPath(void) const { return _jsonPath; }
    inline bool SkeletonAttribBase::isReested(void) const { return !_atlas || !_skeletonData || !_animStateData; }

    inline SkeletonBase::SkeletonBase(Entity& entity): Component(entity) {}
    inline void SkeletonBase::setAnimTimeScale(const float scale) { _animTimeScale = scale; }
    inline float SkeletonBase::getAnimTimeScale(void) const { return _animTimeScale; }



}

#endif // ELYSIAN_SKELETON_HPP

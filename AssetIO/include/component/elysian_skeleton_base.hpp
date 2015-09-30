#ifndef ELYSIAN_SKELETON_BASE_HPP
#define ELYSIAN_SKELETON_BASE_HPP

#include <crtp/elysian_component.hpp>
#include <crtp/elysian_lua_table_proxy.hpp>
#include <spine/Attachment.h>
#include <initializer_list>

#define ELYSIAN_SKELETON_ATLAS_PATH_SIZE        300
#define ELYSIAN_SKELETON_JSON_PATH_SIZE         300
#define ELYSIAN_ANIMATION_NAME_SIZE             100

ELYSIAN_ASSET_TYPEDEF(SkeletonAttrib)
ELYSIAN_ASSET_TYPEDEF(Skeleton)

struct spAtlas;
struct spSkeleton;
struct spSkeletonData;
struct spAnimationStateData;
struct spAnimationState;

namespace elysian {

    struct EntityAttachment: public spAttachment {
        Entity* entity = nullptr;

        EntityAttachment(const char*_name);
    };

    struct AnimationTrack {
        AnimationTrack(const char*name, const bool looping=true): _looping(looping) { strcpy(_name, name); }
        char    _name[ELYSIAN_ANIMATION_NAME_SIZE] = { 0 };
        float   _timeScale = 1.0f;
        bool    _looping    = true;

    };


    struct SkeletalAnimation {
        char _name[ELYSIAN_ANIMATION_NAME_SIZE] = { 0 };
        SkeletalAnimation(const char*const name, const std::initializer_list<AnimationTrack*> &tracks):
            _tracks(tracks) {
            strcpy(_name, name);
        }

        std::vector<AnimationTrack*> _tracks;
        unsigned _trackCount = 0;
    };



    class SkeletonAttribBase:
        public LuaTableProxy<SkeletonAttrib>
    {
    friend class SkeletonBase;
    friend Skeleton;

    ELYSIAN_LUA_PROXY_FIELDS_BEGIN()
        ELYSIAN_LUA_PROXY_FIELD(atlasPath,               getAtlasPath,      setAtlasPath)
        ELYSIAN_LUA_PROXY_FIELD(jsonPath,                getJsonPath,       setJsonPath)
    ELYSIAN_LUA_PROXY_FIELDS_END()


        std::vector<SkeletalAnimation*> _animations;
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

        void                    addAnimations(const std::initializer_list<SkeletalAnimation*>& list);
        SkeletalAnimation*      findAnimation(const char*name) const;

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
        SkeletalAnimation*  _curAnim        = nullptr;

        float               _animTimeScale  = 1.0f;

    public:
                        SkeletonBase(Entity& entity, const SkeletonBase* other=nullptr);
                        SkeletonBase(void) = default;
                        ~SkeletonBase(void);
        SkeletonBase&   operator= (const SkeletonBase& other);

       //reimplemented from component
        static constexpr const Component::TYPE componentType = Component::SKELETON;

        void            update(const float deltaTime) const;
        void            render(void) const;

        void            setAttrib(SkeletonAttrib*const attrib);
        SkeletonAttrib* getAttrib(void) const;

        void            setAnimTimeScale(const float scale);
        float           getAnimTimeScale(void) const;

        void            setAnimByName(const int trackIndex, const char*const animName, const bool loop=true) const;
        void            setAnim(const char* name);

        void            attachEntityToSlot(const char* slot, Entity* entity);
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

    inline SkeletonBase::SkeletonBase(Entity& entity, const SkeletonBase *other): Component(entity) {
        if(other) *this = *other;
    }
    inline void SkeletonBase::setAnimTimeScale(const float scale) { _animTimeScale = scale; }
    inline float SkeletonBase::getAnimTimeScale(void) const { return _animTimeScale; }


    inline void SkeletonAttribBase::addAnimations(const std::initializer_list<SkeletalAnimation*>& list) { _animations.insert(_animations.end(), list); }

    inline SkeletalAnimation* SkeletonAttribBase::findAnimation(const char *name) const {
        for(unsigned i = 0; i < _animations.size(); ++i) {
            if(strcmp(_animations[i]->_name, name) == 0) return _animations[i];
        }
        return nullptr;
    }


}

#endif // ELYSIAN_SKELETON_HPP

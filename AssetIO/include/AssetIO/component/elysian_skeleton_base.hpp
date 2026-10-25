/*
 * Copyright (c) 2013-2026 Elysian Shadows Team
 * SPDX-License-Identifier: MIT
 * Additional permission for Elysian Shadows Kickstarter backers: see LICENSE.
 */

#ifndef ELYSIAN_SKELETON_BASE_HPP
#define ELYSIAN_SKELETON_BASE_HPP

#include <AssetIO/crtp/elysian_component.hpp>
#include <AssetIO/crtp/elysian_lua_table_proxy.hpp>
#include <spine/Attachment.h>
#include <spine/AnimationState.h>
#include <initializer_list>
#include <memory>

#define ELYSIAN_SKELETON_ATLAS_PATH_SIZE        300
#define ELYSIAN_SKELETON_JSON_PATH_SIZE         300
#define ELYSIAN_ANIMATION_NAME_SIZE             100
#define ELYSIAN_ANIMATION_DEFAULT_NAME          "Idle"

ELYSIAN_ASSET_TYPEDEF(AnimationTrack)
ELYSIAN_ASSET_TYPEDEF(Animation)
ELYSIAN_ASSET_TYPEDEF(SkeletonAttrib)
ELYSIAN_ASSET_TYPEDEF(Skeleton)

struct spAtlas;
struct spSlot;
struct spSkeleton;
struct spSkeletonData;
struct spAnimationStateData;
struct spAnimationState;

namespace elysian {

    struct EntityAttachment: public spAttachment {
        Entity*     entity  = nullptr;
        Entity*     parent  = nullptr;
        spSlot*     slot    = nullptr;
                    EntityAttachment(void);

        void        update(void);
        static void dispose(spAttachment* attachment);
    };

    struct AnimationTrackBase: LuaTableProxy<AnimationTrack> {

        ELYSIAN_LUA_PROXY_FIELDS_BEGIN()
            ELYSIAN_LUA_PROXY_FIELD(name,       getName,        setName)
            ELYSIAN_LUA_PROXY_FIELD(timeScale,  getTimeScale,   setTimeScale)
            ELYSIAN_LUA_PROXY_FIELD(looping,    getLooping,     setLooping)
        ELYSIAN_LUA_PROXY_FIELDS_END()

        char        _name[ELYSIAN_ANIMATION_NAME_SIZE] = { 0 };
        float       _timeScale = 1.0f;
        bool        _looping    = true;

                    AnimationTrackBase(void) = default;
                    AnimationTrackBase(const char*name, const bool looping=true);
        const char* getName(void) const;
        void        setName(const char* name);
        float       getTimeScale(void) const;
        void        setTimeScale(const float scale);
        bool        getLooping(void) const;
        void        setLooping(const bool looping);

    };


    struct AnimationBase: public LuaTableProxy<Animation> {

        ELYSIAN_LUA_PROXY_FIELDS_BEGIN()
            ELYSIAN_LUA_PROXY_FIELD(name,           getName,    setName)
            ELYSIAN_LUA_PROXY_ARRAY_FIELD(tracks,   getTrack,   setTrack,   getTrackCount)
        ELYSIAN_LUA_PROXY_FIELDS_END()

        std::vector<AnimationTrack*> _tracks;
        char            _name[ELYSIAN_ANIMATION_NAME_SIZE] = { 0 };

                        AnimationBase(void) = default;
                        AnimationBase(const char*const name, const std::initializer_list<AnimationTrack*>& tracks);
                        ~AnimationBase(void);
        const char*     getName(void) const;
        void            setName(const char*name);
        AnimationTrack* getTrack(const unsigned index) const;
        void            setTrack(const unsigned index, AnimationTrack* track);
        unsigned        getTrackCount(void) const;
    };


    class SkeletonAttribBase:
        public LuaTableProxy<SkeletonAttrib>
    {
    friend class SkeletonBase;
    friend Skeleton;

    ELYSIAN_LUA_PROXY_FIELDS_BEGIN()
        ELYSIAN_LUA_PROXY_FIELD(atlasPath,         getAtlasPath,      setAtlasPath)
        ELYSIAN_LUA_PROXY_FIELD(jsonPath,          getJsonPath,       setJsonPath)
        ELYSIAN_LUA_PROXY_ARRAY_FIELD(animations,  getAnimation,      setAnimation,       getAnimationCount)
    ELYSIAN_LUA_PROXY_FIELDS_END()

        std::vector<Animation*> _animations;
        char                    _atlasPath[ELYSIAN_SKELETON_ATLAS_PATH_SIZE]    = { 0 };
        char                    _jsonPath[ELYSIAN_SKELETON_JSON_PATH_SIZE]      = { 0 };
        char                    _defaultAnimName[ELYSIAN_ANIMATION_NAME_SIZE]   = { 0 };
        spAtlas*                _atlas          = nullptr;
        spSkeletonData*         _skeletonData   = nullptr;
        spAnimationStateData*   _animStateData  = nullptr;

    protected:
        void                    _loadAtlas(void);
        void                    _loadJson(void);

    public:
                                SkeletonAttribBase(void);
                                ~SkeletonAttribBase(void);

        void                    loadAssets(const char*const atlas, const char*const json);
        void                    setAtlasPath(const char*const file);
        const char*             getAtlasPath(void) const;
        void                    setJsonPath(const char*const json);
        const char*             getJsonPath(void) const;
        const char*             getDefaultAnimationName(void) const;

        bool                    isAtlasValid(void) const;
        bool                    isJsonValid(void) const;

        Animation*              getAnimation(const unsigned index) const;
        spAnimation*            findSpineAnimation(const char* name) const;
        void                    setAnimation(const unsigned index, Animation* anim);
        unsigned                getAnimationCount(void) const;
        const char*             getSpineAnimNameByIndex(const unsigned index) const;
        const char*             getSpineSkinNameByIndex(const unsigned index) const;

        void                    addAnimations(const std::initializer_list<Animation*>& list);
        Animation*              findAnimation(const char*name) const;

        bool                    isReested(void) const;
    };

    class SkeletonBase:
            public Component,
            public LuaTableProxy<SkeletonBase, Skeleton>
    {
        ELYSIAN_LUA_PROXY_FIELDS_BEGIN()
            ELYSIAN_LUA_PROXY_FIELD(attrib,     getAttrib,      setAttrib)
            ELYSIAN_LUA_PROXY_FIELD(skin,       getSkin,        setSkin)
            ELYSIAN_LUA_PROXY_FIELD(curAnim,    getAnimName,    playAnimNameBlocking)
        ELYSIAN_LUA_PROXY_FIELDS_END()

        using AnimListener = std::function<bool(int, spEventType, spEvent*, int)>;

        std::vector<EntityAttachment*>
                            _entityAttachments;
        //SkeletonAttrib*     _attrib         = nullptr;
        std::shared_ptr<SkeletonAttrib> _attrib;
        spSkeleton*         _skeleton       = nullptr;
        spAnimationState*   _animState      = nullptr;
        const Animation*    _curAnim        = nullptr;
        AnimListener        _listener;

        float               _animTimeScale  = 1.0f;
        uint32_t            _blockedTracks  = 0;

        EntityAttachment*   _findEntityAttachment(const char*const name) const;
        void                _updateEntityAttachments(void) const;
        void                _clearEntityAttachments(void);
        void                _removeEntityAttachment(EntityAttachment* attachment);

        static void         _animationStateListener(spAnimationState* state, int trackIndex, spEventType type, spEvent* event, int loopCount);

    public:
                        SkeletonBase(Entity& entity, const SkeletonBase* other=nullptr);
                        SkeletonBase(void) = default;
                        ~SkeletonBase(void);
        SkeletonBase&   operator= (const SkeletonBase& other);

       //reimplemented from component
        static constexpr const Component::TYPE componentType = Component::SKELETON;

        void            update(const float deltaTime) const;
        void            render(void) const;

        void            setAttrib(std::shared_ptr<SkeletonAttrib> attrib);
        std::shared_ptr<SkeletonAttrib> getAttrib(void) const;

        void            setAnimTimeScale(const float scale);
        float           getAnimTimeScale(void) const;

        void            setAnimTrackByName(const int trackIndex, const char*const animName, const bool loop=true);
        void            setAnimName(const char* name);
        void            setAnim(const Animation* anim);
        void            setAnimName(const char* name, const AnimListener& listener);
        void            setAnim(const Animation* anim, const AnimListener& listener);
        const char*     getAnimName(void) const;
        void            playAnimNameBlocking(const char* name);


        void            setSkin(const char* name);
        const char*     getSkin(void) const;

        Entity*         getEntityAttachedToSlot(const char*name) const;
        void            attachEntityToSlot(const char* name, Entity* entity);
        void            removeEntityFromSlot(const char* name);
        //void            addAnimByName(const int trackIndex, const char*const animName, const bool loop, const float delay) const;
        //void            mixAnimByName(const int trackIndex, const char*const animName, const bool loop, const float alpha) const;
        //void            clearAnim(const int trackIndex) const;
        //bool            isAnimComplete(const int trackIndex) const;

    };






    // ========= INLINEZ =============

    //------- ANIMATIONTRACK -------
    inline AnimationTrackBase::AnimationTrackBase(const char*name, const bool looping): _looping(looping) { strcpy(_name, name); }
    inline const char* AnimationTrackBase::getName(void) const { return _name; }
    inline void AnimationTrackBase::setName(const char* name) { strcpy(_name, name); }
    inline float AnimationTrackBase::getTimeScale(void) const { return _timeScale; }
    inline void AnimationTrackBase::setTimeScale(const float scale) { _timeScale = scale; }
    inline bool AnimationTrackBase::getLooping(void) const { return _looping; }
    inline void AnimationTrackBase::setLooping(const bool looping) { _looping = looping; }

    //----- ANIMATION -------
    inline AnimationBase::AnimationBase(const char*const name, const std::initializer_list<AnimationTrack*> &tracks):
        _tracks(tracks) {
        strcpy(_name, name);
    }
    inline const char* AnimationBase::getName(void) const { return _name; }
    inline void AnimationBase::setName(const char*name) { strcpy(_name, name); }
    inline AnimationTrack* AnimationBase::getTrack(const unsigned index) const { return index < _tracks.size()? _tracks[index] : nullptr; }
    inline unsigned AnimationBase::getTrackCount(void) const { return _tracks.size(); }



    //------ SKELETONATTRIB -------
    inline SkeletonAttribBase::SkeletonAttribBase(void) { strcpy(_defaultAnimName, ELYSIAN_ANIMATION_DEFAULT_NAME); }
    inline const char* SkeletonAttribBase::getDefaultAnimationName(void) const { return _defaultAnimName; }
    inline const char* SkeletonAttribBase::getAtlasPath(void) const { return _atlasPath; }
    inline const char* SkeletonAttribBase::getJsonPath(void) const { return _jsonPath; }
    inline bool SkeletonAttribBase::isReested(void) const { return !_atlas || !_skeletonData || !_animStateData; }
    inline unsigned SkeletonAttribBase::getAnimationCount(void) const { return _animations.size(); }
    inline Animation* SkeletonAttribBase::getAnimation(const unsigned index) const { return index < _animations.size()? _animations[index] : nullptr; }
    inline bool SkeletonAttribBase::isAtlasValid(void) const { return _atlas; }
    inline bool SkeletonAttribBase::isJsonValid(void) const { return _skeletonData && _animStateData; }
    inline spAnimation* SkeletonAttribBase::findSpineAnimation(const char *name) const { return spSkeletonData_findAnimation(_skeletonData, name); }


    //----- SKELETON ------
    inline std::shared_ptr<SkeletonAttrib> SkeletonBase::getAttrib(void) const { return _attrib; }
    inline SkeletonBase::SkeletonBase(Entity& entity, const SkeletonBase *other): Component(entity) {
        if(other) *this = *other;
    }
    inline void SkeletonBase::setAnimTimeScale(const float scale) { _animTimeScale = scale; }
    inline float SkeletonBase::getAnimTimeScale(void) const { return _animTimeScale; }


    inline void SkeletonAttribBase::addAnimations(const std::initializer_list<Animation*>& list) { _animations.insert(_animations.end(), list); }

    inline Entity* SkeletonBase::getEntityAttachedToSlot(const char *name) const {
        auto* slot = _findEntityAttachment(name);
        return slot? slot->entity : nullptr;
    }


}

#endif // ELYSIAN_SKELETON_HPP

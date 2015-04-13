#ifndef ELYSIAN_WARP_HPP
#define ELYSIAN_WARP_HPP

#include <cstring>
#include <elysian_component.hpp>
#include <elysian_lua_table_proxy.hpp>
#include <elysian_instance_transform.hpp>

#define ELYSIAN_WARP_AREA_PATH_MAX_CHARS    200
#define ELYSIAN_WARP_LEVEL_PATH_MAX_CHARS   200

ELYSIAN_ASSET_TYPEDEF(Entity)

namespace elysian {

    class Warp:
        public Component,
        public LuaTableProxy<Warp>
    {
        friend Entity;
        friend class EntityBase;
        friend class WarpSystem;


    private:
        GYVector3               _toPosition;
        bool                    _changeArea     = false;
        bool                    _changeLevel    = false;
        bool                    _obliqueTran    = true;
        char                    _areaPath[ELYSIAN_WARP_AREA_PATH_MAX_CHARS];
        char                    _levelPath[ELYSIAN_WARP_LEVEL_PATH_MAX_CHARS];

                                Warp(Entity &parent);
                                Warp(void) = default;
        virtual                 ~Warp(void) override = default;

    public:
        ELYSIAN_LUA_PROXY_FIELDS_BEGIN()
            ELYSIAN_LUA_PROXY_FIELD(toPos,              getToPos)
            ELYSIAN_LUA_PROXY_FIELD(changeArea,         getChangeArea,          setChangeArea)
            ELYSIAN_LUA_PROXY_FIELD(changeLevel,        getChangeLevel,         setChangeLevel)
            ELYSIAN_LUA_PROXY_FIELD(areaPath,           getAreaPath,            setAreaPath)
            ELYSIAN_LUA_PROXY_FIELD(levelPath,          getLevelPath,           setLevelPath)
            ELYSIAN_LUA_PROXY_FIELD(obliqueTran,        getObliqueTransition,   setObliqueTransition)
        ELYSIAN_LUA_PROXY_FIELDS_END()

        void                    setToPos(const float x, const float y, const float z);
        Vector3                 getToPos(void);
        const char*             getAreaPath(void) const;
        void                    setAreaPath(const char *name);
        const char*             getLevelPath(void) const;
        void                    setLevelPath(const char *name);
        bool                    getChangeArea(void) const;
        void                    setChangeArea(const bool flag=true);
        bool                    getChangeLevel(void) const;
        void                    setChangeLevel(const bool flag=true);
        void                    setObliqueTransition(const bool flag=true);
        bool                    getObliqueTransition(void) const;

        //Overridden from Component
        virtual bool            load(FILE* file) override;
        virtual bool            save(FILE* file) override;
        virtual bool            canAddTo(Entity &parent) const override;
        virtual bool            addToLevel(void) override;
        virtual bool            removeFromLevel(void) override;
        virtual TYPE            getType(void) const override;
    };















    // ================ INLINEZ ===================

    inline Warp::Warp(Entity& parent): Component(parent) {}

    inline void Warp::setObliqueTransition(const bool flag) {
        _obliqueTran = flag;
    }

    inline bool Warp::getObliqueTransition(void) const { return _obliqueTran; }

    inline void Warp::setToPos(const float x, const float y, const float z) {
        _toPosition.x = x;
        _toPosition.y = y;
        _toPosition.z = z;
    }

    inline Vector3 Warp::getToPos(void) {
        return Vector3(&_toPosition);
    }

    inline const char *Warp::getAreaPath(void) const {
        return _areaPath;
    }

    inline void Warp::setAreaPath(const char *name) {
        strcpy(_areaPath, name);
    }
    inline const char *Warp::getLevelPath(void) const {
        return _levelPath;
    }

    inline void Warp::setLevelPath(const char *name) {
        strcpy(_levelPath, name);
    }

    inline void Warp::setChangeArea(const bool flag) {
        _changeArea = flag;
    }

    inline bool Warp::getChangeArea(void) const {
        return _changeArea;
    }

    inline void Warp::setChangeLevel(const bool flag) {
        _changeLevel = flag;
    }

    inline bool Warp::getChangeLevel(void) const {
        return _changeLevel;
    }

    inline auto Warp::getType(void) const ->TYPE { return Component::WARP; }


}

#endif

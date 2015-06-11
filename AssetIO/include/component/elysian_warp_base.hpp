#ifndef ELYSIAN_WARP_BASE_HPP
#define ELYSIAN_WARP_BASE_HPP

#include <cstring>
#include <elysian_component.hpp>
#include <elysian_lua_table_proxy.hpp>
#include <elysian_instance_transform.hpp>

#define ELYSIAN_WARP_AREA_PATH_MAX_CHARS    200
#define ELYSIAN_WARP_SEVERITY_PATH_MAX_CHARS   200

namespace elysian {
	
    class WarpBase:
        public Component,
        public LuaTableProxy<WarpBase, Warp>
    {
        friend Entity;
        friend class EntityBase;
        friend class WarpSystem;


		
    protected:
        GYVector3               _toPosition;
        bool                    _changeArea     = false;
        bool                    _changeLevel    = false;
        bool                    _obliqueTran    = true;
        char                    _areaPath[ELYSIAN_WARP_AREA_PATH_MAX_CHARS];
        char                    _levelPath[ELYSIAN_WARP_SEVERITY_PATH_MAX_CHARS];
		
                                WarpBase(Entity &parent);
                                WarpBase(void) = default;
                                 //~WarpBase(void) = default;
		
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
        static constexpr const Component::TYPE componentType = Component::WARP;
	};















    // ================ INLINEZ ===================

    inline WarpBase::WarpBase(Entity& parent): Component(parent) {}

    inline void WarpBase::setObliqueTransition(const bool flag) {
		_obliqueTran = flag;
	}
	
    inline bool WarpBase::getObliqueTransition(void) const { return _obliqueTran; }

    inline void WarpBase::setToPos(const float x, const float y, const float z) {
		_toPosition.x = x;
		_toPosition.y = y;
        _toPosition.z = z;
	}
	
    inline Vector3 WarpBase::getToPos(void) {
        return Vector3(&_toPosition);
	}
	
    inline const char *WarpBase::getAreaPath(void) const {
		return _areaPath;
	}
	
    inline void WarpBase::setAreaPath(const char *name) {
		strcpy(_areaPath, name);
	}
    inline const char *WarpBase::getLevelPath(void) const {
		return _levelPath;
	}
	
    inline void WarpBase::setLevelPath(const char *name) {
		strcpy(_levelPath, name);
	}
	
    inline void WarpBase::setChangeArea(const bool flag) {
		_changeArea = flag;
	}
	
    inline bool WarpBase::getChangeArea(void) const {
		return _changeArea;
	}
	
    inline void WarpBase::setChangeLevel(const bool flag) {
		_changeLevel = flag;
	}
	
    inline bool WarpBase::getChangeLevel(void) const {
		return _changeLevel;
	}

	
}

#endif

/*
 * Copyright (c) 2013-2026 Elysian Shadows Team
 * SPDX-License-Identifier: MIT
 * Additional permission for Elysian Shadows Kickstarter backers: see LICENSE.
 */

#ifndef ELYSIAN_WARP_BASE_HPP
#define ELYSIAN_WARP_BASE_HPP

#include <cstring>
#include <AssetIO/crtp/elysian_component.hpp>
#include <AssetIO/crtp/elysian_lua_table_proxy.hpp>
#include <AssetIO/primitive/elysian_instance_transform.hpp>
#include <AssetIO/primitive/elysian_rect.hpp>

#define ELYSIAN_WARP_AREA_PATH_MAX_CHARS        200
#define ELYSIAN_WARP_SEVERITY_PATH_MAX_CHARS    200

namespace elysian {
	
    class WarpBase:
        public Component,
        public LuaTableProxy<WarpBase, Warp>
    {
        friend Entity;
        friend class EntityBase;
        friend class WarpSystem;
		
    protected:
        char                    _areaPath[ELYSIAN_WARP_AREA_PATH_MAX_CHARS]         = { 0 };
        char                    _levelPath[ELYSIAN_WARP_SEVERITY_PATH_MAX_CHARS]    = { 0 };
        Recti                   _cameraBoundaryRect                                 = Recti(-1);
        GYVector3               _toPosition;

        bool                    _changeArea                                         = false;
        bool                    _changeLevel                                        = false;
        bool                    _obliqueTran                                        = true;

		
	public:
        ELYSIAN_LUA_PROXY_FIELDS_BEGIN()
            ELYSIAN_LUA_PROXY_FIELD(toPos,              getToPosVec,            setToPosVec)
            ELYSIAN_LUA_PROXY_FIELD(changeArea,         getChangeArea,          setChangeArea)
            ELYSIAN_LUA_PROXY_FIELD(changeLevel,        getChangeLevel,         setChangeLevel)
            ELYSIAN_LUA_PROXY_FIELD(areaPath,           getAreaPath,            setAreaPath)
            ELYSIAN_LUA_PROXY_FIELD(levelPath,          getLevelPath,           setLevelPath)
            ELYSIAN_LUA_PROXY_FIELD(obliqueTran,        getObliqueTransition,   setObliqueTransition)
            ELYSIAN_LUA_PROXY_FIELD(cameraBoundaryRect, getCameraBoundaryRect,  setCameraBoundaryRect)
        ELYSIAN_LUA_PROXY_FIELDS_END()

                                WarpBase(Entity &parent, const WarpBase*const prefab=nullptr);
                                WarpBase(void) = default;
        WarpBase&               operator=(const WarpBase& rhs);

        void                    setToPos(const float x, const float y, const float z);
        Vector3                 getToPosVec(void);
        void                    setToPosVec(const Vector3& pos);
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
        Recti&                  getCameraBoundaryRect(void);
        void                    setCameraBoundaryRect(Recti rect);

        //Overridden from Component
        static constexpr const Component::TYPE componentType = Component::WARP;
	};







    // ================ INLINEZ ===================

    inline WarpBase::WarpBase(Entity& parent, const WarpBase*const prefab): Component(parent) {
        if(prefab) *this = *prefab;
    }
    inline WarpBase& WarpBase::operator=(const WarpBase& rhs) {
        _toPosition         = rhs._toPosition;
        _changeArea         = rhs._changeArea;
        _changeLevel        = rhs._changeLevel;
        _obliqueTran        = rhs._obliqueTran;
        _cameraBoundaryRect = rhs._cameraBoundaryRect;
        strcpy(_areaPath, rhs._areaPath);
        strcpy(_levelPath, rhs._levelPath);
        return *this;
    }

    inline void WarpBase::setObliqueTransition(const bool flag) {
		_obliqueTran = flag;
	}
	
    inline bool WarpBase::getObliqueTransition(void) const { return _obliqueTran; }

    inline void WarpBase::setToPos(const float x, const float y, const float z) {
		_toPosition.x = x;
		_toPosition.y = y;
        _toPosition.z = z;
	}
	
    inline Vector3 WarpBase::getToPosVec(void) {
        return Vector3(&_toPosition);
	}

    inline void WarpBase::setToPosVec(const Vector3 &pos) {
        _toPosition = pos;
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

    inline Recti& WarpBase::getCameraBoundaryRect(void) { return _cameraBoundaryRect; }
    inline void WarpBase::setCameraBoundaryRect(Recti rect) { _cameraBoundaryRect = rect; }

	
}

#endif

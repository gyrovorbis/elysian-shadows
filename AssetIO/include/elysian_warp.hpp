#ifndef ELYSIAN_WARP_HPP
#define ELYSIAN_WARP_HPP

#include <gyro.h>
#include <cstring>
#include "elysian_component.hpp"
#include "elysian_lua_table_proxy.hpp"

#define ELYSIAN_WARP_AREA_PATH_MAX_CHARS 200
#define ELYSIAN_WARP_LEVEL_PATH_MAX_CHARS 200

namespace elysian {
	
    class Warp:
        public Component,
        public LuaTableProxy<Warp>
    {
        friend class EntityBase;
        friend Entity;
		friend class WarpSystem;
	private:
		
	protected:
		GYVector2		_toPosition;
		unsigned char	_toElev = 0;
		bool			_changeArea;
		bool			_changeLevel;
		bool			_obliqueTran = true;
		char			_areaPath[ELYSIAN_WARP_AREA_PATH_MAX_CHARS];
		char			_levelPath[ELYSIAN_WARP_LEVEL_PATH_MAX_CHARS];
		
		Warp(Entity &parent);
		Warp(void);
        virtual ~Warp(void) override = default;
		
	public:
        ELYSIAN_LUA_PROXY_FIELDS_BEGIN()
        ELYSIAN_LUA_PROXY_FIELDS_END()

		void setToPos(const float x, const float y, const unsigned char elev);
		const GYVector2 &getToPos(void) const;
		const char *getAreaPath(void) const;
		void setAreaPath(const char *name);
		const char *getLevelPath(void) const;
		void setLevelPath(const char *name);
		bool getChangeArea(void) const;
		void setChangeArea(const bool flag=true);
		bool getChangeLevel(void) const;
		void setChangeLevel(const bool flag=true);
		void setObliqueTransition(const bool flag=true);

		
		bool load(FILE* file);
		bool save(FILE* file);
		bool canAddTo(Entity &parent);
		bool addToLevel(void);
		bool removeFromLevel(void);
		Component::TYPE getType(void) const { return Component::WARP; }
	};

	// ====== INLINEZ ======

	inline void Warp::setObliqueTransition(const bool flag) {
		_obliqueTran = flag;
	}
	
	inline void Warp::setToPos(const float x, const float y, const unsigned char elev) {
		_toPosition.x = x;
		_toPosition.y = y;
		_toElev = elev;
	}
	
	inline const GYVector2 &Warp::getToPos(void) const {
		return _toPosition;
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

	
}

#endif

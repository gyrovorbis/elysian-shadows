#ifndef ELYSIAN_WARP_HPP
#define ELYSIAN_WARP_HPP

#include <gyro.h>
#include <cstring>
#include "elysian_component.hpp"

#define ELYSIAN_WARP_AREA_PATH_MAX_CHARS 200
#define ELYSIAN_WARP_LEVEL_PATH_MAX_CHARS 200

namespace elysian {
	
	class Warp: public Component {
		friend class Entity;
		friend class WarpSystem;
	private:
		
	protected:
		GYVector2 _toPosition;
		bool _changeArea, _changeLevel;
		char _areaPath[ELYSIAN_WARP_AREA_PATH_MAX_CHARS];
		char _levelPath[ELYSIAN_WARP_LEVEL_PATH_MAX_CHARS];
		
		Warp(Entity &parent);
		Warp(void);
		virtual ~Warp(void);
		
	public:
		void setToPos(const GYVector2& pos);
		const GYVector2 &getToPos(void) const;
		const char *getAreaPath(void) const;
		void setAreaPath(const char *name);
		const char *getLevelPath(void) const;
		void setLevelPath(const char *name);
		bool getChangeArea(void) const;
		void setChangeArea(const bool flag=true);
		bool getChangeLevel(void) const;
		void setChangeLevel(const bool flag=true);

		
		bool load(std::ifstream& stream);
		bool save(std::ofstream& stream);
		bool canAddTo(Entity &parent);
		bool addToLevel(void);
		bool removeFromLevel(void);
		Component::TYPE getType(void) const { return Component::WARP; }
	};
	
	
	inline void Warp::setToPos(const GYVector2& pos) {
		_toPosition.x = pos.x;
		_toPosition.y = pos.y;
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

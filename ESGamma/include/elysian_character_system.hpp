#ifndef ELYSIAN_CHARACTER_SYSTEM_HPP
#define ELYSIAN_CHARACTER_SYSTEM_HPP

#include "elysian_system.hpp"
#include "elysian_subprocess_manager.hpp"
#include "elysian_entity.hpp"
#include "elysian_character.hpp"

#define ELYSIAN_CHARACTER_WALK_SPEED 18000000.0f
#define ELYSIAN_CHARACTER_SYSTEM_WAYPOINT_INTERP_POOL_SIZE	50

namespace elysian {

	class CharacterSystem;

	class WalkToWayPointSubProcess:
			public MonitorableSubProcess<WalkToWayPointSubProcess, CharacterSystem>
	{
	protected:
		Character* charComp = nullptr;
		GYVector2 _dir;
		GYVector2 _endPos;

	public:

		void _start(CharacterSystem* sys);
		void _stop(CharacterSystem* sys);
		void _update(CharacterSystem* sys, const float deltaTime);
		void setWayPoint(Character* comp, const float endX, const float endY);

	};

	typedef MonitorableSubProcessPool<WalkToWayPointSubProcess,
										CharacterSystem,
										ELYSIAN_CHARACTER_SYSTEM_WAYPOINT_INTERP_POOL_SIZE>
		WalkToWayPointPool;



	class CharacterSystem:
			public SubProcessManager<CharacterSystem, WalkToWayPointPool>,
			public System
	{
	private:
		enum SUBPROCS { SUBPROC_WAYPOINT_POOL };

	public:
		CharacterSystem(void);
		void update(void);

		void walkToWayPoint(Character*const comp, const float x, const float y, EventMonitor*const monitor=nullptr);

	};

	// ======= INLINEZ =======

	inline void WalkToWayPointSubProcess::_start(CharacterSystem* sys) {
		gyVec2Sub(&_dir, &_endPos, (GYVector2*)&charComp->getEntity()->transform._pos);
		gyVec2Normalize(&_dir);
	}

	inline void WalkToWayPointSubProcess::_stop(CharacterSystem* sys) {


	}
	inline void WalkToWayPointSubProcess::_update(CharacterSystem* sys, const float deltaTime) {
		GYVector2 dist;

		gyVec2Sub(&dist, &_endPos, (GYVector2*)&charComp->getEntity()->transform._pos);
		float mag = gyVec2Magnitude(&dist);

		gyVec2Scale(&_dir, &dist, 1.0f/mag);

		if(mag <= 4.0f) {
			stop(sys);
		} else {
			gyVec2Scale(&dist, &_dir, ELYSIAN_CHARACTER_WALK_SPEED*deltaTime);
			charComp->move(dist);
		}
	}

	inline void WalkToWayPointSubProcess::setWayPoint(Character* comp, const float endX, const float endY) {
		charComp = comp;
		_endPos.x = endX; _endPos.y = endY;
	}



	inline void CharacterSystem::walkToWayPoint(Character * const comp, const float x, const float y, EventMonitor * const monitor) {
		auto& proc = get<SUBPROC_WAYPOINT_POOL> ( static_cast<SubProcessManager<CharacterSystem,
												  WalkToWayPointPool>& >(*this));

		auto& subProc = proc.claimIdleSubProcess();
		subProc.setWayPoint(comp, x, y);
		subProc.setEventMonitor(monitor);
		subProc.start(this);
	}
	
}

#endif

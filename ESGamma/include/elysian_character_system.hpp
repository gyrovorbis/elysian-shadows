#ifndef ELYSIAN_CHARACTER_SYSTEM_HPP
#define ELYSIAN_CHARACTER_SYSTEM_HPP

#include "elysian_system.hpp"
#include "elysian_subprocess_manager.hpp"
#include "elysian_entity.hpp"
#include "elysian_character.hpp"
#include "elysian_rigid_body.hpp"

#define ELYSIAN_CHARACTER_SYSTEM_WAYPOINT_INTERP_POOL_SIZE	50

namespace elysian {

	class CharacterSystem;

	class WalkToWayPointSubProcess:
			public MonitorableSubProcess<WalkToWayPointSubProcess, CharacterSystem>
	{
	protected:
        Entity*     _entity         = nullptr;
        bool        _walk           = false;
        GYVector3   _dir;
        GYVector3   _endPos;

	public:

		void _start(CharacterSystem* sys);
		void _stop(CharacterSystem* sys);
		void _update(CharacterSystem* sys, const float deltaTime);
        void setWayPoint(Entity* comp, const float endX, const float endY);
        void setWayPoint(Entity* comp, const float endX, const float endY, float endZ);

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

        void walkToWayPoint(Entity*const entity, const float x, const float y, EventMonitor*const monitor=nullptr);
        void moveToWayPoint(Entity*const entity, const float x, const float y, const float z, EventMonitor*const monitor=nullptr);

	};

	// ======= INLINEZ =======

	inline void WalkToWayPointSubProcess::_start(CharacterSystem* sys) {
        gyVec3Sub(&_dir, &_endPos, &_entity->transform._pos);
        gyVec3Normalize(&_dir);
	}

    inline void WalkToWayPointSubProcess::_stop(CharacterSystem* sys) {}

	inline void WalkToWayPointSubProcess::_update(CharacterSystem* sys, const float deltaTime) {
        GYVector3 dist;

        gyVec3Sub(&dist, &_endPos, &_entity->transform._pos);
        float mag = gyVec3Magnitude(&dist);

        gyVec3Scale(&_dir, &dist, 1.0f/mag);

		if(mag <= 4.0f) {
			stop(sys);
		} else {
            //charComp->move(dist);
            if(_walk) {
                //gyVec3Scale(&dist, &_dir, _entity->getCharacter()->getWalkForce()*deltaTime);
                _entity->getCharacter()->walk(*(GYVector2*)&dist);
            } else {
                gyVec3Scale(&dist, &_dir, ELYSIAN_CHAR_WALK_DEFAULT_FORCE*deltaTime);
                //gyVec3AddAccum(&_entity->transform._pos, &dist);
                _entity->getRigidBody()->applyForce(dist);
            }
        }
	}

    inline void WalkToWayPointSubProcess::setWayPoint(Entity* entity, const float endX, const float endY) {
        _entity = entity;
		_endPos.x = endX; _endPos.y = endY;
        _endPos.z = entity->transform._pos.z;
        _walk = true;
	}

    inline void WalkToWayPointSubProcess::setWayPoint(Entity* entity, const float endX, const float endY, const float endZ) {
        _entity = entity;
        _endPos.x = endX; _endPos.y = endY; _endPos.z = endZ;
        _walk = false;
    }


    inline void CharacterSystem::walkToWayPoint(Entity* entity, const float x, const float y, EventMonitor * const monitor) {
		auto& proc = get<SUBPROC_WAYPOINT_POOL> ( static_cast<SubProcessManager<CharacterSystem,
												  WalkToWayPointPool>& >(*this));

		auto& subProc = proc.claimIdleSubProcess();
        subProc.setWayPoint(entity, x, y);
		subProc.setEventMonitor(monitor);
		subProc.start(this);
	}


    inline void CharacterSystem::moveToWayPoint(Entity* entity, const float x, const float y, const float z, EventMonitor * const monitor) {
        auto& proc = get<SUBPROC_WAYPOINT_POOL> ( static_cast<SubProcessManager<CharacterSystem,
                                                  WalkToWayPointPool>& >(*this));

        auto& subProc = proc.claimIdleSubProcess();
        subProc.setWayPoint(entity, x, y, z);
        subProc.setEventMonitor(monitor);
        subProc.start(this);
    }
	
}

#endif

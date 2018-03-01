#ifndef ELYSIAN_RIGID_BODY_SYSTEM_HPP
#define ELYSIAN_RIGID_BODY_SYSTEM_HPP

#define ELYSIAN_RIGID_BODY_SYSTEM_CONTACT_POOL_SIZE             10000
#define ELYSIAN_RIGID_BODY_FORCE_GENERATOR_MAX_COUNT            20
#define ELYSIAN_RIGID_BODY_CLOSING_VELOCITY_EPSILON             1.0f

#define ELYSIAN_RIGID_BODY_BREAKABLE_IMPULSE_VELOCITY_MIN       20.0f
#define ELYSIAN_RIGID_BODY_BREAKABLE_IMPULSE_ROTATION_MIN       0.4f

#define ELYSIAN_RIGID_BODY_RESTING_CONTACT_ESCAPE_VELOCITY      100.0f

#include <elysian_system.hpp>
#include <utility/elysian_mem_pool.hpp>
#include <elysian_force_generator.hpp>
#include <crtp/elysian_entity.hpp>

#define ELYSIAN_RIGID_BODY_SYSTEM_SHARD_POOL_SIZE               100
#define ELYSIAN_RIGID_BODY_SYSTEM_SHARD_POOL_FADEOUT_TIME       5.0f
#define ELYSIAN_RIGID_BODY_SYSTEM_SHARD_POOL_IDEAL_FREE_COUNT   (0.1f*ELYSIAN_RIGID_BODY_SYSTEM_SHARD_POOL_SIZE)

namespace elysian {

    class CollisionContact;
    class ForceGenerator;
    class RigidBodyShard;
    typedef Pool<RigidBodyShard> ShardPool;

    //clear pool on area change, it'll make pool allocator more efficient
    class RigidBodyShard: public Entity {
    public:
        enum STATE { STATE_NORMAL, STATE_FADE_OUT, STATE_DONE };
    private:
        ShardPool::iterator  _it;
        RigidBodyShard*     _next          = nullptr;
        float               _fadeOutTime   = 0.0f;
        float               _allocTime     = 0.0f;
        STATE               _state         = STATE_NORMAL;

    public:

        void                setIterator(const ShardPool::iterator it);
        ShardPool::iterator getIterator(void) const;
        void                startFade(const float deltaTime);
        STATE               update(const float deltaTime);
        RigidBodyShard*     getNext(void) const;
        void                setNext(RigidBodyShard* next);
        STATE               getState(void) const;
        void                free(void);
    };

    class RigidBodySystem: public System {
        CollisionContact*		_contactPool[ELYSIAN_RIGID_BODY_SYSTEM_CONTACT_POOL_SIZE];
        ShardPool                _shardPool		= ShardPool(ELYSIAN_RIGID_BODY_SYSTEM_SHARD_POOL_SIZE);
        Pool<ForceGenerator*>	_forces			= Pool<ForceGenerator*>(ELYSIAN_RIGID_BODY_FORCE_GENERATOR_MAX_COUNT);
        GravityForceGenerator	_gravityGenerator;
        unsigned				_contactCount	= 0;
        unsigned				_velResIter		= 0;
        bool					_gravityEnabled = true;
        RigidBodyShard*         _shardHead      = nullptr;
        RigidBodyShard*         _shardTail      = nullptr;

        void                _updateContacts(void);
        bool                _checkEntityEntity3D(const unsigned i);
        void                _resolveContactZVelocities(void);
        RigidBodyShard*     _claimShard(void);
        void                _removeShard(RigidBodyShard* shard);

    public:
            RigidBodySystem(void);
           // ~RigidBodySystem(void);
        void update(void);
        void integrate3D(void);

        unsigned getVelResIterUsed(void) const;

        void resolveContactVelocities(void);
        void breakRigidBodies(void);
        void integrate(void);
        void applyForces(void);

        void breakRigidBody(RigidBody*const rigidBody);
        void addForce(ForceGenerator*const force);
        void applyForce(ForceGenerator*const force);
        void removeForce(ForceGenerator*const force);
        void clearForces(void);

        void setGravityEnabled(const bool val);
        void setGravityAccel(const GYVector3& accel);

    };

    //====== INLINEZ =======
    inline void RigidBodySystem::addForce(ForceGenerator * const force) {
        _forces.insertEntry(force);
    }
    inline void RigidBodySystem::removeForce(ForceGenerator * const force) {
        _forces.removeEntry(force);
    }
    inline void RigidBodySystem::clearForces(void) {
        _forces.clear();
    }
    inline unsigned RigidBodySystem::getVelResIterUsed(void) const {
        return _velResIter;
    }

    inline void RigidBodySystem::setGravityEnabled(const bool val) {
        if(_gravityEnabled == val) return;

        if(val) {
            addForce(&_gravityGenerator);
        } else {
            removeForce(&_gravityGenerator);
        }
        _gravityEnabled = val;
    }

    inline void RigidBodySystem::setGravityAccel(const GYVector3 &accel) {_gravityGenerator.setAccel(accel);}

    inline ShardPool::iterator RigidBodyShard::getIterator(void) const { return _it; }
    inline void RigidBodyShard::setIterator(const ShardPool::iterator it) { _it = it; }
    inline RigidBodyShard* RigidBodyShard::getNext(void) const { return _next; }
    inline void RigidBodyShard::setNext(RigidBodyShard *next) { _next = next; }
    inline auto RigidBodyShard::getState(void) const -> STATE { return _state; }
    inline void RigidBodyShard::startFade(const float deltaTime) { _fadeOutTime += deltaTime; _state = STATE_FADE_OUT; }

}

#endif

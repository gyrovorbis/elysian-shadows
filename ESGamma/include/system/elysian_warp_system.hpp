#ifndef ELYSIAN_WARP_SYSTEM_HPP
#define ELYSIAN_WARP_SYSTEM_HPP

#include <ESGamma/system/elysian_system.hpp>
#include <AssetIO/crtp/elysian_warp.hpp>
#include <ESGamma/gui/elysian_sprite.hpp>
#include <ESGamma/system/elysian_collision_system.hpp>

#include <libGyro/gyro.h>

namespace elysian {

	class WarpSystem:	public System,
						public CollisionContactEventMonitor {
	public:
		enum STATE { CHECK_WARP, FADE_OUT, WARP, FADE_IN, PAUSE };
		
		WarpSystem(void); 
        void initialize(void);
		void update(void);
		void render(void);

		//reimplemented from CollisionContactEventMonitor
		virtual void onCollisionEnter(Collidable*const bodyA, Collidable*const bodyB, CollisionContact* contact) override;
		virtual void onCollide(Collidable*const bodyA, Collidable*const bodyB, CollisionContact* contact) override;
		virtual void onCollisionExit(Collidable*const bodyA, Collidable*const bodyB) override;
		
		void setRenderWarps(const bool flag);
		bool getRenderWarps(void) const;
		void Disable(void);
		void Enable(void);

        STATE   getState(void) const;
        Warp& getCurrentWarp(void);


	private:
		void UpdateCheckWarp(void);
		void UpdateFadeOut(void);
		void UpdateFadeIn(void);
		void UpdatePause(void);
		
		bool _renderWarps		= true;
		bool _enabled			= true;
		float _timer			= 0.0f;
        float _walkForce        = 0.0f;
		STATE _state			= CHECK_WARP;
		Warp _currentWarp;
		
		Sprite _screenRect;
	}; 
	




//======= INLINEZ =======

    inline auto WarpSystem::getState(void) const -> STATE { return _state; }

    inline Warp& WarpSystem::getCurrentWarp(void) { return _currentWarp; }

}


#endif

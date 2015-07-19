#ifndef ELYSIAN_WARP_SYSTEM_HPP
#define ELYSIAN_WARP_SYSTEM_HPP

#include "elysian_system.hpp"
#include <crtp/elysian_warp.hpp>
#include "elysian_sprite.hpp"
#include "elysian_collision_system.hpp"

#include <gyro.h>

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


	private:
		void UpdateCheckWarp(void);
		void UpdateFadeOut(void);
		void UpdateFadeIn(void);
		void UpdatePause(void);
		
		bool _renderWarps		= true;
		bool _enabled			= true;
		float _timer			= 0.0f;
		STATE _state			= CHECK_WARP;
		Warp _currentWarp;
		
		Sprite _screenRect;
	}; 
	
}

#endif

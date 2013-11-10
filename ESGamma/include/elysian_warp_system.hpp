#ifndef ELYSIAN_WARP_SYSTEM_HPP
#define ELYSIAN_WARP_SYSTEM_HPP

#include "elysian_system.hpp"
#include "elysian_warp.hpp"
#include "elysian_sprite.hpp"

#include "gyro.h"

namespace elysian {

	class WarpSystem: public System { 
	public:
		enum STATE { CHECK_WARP, FADE_OUT, WARP, FADE_IN, PAUSE };
		
		WarpSystem(void); 
		void Update(void);
		void Render(void);
		
		void setRenderWarps(const bool flag);
		bool getRenderWarps(void) const;
		void Disable(void);
		void Enable(void);


	private:
		void UpdateCheckWarp(void);
		void UpdateFadeOut(void);
		void UpdateFadeIn(void);
		void UpdatePause(void);
		
		bool _renderWarps;
		bool _enabled;
		float _timer;
		Warp _currentWarp;
		
		Sprite _screenRect;
		STATE _state;
	}; 
	
}

#endif

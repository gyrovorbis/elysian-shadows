#ifndef ELYSIAN_DIRECTION_HPP
#define ELYSIAN_DIRECTION_HPP

#include <gyro.h>
#include <cmath>

namespace elysian {

	enum DIR4 { DIR_NONE, DIR_UP, DIR_DOWN, DIR_LEFT, DIR_RIGHT };

	struct Direction {

		inline static DIR4 fromVector(const GYVector2& vec) {
			
			if(vec.x == 0.0f && vec.y == 0.0f) return DIR_NONE;
	
			if(fabs(vec.x) >= fabs(vec.y)) { //Moving sideways
				if(vec.x >= 0.0f) {
					return DIR_RIGHT;
				}
				else {
					return DIR_LEFT;
				}
			}
			else { //Moving up/down
				if(vec.y >= 0.0f) {
					return DIR_DOWN;
				}
				else {
					return DIR_UP;
				}
			}
		}

        static GYVector2 toVector(const DIR4 /*dir*/) {
            GYVector2 vec;
			return vec;
		}

	};

}

#endif

#ifndef ELYSIAN_DIRECTION_HPP
#define ELYSIAN_DIRECTION_HPP

#include <gyro.h>
#include <cmath>

namespace elysian {

	struct Direction {

		enum DIR4 { DIR4_CENTER, DIR4_UP, DIR4_DOWN, DIR4_LEFT, DIR4_RIGHT, DIR4_ANY };

		inline static DIR4 fromVector(const GYVector2& vec) {
			if(vec.x == 0.0f && vec.y == 0.0f) return DIR4_CENTER;
	
			if(fabs(vec.x) >= fabs(vec.y)) {			//Moving sideways4
				return (vec.x >= 0.0f)? DIR4_RIGHT : DIR4_LEFT;
			} else {									//Moving up/down
				return (vec.y >= 0.0f)? DIR4_DOWN : DIR4_UP;
			}
		}


		/*static GYVector2 toVector(const DIR4 dir) {
            GYVector2 vec;
			return vec;
		}*/

	};

}

#endif

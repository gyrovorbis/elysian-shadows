#ifndef ELYSIAN_DIRECTION_HPP
#define ELYSIAN_DIRECTION_HPP

#include <gyro_matrix_api.h>
#include <cmath>

namespace elysian {

    struct Direction {

        enum DIR4 { DIR4_CENTER, DIR4_UP, DIR4_DOWN, DIR4_LEFT, DIR4_RIGHT, DIR4_ANY };

        inline static DIR4 fromVector2(const GYVector2& vec) {
            if(vec.x == 0.0f && vec.y == 0.0f) return DIR4_CENTER;

            if(fabs(vec.x) >= fabs(vec.y)) {			//Moving sideways4
                return (vec.x >= 0.0f)? DIR4_RIGHT : DIR4_LEFT;
            } else {									//Moving up/down
                return (vec.y >= 0.0f)? DIR4_DOWN : DIR4_UP;
            }
        }


        inline static GYVector2 toVector2(const DIR4 dir) {
            switch(dir) {
            case DIR4_UP:       return GYVector2 { 0.0f, -1.0f };
            case DIR4_DOWN:     return GYVector2 { 0.0f, 1.0f };
            case DIR4_LEFT:     return GYVector2 { -1.0f, 0.0f };
            case DIR4_RIGHT:    return GYVector2 { 1.0f, 0.0f };
            default:            return GYVector2 { 0.0f, 0.0f };
            }
        }

        inline static GYVector3 toVector3(const DIR4 dir, const float z=0.0f) {
            switch(dir) {
            case DIR4_UP:       return GYVector3 { 0.0f, -1.0f, z };
            case DIR4_DOWN:     return GYVector3 { 0.0f, 1.0f, z };
            case DIR4_LEFT:     return GYVector3 { -1.0f, 0.0f, z };
            case DIR4_RIGHT:    return GYVector3 { 1.0f, 0.0f, z };
            default:            return GYVector3 { 0.0f, 0.0f, z };
            }
        }

    };

}

#endif

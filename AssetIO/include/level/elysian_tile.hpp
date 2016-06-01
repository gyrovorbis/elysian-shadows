#ifndef ELYSIAN_TILE_HPP
#define ELYSIAN_TILE_HPP

#include <gyro_video_api.h>
#include <kinetic/gyro_kinetic_collider.h>

#define ELYSIAN_TILE_WIDTH					32
#define ELYSIAN_TILE_HEIGHT					32

struct GYWav;

namespace elysian {

    struct Tile {
		float animTime = 0.0f; //time to elapse before moving to next index
		float damping = 1.0f;

        enum COLLIDER_TYPE {
            COLLIDER_NONE, COLLIDER_FULL,
            COLLIDER_HALF_E, COLLIDER_HALF_N, COLLIDER_HALF_W, COLLIDER_HALF_S,
            COLLIDER_QUAD_SE, COLLIDER_QUAD_NE, COLLIDER_QUAD_NW, COLLIDER_QUAD_SW,
            COLLIDER_L_SE, COLLIDER_L_NE, COLLIDER_L_NW, COLLIDER_L_SW,
            COLLIDER_QUADS_SE_NW, COLLIDER_QUADS_SW_NE,
            COLLIDER_45DEG_SE, COLLIDER_45DEG_NE, COLLIDER_45DEG_NW, COLLIDER_45DEG_SW,
            COLLIDER_MAX
        } colliderType = COLLIDER_NONE;

        uint16_t    nextIndex = 0; //tile to turn into after animTime has elapsed
        bool        animated = false;

        bool        standOn = true;
        bool        transparency = false;

        GYWav*      walkSfx = nullptr;
        GYWav*      hitSfx = nullptr;

        static const COLLIDER_TYPE colliderReorientLUT[COLLIDER_MAX][GY_RECT_TEX_ORIENT_MAX];

                    ~Tile(void);

        unsigned    getColliderType(void) const;
        void        setColliderType(const unsigned type);

        void        setAnimated(const bool v);
        bool        getAnimated(void) const;
        float       getDamping(void) const;
        void        setDamping(const float factor);
        void        setAnimTime(const float t);
        float       getAnimTime(void) const;
        void        setNextIndex(const uint16_t i);
        uint16_t    getNextIndex(void) const;
        void        setWalkSfx(GYWav* const wav);
        void        setHitSfx(GYWav* const wav);
        bool        getTransparency(void) const;
        void        setTransparency(const bool v);
        bool        getStandOn(void) const;
        void        setStandOn(const bool v);

        static COLLIDER_TYPE combineColliders(const COLLIDER_TYPE col1, const COLLIDER_TYPE col2);
        static COLLIDER_TYPE reorientCollider(const COLLIDER_TYPE collider, const GY_RECT_TEX_ORIENT orient);

    };









    // ====== INLINEZ ======

    inline bool Tile::getStandOn(void) const { return standOn; }
    inline void Tile::setStandOn(const bool v) { standOn = v; }

    inline bool Tile::getTransparency(void) const { return transparency; }
    inline void Tile::setTransparency(const bool v) { transparency = v; }

    inline void Tile::setWalkSfx(GYWav* const wav) { walkSfx = wav; }
    inline void Tile::setHitSfx(GYWav* const wav) { hitSfx = wav; }

    inline float Tile::getDamping(void) const { return damping; }
    inline void Tile::setDamping(const float factor) { damping = factor; }

    inline unsigned Tile::getColliderType(void) const { return (unsigned)colliderType; }

    inline void Tile::setColliderType(const unsigned type) {
        colliderType = ((COLLIDER_TYPE)type >= COLLIDER_MAX)? COLLIDER_NONE : (COLLIDER_TYPE)type;
    }


    //IMPLEMENT THE FUCK OUT OF ME!!!!
    inline Tile::COLLIDER_TYPE Tile::reorientCollider(const COLLIDER_TYPE col, const GY_RECT_TEX_ORIENT orient) {
        return colliderReorientLUT[col][orient];
    }

    inline void Tile::setAnimated(const bool v) {
        animated = v;
    }

    inline bool Tile::getAnimated(void) const {
        return animated;
    }

    inline void Tile::setAnimTime(const float t) { animTime = t; }
    inline float Tile::getAnimTime(void) const { return animTime; }

    inline void Tile::setNextIndex(const uint16_t i) { if(i > 511) return; nextIndex = i; }
    inline uint16_t Tile::getNextIndex(void) const { return nextIndex; }

    /*
      MAYBE consider making this a LUT later, but it will be fucking HUGE or will have to involve
      the calculation of an arithmetic sequence to not be redundant. Formula for indexing into
      a nonredundant LUT (bitshift the division):
            index = (length*col1) - (col1*(col1+1))/2 + index2
            note: col1 < col2
      */
    inline Tile::COLLIDER_TYPE Tile::combineColliders(const COLLIDER_TYPE col1, const COLLIDER_TYPE col2) {
        COLLIDER_TYPE colA, colB;
        if((int)col1 <= (int)col2) {
            colA = col1;
            colB = col2;
        } else {
            colA = col2;
            colB = col1;
        }

        switch(colA) {
        case COLLIDER_NONE:
            return colB;
        case COLLIDER_FULL:
            return COLLIDER_FULL;
        case COLLIDER_HALF_E:
            switch(colB) {
            case COLLIDER_HALF_E:
                return COLLIDER_HALF_E;
            case COLLIDER_HALF_N:
                return COLLIDER_L_NE;
            case COLLIDER_HALF_W:
                return COLLIDER_FULL;
            case COLLIDER_HALF_S:
                return COLLIDER_L_SE;
            case COLLIDER_QUAD_SE:
                return COLLIDER_HALF_E;
            case COLLIDER_QUAD_NE:
                return COLLIDER_HALF_E;
            case COLLIDER_QUAD_NW:
                return COLLIDER_L_NE;
            case COLLIDER_QUAD_SW:
                return COLLIDER_L_SE;
            case COLLIDER_L_SE:
                return COLLIDER_L_SE;
            case COLLIDER_L_NE:
                return COLLIDER_L_NE;
            case COLLIDER_L_NW:
                return COLLIDER_FULL;
            case COLLIDER_L_SW:
                return COLLIDER_FULL;
            case COLLIDER_QUADS_SE_NW:
                return COLLIDER_L_NE;
            case COLLIDER_QUADS_SW_NE:
                return COLLIDER_L_SE;
            case COLLIDER_45DEG_SE:
                return COLLIDER_NONE;
            case COLLIDER_45DEG_NE:
                return COLLIDER_NONE;
            case COLLIDER_45DEG_NW:
                return COLLIDER_NONE;
            case COLLIDER_45DEG_SW:
                return COLLIDER_NONE;
            default:
                assert(0);
            }
        case COLLIDER_HALF_N:
            switch(colB) {
            case COLLIDER_HALF_N:
                return COLLIDER_HALF_N;
            case COLLIDER_HALF_W:
                return COLLIDER_L_NW;
            case COLLIDER_HALF_S:
                return COLLIDER_FULL;
            case COLLIDER_QUAD_SE:
                return COLLIDER_L_NE;
            case COLLIDER_QUAD_NE:
                return COLLIDER_HALF_N;
            case COLLIDER_QUAD_NW:
                return COLLIDER_HALF_N;
            case COLLIDER_QUAD_SW:
                return COLLIDER_L_NW;
            case COLLIDER_L_SE:
                return COLLIDER_FULL;
            case COLLIDER_L_NE:
                return COLLIDER_L_NE;
            case COLLIDER_L_NW:
                return COLLIDER_L_NW;
            case COLLIDER_L_SW:
                return COLLIDER_FULL;
            case COLLIDER_QUADS_SE_NW:
                return COLLIDER_L_NE;
            case COLLIDER_QUADS_SW_NE:
                return COLLIDER_L_NW;
            case COLLIDER_45DEG_SE:
                return COLLIDER_NONE;
            case COLLIDER_45DEG_NE:
                return COLLIDER_NONE;
            case COLLIDER_45DEG_NW:
                return COLLIDER_NONE;
            case COLLIDER_45DEG_SW:
                return COLLIDER_NONE;
            default:
                assert(0);
            }
            break;
        case COLLIDER_HALF_W:
            switch(colB) {
            case COLLIDER_HALF_W:
                return COLLIDER_HALF_W;
            case COLLIDER_HALF_S:
                return COLLIDER_L_SW;
            case COLLIDER_QUAD_SE:
                return COLLIDER_L_SW;
            case COLLIDER_QUAD_NE:
                return COLLIDER_L_NW;
            case COLLIDER_QUAD_NW:
                return COLLIDER_HALF_W;
            case COLLIDER_QUAD_SW:
                return COLLIDER_HALF_W;
            case COLLIDER_L_SE:
                return COLLIDER_FULL;
            case COLLIDER_L_NE:
                return COLLIDER_FULL;
            case COLLIDER_L_NW:
                return COLLIDER_L_NW;
            case COLLIDER_L_SW:
                return COLLIDER_L_SW;
            case COLLIDER_QUADS_SE_NW:
                return COLLIDER_L_SW;
            case COLLIDER_QUADS_SW_NE:
                return COLLIDER_L_NW;
            case COLLIDER_45DEG_SE:
                return COLLIDER_NONE;
            case COLLIDER_45DEG_NE:
                return COLLIDER_NONE;
            case COLLIDER_45DEG_NW:
                return COLLIDER_NONE;
            case COLLIDER_45DEG_SW:
                return COLLIDER_NONE;
            default:
                assert(0);
            }
            break;
        case COLLIDER_HALF_S:
            switch(colB) {
            case COLLIDER_HALF_S:
                return COLLIDER_HALF_S;
            case COLLIDER_QUAD_SE:
                return COLLIDER_HALF_S;
            case COLLIDER_QUAD_NE:
                return COLLIDER_L_SE;
            case COLLIDER_QUAD_NW:
                return COLLIDER_L_SW;
            case COLLIDER_QUAD_SW:
                return COLLIDER_HALF_S;
            case COLLIDER_L_SE:
                return COLLIDER_L_SE;
            case COLLIDER_L_NE:
                return COLLIDER_FULL;
            case COLLIDER_L_NW:
                return COLLIDER_FULL;
            case COLLIDER_L_SW:
                return COLLIDER_L_SW;
            case COLLIDER_QUADS_SE_NW:
                return COLLIDER_L_SW;
            case COLLIDER_QUADS_SW_NE:
                return COLLIDER_L_SE;
            case COLLIDER_45DEG_SE:
                return COLLIDER_NONE;
            case COLLIDER_45DEG_NE:
                return COLLIDER_NONE;
            case COLLIDER_45DEG_NW:
                return COLLIDER_NONE;
            case COLLIDER_45DEG_SW:
                return COLLIDER_NONE;
            default:
                assert(0);
            }
            break;
        case COLLIDER_QUAD_SE:
            switch(colB) {
            case COLLIDER_QUAD_SE:
                return COLLIDER_QUAD_SE;
            case COLLIDER_QUAD_NE:
                return COLLIDER_HALF_E;
            case COLLIDER_QUAD_NW:
                return COLLIDER_QUADS_SE_NW;
            case COLLIDER_QUAD_SW:
                return COLLIDER_HALF_S;
            case COLLIDER_L_SE:
                return COLLIDER_L_SE;
            case COLLIDER_L_NE:
                return COLLIDER_L_NE;
            case COLLIDER_L_NW:
                return COLLIDER_FULL;
            case COLLIDER_L_SW:
                return COLLIDER_L_SW;
            case COLLIDER_QUADS_SE_NW:
                return COLLIDER_QUADS_SE_NW;
            case COLLIDER_QUADS_SW_NE:
                return COLLIDER_L_SE;
            case COLLIDER_45DEG_SE:
                return COLLIDER_NONE;
            case COLLIDER_45DEG_NE:
                return COLLIDER_NONE;
            case COLLIDER_45DEG_NW:
                return COLLIDER_NONE;
            case COLLIDER_45DEG_SW:
                return COLLIDER_NONE;
            default:
                assert(0);
            }
            break;
        case COLLIDER_QUAD_NE:
            switch(colB) {
            case COLLIDER_QUAD_NE:
                return COLLIDER_QUAD_NE;
            case COLLIDER_QUAD_NW:
                return COLLIDER_HALF_N;
            case COLLIDER_QUAD_SW:
                return COLLIDER_QUADS_SW_NE;
            case COLLIDER_L_SE:
                return COLLIDER_L_SE;
            case COLLIDER_L_NE:
                return COLLIDER_L_NE;
            case COLLIDER_L_NW:
                return COLLIDER_L_NW;
            case COLLIDER_L_SW:
                return COLLIDER_FULL;
            case COLLIDER_QUADS_SE_NW:
                return COLLIDER_L_NE;
            case COLLIDER_QUADS_SW_NE:
                return COLLIDER_QUADS_SW_NE;
            case COLLIDER_45DEG_SE:
                return COLLIDER_NONE;
            case COLLIDER_45DEG_NE:
                return COLLIDER_NONE;
            case COLLIDER_45DEG_NW:
                return COLLIDER_NONE;
            case COLLIDER_45DEG_SW:
                return COLLIDER_NONE;
            default:
                assert(0);
            }
            break;
        case COLLIDER_QUAD_NW:
            switch(colB) {
            case COLLIDER_QUAD_NW:
                return COLLIDER_QUAD_NW;
            case COLLIDER_QUAD_SW:
                return COLLIDER_HALF_S;
            case COLLIDER_L_SE:
                return COLLIDER_FULL;
            case COLLIDER_L_NE:
                return COLLIDER_L_NE;
            case COLLIDER_L_NW:
                return COLLIDER_L_NW;
            case COLLIDER_L_SW:
                return COLLIDER_L_SW;
            case COLLIDER_QUADS_SE_NW:
                return COLLIDER_QUADS_SE_NW;
            case COLLIDER_QUADS_SW_NE:
                return COLLIDER_L_NW;
            case COLLIDER_45DEG_SE:
                return COLLIDER_NONE;
            case COLLIDER_45DEG_NE:
                return COLLIDER_NONE;
            case COLLIDER_45DEG_NW:
                return COLLIDER_NONE;
            case COLLIDER_45DEG_SW:
                return COLLIDER_NONE;
            default:
                assert(0);
            }
            break;
        case COLLIDER_QUAD_SW:
            switch(colB) {
            case COLLIDER_QUAD_SW:
                return COLLIDER_QUAD_SW;
            case COLLIDER_L_SE:
                return COLLIDER_HALF_S;
            case COLLIDER_L_NE:
                return COLLIDER_FULL;
            case COLLIDER_L_NW:
                return COLLIDER_L_NW;
            case COLLIDER_L_SW:
                return COLLIDER_L_SW;
            case COLLIDER_QUADS_SE_NW:
                return COLLIDER_L_SW;
            case COLLIDER_QUADS_SW_NE:
                return COLLIDER_QUADS_SW_NE;
            case COLLIDER_45DEG_SE:
                return COLLIDER_NONE;
            case COLLIDER_45DEG_NE:
                return COLLIDER_NONE;
            case COLLIDER_45DEG_NW:
                return COLLIDER_NONE;
            case COLLIDER_45DEG_SW:
                return COLLIDER_NONE;
            default:
                assert(0);
            }
            break;
        case COLLIDER_L_SE:
            switch(colB) {
            case COLLIDER_L_SE:
                return COLLIDER_L_SE;
            case COLLIDER_L_NE:
                return COLLIDER_FULL;
            case COLLIDER_L_NW:
                return COLLIDER_FULL;
            case COLLIDER_L_SW:
                return COLLIDER_FULL;
            case COLLIDER_QUADS_SE_NW:
                return COLLIDER_FULL;
            case COLLIDER_QUADS_SW_NE:
                return COLLIDER_L_SE;
            case COLLIDER_45DEG_SE:
                return COLLIDER_NONE;
            case COLLIDER_45DEG_NE:
                return COLLIDER_NONE;
            case COLLIDER_45DEG_NW:
                return COLLIDER_NONE;
            case COLLIDER_45DEG_SW:
                return COLLIDER_NONE;
            default:
                assert(0);
            }
            break;
        case COLLIDER_L_NE:
            switch(colB) {
            case COLLIDER_L_NE:
                return COLLIDER_L_NE;
            case COLLIDER_L_NW:
                return COLLIDER_FULL;
            case COLLIDER_L_SW:
                return COLLIDER_FULL;
            case COLLIDER_QUADS_SE_NW:
                return COLLIDER_L_NE;
            case COLLIDER_QUADS_SW_NE:
                return COLLIDER_FULL;
            case COLLIDER_45DEG_SE:
                return COLLIDER_NONE;
            case COLLIDER_45DEG_NE:
                return COLLIDER_NONE;
            case COLLIDER_45DEG_NW:
                return COLLIDER_NONE;
            case COLLIDER_45DEG_SW:
                return COLLIDER_NONE;
            default:
                assert(0);
            }
            break;
        case COLLIDER_L_NW:
            switch(colB) {
            case COLLIDER_L_NW:
                return COLLIDER_L_NW;
            case COLLIDER_L_SW:
                return COLLIDER_FULL;
            case COLLIDER_QUADS_SE_NW:
                return COLLIDER_FULL;
            case COLLIDER_QUADS_SW_NE:
                return COLLIDER_L_NW;
            case COLLIDER_45DEG_SE:
                return COLLIDER_NONE;
            case COLLIDER_45DEG_NE:
                return COLLIDER_NONE;
            case COLLIDER_45DEG_NW:
                return COLLIDER_NONE;
            case COLLIDER_45DEG_SW:
                return COLLIDER_NONE;
            default:
                assert(0);
            }
            break;
        case COLLIDER_L_SW:
            switch(colB) {
            case COLLIDER_L_SW:
                return COLLIDER_L_SW;
            case COLLIDER_QUADS_SE_NW:
                return COLLIDER_L_SW;
            case COLLIDER_QUADS_SW_NE:
                return COLLIDER_FULL;
            case COLLIDER_45DEG_SE:
                return COLLIDER_NONE;
            case COLLIDER_45DEG_NE:
                return COLLIDER_NONE;
            case COLLIDER_45DEG_NW:
                return COLLIDER_NONE;
            case COLLIDER_45DEG_SW:
                return COLLIDER_NONE;
            default:
                assert(0);
            }
            break;
        case COLLIDER_QUADS_SE_NW:
            switch(colB) {
            case COLLIDER_QUADS_SE_NW:
                return COLLIDER_QUADS_SE_NW;
            case COLLIDER_QUADS_SW_NE:
                return COLLIDER_FULL;
            case COLLIDER_45DEG_SE:
                return COLLIDER_NONE;
            case COLLIDER_45DEG_NE:
                return COLLIDER_NONE;
            case COLLIDER_45DEG_NW:
                return COLLIDER_NONE;
            case COLLIDER_45DEG_SW:
                return COLLIDER_NONE;
            default:
                assert(0);
            }
            break;
        case COLLIDER_QUADS_SW_NE:
            switch(colB) {
            case COLLIDER_QUADS_SW_NE:
                return COLLIDER_QUADS_SW_NE;
            case COLLIDER_45DEG_SE:
                return COLLIDER_NONE;
            case COLLIDER_45DEG_NE:
                return COLLIDER_NONE;
            case COLLIDER_45DEG_NW:
                return COLLIDER_NONE;
            case COLLIDER_45DEG_SW:
                return COLLIDER_NONE;
            default:
                assert(0);
            }
            break;
        case COLLIDER_45DEG_SE:
            return COLLIDER_NONE;
        case COLLIDER_45DEG_NE:
            return COLLIDER_NONE;
        case COLLIDER_45DEG_NW:
            return COLLIDER_NONE;
        case COLLIDER_45DEG_SW:
            return COLLIDER_NONE;
        default:
            assert(0);
        }

        return COLLIDER_NONE;
    }

}

#endif

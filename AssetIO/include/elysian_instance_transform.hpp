#ifndef ELYSIAN_INSTANCE_TRANSFORM_HPP
#define ELYSIAN_INSTANCE_TRANSFORM_HPP

#include <gyro_math_api.h>
#include <gyro_matrix_api.h>
#include <gyro_video_api.h>

#include "elysian_asset.hpp"
#include "elysian_lua_table_proxy.hpp"

namespace elysian {

    struct Vector2: public GYVector2, public LuaTableProxy<Vector2> {
        ELYSIAN_LUA_PROXY_FIELDS_BEGIN()
            ELYSIAN_LUA_PROXY_FIELD(x, getX, setX)
            ELYSIAN_LUA_PROXY_FIELD(y, getY, setY)
        ELYSIAN_LUA_PROXY_FIELDS_END()

                Vector2(void);
                Vector2(const float x, const float y);
        float   getX(void) const;
        float   getY(void) const;
        void    setX(const float val);
        void    setY(const float val);
    };

    struct Vector3: public GYVector3, public LuaTableProxy<Vector3> {
        ELYSIAN_LUA_PROXY_FIELDS_BEGIN()
            ELYSIAN_LUA_PROXY_FIELD(x, getX, setX)
            ELYSIAN_LUA_PROXY_FIELD(y, getY, setY)
            ELYSIAN_LUA_PROXY_FIELD(z, getZ, setZ)
        ELYSIAN_LUA_PROXY_FIELDS_END()

                Vector3(void);
                Vector3(const float x, const float y, const float z);
        float   getX(void) const;
        float   getY(void) const;
        float   getZ(void) const;
        void    setX(const float val);
        void    setY(const float val);
        void    setZ(const float val);
    };

    struct Vector4: public GYVector4, public LuaTableProxy<Vector4> {
        ELYSIAN_LUA_PROXY_FIELDS_BEGIN()
            ELYSIAN_LUA_PROXY_FIELD(x, getX, setX)
            ELYSIAN_LUA_PROXY_FIELD(y, getY, setY)
            ELYSIAN_LUA_PROXY_FIELD(z, getZ, setZ)
            ELYSIAN_LUA_PROXY_FIELD(w, getW, setW)
        ELYSIAN_LUA_PROXY_FIELDS_END()

                Vector4(void);
                Vector4(const float x, const float y, const float z, const float w);
        float   getX(void) const;
        float   getY(void) const;
        float   getZ(void) const;
        float   getW(void) const;
        void    setX(const float val);
        void    setY(const float val);
        void    setZ(const float val);
        void    setW(const float val);
    };


/*
	Essentially a thin, inlined-to-fuck, OO C++ wrapper around LibGyro's InstanceMat() operation.
*/
    struct InstanceTransform:
            public GYInstMat,
            public Asset,
            public LuaTableProxy<InstanceTransform>
    {
		//This is public for overhead reduction, but the accessors are safer.
		//Lua should treat this as a class

        ELYSIAN_LUA_PROXY_FIELDS_BEGIN()
            ELYSIAN_LUA_PROXY_FIELD(orientZ, getOrientZ, setOrientZ)
            ELYSIAN_LUA_PROXY_FIELD(size, getSizeVec, setSizeVec)
            ELYSIAN_LUA_PROXY_FIELD(scale, getScaleVec, setScaleVec)
            ELYSIAN_LUA_PROXY_FIELD(pos, getPosVec, setPosVec)
        ELYSIAN_LUA_PROXY_FIELDS_END()

        InstanceTransform(void):
            GYInstMat({
                { 100.0f, 100.0f, 0.0f },
                { 100.0f, 100.0f, 100.0f },
                { 1.0f, 1.0f, 1.0f },
                0.0f,
                nullptr
            }) {}

		void apply(void) const {
			gyMatInstance(this);
		}

        GYVector3& getPosVec(void);
        GYVector3& getSizeVec(void);
        GYVector3& getScaleVec(void);
        void setPosVec(const OOLUA::Lua_table_ref valTable);
        void setSizeVec(const OOLUA::Lua_table_ref valTable);
        void setScaleVec(const OOLUA::Lua_table_ref valTable);

		void setPos(const float x, const float y, const float z) { _pos.x = x; _pos.y = y; _pos.z = z; }
		void setPos(const float x, const float y) { _pos.x = x; _pos.y = y; }
		void setPos(const GYVector3& pos) { _pos.x = pos.x; _pos.y = pos.y; _pos.z = pos.z; }
		void setPos(const GYVector2& pos) { _pos.x = pos.x; _pos.y = pos.y; }
		void getPos(float& x, float& y) const { x = _pos.x; y = _pos.y; }
		void getPos(float& x, float& y, float& z) const { x = _pos.x; y = _pos.y; z = _pos.z; }
		void getPos(GYVector3& pos) const { pos.x = _pos.x; pos.y = _pos.y; pos.z = _pos.z; }
		void getPos(GYVector2& pos) const { pos.x = _pos.x; pos.y = _pos.y; }
		void setPosZ(const float z) { _pos.z = z; }
		float getPosZ(void) const { return _pos.z; }

        void setSize(const float x, const float y, const float z) { _size.x = x; _size.y = y; _size.z = z; }
		void setSize(const float x, const float y) { _size.x = x; _size.y = y; }
		//void setSize(const GYVector3& size) { _size = size; }
		void getSize(float& x, float& y) const { x = _size.x; y = _size.y; }
		//void getSize(GYVector3& size) const { size = _size; }
		void getSize(GYVector2& size) const { size.x = _size.x; size.y = _size.y; }

		void setScale(const float x, const float y, const float z) { _scale.x = x; _scale.y = y; _scale.z = z; }
		void setScale(const float x, const float y) { _scale.x = x; _scale.y = y; }
		void setScale(const GYVector3& scale) { _scale = scale; }
		void getScale(float& x, float& y) const { x = _scale.x; y = _scale.y; }
		//void getScale(GYVector3& scale) const { scale = _scale; }
		void getScale(GYVector2& scale) const { scale.x = _scale.x; scale.y = _scale.y; }

		void setOrientZ(const float z) { _orientZ = z; }
		float getOrientZ(void) const { return _orientZ; }

		void rotate(const float z) { _orientZ += z; }

		void translate(const float x, const float y, const float z) { _pos.x += x; _pos.y += y; _pos.z += z; }
		void translate(const float x, const float y) { _pos.x += x; _pos.y += y; }
		void translate(const GYVector3& dist) { _pos.x += dist.x; _pos.y += dist.y; _pos.z += dist.z; }
		void translate(const GYVector2& dist) { _pos.x += dist.x; _pos.y += dist.y; }

        void scale(const float x, const float y, const float /*z*/) { _scale.x *= x; _scale.y *= y; }
		void scale(const float x, const float y) { _scale.x *= x; _scale.y *= y; }
		//void scale(const GYVector2& factor) { gyVec2CompProdAccum(&_scale, &factor); }

		void save(FILE* /*file*/) const { /*save(*this, stream);*/ }
		void load(FILE* /*file*/) { /*load(*this, stream);*/ }

        //static void save(const GYInstMat &mat, std::ofstream &stream) {}
        //static void load(GYInstMat &mat, std::ifstream &stream) {}

		void linearInterpolate(const InstanceTransform& startFrame, const InstanceTransform& endFrame,
							   const float interval, const float elapsed)
		{
			gyLinearInterp((float*)&_pos.x, 10,
						   (float*)&startFrame._pos.x, (float*)&endFrame._pos.x,
						   interval, elapsed);
		}
	};





    // ====== INLINEZ ======

    //Vector2
    inline Vector2::Vector2(void): GYVector2({0.0f, 0.0f}) {}
    inline Vector2::Vector2(const float x, const float y): GYVector2({x, y}){}
    inline float Vector2::getX(void) const { return x; }
    inline float Vector2::getY(void) const { return y; }
    inline void Vector2::setX(const float val) { x = val; }
    inline void Vector2::setY(const float val) { y = val; }

    inline Vector3::Vector3(void): GYVector3({0.0f, 0.0f, 0.0f}) {}
    inline Vector3::Vector3(const float x, const float y, const float z): GYVector3({x, y, z}){}
    inline float Vector3::getX(void) const { return x; }
    inline float Vector3::getY(void) const { return y; }
    inline float Vector3::getZ(void) const { return z; }
    inline void Vector3::setX(const float val) { x = val; }
    inline void Vector3::setY(const float val) { y = val; }
    inline void Vector3::setZ(const float val) { z = val; }

    inline Vector4::Vector4(void): GYVector4({0.0f, 0.0f, 0.0f, 0.0f}) {}
    inline Vector4::Vector4(const float x, const float y, const float z, const float w): GYVector4({x, y, z, w}){}
    inline float Vector4::getX(void) const { return x; }
    inline float Vector4::getY(void) const { return y; }
    inline float Vector4::getZ(void) const { return z; }
    inline float Vector4::getW(void) const { return w; }
    inline void Vector4::setX(const float val) { x = val; }
    inline void Vector4::setY(const float val) { y = val; }
    inline void Vector4::setZ(const float val) { z = val; }
    inline void Vector4::setW(const float val) { w = val; }

    //inline GYVector3& InstanceTransform::getPosVec(void) { return _pos; }
    //inline GYVector3& InstanceTransform::getSizeVec(void) { return _size; }
    //inline GYVector3& InstanceTransform::getScaleVec(void) { return _scale; }


}


#endif

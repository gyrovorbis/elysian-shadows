#ifndef ELYSIAN_INSTANCE_TRANSFORM_HPP
#define ELYSIAN_INSTANCE_TRANSFORM_HPP

#include <gyro_math_api.h>
#include <gyro_matrix_api.h>
#include <gyro_video_api.h>

#include "elysian_asset.hpp"
#include "elysian_lua_table_proxy.hpp"

namespace elysian {

    struct Vector2Proxy: public GYVector2, public LuaTableProxy<Vector2Proxy, GYVector2> {
        ELYSIAN_LUA_PROXY_FIELDS_BEGIN(2)
            ELYSIAN_LUA_PROXY_FIELD(x, getX, setX)
            ELYSIAN_LUA_PROXY_FIELD(y, getY, setY)
        ELYSIAN_LUA_PROXY_FIELDS_END()
    };

    struct Vector3Proxy: public GYVector3, public LuaTableProxy<Vector3Proxy, GYVector3> {
        ELYSIAN_LUA_PROXY_FIELDS_BEGIN(3)
            ELYSIAN_LUA_PROXY_FIELD(x, getX, setX)
            ELYSIAN_LUA_PROXY_FIELD(y, getY, setY)
            ELYSIAN_LUA_PROXY_FIELD(z, getZ, setZ)
        ELYSIAN_LUA_PROXY_FIELDS_END()
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
	


        ELYSIAN_LUA_PROXY_FIELDS_BEGIN(2)
            ELYSIAN_LUA_PROXY_FIELD(orientZ, getOrientZ, setOrientZ)
            //ELYSIAN_LUA_PROXY_FIELD(size, getSizeVec)
            //ELYSIAN_LUA_PROXY_FIELD(scale, getScaleVec)
            ELYSIAN_LUA_PROXY_FIELD(zeta, getPostVec)
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

        GYVector3& getPostVec(void);
        GYVector3& getSizeVec(void);
        GYVector3& getScaleVec(void);

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

    inline GYVector3& InstanceTransform::getPostVec(void) { return _pos; }
    inline GYVector3& InstanceTransform::getSizeVec(void) { return _size; }
    inline GYVector3& InstanceTransform::getScaleVec(void) { return _scale; }



}


#endif

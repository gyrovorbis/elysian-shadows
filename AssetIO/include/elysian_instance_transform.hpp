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
        GYVector2*  _ptr;

                    Vector2(void);
                    Vector2(const float x, const float y);
                    Vector2(GYVector2*const vec2);
                    Vector2(const Vector2& other);
                    ~Vector2(void) = default;

        float       getX(void) const;
        float       getY(void) const;
        void        setX(const float val) const;
        void        setY(const float val) const;
    };

    struct Vector3: public GYVector3, public LuaTableProxy<Vector3> {
        ELYSIAN_LUA_PROXY_FIELDS_BEGIN()
            ELYSIAN_LUA_PROXY_FIELD(x, getX, setX)
            ELYSIAN_LUA_PROXY_FIELD(y, getY, setY)
            ELYSIAN_LUA_PROXY_FIELD(z, getZ, setZ)
        ELYSIAN_LUA_PROXY_FIELDS_END()

        GYVector3*  _ptr;

                    Vector3(void);
                    Vector3(const float x, const float y, const float z);
                    Vector3(GYVector3*const vec3);
                    Vector3(const Vector3& other);
                    ~Vector3(void) = default;

        float       getX(void) const;
        float       getY(void) const;
        float       getZ(void) const;
        void        setX(const float val) const;
        void        setY(const float val) const;
        void        setZ(const float val) const;
    };

    struct Vector4: public GYVector4, public LuaTableProxy<Vector4> {
        ELYSIAN_LUA_PROXY_FIELDS_BEGIN()
            ELYSIAN_LUA_PROXY_FIELD(x, getX, setX)
            ELYSIAN_LUA_PROXY_FIELD(y, getY, setY)
            ELYSIAN_LUA_PROXY_FIELD(z, getZ, setZ)
            ELYSIAN_LUA_PROXY_FIELD(w, getW, setW)
        ELYSIAN_LUA_PROXY_FIELDS_END()

        GYVector4*  _ptr;

                    Vector4(void);
                    Vector4(const float x, const float y, const float z, const float w);
                    Vector4(GYVector4*const vec4);
                    Vector4(const Vector4& other);
                    ~Vector4(void) = default;

        float       getX(void) const;
        float       getY(void) const;
        float       getZ(void) const;
        float       getW(void) const;
        void        setX(const float val) const;
        void        setY(const float val) const;
        void        setZ(const float val) const;
        void        setW(const float val) const;
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
            ELYSIAN_LUA_PROXY_VIRTUAL_TABLE_FIELD_BEGIN(pos)
                ELYSIAN_LUA_PROXY_FIELD(x, getPosX, setPosX)
                ELYSIAN_LUA_PROXY_FIELD(y, getPosY, setPosY)
                ELYSIAN_LUA_PROXY_FIELD(z, getPosZ, setPosZ)
            ELYSIAN_LUA_PROXY_VIRTUAL_TABLE_FIELD_END()
            ELYSIAN_LUA_PROXY_VIRTUAL_TABLE_FIELD_BEGIN(size)
                ELYSIAN_LUA_PROXY_FIELD(x, getSizeX, setSizeX)
                ELYSIAN_LUA_PROXY_FIELD(y, getSizeY, setSizeY)
                ELYSIAN_LUA_PROXY_FIELD(z, getSizeZ, setSizeZ)
            ELYSIAN_LUA_PROXY_VIRTUAL_TABLE_FIELD_END()
            ELYSIAN_LUA_PROXY_VIRTUAL_TABLE_FIELD_BEGIN(scale)
                ELYSIAN_LUA_PROXY_FIELD(x, getScaleX, setScaleX)
                ELYSIAN_LUA_PROXY_FIELD(y, getScaleY, setScaleY)
                ELYSIAN_LUA_PROXY_FIELD(z, getScaleZ, setScaleZ)
            ELYSIAN_LUA_PROXY_VIRTUAL_TABLE_FIELD_END()

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
        void setPosX(const float x) { _pos.x = x; }
        float getPosX(void) const { return _pos.x; }
        void setPosY(const float y) { _pos.y = y; }
        float getPosY(void) const { return _pos.y; }

        void setSizeZ(const float z) { _size.z = z; }
        float getSizeZ(void) const { return _size.z; }
        void setSizeX(const float x) { _size.x = x; }
        float getSizeX(void) const { return _size.x; }
        void setSizeY(const float y) { _size.y = y; }
        float getSizeY(void) const { return _size.y; }

        void setScaleZ(const float z) { _scale.z = z; }
        float getScaleZ(void) const { return _scale.z; }
        void setScaleX(const float x) { _scale.x = x; }
        float getScaleX(void) const { return _scale.x; }
        void setScaleY(const float y) { _scale.y = y; }
        float getScaleY(void) const { return _scale.y; }

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
    inline Vector2::Vector2(void): Vector2(0.0f, 0.0f) {}
    inline Vector2::Vector2(const float x, const float y): GYVector2({x, y}), _ptr(this) {}
    inline Vector2::Vector2(GYVector2 * const vec2): _ptr(vec2) {}
    inline Vector2::Vector2(const Vector2 &other) {
        if(other._ptr != &other) {
            _ptr = other._ptr;
        } else {
            *(GYVector2*)this = *(const GYVector2*)&other;
            _ptr = this;
        }
    }
    inline float Vector2::getX(void) const { return _ptr->x; }
    inline float Vector2::getY(void) const { return _ptr->y; }
    inline void Vector2::setX(const float val) const { _ptr->x = val; }
    inline void Vector2::setY(const float val) const { _ptr->y = val; }

    inline Vector3::Vector3(void): Vector3(0.0f, 0.0f, 0.0f) {}
    inline Vector3::Vector3(const float x, const float y, const float z): GYVector3({x, y, z}), _ptr(this) {}
    inline Vector3::Vector3(const Vector3 &other) {
        if(other._ptr != &other) {
            _ptr = other._ptr;
        } else {
            *(GYVector3*)this = *(const GYVector3*)&other;
            _ptr = this;
        }
    }
    inline Vector3::Vector3(GYVector3 * const vec3): _ptr(vec3) {}

    inline float Vector3::getX(void) const { return _ptr->x; }
    inline float Vector3::getY(void) const { return _ptr->y; }
    inline float Vector3::getZ(void) const { return _ptr->z; }
    inline void Vector3::setX(const float val) const { _ptr->x = val; }
    inline void Vector3::setY(const float val) const { _ptr->y = val; }
    inline void Vector3::setZ(const float val) const { _ptr->z = val; }

    inline Vector4::Vector4(void): Vector4(0.0f, 0.0f, 0.0f, 0.0f) {}
    inline Vector4::Vector4(const float x, const float y, const float z, const float w): GYVector4({x, y, z, w}), _ptr(this){}
    inline Vector4::Vector4(const Vector4 &other) {
        if(other._ptr != &other) {
            _ptr = other._ptr;
        } else {
            *(GYVector4*)this = *(const GYVector4*)&other;
            _ptr = this;
        }
    }
    inline Vector4::Vector4(GYVector4 * const vec4): _ptr(vec4) {}
    inline float Vector4::getX(void) const { return _ptr->x; }
    inline float Vector4::getY(void) const { return _ptr->y; }
    inline float Vector4::getZ(void) const { return _ptr->z; }
    inline float Vector4::getW(void) const { return _ptr->w; }
    inline void Vector4::setX(const float val) const { _ptr->x = val; }
    inline void Vector4::setY(const float val) const { _ptr->y = val; }
    inline void Vector4::setZ(const float val) const { _ptr->z = val; }
    inline void Vector4::setW(const float val) const { _ptr->w = val; }

    inline GYVector3& InstanceTransform::getPosVec(void) { return _pos; }
    //inline GYVector3& InstanceTransform::getSizeVec(void) { return _size; }
    //inline GYVector3& InstanceTransform::getScaleVec(void) { return _scale; }


}


#endif

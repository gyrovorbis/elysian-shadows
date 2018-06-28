#ifndef ELYSIAN_INSTANCE_TRANSFORM_HPP
#define ELYSIAN_INSTANCE_TRANSFORM_HPP

#include <gyro_math_api.h>
#include <gyro_matrix_api.h>

#include <crtp/elysian_lua_table_proxy.hpp>
#include <utility/elysian_generic.hpp>

namespace elysian {

    struct Vector2: public GYVector2, public VirtualArray<Vector2, int, float>, public LuaTableProxy<Vector2> {
        ELYSIAN_LUA_PROXY_FIELDS_BEGIN()
            ELYSIAN_LUA_PROXY_FIELD(x, getX, setX)
            ELYSIAN_LUA_PROXY_FIELD(y, getY, setY)
        ELYSIAN_LUA_PROXY_FIELDS_END()
        GYVector2*  _ptr;

                    Vector2(void);
                    Vector2(const float x, const float y);
                    Vector2(const float val);
                    Vector2(GYVector2*const gyvec2);
                    Vector2(const GYVector2& gyvec2);
                    Vector2(const Vector2& other);
                    ~Vector2(void) = default;

        bool        operator==(const Vector2& rhs) const;
        bool        operator!=(const Vector2& rhs) const;
        Vector2     operator+(const Vector2& rhs) const;
        Vector2     operator-(const Vector2& rhs) const;
        Vector2     operator*(const Vector2& rhs) const;
        Vector2     operator/(const Vector2& rhs) const;
        Vector2&    operator=(const Vector2& rhs);

        Vector2&    operator+=(const Vector2& rhs);
        Vector2&    operator-=(const Vector2& rhs);
        Vector2&    operator*=(const Vector2& rhs);
        Vector2&    operator/=(const Vector2& rhs);

        Vector2     operator*(float scalar) const;
        Vector2     operator/(float scalar) const;
        Vector2&    operator*=(float scalar);
        Vector2&    operator/=(float scalar);


        void        setValueByIndex(int index, float val);
        float       getValueByIndex(int index) const;

        float       getX(void) const;
        float       getY(void) const;
        void        setX(const float val) const;
        void        setY(const float val) const;

        void        addVec(const Vector2& rhs);
        void        subVec(const Vector2& rhs);
        void        multVec(const Vector2& rhs);
        void        divVec(const Vector2& rhs);
        void        multScalar(float scalar);
        void        divScalar(float scalar);

        float       getMagnitude(void) const;
        float       squaredDistance(const Vector2& rhs) const;
        float       distance(const Vector2& rhs) const;
        Vector2     getUnitVector(void) const;
        Vector2     getRotatedVector(float radians) const;
        Vector2     getRotatedVectorAboutPoint(const Vector2& point, float radians) const;
        void        rotateAboutPoint(const Vector2& point, float radians);
        void        rotate(float radians);
        void        normalize(void);
        float       dotProduct(const Vector2& rhs) const;
        Vector2     scalarMult(float scalar) const;
        float       scalarCrossProduct(const Vector2& rhs) const;
        Vector2     vectorCrossProduct(float scalar) const;

        float       toAngle(void) const;
    };

    struct Vector3: public GYVector3, public LuaTableProxy<Vector3>, public VirtualArray<Vector3, int, float> {
        ELYSIAN_LUA_PROXY_FIELDS_BEGIN()
            ELYSIAN_LUA_PROXY_FIELD(x, getX, setX)
            ELYSIAN_LUA_PROXY_FIELD(y, getY, setY)
            ELYSIAN_LUA_PROXY_FIELD(z, getZ, setZ)
        ELYSIAN_LUA_PROXY_FIELDS_END()

        GYVector3*  _ptr;

                    Vector3(void);
                    Vector3(const float val);
                    Vector3(const float x, const float y, const float z);
                    Vector3(GYVector3*const vec3);
                    Vector3(const GYVector3& vec3);
                    Vector3(const Vector3& other);
                    Vector3(const Vector2& other, float z=0.0f);
                    ~Vector3(void) = default;

        bool        operator==(const Vector3& rhs) const;
        bool        operator!=(const Vector3& rhs) const;
        Vector3     operator+(const Vector3& rhs) const;
        Vector3     operator-(const Vector3& rhs) const;
        Vector3     operator*(const Vector3& rhs) const;
        Vector3     operator/(const Vector3& rhs) const;
        Vector3&    operator=(const Vector3& rhs);

        Vector3&    operator+=(const Vector3& rhs);
        Vector3&    operator-=(const Vector3& rhs);
        Vector3&    operator*=(const Vector3& rhs);
        Vector3&    operator/=(const Vector3& rhs);

        Vector3     operator*(float scalar) const;
        Vector3     operator/(float scalar) const;
        Vector3&    operator*=(float scalar);
        Vector3&    operator/=(float scalar);

        void        setValueByIndex(int index, float val);
        float       getValueByIndex(int index) const;

        float       getX(void) const;
        float       getY(void) const;
        float       getZ(void) const;
        void        setX(const float val) const;
        void        setY(const float val) const;
        void        setZ(const float val) const;

        void        addVec(const Vector3& rhs);
        void        subVec(const Vector3& rhs);
        void        multVec(const Vector3& rhs);
        void        divVec(const Vector3& rhs);
        void        multScalar(float scalar);
        void        divScalar(float scalar);

        float       getMagnitude(void) const;
        float       squaredDistance(const Vector3& rhs) const;
        float       distance(const Vector3& rhs) const;
        Vector3     getUnitVector(void) const;
        void        normalize(void);
        float       dotProduct(const Vector3& rhs) const;
        Vector3     scalarMult(float scalar) const;
        Vector3     crossProduct(const Vector3& rhs) const;
    };

    struct Vector4: public GYVector4, public VirtualArray<Vector4, int, float>, public LuaTableProxy<Vector4> {
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

        Vector4&    operator=(const Vector4& rhs);

        void        setValueByIndex(int index, float val);
        float       getValueByIndex(int index) const;

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
            public LuaTableProxy<InstanceTransform>
    {
        //This is public for overhead reduction, but the accessors are safer.
        //Lua should treat this as a class

        ELYSIAN_LUA_PROXY_FIELDS_BEGIN()
            ELYSIAN_LUA_PROXY_FIELD(angle, getAngle, setAngle)
            ELYSIAN_LUA_PROXY_VIRTUAL_TABLE_FIELD_BEGIN(pos)
                ELYSIAN_LUA_PROXY_FIELD(x, getPosX, setPosX)
                ELYSIAN_LUA_PROXY_FIELD(y, getPosY, setPosY)
                ELYSIAN_LUA_PROXY_FIELD(z, getPosZ, setPosZ)
            ELYSIAN_LUA_PROXY_VIRTUAL_TABLE_FIELD_END()
            ELYSIAN_LUA_PROXY_VIRTUAL_TABLE_FIELD_BEGIN(orient)
                ELYSIAN_LUA_PROXY_FIELD(x, getOrientX, setOrientX)
                ELYSIAN_LUA_PROXY_FIELD(y, getOrientY, setOrientY)
                ELYSIAN_LUA_PROXY_FIELD(z, getOrientZ, setOrientZ)
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
                { 0.0f, 0.0f, 0.0f },
                0.0f,
                GY_INST_MAT_FLIP_NONE,
                nullptr
            }) {}

        InstanceTransform(const GYInstMat& mat):
            GYInstMat(mat)
        {}

        void apply(void) const {
            gyMatInstance(this);
        }

        GYVector3& getPos(void);
        Vector3    getPos(void) const;
        GYVector3& getSize(void);
        Vector3    getSize(void) const;
        GYVector3& getScale(void);
        Vector3    getScale(void) const;
        GYVector3& getOrient(void);

        //void setPosVec(const OOLUA::Lua_table_ref valTable);
        //void setSizeVec(const OOLUA::Lua_table_ref valTable);
        //void setScaleVec(const OOLUA::Lua_table_ref valTable);

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

        float getOrientX(void) const;
        float getOrientY(void) const;
        float getOrientZ(void) const;
        void setOrientX(const float orient);
        void setOrientY(const float orient);
        void setOrientZ(const float orient);

        bool    getFlipX(void) const;
        bool    getFlipY(void) const;
        bool    getFlipZ(void) const;
        void    setFlipX(const bool v);
        void    setFlipY(const bool v);
        void    setFlipZ(const bool v);

        float getAngle(void) const;
        void setAngle(const float angle);

        void setSize(const float x, const float y, const float z) { _size.x = x; _size.y = y; _size.z = z; }
        void setSize(const float x, const float y) { _size.x = x; _size.y = y; }
        //void setSize(const GYVector3& size) { _size = size; }
        void getSize(float& x, float& y) const { x = _size.x; y = _size.y; }
        //void getSize(GYVector3& size) const { size = _size; }
        void getSize(GYVector2& size) const { size.x = _size.x; size.y = _size.y; }

        void setScale(const float x, const float y, const float z) { _scale.x = x; _scale.y = y; _scale.z = z; }
        void setScale(const float x, const float y) { _scale.x = x; _scale.y = y; }
        void setScale(const GYVector3& scale) { _scale = scale; }
        void setScale(const GYVector2& scale) { _scale.x = scale.x; _scale.y = scale.y; }
        void getScale(float& x, float& y) const { x = _scale.x; y = _scale.y; }
        //void getScale(GYVector3& scale) const { scale = _scale; }

        void getOrient(float& x, float& y, float& z) const;
        void setOrient(const float x, const float y, const float z);

        void rotate(const float delta) { _angle += delta; }

        void translate(const float x, const float y, const float z) { _pos.x += x; _pos.y += y; _pos.z += z; }
        void translate(const float x, const float y) { _pos.x += x; _pos.y += y; }
        void translate(const GYVector3& dist) { _pos.x += dist.x; _pos.y += dist.y; _pos.z += dist.z; }
        void translate(const GYVector2& dist) { _pos.x += dist.x; _pos.y += dist.y; }

        void scale(const float x, const float y, const float z) { _scale.x *= x; _scale.y *= y; _scale.z *= z; }
        void scale(const float x, const float y) { _scale.x *= x; _scale.y *= y; }
        //void scale(const GYVector2& factor) { gyVec2CompProdAccum(&_scale, &factor); }

        void linearInterpolate(const InstanceTransform& startFrame, const InstanceTransform& endFrame,
                               const float interval, const float elapsed)
        {
            gyLinearInterp((float*)&_pos.x, 13,
                           (float*)&startFrame._pos.x, (float*)&endFrame._pos.x,
                           interval, elapsed);
        }
    };





    // ====== INLINEZ ======

    //Vector2
    inline Vector2::Vector2(void): Vector2(0.0f, 0.0f) {}
    inline Vector2::Vector2(const float x, const float y): GYVector2({x, y}), _ptr(this) {}
    inline Vector2::Vector2(const float val): Vector2(val, val) {}
    inline Vector2::Vector2(GYVector2 * const gyvec2): _ptr(gyvec2) {}
    inline Vector2::Vector2(const GYVector2 &gyvec2): GYVector2(gyvec2), _ptr(this) {}
    inline Vector2::Vector2(const Vector2 &other) : LuaTableProxy<Vector2>() {
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
    inline float Vector2::getValueByIndex(int index) const {
        GY_ASSERT(index == 0 || index == 1);
        return (index)? _ptr->y : _ptr->x;
    }
    inline void Vector2::setValueByIndex(int index, float value) {
        GY_ASSERT(index == 0 || index == 1);
        if(!index) _ptr->x = value;
        else _ptr->y = value;
    }
    inline float Vector2::getMagnitude(void) const {
        return gyVec2Magnitude(_ptr);
    }
    inline void Vector2::normalize(void) {
        gyVec2Normalize(_ptr);
    }
    inline Vector3 Vector3::getUnitVector(void) const {
        Vector3 vec(*this);
        gyVec3Normalize(&vec);
        return vec;
    }
    inline Vector2 Vector2::getUnitVector(void) const {
        Vector2 vec(*this);
        gyVec2Normalize(&vec);
        return vec;
    }
    inline bool Vector2::operator==(const Vector2& rhs) const {
        return (getX() == rhs.getX() && getY() == rhs.getY());
    }
    inline bool Vector2::operator!=(const Vector2& rhs) const {
        return !(*this == rhs);
    }
    inline Vector2& Vector2::operator=(const Vector2& rhs) {
        _ptr = this;
        setX(rhs.getX());
        setY(rhs.getY());
        return *this;
    }
    inline void Vector2::addVec(const Vector2& rhs) {
        setX(getX() + rhs.getX()); setY(getY() + rhs.getY());
    }
    inline void Vector2::subVec(const Vector2& rhs) {
        setX(getX() - rhs.getX()); setY(getY() - rhs.getY());
    }
    inline void Vector2::multVec(const Vector2& rhs) {
        setX(getX() * rhs.getX()); setY(getY() * rhs.getY());
    }
    inline void Vector2::divVec(const Vector2& rhs) {
        setX(getX() / rhs.getX()); setY(getY() / rhs.getY());
    }
    inline void Vector2::multScalar(float scalar) {
        setX(getX() * scalar); setY(getY() * scalar);
    }
    inline void Vector2::divScalar(float scalar) {
        setX(getX() / scalar); setY(getY() / scalar);
    }

    inline Vector2 Vector2::getRotatedVector(float radians) const {
        Vector2 dest;
        gyVec2RotOrigin(&dest, this, radians);
        return dest;
    }

    inline void Vector2::rotate(float radians) {
        gyVec2RotOrigin(this, this, radians);
    }

    inline Vector2 Vector2::getRotatedVectorAboutPoint(const Vector2& point, float radians) const {
        Vector2 dest;
        gyVec2RotPoint(&dest, this, &point, radians);
        return dest;
    }

    inline void Vector2::rotateAboutPoint(const Vector2& point, float radians) {
        Vector2 src(this);
        gyVec2RotPoint(this, &src, &point, radians);
    }


    inline Vector2 Vector2::operator+(const Vector2& rhs) const {
        return Vector2(x + rhs.x, y + rhs.y);
    }

    inline Vector2 Vector2::operator-(const Vector2& rhs) const {
        return Vector2(x - rhs.x, y - rhs.y);
    }

    inline Vector2 Vector2::operator*(const Vector2& rhs) const {
        return Vector2(x * rhs.x, y * rhs.y);
    }
    inline Vector2 Vector2::operator*(float scalar) const {
        return Vector2(x * scalar, y * scalar);
    }

    inline Vector2 Vector2::operator/(const Vector2& rhs) const {
        return Vector2(x / rhs.x, y / rhs.y);
    }

    inline Vector2 Vector2::scalarMult(float scalar) const {
        return *this * scalar;
    }

   inline float Vector2::dotProduct(const Vector2 &rhs) const {
        return gyVec2DotProd(this, &rhs);
   }

   inline Vector2 Vector2::vectorCrossProduct(float scalar) const {
        Vector2 result;
        gyVec2VectorCrossProd((GYVector2*)&result, this, scalar);
        return result;
   }

   inline float Vector2::scalarCrossProduct(const Vector2 &rhs) const {
        return gyVec2ScalarCrossProd(this, &rhs);
   }

   inline float Vector2::squaredDistance(const Vector2& rhs) const {
       return gyVec2SquareDist(this, &rhs);
   }

   inline float Vector2::distance(const Vector2 &rhs) const {
        return gyVec2Dist(this, &rhs);
   }

   inline Vector2& Vector2::operator +=(const Vector2& rhs) {
        gyVec2AddAccum(this, &rhs);
        return *this;
   }

   inline Vector2& Vector2::operator -=(const Vector2& rhs) {
       gyVec2SubFrom(this, &rhs);
       return *this;
   }

   inline Vector2& Vector2::operator *=(const Vector2& rhs) {
       setX(getX()*rhs.getX());
       setY(getY()*rhs.getY());
       return *this;
   }

   inline Vector2& Vector2::operator /=(const Vector2& rhs) {
       setX(getX()/rhs.getX());
       setY(getY()/rhs.getY());
       return *this;
   }

   inline Vector2 Vector2::operator/(float scalar) const {
       return Vector2(getX()/scalar, getY()/scalar);

   }

   inline Vector2& Vector2::operator*=(float scalar) {
        gyVec2ScaleAccum(this, scalar);
        return *this;
   }

   inline Vector2& Vector2::operator/=(float scalar) {
       setX(getX()/scalar);
       setY(getY()/scalar);
       return *this;
   }

    inline float Vector2::toAngle(void) const {
        return atan2f(getY(), getX());
    }


    inline Vector3::Vector3(void): Vector3(0.0f, 0.0f, 0.0f) {}
    inline Vector3::Vector3(const float val): Vector3(val, val, val) {}
    inline Vector3::Vector3(const float x, const float y, const float z): GYVector3({x, y, z}), _ptr(this) {}
    inline Vector3::Vector3(const GYVector3& vec3): Vector3(vec3.x, vec3.y, vec3.z) {}
    inline Vector3::Vector3(const Vector3 &other) : LuaTableProxy<Vector3>() {
        *this = other;
    }
    inline Vector3::Vector3(const Vector2& other, float z):
        Vector3(other.getX(), other.getY(), z) {}
    inline Vector3& Vector3::operator =(const Vector3& other) {
        if(other._ptr != static_cast<const GYVector3*>(&other)) {
            _ptr = other._ptr;
        } else {
            _ptr = this;
            setX(other.getX()); setY(other.getY()); setZ(other.getZ());
        }
        return *this;
    }
    inline Vector3::Vector3(GYVector3 * const vec3): _ptr(vec3) {}
    inline float Vector3::getX(void) const { return _ptr->x; }
    inline float Vector3::getY(void) const { return _ptr->y; }
    inline float Vector3::getZ(void) const { return _ptr->z; }
    inline void Vector3::setX(const float val) const { _ptr->x = val; }
    inline void Vector3::setY(const float val) const { _ptr->y = val; }
    inline void Vector3::setZ(const float val) const { _ptr->z = val; }
    inline void Vector3::normalize(void) { gyVec3Normalize(_ptr); }
    inline float Vector3::getValueByIndex(int index) const {
        GY_ASSERT(index >= 0 || index <= 2);
        switch(index) {
        case 0:
            return _ptr->x;
        case 1:
            return _ptr->y;
        default:
        case 2:
            return _ptr->z;
        }
    }
    inline void Vector3::setValueByIndex(int index, float value) {
        GY_ASSERT(index >= 0 || index <= 2);
        switch(index) {
        case 0:
            _ptr->x = value;
            break;
        case 1:
            _ptr->y = value;
            break;
        case 2:
            _ptr->z = value;
            break;
        }
    }
    inline bool Vector3::operator==(const Vector3& rhs) const {
        return (getX() == rhs.getX() && getY() == rhs.getY() && getZ() == rhs.getZ());
    }
    inline bool Vector3::operator!=(const Vector3& rhs) const {
        return !(*this == rhs);
    }

    inline Vector3 Vector3::operator+(const Vector3& rhs) const {
        Vector3 vec;
        gyVec3Add(&vec, this, &rhs);
        return vec;
    }

    inline Vector3 Vector3::operator-(const Vector3& rhs) const {
        Vector3 vec;
        gyVec3Sub(&vec, this, &rhs);
        return vec;
    }

    inline Vector3 Vector3::operator*(const Vector3& rhs) const {
        Vector3 vec;
        gyVec3Mult(&vec, this, &rhs);
        return vec;
    }
    inline Vector3 Vector3::operator*(float scalar) const {
        Vector3 vec;
        gyVec3Scale(&vec, this, scalar);
        return vec;
    }

    inline Vector3 Vector3::operator/(const Vector3& rhs) const {
        Vector3 vec;
        gyVec3Div(&vec, this, &rhs);
        return vec;
    }

    inline float Vector3::getMagnitude(void) const {
        return gyVec3Magnitude(this);
    }

    inline Vector3 Vector3::scalarMult(float scalar) const {
        return *this * scalar;
    }

   inline float Vector3::dotProduct(const Vector3& rhs) const {
        return gyVec3DotProd(this, &rhs);
   }

   inline Vector3 Vector3::crossProduct(const Vector3& rhs) const {
        Vector3 result;
        gyVec3CrossProd((GYVector3*)&result, this, &rhs);
        return result;
   }

   inline float Vector3::distance(const Vector3 &rhs) const {
        return gyVec3Dist(this, &rhs);
   }

   inline float Vector3::squaredDistance(const Vector3& rhs) const {
       return gyVec3SquareDist(this, &rhs);
   }

   inline Vector3& Vector3::operator +=(const Vector3& rhs) {
        gyVec3AddAccum(this, &rhs);
        return *this;
   }

   inline Vector3& Vector3::operator -=(const Vector3& rhs) {
       gyVec3SubFrom(this, &rhs);
       return *this;
   }

   inline Vector3& Vector3::operator *=(const Vector3& rhs) {
       gyVec3MultAccum(this, &rhs);
       return *this;
   }

   inline Vector3& Vector3::operator /=(const Vector3& rhs) {
       gyVec3DivAccum(this, &rhs);
       return *this;
   }

   inline Vector3 Vector3::operator/(float scalar) const {
       Vector3 vec;
       gyVec3Scale(&vec, this, 1.0f/scalar);
       return vec;

   }

   inline Vector3& Vector3::operator*=(float scalar) {
        gyVec3ScaleAccum(this, scalar);
        return *this;
   }

   inline Vector3& Vector3::operator/=(float scalar) {
       gyVec3ScaleAccum(this, 1.0f/scalar);
       return *this;
   }

   inline void Vector3::addVec(const Vector3& rhs) {
       setX(getX() + rhs.getX()); setY(getY() + rhs.getY()); setZ(getZ() + rhs.getZ());
   }

   inline void Vector3::subVec(const Vector3& rhs) {
       setX(getX() - rhs.getX()); setY(getY() - rhs.getY()); setZ(getZ() - rhs.getZ());
   }

   inline void Vector3::multVec(const Vector3& rhs) {
        setX(getX() * rhs.getX()); setY(getY() * rhs.getY()); setZ(getZ() * rhs.getZ());
   }

   inline void Vector3::divVec(const Vector3& rhs) {
        setX(getX() / rhs.getX()); setY(getY() / rhs.getY()); setZ(getZ() / rhs.getZ());
   }

   inline void Vector3::multScalar(float scalar) {
        multVec(Vector3(scalar));
   }

   inline void Vector3::divScalar(float scalar) {
       const float inv = 1.0f/scalar;
        multVec(Vector3(inv));
   }

    inline Vector4::Vector4(void): Vector4(0.0f, 0.0f, 0.0f, 0.0f) {}
    inline Vector4::Vector4(const float x, const float y, const float z, const float w): GYVector4({x, y, z, w}), _ptr(this){}
    inline Vector4::Vector4(const Vector4 &other) :LuaTableProxy<Vector4>() {
        *this = other;
    }
    inline Vector4& Vector4::operator =(const Vector4& other) {
        if(other._ptr != static_cast<const GYVector4*>(&other)) {
            _ptr = other._ptr;
        } else {
            _ptr = this;
            setX(other.getX()); setY(other.getY()); setZ(other.getZ()); setW(other.getW());
        }
        return *this;
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
    inline float Vector4::getValueByIndex(int index) const {
        GY_ASSERT(index >= 0 || index <= 3);
        switch(index) {
        case 0:
            return _ptr->x;
        case 1:
            return _ptr->y;
        case 2:
            return _ptr->z;
        default:
        case 3:
            return _ptr->w;
        }
    }
    inline void Vector4::setValueByIndex(int index, float value) {
        GY_ASSERT(index >= 0 || index <= 3);
        switch(index) {
        case 0:
            _ptr->x = value;
            break;
        case 1:
            _ptr->y = value;
            break;
        case 2:
            _ptr->z = value;
            break;
        default:
        case 3:
            _ptr->w = value;
            break;
        }
    }



    inline bool InstanceTransform::getFlipX(void) const { return (bool)(_flip&(unsigned)GY_INST_MAT_FLIP_X_AXIS); }
    inline bool InstanceTransform::getFlipY(void) const { return (bool)(_flip&(unsigned)GY_INST_MAT_FLIP_Y_AXIS); }
    inline bool InstanceTransform::getFlipZ(void) const { return (bool)(_flip&(unsigned)GY_INST_MAT_FLIP_Z_AXIS); }

    inline void InstanceTransform::setFlipX(const bool v) { _flip = (_flip&~(unsigned)GY_INST_MAT_FLIP_X_AXIS)|(v? (unsigned)GY_INST_MAT_FLIP_X_AXIS : 0u); }
    inline void InstanceTransform::setFlipY(const bool v) { _flip = (_flip&~(unsigned)GY_INST_MAT_FLIP_Y_AXIS)|(v? (unsigned)GY_INST_MAT_FLIP_Y_AXIS : 0u); }
    inline void InstanceTransform::setFlipZ(const bool v) { _flip = (_flip&~(unsigned)GY_INST_MAT_FLIP_Z_AXIS)|(v? (unsigned)GY_INST_MAT_FLIP_Z_AXIS : 0u); }


    inline void InstanceTransform::getOrient(float& x, float& y, float& z) const { x = _orient.x; y = _orient.y; z = _orient.z; }
    inline void InstanceTransform::setOrient(const float x, const float y, const float z) { _orient.x = x; _orient.y = y; _orient.z = z; }
    inline float InstanceTransform::getAngle(void) const { return _angle; }
    inline void InstanceTransform::setAngle(const float angle) { _angle = angle; }
    inline float InstanceTransform::getOrientX(void) const { return _orient.x; }
    inline float InstanceTransform::getOrientY(void) const { return _orient.y; }
    inline float InstanceTransform::getOrientZ(void) const { return _orient.z; }
    inline void InstanceTransform::setOrientX(const float orient) { _orient.x = orient; }
    inline void InstanceTransform::setOrientY(const float orient) { _orient.y = orient; }
    inline void InstanceTransform::setOrientZ(const float orient) { _orient.z = orient; }
    inline GYVector3& InstanceTransform::getPos(void) { return _pos; }
    inline Vector3 InstanceTransform::getPos(void) const { return Vector3(_pos); }
    inline GYVector3& InstanceTransform::getSize(void) { return _size; }
    inline Vector3 InstanceTransform::getSize(void) const { return Vector3(_size); }
    inline GYVector3& InstanceTransform::getScale(void) { return _scale; }
    inline Vector3 InstanceTransform::getScale(void) const { return Vector3(_scale); }
    inline GYVector3& InstanceTransform::getOrient(void) { return _orient; }

    //inline GYVector3& InstanceTransform::getSizeVec(void) { return _size; }
    //inline GYVector3& InstanceTransform::getScaleVec(void) { return _scale; }


}


#endif

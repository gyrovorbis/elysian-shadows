#ifndef ELYSIAN_INSTANCE_TRANSFORM_HPP
#define ELYSIAN_INSTANCE_TRANSFORM_HPP


#include <array>

#include <libGyro/gyro_math_api.h>
#include <libGyro/gyro_matrix_api.h>

#include <AssetIO/crtp/elysian_lua_table_proxy.hpp>
#include <AssetIO/utility/elysian_generic.hpp>

namespace elysian {

    struct InstanceTransform;

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

        Vector2     operator/(LuaVariant variant) const;
        Vector2     operator*(LuaVariant variant) const;

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
                    Vector4(const Vector3& vec3, float w=0.0f);
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

#if 0
    const GYQuaternion* gyQuaternionIdentity(void);
    void                gyQuaternionFromAngleAxis(GYQuaternion* dst, const GYVector3* vec, float angle);
    void                gyQuaternionFromMatrix4(GYQuaternion* dst, const GYMatrix4* mat);
    void                gyQuaternionToAngleAxis(const GYQuaternion* src, GYVector3* axis, float* angle);
    float               gyQuaternionDotProduct(const GYQuaternion* src1, const GYQuaternion* src2);
    void                gyQuaternionEuler(GYQuaternion* dst, float x, float y, float z)
    void                gyQuaternionToEuler(const GYQuaternion* src, float* x, float* y, float* z);
    void                gyQuaternionReorientDirection(GYQuaternion* dst, const GYVector3* fromDir, const GYVector3* toDir);
    void                gyQuaternionInverse(GYQuaternion* dst, const GYQuaternion* src);
    void                gyQuaternionLerp(GYQuaternion* dst, const GYQuaternion* start, const GYQuaternion* stop, float elapsed);
    void                gyQuaternionLookRotation(GYQuaternion* dst, const GYVector3* forward, const GYVector3* upwards)
    void                gyQuaternionNormalize(GYQuaternion* dst, const GYQuaternion* src);
    void                gyQuaternionRotateTowards(GYQuaternion* dst, const GYQuaternion* from, const GYQuaternion* to, float maxDegreesDelta);
    void                gyQuaternionSlerp(GYQuaternion* dst, const GYQuaternion* sfart, const GYQuaternion* stop, float elapsed);
    void                gyQuaternionMult(GYQuaternion* dst, const GYQuaternion* src1, const GYQuaternion* src2);
#endif

#define ELYSIAN_QUATERNION_NORMALIZED_MAGNITUDE_EPSILON    0.001f

    class Quaternion;
    Quaternion operator+(const Quaternion& lhs, const Quaternion& rhs);
    Quaternion operator-(const Quaternion& lhs, const Quaternion& rhs);
    Quaternion operator*(const Quaternion& quaternion, float scalar);
    Quaternion operator/(const Quaternion& quaternion, float scalar);

    class Quaternion:
            public GYQuaternion,
            public VirtualArray<Quaternion, int, float>,
            public LuaTableProxy<Quaternion>
    {
        ELYSIAN_LUA_PROXY_FIELDS_BEGIN()
        ELYSIAN_LUA_PROXY_FIELDS_END()
    public:
                            Quaternion(void);
                            Quaternion(const Quaternion& other) = default;
                            Quaternion(const GYQuaternion& gyQuat);
                            Quaternion(const Vector3& axis, float angle);
                            Quaternion(const Vector4& vec4);
                            Quaternion(float w, float x, float y, float z);

        float               getW(void) const;
        float               getX(void) const;
        float               getY(void) const;
        float               getZ(void) const;

        void                setW(float value);
        void                setX(float value);
        void                setY(float value);
        void                setZ(float value);

        float               getReal(void) const;
        Vector3             getImaginary(void) const;

        void                setReal(float value);
        void                setImaginary(const Vector3& axis);

        void                setValueByIndex(int index, float value);
        float               getValueByIndex(int index) const;

        //operations
        void                invert(void);
        void                normalize(void);
        bool                renormalize(void); //only renormalizes if normal has drifted > epsilon
        void                rotateTowards(const Quaternion& quat, float angle);

        //Query/properties
        bool                isUnit(void) const;
        bool                isReal(void) const;
        bool                isPure(void) const;
        bool                isIdentity(void) const;

        //const operations
        void                getAngleAxis(Vector3& axis, float& angle) const;
        void                getEulerAngles(float& x, float& y, float& z) const;
        Quaternion          getInverse(void) const;
        Quaternion          getNormalized(void) const;
        Quaternion          getUnitNorm(void) const;
        float               getSquaredMagnitude(void) const;
        float               getMagnitude(void) const;
        InstanceTransform   getRotationMatrix(void) const;

        //math
        Quaternion          quaternionProduct(const Quaternion& rhs) const;
        void                applyQuaternion(const Quaternion& rhs);
        float               dotProduct(const Quaternion& rhs) const;
        void                scale(float scalar);
        Quaternion          scaled(float scalar);
        Vector3             multVector(Vector3 rhs) const;
        Vector4             multVector(Vector4 rhs) const;


        //globally overloaded operators (friends)
        friend Quaternion   operator+(const Quaternion& lhs, const Quaternion& rhs);
        friend Quaternion   operator-(const Quaternion& lhs, const Quaternion& rhs);
        friend Quaternion   operator*(const Quaternion& quaternion, float scalar);
        friend Quaternion   operator/(const Quaternion& quaternion, float scalar);

        //internally overloaded operators
        const Quaternion&   operator*=(float scalar);
        const Quaternion&   operator/=(float scalar);
        const Quaternion&   operator+=(const Quaternion& rhs);
        const Quaternion&   operator-=(const Quaternion& rhs);
        Vector3             operator*(const Vector3& rhs) const;
        Vector4             operator*(const Vector4& rhs) const;
        Quaternion          operator*(const Quaternion& rhs) const;
        const Quaternion&   operator*=(const Quaternion& rhs);
        const Quaternion&   operator=(const Quaternion& rhs);
        const Quaternion&   operator=(const GYQuaternion& rhs);
        bool                operator==(const Quaternion& rhs) const;

        //statics
        static auto         identity(void) -> const Quaternion&;

        static Quaternion   fromAngleAxis(const Vector3& axis, float angle);
        static Quaternion   fromRotationMatrix(const InstanceTransform& transform);
        static Quaternion   fromEulerAngles(float x, float y, float z);
        static Quaternion   fromDirReorient(const Vector3& fromDir, const Vector3& toDir);
        static Quaternion   fromLookRotation(const Vector3& forward, const Vector3& upwards);

        static float        angleBetween(const Quaternion& lhs, const Quaternion& rhs);
        static Quaternion   lerp(const Quaternion& start, const Quaternion& end, float elapsed);
        static Quaternion   slerp(const Quaternion& start, const Quaternion& end, float elapsed);
    };

    template<int S, typename T=float>
    class Vector {
        T               _e[S] = { 0.0f };

    };

    template<typename T, size_t... Dims>
    class Matrix;

    template<typename T, size_t I, size_t... J>
    struct MatrixDecayTypes {
        using DecayParent = MatrixDecayTypes<T, J...>;

        const static constexpr size_t
            DimCount            = DecayParent::DimCount + 1;
        const static constexpr size_t
            ElemCount           = I * DecayParent::ElemCount;
        const static constexpr size_t
            ExpectedSize        = I * DecayParent::ExpectedSize;

        using ElysianMat        = Matrix<T, I, J...>;
        using ElysianSubMat     = typename DecayParent::ElysianMat;
        using StdArrayND        = std::array<typename DecayParent::StdArrayND, I>;
        using CArrayND          = typename DecayParent::CArrayND[I];
        using StdArray1D        = std::array<T, ElemCount>;
        using CArray1D          = T[ElemCount];
        using InitializerList   = std::initializer_list<typename DecayParent::InitializerList>;

        static constexpr size_t getDimSize(size_t dim) {
            static_assert(dim <= DimCount, "Dimension index is invalid!");
            return (dim < DimCount)? DecayParent::getDimSize(dim) : ElemCount;
        }

        static_assert(sizeof(StdArrayND) == ExpectedSize,
                      "Matrix::StdArray2D size does snot match expected size!");
        static_assert(sizeof(StdArray1D) == ExpectedSize,
                      "Matrix::StdArray1D size does not match expected size!");
        static_assert(sizeof(CArrayND) == ExpectedSize,
                      "Matrix::CArray2D size does not match expected size!");
        static_assert(sizeof(CArray1D) == ExpectedSize,
                      "Matrix::CArray1D size does not match expected size!");
    };

    template<typename T, size_t I>
    struct MatrixDecayTypes<T, I> {
        const static constexpr size_t
            ElemCount           = I;
        const static constexpr size_t
            ExpectedSize        = sizeof(T) * I;
        const static constexpr size_t
            DimCount            = 1;

        static constexpr size_t getDimSize(size_t dim) {
            static_assert(dim == DimCount, "Couldn't find dim index!");
            return dim == DimCount? ElemCount : 0;
        }

        using ElysianMat        = Matrix<T, I>;
        using ElysianSubMat     = T;
        using ElysianVec        = Vector<I, T>;
        using StdArrayND        = std::array<T, I>;
        using CArrayND          = T[I];
        using StdArray1D        = std::array<T, ElemCount>;
        using CArray1D          = T[ElemCount];
        using InitializerList   = std::initializer_list<T>;
    };



    template<size_t index, typename T, size_t I, size_t... J>
    struct MatrixSubType: MatrixSubType<index-1, T, J...> {

        using SubType = typename MatrixSubType<index-1, T, J...>::Type;
        using Type = Matrix<T, I, J...>;
    };

    template<typename T, size_t I>
    struct MatrixSubType<0, T, I> {
        using SubType = T;
        using Type = Matrix<T, I>;
    };


    template<typename T, size_t... size>
    class SquareMatrixOps;

    template<typename T, size_t... size>
    class SpecializedMatrixOps;


    template<typename T, size_t... Dims>
    class alignas(GYRO_MATRIX4_ALIGNMENT) Matrix:
            public MatrixDecayTypes<T, Dims...>,
            public SquareMatrixOps<T, Dims...>,
            public SpecializedMatrixOps<T, Dims...>

    {
    private:
        using DecayTypes = MatrixDecayTypes<T, Dims...>;
        static const constexpr size_t ExpectedSize  = DecayTypes::ExpectedSize;
        static const constexpr size_t DimCount      = DecayTypes::DimCount;

        using StdArrayND        = typename DecayTypes::StdArrayND;
        using CArrayND          = typename DecayTypes::CArrayND;
        using StdArray1D        = typename DecayTypes::StdArray1D;
        using CArray1D          = typename DecayTypes::CArray1D;
        using InitializerList   = typename DecayTypes::InitializerList;
        using SubMatType        = typename DecayTypes::ElysianSubMat;

        union {
            StdArrayND      _stdArrayND;        //ALIGNMENT!!!
            StdArray1D      _stdArray1D;
            CArrayND        _cArrayND;
            CArray1D        _cArray1D;
        };

    public:
        constexpr           Matrix(void);
        constexpr           Matrix(InitializerList initList);
        constexpr           Matrix(const StdArrayND& stdArrayND);

        template<typename F = StdArray1D,
                typename = std::enable_if_t<!std::is_same<F, StdArrayND>::value, bool>>
        constexpr           Matrix(const F& stdArray1D);

        constexpr           Matrix(const CArrayND& cArrayND);
#if 0

        template<typename F = CArray1D,
                typename = std::enable_if_t<!std::is_same<F, CArrayND>::value, bool>>
        constexpr           Matrix(const F& cArray1D);
#endif
        void                setElements(const StdArrayND& stdArrayND);


        template<typename F = StdArray1D,
                typename = std::enable_if_t<!std::is_same<F, StdArrayND>::value, bool>>
        void                setElements(const F& stdArray1D);

        void                setElements(const CArrayND& cArrayND);
#if 0
        template<typename F = CArray1D,
                typename = std::enable_if_t<!std::is_same<F, CArrayND>::value, bool>>
        void                setElements(const F& cArray1D);
     #endif
        //void                setElements(const CArray1D& cArray1D);

        T                   getElement(std::array<size_t, DimCount> dimIndices) const;
        void                setElement(unsigned c, unsigned r, T val);
        T                   getElement(unsigned index) const;
        void                setElement(unsigned index, T val);

        const StdArrayND&   getStdArrayND(void) const;
        const StdArray1D&   getStdArray1D(void) const;
        const CArrayND&     getCArrayND(void) const;
        const CArray1D&     getCArray1D(void) const;

        //Set submatrix, extract submatrix

        //Matrix<R, C, T>     getTranspose(void) const;


        template<size_t DimCount,
                 typename SubMatrixType =  MatrixSubType<DimCount, T, Dims...>>
        auto                getSubMatrix(std::array<T, DimCount> elementArray) -> typename SubMatrixType::SubType;


        //STD ENBABLE_IF DIAGONALIZABLE
        Matrix              getDiagonalMatrix(void) const;
        //Vector<C, T>        getEigenValues(void) const;



        //Transformations
        Matrix              multMatrix(const Matrix& rhs) const;
        //ColVec              multVector(const ColVec& rhs) const;
        void                applyMatrix(const Matrix& rhs);

        //overloaded operators
        const SubMatType    operator[](unsigned index) const;

        bool                operator ==(const Matrix& rhs) const;
        bool                operator ==(const StdArrayND& stdArrayND) const;

        template<typename F = StdArray1D>
        typename std::enable_if_t<!std::is_same<F, StdArrayND>::value, bool>
                            operator ==(const F& stdArray1D) const;

        template<typename F = CArray1D>
        typename std::enable_if_t<!std::is_same<F, CArrayND>::value, bool>
                            operator ==(const F& cArray1D) const;


        bool                operator ==(const CArrayND& cArrayND) const;


        bool                operator !=(const Matrix& rhs) const;
        bool                operator !=(const StdArrayND& stdArrayND) const;

        template<typename F = StdArray1D>
        typename std::enable_if_t<!std::is_same<F, StdArrayND>::value, bool>
                            operator !=(const F& stdArray1D) const;

        template<typename F = CArray1D>
        typename std::enable_if_t<!std::is_same<F, CArrayND>::value, bool>
                            operator !=(const CArray1D& cArray1D) const;

        bool                operator !=(const CArrayND& cArrayND) const;

        const Matrix&       operator =(const Matrix& rhs);
        const Matrix&       operator =(const StdArrayND& stdArrayND);


        template<typename F = StdArray1D>
        typename std::enable_if_t<!std::is_same<F, StdArrayND>::value, Matrix&>
                            operator =(const F& stdArray1D) const;

        template<typename F = CArray1D>
        typename std::enable_if_t<!std::is_same<F, CArrayND>::value, Matrix&>
                            operator =(const F& cArray1D) const;


        const Matrix&       operator =(const CArrayND& cArrayND);

        const Matrix&       operator*(const Matrix& rhs) const;
        const Matrix&       operator*=(const Matrix& rhs);


        //template out multiplication, addition, subtraction, product

        //===== STATICS =====
        static unsigned     dimSize(size_t dimNumber);
        static unsigned     elemCount(void);


    };

    template<typename T, size_t S>
    using Vectorx = Matrix<T, S>;

    using Vectorx2 = Vectorx<float, 2>;
    using Vectorx3 = Vectorx<float, 3>;
    using Vectorx4 = Vectorx<float, 4>;

    using Matrix2 = Matrix<float, 2, 2>;
    using Matrix3 = Matrix<float, 3, 3>;
    using Matrix4 = Matrix<float, 4, 4>;

    template<typename T, size_t... size>
    class SquareMatrixOps {
    public:
        bool    isDiagonal(void) const { return false; }
        bool    hasInverse(void) const { return false; }
    };

    template<typename T, size_t S>
    class SquareMatrixOps<T, S, S> {
    public:
        bool            isIdentity(void);
        static bool     isSquare(void) { return true; }
        T               getTrace(void) const;
        T               getDeterminant(void) const;
        bool            isDiagonal(void) const;
        bool            isTriangular(void) const { return true; }
        bool            isUpperTriangular(void) const;
        bool            isLowerTriangular(void) const;
        bool            isSingular(void) const; //determinant is 0, no inverse
        bool            isDiagonalizable(void) const;
        bool            hasInverse(void) const;
        Matrix<T, S, S> getInverse(void) const;
    };

    template<typename T, size_t... size>
    class SpecializedMatrixOps {

    };

    template<>
    class SpecializedMatrixOps<float, 4, 4> {
    public:
        void            stackPush(void);
        void            stackPop(void);
        void            stackLoad(Matrix4& mat);
        void            stackStore(Matrix4& mat);
        void            stackApply(Matrix4& mat);
        void            stackMultVectorBatch(size_t batchSize, const Vector4* inVecs, Vector4* outVecs);

        Vector3         extractTranslation(void) const;
        Vector3         extractScale(void) const;
        auto            extractRotation(void) const -> std::pair<Vector3, float>;
        Quaternion      extractQuaternion(void) const;
        Vector3         extractShear(void) const;

        //Affine transformations
        static Matrix4  fromTranslation(const Vector3& vec);
        static Matrix4  fromScale(const Vector4& vec);
        static Matrix4  fromRotation(const Vector3& axis, float angle);
        static Matrix4  fromQuaternion(const Quaternion& quat);
        static Matrix4  fromShear(const Vector3& vec);

        static Matrix4  fromOrthographic(const Vector3& pos, float scale, float angle, const Vector3& dir);
        static Matrix4  fromPerspective(float fov, float aspect, float znear, float zfar);

    };

    template<>
    class SpecializedMatrixOps<float, 3, 3> {
        //MAYBE EXTEND TO USE STACK LIKE MAT4?!

        //Affine transformations
        static Matrix3 fromTranslation(const Vector2& vec);
        static Matrix3 fromScale(const Vector2& vec);
        static Matrix3 fromRotation(float angle);
        static Matrix3 fromShear(const Vector2& vec);
    };



    template<typename T, size_t S>
    struct VectorAccessors {

    };

    template<>
    struct VectorAccessors<float, 2> {
        float getX(void) const;
        float getY(void) const;
        void setX(float val);
        void setY(float val);

#if 0
        Vector2 yx(void) const;
#else
        Vector2 swizzle(unsigned i1, unsigned i2);
#endif
    };

    template<>
    struct VectorAccessors<float, 3>:
        public VectorAccessors<float, 2> {

        float getZ(void) const;
        void setZ(float val);

#if 0
        //Swizzles
        //Vector3 xyz(void) const;

        Vector3 yyz(void) const;
        Vector3 zyz(void) const;

        Vector3 xxz(void) const;
        Vector3 xzz(void) const;

        Vector2 xyx(void) const;
        Vector2 xyy(void) const;
#else
        Vector2 swizzle(unsigned i1, unsigned i2); //pass up <z
        Vector3 swizzle(unsigned i1, unsigned i2, unsigned i3) const;
#endif



    };

    template<>
    struct VectorAccessors<float, 4>:
        public VectorAccessors<float, 3> {

        float getW(void) const;
        void setW(float val) const;

        Vector2 swizzle(unsigned i1, unsigned i2); //pass up <w
        Vector3 swizzle(unsigned i1, unsigned i2, unsigned i3);
        Vector4 swizzle(unsigned i1, unsigned i2, unsigned i3, unsigned i4);
    };

    template<typename T, size_t S>
    struct VectorOps{
    };

    template<>
    struct VectorOps<float, 2> {
        float scalarCrossProduct(const Vector2& other);
        Vector2 vectorCrossProduct(const Vector2& other);
        float getAngle(void) const;

        static Vector2  fromAngle(float angle);
    };

    template<>
    struct VectorOps<float, 3> {

        Vector3 crossProduct(const Vector3& other) const;

    };

    template<typename T, size_t S>
    class SpecializedMatrixOps<T, S>:
        public VectorAccessors<T, S>,
        public VectorOps<T, S>
    {
    public:
        T dotProduct(const Matrix<T, S>& other) const;
        T getMagnitude(void) const;
        T getSquaredMagnitude(void) const;
        void normalize(void) const;
        bool isUnit(void) const;

        Matrix<T, S> lerp(const Matrix<T, S>& start, const Matrix<T, S>& end, float interval);

        T getImdex(size_t index) const;
        void setIndex(size_t index, T value);

        T getDistanceBetween(const Matrix<T, S>& other) const;

        T operator[](size_t index) const;
        //+, -, *, / ==, !+, etc
    };

    static inline void testLulz(void) {
        Matrix<float, 3, 3> mat3;
        mat3.isSquare(); //true
        mat3.isTriangular(); //exists, cuz square matrix.

        Matrix4 matReest;
        matReest.isSquare(); //false
        matReest.isTriangular(); //compiler error, only square matrice have this shit!

        Matrix<float, 4> vec;
        vec.dotProduct(vec);

        Vectorx3 vex;
        vex.crossProduct(Vector3(0.0));

        Matrix<float, 2> vet;
        vet.getX();

        Vectorx4 ves;
        ves *= ves.swizzle(0, 3, 1, 2);

    }

    template<typename T, size_t... Dims>
    constexpr inline Matrix<T, Dims...>::Matrix(void) {
        static_assert(sizeof(Matrix<T, Dims...>) == ExpectedSize,
                      "Matrix<T, R, C> size does not match expected size!");
    }

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
                GY_INST_MAT_FLIP_NONE
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

        Quaternion getOrientQuaternion(void) const;

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

    //====== VECTOR2 =======
    inline Vector2::Vector2(void): Vector2(0.0f, 0.0f) {}
    inline Vector2::Vector2(const float x, const float y): GYVector2({x, y}), _ptr(this) {}
    inline Vector2::Vector2(const float val): Vector2(val, val) {}
    inline Vector2::Vector2(GYVector2 * const gyvec2): GYVector2(*gyvec2), _ptr(gyvec2) {}
    inline Vector2::Vector2(const GYVector2 &gyvec2): GYVector2(gyvec2), _ptr(this) {}
    inline Vector2::Vector2(const Vector2 &other) : LuaTableProxy<Vector2>() {
        if(other._ptr != &other) {
            _ptr = other._ptr;
            *(GYVector2*)this = *(const GYVector2*)&other;
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
    inline Vector2 Vector2::getUnitVector(void) const {
        Vector2 vec(*this);
        vec.normalize();
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
        gyVec2RotOrigin(static_cast<GYVector2*>(&dest), static_cast<const GYVector2*>(this), radians);
        return dest;
    }

    inline void Vector2::rotate(float radians) {
        gyVec2RotOrigin(static_cast<GYVector2*>(this), static_cast<GYVector2*>(this), radians);
    }

    inline Vector2 Vector2::getRotatedVectorAboutPoint(const Vector2& point, float radians) const {
        Vector2 dest;
        gyVec2RotPoint(static_cast<GYVector2*>(&dest), static_cast<const GYVector2*>(this), static_cast<const GYVector2*>(&point), radians);
        return dest;
    }

    inline void Vector2::rotateAboutPoint(const Vector2& point, float radians) {
        Vector2 src(static_cast<GYVector2*>(this));
        gyVec2RotPoint(static_cast<GYVector2*>(this), static_cast<GYVector2*>(&src), static_cast<const GYVector2*>(&point), radians);
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
        return gyVec2DotProd(static_cast<const GYVector2*>(this), static_cast<const GYVector2*>(&rhs));
   }

   inline Vector2 Vector2::vectorCrossProduct(float scalar) const {
        Vector2 result;
        gyVec2VectorCrossProd((GYVector2*)&result, static_cast<const GYVector2*>(this), scalar);
        return result;
   }

   inline float Vector2::scalarCrossProduct(const Vector2 &rhs) const {
        return gyVec2ScalarCrossProd(static_cast<const GYVector2*>(this),  static_cast<const GYVector2*>(&rhs));
   }

   inline float Vector2::squaredDistance(const Vector2& rhs) const {
       return gyVec2SquareDist(static_cast<const GYVector2*>(this),  static_cast<const GYVector2*>(&rhs));
   }

   inline float Vector2::distance(const Vector2 &rhs) const {
        return gyVec2Dist(static_cast<const GYVector2*>(this), static_cast<const GYVector2*>(&rhs));
   }

   inline Vector2& Vector2::operator +=(const Vector2& rhs) {
        gyVec2AddAccum(static_cast<GYVector2*>(this),  static_cast<const GYVector2*>(&rhs));
        return *this;
   }

   inline Vector2& Vector2::operator -=(const Vector2& rhs) {
       gyVec2SubFrom(static_cast<GYVector2*>(this),  static_cast<const GYVector2*>(&rhs));
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


    inline Vector2 Vector2::operator/(LuaVariant variant) const {
        switch(variant.getType()) {
        case LUA_TNUMBER: return *this / Vector2(variant.getValue<float>());
        case LUA_TUSERDATA:
            if(strcmp(variant.getTypeString(), "Vector2") == 0)
                return *this/variant.getValue<Vector2>();
        default: break; //you dun fucked up
        }

        return Vector2(0.0f);
    }
    inline Vector2 Vector2::operator*(LuaVariant variant) const {
        switch(variant.getType()) {
        case LUA_TNUMBER: return *this * Vector2(variant.getValue<float>());
        case LUA_TUSERDATA:
            if(strcmp(variant.getTypeString(), "Vector2") == 0)
                return *this * variant.getValue<Vector2>();
        default: break; //you dun fucked up
        }

        return Vector2(0.0f);
    }

    //====== VECTOR3 ========
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
   inline Vector3 Vector3::getUnitVector(void) const {
       Vector3 vec(*this);
       gyVec3Normalize(&vec);
       return vec;
   }

   //===== VECTOR4 ======
    inline Vector4::Vector4(void): Vector4(0.0f, 0.0f, 0.0f, 0.0f) {}
    inline Vector4::Vector4(const float x, const float y, const float z, const float w):
        GYVector4({x, y, z, w}), _ptr(this){}
    inline Vector4::Vector4(const Vector4 &other): LuaTableProxy<Vector4>() {
        *this = other;
    }
    inline Vector4::Vector4(const Vector3& vec3, float w):
        GYVector4({vec3.getX(), vec3.getY(), vec3.getZ(), w }) {}
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

    //======= QUATERNION =======
    inline Quaternion::Quaternion(void): Quaternion(identity()) {}
    inline Quaternion::Quaternion(const GYQuaternion& gyQuat):
        GYQuaternion(gyQuat) {}
    inline Quaternion::Quaternion(const Vector3& axis, float angle):
        Quaternion(fromAngleAxis(axis, angle)) {}
    inline Quaternion::Quaternion(const Vector4& vec4) {
        memcpy(static_cast<GYQuaternion*>(this), static_cast<const GYVector4*>(&vec4), sizeof(GYQuaternion));
    }
    inline Quaternion::Quaternion(float w, float x, float y, float z):
        GYQuaternion({ w, x, y, z }) {}

    inline float Quaternion::getW(void) const { return w; }
    inline float Quaternion::getX(void) const { return x; }
    inline float Quaternion::getY(void) const { return y; }
    inline float Quaternion::getZ(void) const { return z; }

    inline void Quaternion::setW(float value) { w = value; }
    inline void Quaternion::setX(float value) { x = value; }
    inline void Quaternion::setY(float value) { y = value; }
    inline void Quaternion::setZ(float value) { z = value; }

    inline float Quaternion::getReal(void) const { return w; }
    inline Vector3 Quaternion::getImaginary(void) const { return Vector3(x, y, z); }
    inline void Quaternion::setReal(float value) { w = value; }
    inline void Quaternion::setImaginary(const Vector3& axis) { x = axis.x; y = axis.y; z = axis.z; }

    inline void Quaternion::setValueByIndex(int index, float value) {
        GY_ASSERT(index < 4);
        switch(index) {
        case 0: w = value; break;
        case 1: x = value; break;
        case 2: y = value; break;
        case 3: z = value; break;
        default: break;
        }
    }
    inline float Quaternion::getValueByIndex(int index) const {
        GY_ASSERT(index < 4);
        switch(index) {
        case 0: return w;
        case 1: return x;
        case 2: return y;
        case 3: return z;
        default: return 0.0f;
        }
    }

    inline void Quaternion::invert(void) {
        gyQuaternionInverse(static_cast<GYQuaternion*>(this),
                            static_cast<const GYQuaternion*>(this));
    }
    inline void Quaternion::normalize(void) {
        gyQuaternionNormalize(static_cast<GYQuaternion*>(this),
                              static_cast<const GYQuaternion*>(this));
    }
    inline bool Quaternion::renormalize(void) {
        if(isUnit()) {
            return false;
        } else {
            normalize();
            return true;
        }
    }
    inline void Quaternion::rotateTowards(const Quaternion& quat, float angle) {
        gyQuaternionRotateTowards(static_cast<GYQuaternion*>(this),
                                  static_cast<const GYQuaternion*>(this),
                                  &quat,
                                  angle);
    }

    inline void Quaternion::getAngleAxis(Vector3& axis, float& angle) const {
        gyQuaternionToAngleAxis(static_cast<const GYQuaternion*>(this), &axis, &angle);
    }

    inline void Quaternion::getEulerAngles(float& x, float& y, float& z) const {
        gyQuaternionToEulerAngles(static_cast<const GYQuaternion*>(this), &x, &y, &z);
    }
    inline Quaternion Quaternion::getInverse(void) const {
        Quaternion ret;
        gyQuaternionInverse(&ret, static_cast<const GYQuaternion*>(this));
        return ret;
    }
    inline Quaternion Quaternion::getNormalized(void) const {
        Quaternion ret;
        gyQuaternionNormalize(&ret, static_cast<const GYQuaternion*>(this));
        return ret;
    }
    inline Quaternion Quaternion::getUnitNorm(void) const {
        Quaternion ret;
        gyQuaternionUnitNorm(&ret, static_cast<const GYQuaternion*>(this));
        return ret;
    }
    inline float Quaternion::getMagnitude(void) const {
        return gyQuaternionMagnitude(static_cast<const GYQuaternion*>(this));
    }
    inline float Quaternion::getSquaredMagnitude(void) const {
        return gyQuaternionSquaredMagnitude(static_cast<const GYQuaternion*>(this));
    }
    inline bool Quaternion::isUnit(void) const {
        return fabs(getMagnitude() - 1.0f) < ELYSIAN_QUATERNION_NORMALIZED_MAGNITUDE_EPSILON;
    }

    inline bool Quaternion::isReal(void) const {
        GYVector3 zeroVec = { 0.0f, 0.0f, 0.0f };
        return gyVec3Compare(static_cast<const GYQuaternion*>(this), &zeroVec);
    }

    inline bool Quaternion::isPure(void) const {
        return w == 0.0f;
    }

    inline bool Quaternion::isIdentity(void) const {
        return memcmp(static_cast<const GYQuaternion*>(this),
                      static_cast<const GYQuaternion*>(&identity()),
                      sizeof(GYQuaternion)) == 0;
    }

    inline Quaternion Quaternion::quaternionProduct(const Quaternion& rhs) const {
        Quaternion ret;
        gyQuaternionMult(&ret,static_cast<const GYQuaternion*>(this), &rhs);
        return ret;
    }
    inline void Quaternion::applyQuaternion(const Quaternion& rhs) {
        gyQuaternionMult(static_cast<GYQuaternion*>(this),
                         static_cast<GYQuaternion*>(this),
                         &rhs);
    }
    inline float Quaternion::dotProduct(const Quaternion& rhs) const {
        return gyQuaternionDotProduct(static_cast<const GYQuaternion*>(this), &rhs);
    }
    inline Vector3 Quaternion::multVector(Vector3 rhs) const {
        Vector3 ret;
        gyQuaternionMultVec3(&ret, static_cast<const GYQuaternion*>(this), &rhs);
        return ret;
    }
    inline Vector4 Quaternion::multVector(Vector4 rhs) const {
        return Vector4(multVector(Vector3(rhs.getX(), rhs.getY(), rhs.getZ())), rhs.getW());
    }

    inline const Quaternion& Quaternion::operator*=(float scalar) {
        gyVec4ScaleAccum(static_cast<GYQuaternion*>(this), scalar);
        return *this;
    }
    inline const Quaternion& Quaternion::operator/=(float scalar) {
       return  (*this *= 1.0f/scalar);
    }
    inline const Quaternion& Quaternion::operator+=(const Quaternion& rhs) {
        setW(getW() + rhs.getW());
        setX(getX() + rhs.getX());
        setY(getY() + rhs.getY());
        setZ(getZ() + rhs.getZ());
        return *this;
    }
    inline const Quaternion& Quaternion::operator-=(const Quaternion& rhs) {
        setW(getW() - rhs.getW());
        setX(getX() - rhs.getX());
        setY(getY() - rhs.getY());
        setZ(getZ() - rhs.getZ());
        return *this;
    }
    inline Vector3 Quaternion::operator*(const Vector3& rhs) const {
        return multVector(rhs);
    }
    inline Vector4 Quaternion::operator*(const Vector4& rhs) const {
        return multVector(rhs);
    }
    inline Quaternion Quaternion::operator*(const Quaternion& rhs) const {
        return quaternionProduct(rhs);
    }
    inline const Quaternion& Quaternion::operator*=(const Quaternion& rhs) {
        applyQuaternion(rhs);
        return *this;
    }
    inline bool Quaternion::operator==(const Quaternion& rhs) const {
        return (fabs(w - rhs.getW()) < ELYSIAN_QUATERNION_NORMALIZED_MAGNITUDE_EPSILON &&
                fabs(x - rhs.getX()) < ELYSIAN_QUATERNION_NORMALIZED_MAGNITUDE_EPSILON &&
                fabs(y - rhs.getY()) < ELYSIAN_QUATERNION_NORMALIZED_MAGNITUDE_EPSILON &&
                fabs(z - rhs.getZ()) < ELYSIAN_QUATERNION_NORMALIZED_MAGNITUDE_EPSILON);
    }
    inline const Quaternion& Quaternion::operator=(const GYQuaternion& rhs) {
        memcpy(static_cast<GYQuaternion*>(this), &rhs, sizeof(GYQuaternion));
        return *this;
    }
    inline const Quaternion& Quaternion::operator=(const Quaternion& rhs) {
        setW(rhs.getW()); setX(rhs.getX()); setY(rhs.getY()); setZ(rhs.getZ());
        return *this;
    }
    inline auto Quaternion::identity(void) -> const Quaternion& {
        static const Quaternion ident(1.0f, 0.0f, 0.0f, 0.0f);
        return ident;
    }
    inline Quaternion Quaternion::fromAngleAxis(const Vector3& axis, float angle) {
        Quaternion q;
        gyQuaternionFromAngleAxis(&q, &axis, angle);
        return q;
    }
    inline Quaternion Quaternion::fromEulerAngles(float x, float y, float z) {
        Quaternion q;
        gyQuaternionFromEulerAngles(&q, x, y, z);
        return q;
    }
    inline Quaternion Quaternion::fromDirReorient(const Vector3& fromDir, const Vector3& toDir) {
        Quaternion q;
        gyQuaternionReorientDirection(&q, &fromDir, &toDir);
        return q;
    }
    inline Quaternion Quaternion::fromLookRotation(const Vector3& forward, const Vector3& upwards) {
        Quaternion q;
        gyQuaternionFromLookRotation(&q, &forward, &upwards);
        return q;
    }
    inline float Quaternion::angleBetween(const Quaternion& lhs, const Quaternion& rhs) {
        return gyQuaternionAngle(&lhs, &rhs);
    }
    inline Quaternion Quaternion::lerp(const Quaternion& start, const Quaternion& end, float elapsed) {
        Quaternion q;
        gyQuaternionLerp(&q, &start, &end, elapsed);
        return q;
    }
    inline Quaternion Quaternion::slerp(const Quaternion& start, const Quaternion& end, float elapsed) {
        Quaternion q;
        gyQuaternionSlerp(&q, &start, &end, elapsed);
        return q;
    }
    inline Quaternion operator+(const Quaternion& lhs, const Quaternion& rhs) {
        return Quaternion(lhs.w + rhs.w, lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z);
    }
    inline Quaternion operator-(const Quaternion& lhs, const Quaternion& rhs) {
        return Quaternion(lhs.w - rhs.w, lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z);
    }
    inline Quaternion operator*(const Quaternion& quaternion, float scalar) {
        Quaternion ret;
        gyVec4Scale(&ret, &quaternion, scalar);
        return ret;
    }
    inline Quaternion operator/(const Quaternion& quaternion, float scalar) {
        return quaternion * (1.0f / scalar);
    }
    inline Quaternion Quaternion::fromRotationMatrix(const InstanceTransform&) {
        return Quaternion();
    }

    inline InstanceTransform Quaternion::getRotationMatrix(void) const {
        return InstanceTransform();
    }

    //===== INSTANCETRANSFORM =======
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

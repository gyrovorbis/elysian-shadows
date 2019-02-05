#ifndef ELYSIAN_SCENE_OBJECT_HPP
#define ELYSIAN_SCENE_OBJECT_HPP

#include <AssetIO/utility/elysian_bit_mask.hpp>
#include <libGyro/gyro_matrix_api.h>
#include <AssetIO/primitive/elysian_instance_transform.hpp>
#include <ESGamma/crtp/elysian_render_system.hpp>
#include <AssetIO/crtp/elysian_blend_mode.hpp>
#include <cfloat>

namespace elysian {

    //inherit some libGyro shit
    class AxisAlignedBoundingBox {
    public:
        enum VERTEX_TYPE { VERTEX_MIN, VERTEX_MAX, VERTEX_COUNT };
    protected:
        Vector4         _verts[VERTEX_COUNT];
    public:
                        AxisAlignedBoundingBox(void) = default;
                        AxisAlignedBoundingBox(Vector4 min, Vector4 max);

        Vector4&        getMinVertex(void);
        const Vector4&  getMinVertex(void) const;
        Vector4&        getMaxVertex(void);
        const Vector4&  getMaxVertex(void) const;

        void            setMaxVert(const Vector4& val);
        void            setMinVert(const Vector4& val);

        Vector4&        operator[](size_t type);
        const Vector4   operator[](size_t type) const;

    };

    struct RenderPriority: public BitMask<uint64_t> {
        const static constexpr uint64_t DepthBit            = 32;
        const static constexpr uint64_t DepthMask           = 0xffffffff00000000;
        const static constexpr uint64_t ShaderBit           = 24;
        const static constexpr uint64_t ShaderMask          = 0x00000000ff000000;
        const static constexpr uint64_t TextureBit          = 15;
        const static constexpr uint64_t TextureMask         = 0x0000000000ff8000;
        const static constexpr uint64_t BlendModeBit        = 5;
        const static constexpr uint64_t BlendModeMask       = 0x0000000000007fe0;   //Both blend factors combined
        const static constexpr uint64_t BlendFactorSrcBit   = 10;
        const static constexpr uint64_t BlendFactorSrcMask  = 0x0000000000007c00;   //5 bits per blend factor
        const static constexpr uint64_t BlendFactorDstBit   = 5;
        const static constexpr uint64_t BlendFactorDstMask  = 0x00000000000003e0;   //5 unused bits remaining


        float           getSceneDepth(void) const;
        uint8_t         getShader(void) const;
        uint16_t        getTexture(void) const;
        BlendMode       getBlendMode(void) const;
        GY_BLEND_FACTOR getBlendFactorDst(void) const;
        GY_BLEND_FACTOR getBlendFactorSrc(void) const;
        bool            isTransparent(void) const;

        void            setSceneDepth(float depth);
        void            setShader(uint8_t shader);
        void            setTexture(uint16_t texture);
        void            setBlendFactors(GY_BLEND_FACTOR srcFactor, GY_BLEND_FACTOR dstFactor);
        void            setBlendFactorSrc(GY_BLEND_FACTOR factor);
        void            setBlendFactorDst(GY_BLEND_FACTOR factor);
        void            setBlendMode(BlendMode mode);

        void            setValues(float sceneDepth, uint8_t shader, uint8_t texture, BlendMode mode);
        void            setValues(float sceneDepth, uint8_t shader, uint8_t texture, GY_BLEND_FACTOR srcFactor, GY_BLEND_FACTOR dstFactor);
    };

    class SceneObject {
    public:
    protected:
        AxisAlignedBoundingBox  _aabb;
        RenderPriority          _renderPriority;
        float                   _depthSquaredDist		= 0.0f;
        bool                    _isVisible 				= true;
        bool                    _sceneObjTransformDirty	= true;

    public:
        virtual                 ~SceneObject(void) = default;
        RenderPriority          getRenderPriority(void) const;
        bool                    isVisible(void) const;
        void                    setVisible(bool val);
        bool                    isTransparent(void) const;
        AxisAlignedBoundingBox& getAABB(void);
        const AxisAlignedBoundingBox getAABB(void) const;

        bool                    isSceneObjTransformDirty(void) const;
        void                    setSceneObjTransformDirty(bool val=true);
        auto                    getSceneBoundingBox(void) const -> const AxisAlignedBoundingBox&;

        //Actual overridable polymorphic functionality used by the SceneManager
        virtual void            updateBoundingVolume(void); //WTF why argument? (const Vector3& origin, cosnt GYMatrix4& viewProjMat);
        virtual void            updateRenderPriority(void);
        virtual void            render(void);
    };

    template<typename P>
    class SceneObjectAdapter: public SceneObject {
    protected:

        //CRTP overriddable within child implementation:
        const BlendMode*    getBlendMode(void) const;
        uint8_t             getShader(void) const;
        uint16_t            getTexture(void) const;

    public:
        //virtual void updateBoundingVolume(void); //use some accessors to do this?
        virtual void        updateRenderPriority(void) override;
    };


    class TransparentTileSceneObject: public SceneObject {

    };

    //represents overall view boundary of all active particles for the emitter
    class ParticleEmitterSceneObject: public SceneObject {


    };

    //Combine geometry/bullhsit for BOTH Skeleton + Renderable when both are present... ? maybe not? WTf?
    class EntitySceneObject: public SceneObject {
    public:
#if 0
        uint8_t			getTextureId(void) {
            if(getRenderable())  {
                return getRenderable()->getSpriteAttrib()->getTexture()->id;
            } else if(getSkeleton()) {
                return getSkeleton()->getAttrib()->getTexture()->id;
            }
            return 0; //shit, what's untextured?
        }

        uint8_t			getBlendMode(void) const {
            if(!alpha) return GY_VID_BLEND_NONE;
            if(getRenderable()) {
                return getRenderable()->getSpriteAttrib()->getBlendMode();
            } else if(getSkeleton()) {
                return getSkeleton()->getAttrib()->getBlendMode();
            }
        }
#endif

                virtual void 					updateBoundingVolume(const Vector3& origin);
    };





    //================= INLINEZ ==================


inline AxisAlignedBoundingBox::AxisAlignedBoundingBox(Vector4 min, Vector4 max) {
        _verts[VERTEX_MIN] = min;
        _verts[VERTEX_MAX] = max;
}

inline Vector4& AxisAlignedBoundingBox::getMinVertex(void) {
    return _verts[VERTEX_MIN];
}
inline const Vector4& AxisAlignedBoundingBox::getMinVertex(void) const {
    return _verts[VERTEX_MIN];
}
inline Vector4& AxisAlignedBoundingBox::getMaxVertex(void) {
    return _verts[VERTEX_MAX];
}
inline const Vector4& AxisAlignedBoundingBox::getMaxVertex(void) const {
    return _verts[VERTEX_MAX];
}

inline void AxisAlignedBoundingBox::setMaxVert(const Vector4& val) {
    _verts[VERTEX_MAX] = val;
}
inline void AxisAlignedBoundingBox::setMinVert(const Vector4& val) {
    _verts[VERTEX_MIN] = val;
}

inline Vector4& AxisAlignedBoundingBox::operator[](size_t type) {
    assert(type < VERTEX_COUNT);
    return _verts[type];
}
inline const Vector4 AxisAlignedBoundingBox::operator[](size_t type) const {
    assert(type < VERTEX_COUNT);
    return _verts[type];
}


    inline void RenderPriority::setValues(float sceneDepth, uint8_t shader, uint8_t texture, BlendMode mode) {
        setValues(sceneDepth, shader, texture, mode.getSrcFactor(), mode.getDstFactor());
    }

   inline  void RenderPriority::setValues(float sceneDepth, uint8_t shader, uint8_t texture, GY_BLEND_FACTOR srcBlend, GY_BLEND_FACTOR dstBlend) {
       setSceneDepth(sceneDepth);
       setShader(shader);
       setTexture(texture);
       setBlendFactors(srcBlend, dstBlend);
   }

   inline float RenderPriority::getSceneDepth(void) const {
       return static_cast<float>(getSubMaskValue(DepthBit, DepthMask));
   }

   inline uint8_t RenderPriority::getShader(void) const {
       return static_cast<uint8_t>(getSubMaskValue(ShaderBit, ShaderMask));
   }

   inline uint16_t RenderPriority::getTexture(void) const {
        return static_cast<uint16_t>(getSubMaskValue(TextureBit, TextureMask));
   }

   inline BlendMode RenderPriority::getBlendMode(void) const {
        return BlendMode(getBlendFactorSrc(), getBlendFactorDst());
   }

   inline GY_BLEND_FACTOR RenderPriority::getBlendFactorDst(void) const {
        return static_cast<GY_BLEND_FACTOR>(getSubMaskValue(BlendFactorDstBit, BlendFactorDstMask));
   }

   inline GY_BLEND_FACTOR RenderPriority::getBlendFactorSrc(void) const {
        return static_cast<GY_BLEND_FACTOR>(getSubMaskValue(BlendFactorSrcBit, BlendFactorSrcMask));
   }

   inline void RenderPriority::setBlendFactorSrc(GY_BLEND_FACTOR factor) {
       setSubMaskValue(BlendFactorSrcBit, BlendFactorSrcMask, factor);
   }

   inline void RenderPriority::setBlendFactorDst(GY_BLEND_FACTOR factor) {
       setSubMaskValue(BlendFactorDstBit, BlendFactorDstMask, factor);
   }

   inline bool RenderPriority::isTransparent(void) const {
       return false;
   }

   inline void RenderPriority::setSceneDepth(float depth) {
       setSubMaskValue(DepthBit, DepthMask, static_cast<uint32_t>(depth));
   }

   inline void RenderPriority::setShader(uint8_t shader) {
        setSubMaskValue(ShaderBit, ShaderMask, shader);
   }
   inline void RenderPriority::setTexture(uint16_t texture) {
        setSubMaskValue(TextureBit, TextureMask, texture);
   }

   inline void RenderPriority::setBlendFactors(GY_BLEND_FACTOR srcFactor, GY_BLEND_FACTOR dstFactor) {
        setBlendFactorSrc(srcFactor);
        setBlendFactorDst(dstFactor);
   }

    inline RenderPriority SceneObject::getRenderPriority(void) const {
        return _renderPriority;
    }

    inline bool	SceneObject::isVisible(void) const {
        return _isVisible;
    }

    inline AxisAlignedBoundingBox& SceneObject::getAABB(void) {
        return _aabb;
    }
    inline const AxisAlignedBoundingBox SceneObject::getAABB(void) const {
        return _aabb;
    }

    inline void	SceneObject::setVisible(bool val) { _isVisible = val; }

    inline void SceneObject::updateRenderPriority(void) {}

    inline void SceneObject::updateBoundingVolume(void) {}

    inline void SceneObject::render(void) {}


    inline void EntitySceneObject::updateBoundingVolume(const Vector3& origin) {
#if 0
        if(_sceneObjTransformDirty) {
            pushViewTransform();
            gyMatMultVec(aabbLocalVerts, _sceneAABB, 4);
            popViewTransform();
            _sceneObjTransformDirty = false;
        }

        _depthSquaredDist = FLOAT_MAX;
        for(unsigned v = 0; v < 4; ++v) {
            const float dist = gyVec3SquaredDist(&_sceneAABB[v]);
            if(dist < _depthSquaredDist) _depthSquaredDist = dist;
        }
#endif
    }


    template<typename P>
    inline void SceneObjectAdapter<P>::updateRenderPriority(void) {
#if 0
        _renderPriority.setShader(static_cast<P*>(this)->getShader());
        _renderPriority.setTexture(static_cast<P*>(this)->getTexture());
        _renderPriority.setBlendMode(static_cast<P*>(this)->getBlendMode());
#endif //not ready yet
    }

}


#endif // ELYSIAN_SCENE_OBJECT_HPP

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
        GYVector4 						_sceneAABB[4];
    };

    struct RenderPriority: public BitMask<uint64_t> {
        const static constexpr uint64_t DepthBit            = 32;
        const static constexpr uint64_t DepthMask           = 0xffffffff00000000;
        const static constexpr uint64_t ShaderBit           = 24;
        const static constexpr uint64_t ShaderMask          = 0x00000000ff000000;
        const static constexpr uint64_t TextureBit          = 16;
        const static constexpr uint64_t TextureMask         = 0x0000000000ff0000;
        const static constexpr uint64_t BlendModeBit        = 6;
        const static constexpr uint64_t BlendModeMask       = 0x000000000000ffc0;   //Both blend factors combined
        const static constexpr uint64_t BlendFactorSrcBit   = 11;
        const static constexpr uint64_t BlendFactorSrcMask  = 0x000000000000f800;   //5 bits per blend factor
        const static constexpr uint64_t BlendFactorDstBit   = 6;
        const static constexpr uint64_t BlendFactorDstMask  = 0x00000000000007c0;   //6 unused bits remaining


        float           getSceneDepth(void) const;
        uint8_t         getShader(void) const;
        uint8_t         getTexture(void) const;
        BlendMode       getBlendMode(void) const;
        GY_BLEND_FACTOR getBlendFactorDst(void) const;
        GY_BLEND_FACTOR getBlendFactorSrc(void) const;
        bool            isTransparent(void) const;

        void            setSceneDepth(float depth);
        void            setShader(uint8_t shader);
        void            setTexture(uint8_t texture);
        void            setBlendFactors(GY_BLEND_FACTOR srcFactor, GY_BLEND_FACTOR dstFactor);
        void            setBlendMode(BlendMode mode);

        void            setValues(float sceneDepth, uint8_t shader, uint8_t texture, BlendMode mode);
        void            setValues(float sceneDepth, uint8_t shader, uint8_t texture, GY_BLEND_FACTOR srcFactor, GY_BLEND_FACTOR dstFactor);

    };

    class SceneObject {
        AxisAlignedBoundingBox          _aabb;
        RenderPriority  				_renderPriority;
        float							_depthSquaredDist		= 0.0f;
        bool							_isVisible 				= true;
        bool							_sceneObjTransformDirty	= true;

        RenderPriority                  getRenderPriority(void);
        bool							isVisible(void);
        void							setVisible(bool val);
        bool							isSceneObjTransformDirty(void) const;
        void							setSceneObjTransformDirty(bool val=true);

        //Actual overridable polymorphic functionality used by the SceneManager
        virtual void 					updateBoundingVolume(void); //WTF why argument? (const Vector3& origin, cosnt GYMatrix4& viewProjMat);
        virtual void 	 				updateRenderPriority(void);
        virtual void                    render(void);
    };


    class SceneObject {
    private:
        AxisAlignedBoundingBox          _aabb;
        RenderPriority  				_renderPriority;
        float							_depthSquaredDist		= 0.0f;
        bool							_isVisible 				= true;
        bool							_sceneObjTransformDirty	= true;
    public:

        RenderPriority                  getRenderPriority(void);
        bool							isVisible(void);
        void							setVisible(bool val);
        bool							isSceneObjTransformDirty(void) const;
        void							setSceneObjTransformDirty(bool val=true);

    //==== OVERRIDABLE API ========

        virtual void					render(void);
        virtual const GYVector4*		getSceneBoundingBox(void) const;

        //implement these via CRTP
        virtual float					getSceneDepth(void) const;
        virtual uint8_t					getShaderId(void) const;
        virtual uint8_t					getTextureId(void) const;
        virtual uint8_t					getBlendMode(void) const;
        virtual bool					isTransparent(void) const;

        //Overridable polymorphic methods

    };


    class OpaqueTilesSceneObject: public SceneObject {

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
       return static_cast<float>(getSubMaskValue(DepthBit, DepthMask))
   }

   inline uint8_t RenderPriority::getShader(void) const {
       return static_cast<uint8_t>(getSubMaskValue(ShaderBit, ShaderMask));
   }

   inline uint8_t RenderPriority::getTexture(void) const;
   inline BlendMode RenderPriority::getBlendMode(void) const;
   inline GY_BLEND_FACTOR RenderPriority::getBlendFactorDst(void) const;
   inline GY_BLEND_FACTOR RenderPriority::getBlendFactorSrc(void) const;
   inline bool RenderPriority::isTransparent(void) const;

   inline void RenderPriority::setSceneDepth(float depth) {
       setSubMaskValue(DepthBit, DepthMask, depth);
   }

   inline void RenderPriority::setShader(uint8_t shader) {
        setSubMaskValue(ShaderBit, ShaderMask, shader);
   }
   inline void RenderPriority::setTexture(uint8_t texture);
   inline void RenderPriority::setBlendFactors(GY_BLEND_FACTOR srcFactor, GY_BLEND_FACTOR dstFactor);


    inline RenderPriority SceneObject::getRenderPriority(void) const { return _renderPriority; }
    inline bool	SceneObject::isVisible(void) const { return _isVisible; }
    inline void	SceneObject::setVisible(bool val) { _isVisible = val; }

    inline void SceneObject::updateRenderPriority(void) {
           _renderPriority = (uint64_t)(((uint64_t)getSceneDepth()  << 32)
                               |    (uint64_t)(getShaderId()        << 24)
                               |    (uint64_t)(getTextureId()       << 16)
                               |	(uint64_t)(getBlendMode()));
       }


    void EntitySceneObject::updateBoundingVolume(const Vector3& origin) {
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


    inline void SceneObject::render(void) {}
    inline const GYVector4* SceneObject::getSceneBoundingBox(void) const { return nullptr; }
    inline  float SceneObject::getSceneDepth(void) const { return FLT_MAX; }
    inline  uint8_t	SceneObject::getShaderId(void) const { return 0; }
    inline uint8_t	SceneObject::getTextureId(void) const { return 0; }
    inline uint8_t	SceneObject::getBlendMode(void) const { return 0; }
    inline bool	SceneObject::isTransparent(void) const { return 0; }
    inline void SceneObject::updateBoundingVolume(const Vector3&) {}


}


#endif // ELYSIAN_SCENE_OBJECT_HPP

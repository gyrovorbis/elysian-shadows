#ifndef ELYSIAN_SCENE_OBJECT_HPP
#define ELYSIAN_SCENE_OBJECT_HPP

#include <utility/elysian_bit_mask.hpp>
#include <gyro_matrix_api.h>
#include <primitive/elysian_instance_transform.hpp>
#include <cfloat>

namespace elysian {

    using RenderPriority = BitMask<uint64_t>;

    class SceneObject {
    private:
        GYVector4 						_sceneAABB[4];
        RenderPriority  				_renderPriority;
        float							_depthSquaredDist		= 0.0f;
        bool							_isVisible 				= true;
        bool							_sceneObjTransformDirty	= true;
    public:

        RenderPriority                  getRenderPriority(void) const { return _renderPriority; }
        bool							isVisible(void) const { return _isVisible; }
        void							setVisible(bool val) { _isVisible = val; }
        bool							isSceneObjTransformDirty(void) const;
        void							setSceneObjTransformDirty(bool val=true);

    //==== OVERRIDABLE API ========

        virtual void					render(void);
        virtual const GYVector4*		getSceneBoundingBox(void) const;

        virtual float					getSceneDepth(void) const;
        virtual uint8_t					getShaderId(void) const;
        virtual uint8_t					getTextureId(void) const;
        virtual uint8_t					getBlendMode(void) const;
        virtual bool					isTransparent(void) const;

        virtual void 					updateBoundingVolume(const Vector3& origin);

        virtual void 	 				updateRenderPriority(void);
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

    void SceneObject::updateRenderPriority(void) {
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

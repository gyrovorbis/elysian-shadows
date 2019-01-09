#ifndef ELYSIAN_RENDERABLE_BASE_HPP
#define ELYSIAN_RENDERABLE_BASE_HPP

#include <AssetIO/crtp/elysian_component.hpp>
#include <AssetIO/primitive/elysian_quad_sprite_attrib.hpp>
#include <AssetIO/primitive/elysian_material.hpp>
#include <AssetIO/crtp/elysian_entity.hpp>
#include <AssetIO/crtp/elysian_lua_table_proxy.hpp>
#include <libGyro/gyro_video_api.h>
#include <memory>

ELYSIAN_ASSET_TYPEDEF(Entity)

namespace elysian {

    class RenderableBase:
            public Component,
            public LuaTableProxy<RenderableBase, Renderable> {
    friend class EntityBase;
    friend Entity;

    protected:
        GYTexCoord2                 _texCoord[4] = { { 0.0f, 0.0f } };
        shared_ptr<SpriteAttrib>	_spriteAttrib;
        int                         _frame			= 0;			//for animation
        GYVector2                   _offset			= {0.0f, 0.0f};	//texture offset from base transform (local coords)
        GY_RECT_TEX_ORIENT          _texOrient		= GY_RECT_TEX_ORIENT_NORMAL;



  public:

        ELYSIAN_LUA_PROXY_FIELDS_BEGIN()
            ELYSIAN_LUA_PROXY_FIELD(frame,          getFrame,           setFrame)
            ELYSIAN_LUA_PROXY_FIELD(texOrient,      getTexOrient,       setTexOrient)
            ELYSIAN_LUA_PROXY_FIELD(spriteAttrib,   getSpriteAttrib,    setSpriteAttrib)
            ELYSIAN_LUA_PROXY_FIELD(offset,         getOffsetVec,       setOffsetVec)
            ELYSIAN_LUA_PROXY_ARRAY_FIELD(uvCoords, getUVCoords,        setUVCoords,    getUVCoordsCount)
    ELYSIAN_LUA_PROXY_FIELDS_END()

                            RenderableBase(void) = default;
                            RenderableBase(Entity &parent, RenderableBase* prefab=nullptr);
        RenderableBase&     operator=(RenderableBase& rhs);

        static bool         hasPrereqs(Entity &entity);
        static constexpr const Component::TYPE componentType = Component::RENDERABLE;

        /*Split into phases JUST IN CASE we wanted to optimize for
        header submission/state changes when renderables share attributes*/
        void                beginRender(void);
        void                render(void);
        void                endRender(void);

        unsigned			getFrame(void) const;
        void				setFrame(const unsigned fr);
        void				setOffset(const GYVector2 &offset);
        void				getOffset(GYVector2 &offset) const;
        Vector2             getOffsetVec(void);
        void                setOffsetVec(const Vector2& vec);
        void				setTexOrient(const GY_RECT_TEX_ORIENT texOrient);
        int             	getTexOrient(void) const;

        shared_ptr<SpriteAttrib>       getSpriteAttrib(void) const;
        shared_ptr<QuadSpriteAttrib>   getQuadSpriteAttrib(void) const;
        void                setSpriteAttrib(shared_ptr<SpriteAttrib> attrib);

        void                setTexCoord(const unsigned index, const float u, const float v);

        unsigned            getUVCoordsCount(void) const;
        Vector2             getUVCoords(const unsigned index);
        void                setUVCoords(const unsigned index, const Vector2& coords);




    };

    //=================== INLINEZ ======================
    inline RenderableBase::RenderableBase(Entity &parent, RenderableBase*const prefab): Component(parent) {
        if(prefab) *this = *prefab;
    }
    inline RenderableBase& RenderableBase::operator=(RenderableBase& rhs) {
        memcpy(_texCoord, rhs._texCoord, sizeof(_texCoord));
        _spriteAttrib   = rhs._spriteAttrib;
        _frame          = rhs._frame;
        _offset         = rhs._offset;
        _texOrient      = rhs._texOrient;
        return *this;
    }

    inline void RenderableBase::beginRender(void) {}
    inline void RenderableBase::endRender(void) {}
    inline unsigned RenderableBase::getFrame(void) const { return _frame; }
    inline void	RenderableBase::setFrame(const unsigned fr) { _frame = fr; }
    inline void	RenderableBase::setOffset(const GYVector2 &offset) { _offset = offset; }
    inline void	RenderableBase::getOffset(GYVector2 &offset) const { offset = _offset; }
    inline void	RenderableBase::setTexOrient(const GY_RECT_TEX_ORIENT texOrient) { _texOrient = texOrient; }
    inline int RenderableBase::getTexOrient(void) const { return _texOrient; }
    inline shared_ptr<SpriteAttrib> RenderableBase::getSpriteAttrib(void) const { return _spriteAttrib; }

    inline void RenderableBase::setSpriteAttrib(shared_ptr<SpriteAttrib> attrib) { _spriteAttrib = attrib; if(attrib && _frame != -1) _frame = attrib->_frame; }
    inline Vector2 RenderableBase::getOffsetVec(void) { return Vector2(&_offset); }
    inline void RenderableBase::setOffsetVec(const Vector2 &vec) { _offset = vec; }


    inline void RenderableBase::setTexCoord(const unsigned index, const float u, const float v) {
        _frame = -1;
        _texCoord[index].u = u; _texCoord[index].v = v;
    }

    inline void RenderableBase::render(void) {


        gyVidBlendMode(_spriteAttrib->getBlendMode().getSrcFactor(),
                       _spriteAttrib->getBlendMode().getDstFactor());

       gyVidTexBind(_spriteAttrib? _spriteAttrib->_tex : nullptr);
       gyVidNormalMapBind(_spriteAttrib? _spriteAttrib->getNormalMap() : nullptr);
       gyVidSpecularMapBind(_spriteAttrib? _spriteAttrib->getSpecularMap() : nullptr);
       gyVidMaterialBind(_spriteAttrib? _spriteAttrib->getMaterial() : nullptr);

        //gyMatTranslate(_offset.x, _offset.y, 0.0f);

        //branch assumes shit will usually be depth buffered

        gyVidRectPlaneType(_entity->getPlaneType());
        bool alpha = false;
        (void)alpha; // TODO use or remove

        //Submit vertices

        switch(_spriteAttrib->getType()) {
        case PrimAttrib::QUAD_SPRITE:

            for(unsigned i = 0; i < 4; ++i) {
                if(std::static_pointer_cast<QuadSpriteAttrib>(_spriteAttrib)->getVertexAlpha(i) != 255) {
                    alpha = true;
                    break;
                }
            }

             //yVidAlphaLevel((alpha)? GY_ALPHA_PUNCHTHRU : GY_ALPHA_FULL);
             //gyVidAlphaLevel(GY_ALPHA_NONE);


             //newZFactor = glalOrientNormalMapZFactor(_texOrient);
             //glalVidNormalMapZFactorUpdateUniform(newZFactor);
            gyVidQuadListBegin(&_spriteAttrib->_color);
            gyVidAlphaLevel(GY_ALPHA_FULL);
            gyVidColorArray(&_spriteAttrib->_color);
            if(_spriteAttrib->_tex) {
                if(_frame != -1)
                    gyVidTexCoordArray(gyVidTexCoordArrayFromFrame(&_spriteAttrib->_tex->_coord->texCoord[_frame],
                                                               _texOrient));
                else
                    gyVidTexCoordArray(_texCoord);

            }
            //gyVidTriangleStrip(localVerts, 0, 4);
            gyMatTranslate(_offset.x, _offset.y, 0.0f);
            gyVidRect(-1); //HACK AS FUCK PROBABLY BROKE OTHER NON-GLAL PLATFORMS!!!
            gyVidQuadListEnd();
            break;
        case PrimAttrib::SPRITE:
            if(_spriteAttrib->_color.a == 0.0f) return;
            gyVidAlphaLevel((_spriteAttrib->_color.a < 255)? GY_ALPHA_FULL : GY_ALPHA_NONE);

           // newZFactor = glalOrientNormalMapZFactor(_texOrient);
            //glalVidNormalMapZFactorUpdateUniform(newZFactor);
            gyVidQuadListBegin(&_spriteAttrib->_color);

            if(_frame == -1) {
                //for(unsigned i = 0; i < 4; ++i) colorArray[i] = _spriteAttrib->_color;
                gyVidTexCoordArray(_texCoord);
                //gyVidColorArray(colorArray);
            } else {
                gyVidRectTexOrient(_texOrient);
            }

            gyMatTranslate(_offset.x, _offset.y, 0.0f);
            gyVidRect(_frame);
            gyVidQuadListEnd();
            break;
        default: break;//the fuck is wrong with you, son?
        }

    }

    inline unsigned RenderableBase::getUVCoordsCount(void) const { return 4; }
    inline Vector2 RenderableBase::getUVCoords(const unsigned index) { return Vector2((GYVector2*)&_texCoord[index]); }
    inline void RenderableBase::setUVCoords(const unsigned index, const Vector2& coords) { _texCoord[index].u = coords.x; _texCoord[index].v = coords.y; }

}

#endif

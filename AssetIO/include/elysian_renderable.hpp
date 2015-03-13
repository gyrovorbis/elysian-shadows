#ifndef ELYSIAN_RENDERABLE_HPP
#define ELYSIAN_RENDERABLE_HPP

#include "elysian_component.hpp"
#include "elysian_quad_sprite_attrib.hpp"
#include "elysian_material.hpp"
#include "elysian_asset_typedefs.hpp"
#include "elysian_entity.hpp"
#include "elysian_lua_table_proxy.hpp"
#include <gyro_video_api.h>

ELYSIAN_ASSET_TYPEDEF(Entity)

namespace elysian {

    class Renderable:
            public Component,
            public LuaTableProxy<Renderable> {
    friend class EntityBase;
    friend Entity;
	private:
		GYTexCoord2			_texCoord[4];
		Material*			_material		= nullptr;
		GYTexture*			_normalMap		= nullptr;
		GYTexture*			_specularMap	= nullptr;
        SpriteAttrib*		_spriteAttrib	= nullptr;      //Prefab-able attributes
		int					_frame			= 0;			//for animation
        GYVector2			_offset			= {0.0f, 0.0f};	//texture offset from base transform (local coords)
        GY_RECT_TEX_ORIENT	_texOrient		= GY_RECT_TEX_ORIENT_NORMAL;

        Renderable(Entity &parent);
	public:

        ELYSIAN_LUA_PROXY_FIELDS_BEGIN()
			ELYSIAN_LUA_PROXY_FIELD(frame, getFrame, setFrame)
            //ELYSIAN_LUA_PROXY_FIELD(offset, getOffset, setOffset)
            ELYSIAN_LUA_PROXY_FIELD(texOrient, getTexOrient, setTexOrient)
		ELYSIAN_LUA_PROXY_FIELDS_END()

        virtual bool        canAddTo(Entity& /*entity*/);
        virtual bool        load(FILE* file) override;
        virtual bool        save(FILE* file) override;
        virtual bool        addToLevel(void) override;
        virtual bool        removeFromLevel(void) override;
        virtual TYPE        getType(void) const override;

		/*Split into phases JUST IN CASE we wanted to optimize for 
		header submission/state changes when renderables share attributes*/
        void                beginRender(void);
        void                render(void);
        void                endRender(void);

        unsigned			getFrame(void) const;
        void				setFrame(const unsigned fr);
        void				setOffset(const GYVector2 &offset);
        void				getOffset(GYVector2 &offset) const;
        void				setTexOrient(const GY_RECT_TEX_ORIENT texOrient);
        GY_RECT_TEX_ORIENT	getTexOrient(void) const;

        SpriteAttrib*       getSpriteAttrib(void) const;
        QuadSpriteAttrib*   getQuadSpriteAttrib(void) const;
        void                setSpriteAttrib(SpriteAttrib *const attrib);

        void                setMaterial(Material*const material);
        Material*           getMaterial(void) const;

        void                loadNormalMap(const char*const path, const unsigned /*width*/, const unsigned /*height*/);
        void                loadSpecularMap(const char*const path, const unsigned /*width*/, const unsigned /*height*/);
        void                setTexCoord(const unsigned index, const float u, const float v);
		
	};







    //=================== INLINEZ ======================
    inline Renderable::Renderable(Entity &parent): Component(parent) {}
    inline bool Renderable::canAddTo(Entity& /*entity*/) { return true; } //no prereqs
    inline auto Renderable::getType(void) const -> TYPE { return Component::RENDERABLE; }
    inline void Renderable::beginRender(void) {}
    inline void Renderable::endRender(void) {}
    inline unsigned Renderable::getFrame(void) const { return _frame; }
    inline void	Renderable::setFrame(const unsigned fr) { _frame = fr; }
    inline void	Renderable::setOffset(const GYVector2 &offset) { _offset = offset; }
    inline void	Renderable::getOffset(GYVector2 &offset) const { offset = _offset; }
    inline void	Renderable::setTexOrient(const GY_RECT_TEX_ORIENT texOrient) { _texOrient = texOrient; }
    inline GY_RECT_TEX_ORIENT Renderable::getTexOrient(void) const { return _texOrient; }
    inline SpriteAttrib* Renderable::getSpriteAttrib(void) const { return _spriteAttrib; }
    inline QuadSpriteAttrib* Renderable::getQuadSpriteAttrib(void) const { return static_cast<QuadSpriteAttrib*>(_spriteAttrib); }
    inline void Renderable::setSpriteAttrib(SpriteAttrib *const attrib) { _spriteAttrib = attrib; if(attrib && _frame != -1) _frame = attrib->_frame; }
    inline void Renderable::setMaterial(Material*const material) { _material = material; }
    inline Material* Renderable::getMaterial(void) const { return _material;}

    inline void Renderable::loadNormalMap(const char*const path, const unsigned /*width*/, const unsigned /*height*/) {
        if(_normalMap) gyVidTexDestroy(_normalMap);
        _normalMap = gyVidTexCreate(path);
    }
    inline void Renderable::loadSpecularMap(const char*const path, const unsigned /*width*/, const unsigned /*height*/) {
        if(_specularMap) gyVidTexDestroy(_specularMap);
        _specularMap = gyVidTexCreate(path);
    }
    inline void Renderable::setTexCoord(const unsigned index, const float u, const float v) {
        _frame = -1;
        _texCoord[index].u = u; _texCoord[index].v = v;
    }





    inline void Renderable::render(void) {

        gyVidTexBind(_spriteAttrib->_tex);
        gyVidNormalMapBind(_normalMap);
        gyVidSpecularMapBind(_specularMap);
        gyVidMaterialBind(_material);

        //Why aren't we just pushing another matrix to handle this?
        /*if(_spriteAttrib->_tex && _frame != -1) {
            const GYVector2 *size = (GYVector2*)&_entity->transform._size;
            const GYTexCoordFrame *fr = &_spriteAttrib->_tex->_coord->texCoord[_frame];
            const float locX = ((float) fr->spriteWidth / size->x)/2.0f;
            const float locY = ((float) fr->spriteHeight / size->y)/2.0f;
            localVerts[0].x = -locX + _offset.x;
            localVerts[0].y = -locY + _offset.y;
            localVerts[0].z = locY*2.0f + _offset.y;
            localVerts[1].x = locX + _offset.x;
            localVerts[1].y = -locY + _offset.y;
            localVerts[1].z = locY*2.0f + _offset.y;
            localVerts[2].x = -locX + _offset.x;
            localVerts[2].y = locY + _offset.y;
        //	localVerts[0].z = -locY + _offset.y;
            localVerts[3].x = locX + _offset.x;
            localVerts[3].y = locY + _offset.y;
        //	localVerts[0].z = -locY + _offset.y;
        }*/

        gyMatTranslate(_offset.x, _offset.y, 0.0f);

        //branch assumes shit will usually be depth buffered
    /*	if(getEntity()->getNormalType() != NORMAL_FRONT) {
            localVerts[0].z = localVerts[1].z = 0.0f;
            gyVidRectPlaneType(GY_RECT_PLANE_BOTTOM);
        } else {
            gyVidRectPlaneType(GY_RECT_PLANE_FRONT);
        }*/

        gyVidRectPlaneType(_entity->getPlaneType());

        //Submit vertices
        switch(_spriteAttrib->getType()) {
        case PrimAttrib::QUAD_SPRITE:
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
            gyVidRect(-1); //HACK AS FUCK PROBABLY BROKE OTHER NON-GLAL PLATFORMS!!!
            gyVidQuadListEnd();
            break;
        case PrimAttrib::SPRITE:
            if(_spriteAttrib->_color.a == 0.0f) return;
            //gyVidAlphaLevel((_spriteAttrib->_color.a == 255)? GY_ALPHA_PUNCHTHRU : GY_ALPHA_FULL);
            gyVidQuadListBegin(&_spriteAttrib->_color);

            if(_frame == -1) {
                //for(unsigned i = 0; i < 4; ++i) colorArray[i] = _spriteAttrib->_color;
                gyVidTexCoordArray(_texCoord);
                //gyVidColorArray(colorArray);
            } else {
                gyVidRectTexOrient(_texOrient);
            }

            gyVidRect(_frame);
            gyVidQuadListEnd();
            break;
        default: break;//the fuck is wrong with you, son?
        }

    }


}

#endif

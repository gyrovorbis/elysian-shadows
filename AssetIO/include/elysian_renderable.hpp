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
            ELYSIAN_LUA_PROXY_FIELD(texOrient, getTexOrient, setTexOrient)
            ELYSIAN_LUA_PROXY_FIELD(spriteAttrib, getSpriteAttrib)
            ELYSIAN_LUA_PROXY_FIELD(normalMap, getNormalMapPath)
            ELYSIAN_LUA_PROXY_FIELD(specularMap, getSpecularMapPath)
            ELYSIAN_LUA_PROXY_FIELD(material, getMaterial)
            ELYSIAN_LUA_PROXY_FIELD(offset, getOffset)
		ELYSIAN_LUA_PROXY_FIELDS_END()

        virtual bool        canAddTo(Entity& /*entity*/) const override;
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
        Vector2             getOffset(void);
        void				setTexOrient(const GY_RECT_TEX_ORIENT texOrient);
        int             	getTexOrient(void) const;

        SpriteAttrib*       getSpriteAttrib(void) const;
        QuadSpriteAttrib*   getQuadSpriteAttrib(void) const;
        void                setSpriteAttrib(SpriteAttrib *const attrib);

        void                setMaterial(Material*const material);
        Material*           getMaterial(void) const;

        void                loadNormalMap(const char*const path, const unsigned /*width*/, const unsigned /*height*/);
        void                loadSpecularMap(const char*const path, const unsigned /*width*/, const unsigned /*height*/);
        void                setTexCoord(const unsigned index, const float u, const float v);

        const char*         getNormalMapPath(void) const;
        const char*         getSpecularMapPath(void) const;
		
	};







    //=================== INLINEZ ======================
    inline Renderable::Renderable(Entity &parent): Component(parent) {}
    inline bool Renderable::canAddTo(Entity& /*entity*/) const { return true; } //no prereqs
    inline auto Renderable::getType(void) const -> TYPE { return Component::RENDERABLE; }
    inline void Renderable::beginRender(void) {}
    inline void Renderable::endRender(void) {}
    inline unsigned Renderable::getFrame(void) const { return _frame; }
    inline void	Renderable::setFrame(const unsigned fr) { _frame = fr; }
    inline void	Renderable::setOffset(const GYVector2 &offset) { _offset = offset; }
    inline void	Renderable::getOffset(GYVector2 &offset) const { offset = _offset; }
    inline void	Renderable::setTexOrient(const GY_RECT_TEX_ORIENT texOrient) { _texOrient = texOrient; }
    inline int Renderable::getTexOrient(void) const { return _texOrient; }
    inline SpriteAttrib* Renderable::getSpriteAttrib(void) const { return _spriteAttrib; }
    inline QuadSpriteAttrib* Renderable::getQuadSpriteAttrib(void) const { return static_cast<QuadSpriteAttrib*>(_spriteAttrib); }
    inline void Renderable::setSpriteAttrib(SpriteAttrib *const attrib) { _spriteAttrib = attrib; if(attrib && _frame != -1) _frame = attrib->_frame; }
    inline void Renderable::setMaterial(Material*const material) { _material = material; }
    inline Material* Renderable::getMaterial(void) const { return _material;}
    inline Vector2 Renderable::getOffset(void) { return Vector2(&_offset); }

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

    inline const char* Renderable::getNormalMapPath(void) const {
        return _normalMap ? _normalMap->filename : "none";
    }

    inline const char* Renderable::getSpecularMapPath(void) const {
        return _specularMap ? _specularMap->filename : "none";
    }



    inline void Renderable::render(void) {

        if(_spriteAttrib->_color.a == 0.0f) return;
        gyVidAlphaLevel((_spriteAttrib->_color.a == 255)? GY_ALPHA_PUNCHTHRU : GY_ALPHA_FULL);

        gyVidTexBind(_spriteAttrib->_tex);
        gyVidNormalMapBind(_normalMap);
        gyVidSpecularMapBind(_specularMap);
        gyVidMaterialBind(_material);

        gyMatTranslate(_offset.x, _offset.y, 0.0f);

        //branch assumes shit will usually be depth buffered

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

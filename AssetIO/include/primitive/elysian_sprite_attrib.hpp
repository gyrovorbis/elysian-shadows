#ifndef ELYSIAN_SPRITE_ATTRIB_HPP
#define ELYSIAN_SPRITE_ATTRIB_HPP

#include <primitive/elysian_prim_attrib.hpp>
#include <crtp/elysian_lua_table_proxy.hpp>
#include <primitive/elysian_color.hpp>

ELYSIAN_ASSET_TYPEDEF(Renderable)

namespace elysian {

    class Material;

    class SpriteAttrib:
            public PrimAttrib,
            public LuaTableProxy<SpriteAttrib>
    {
        friend Renderable;
        friend class RenderableBase;
	protected:
        GYTexCoord2	_texCoords[4];
        GYTexture*	_tex			= nullptr;
        GYTexture*	_normalMap		= nullptr;
        GYTexture*	_specularMap	= nullptr;
        Material*   _material		= nullptr;
        unsigned	_spriteWidth	= 0;
        unsigned	_spriteHeight	= 0;
        int			_frame			= 0;
        GYColor4	_color			= {255, 255, 255, 255};
    public:

        ELYSIAN_LUA_PROXY_FIELDS_BEGIN()
            ELYSIAN_LUA_PROXY_FIELD(texture,        getTexPath, setTexPath)
            ELYSIAN_LUA_PROXY_FIELD(normalMap,      getNormalMapPath,   setNormalMapPath)
            ELYSIAN_LUA_PROXY_FIELD(specularMap,    getSpecularMapPath, setSpecularMapPath)
            ELYSIAN_LUA_PROXY_FIELD(material,       getMaterial,        setMaterial)
            ELYSIAN_LUA_PROXY_FIELD(color,          getColor4, setColor4)
            ELYSIAN_LUA_PROXY_FIELD(frame,          getFrame, setFrame)
        ELYSIAN_LUA_PROXY_FIELDS_END()

        void                setSpecularMapPath(const char*const path);
        void                setNormalMapPath(const char*const path);
        const char*         getNormalMapPath(void) const;
        const char*         getSpecularMapPath(void) const;
        void                setMaterial(Material*const material);
        Material*           getMaterial(void) const;

        //Not super efficient, but since each rect needs its own color, header must be resubmitted...
        void                submit(void);

        GYTexture*          getTexture(void) const;
        GYTexture*          getNormalMap(void) const;
        GYTexture*          getSpecularMap(void) const;

        unsigned char getColorR(void) const;
        unsigned char getColorG(void) const;
        unsigned char getColorB(void) const;
        unsigned char getColorA(void) const;
        void setColorR(const unsigned char r);
        void setColorG(const unsigned char g);
        void setColorB(const unsigned char b);
        void setColorA(const unsigned char a);

        virtual ~SpriteAttrib(void) override;

        inline void submitHeader(void);

        inline void submitFinish(void);

        virtual void submitVerts(GYVector4* verts = nullptr);

        void setTexCoord(const unsigned index, const float u, const float v);
        void getTexCoord(const unsigned index, float &u, float &v);

        Color4 getColor4(void);
        void setColor4(const Color4& color);

        void setColor(const unsigned char r, const unsigned char g, const unsigned char b, const unsigned char a);
        void setColor(const GYColor4& color);

        void getColor(unsigned char& r, unsigned char& g, unsigned char& b, unsigned char& a) const;
        void getColor(GYColor4& color) const;
        GYColor4 getColor(void) const;

        void setAlpha(const unsigned char a);
        unsigned char getAlpha(void) const;

        PrimAttrib::TYPE getType(void) const;

		//Gonna shit their pants if there is no texture allocated yet...
        void getSheetSize(unsigned& sheetWidth, unsigned& sheetHeight) const;
        void getSpriteSize(unsigned& spriteWidth, unsigned& spriteHeight) const;
        const char* getTexPath(void) const;
        void setTexPath(const char*const path);

        void loadTexture(const char* filename, const unsigned sheetWidth, const unsigned sheetHeight, const unsigned spriteWidth, const unsigned spriteHeight);

        void loadSheet(const char* filename, const unsigned sheetWidth, const unsigned sheetHeight);
        void setFrame(const unsigned fr);
        int getFrame(void) const;

	};




    //====== INLINEZ =======

    inline void SpriteAttrib::setSpecularMapPath(const char * const path) {
        if(_normalMap) gyVidTexDestroy(_normalMap);
        _normalMap = gyVidTexCreate(path);
    }
    inline void SpriteAttrib::setNormalMapPath(const char * const path) {
        if(_specularMap) gyVidTexDestroy(_specularMap);
        _specularMap = gyVidTexCreate(path);
    }

    inline void SpriteAttrib::setMaterial(Material*const material) { _material = material; }
    inline Material* SpriteAttrib::getMaterial(void) const { return _material;}

    inline const char* SpriteAttrib::getNormalMapPath(void) const {
        return _normalMap ? _normalMap->filename : "none";
    }

    inline const char* SpriteAttrib::getSpecularMapPath(void) const {
        return _specularMap ? _specularMap->filename : "none";
    }

    inline void SpriteAttrib::submit(void) {

        submitHeader();
        gyVidRect(_frame);
        submitFinish();

    }

    inline unsigned char SpriteAttrib::getColorR(void) const { return _color.r; }
    inline unsigned char SpriteAttrib::getColorG(void) const { return _color.g; }
    inline unsigned char SpriteAttrib::getColorB(void) const { return _color.b; }
    inline unsigned char SpriteAttrib::getColorA(void) const { return _color.a; }
    inline void SpriteAttrib::setColorR(const unsigned char r) { _color.r = r; }
    inline void SpriteAttrib::setColorG(const unsigned char g) { _color.g = g; }
    inline void SpriteAttrib::setColorB(const unsigned char b) { _color.b = b; }
    inline void SpriteAttrib::setColorA(const unsigned char a) { _color.a = a; }
    inline GYTexture* SpriteAttrib::getSpecularMap(void) const { return _specularMap; }
    inline GYTexture* SpriteAttrib::getNormalMap(void) const { return _normalMap; }
    inline GYTexture* SpriteAttrib::getTexture(void) const { return _tex; }

    inline SpriteAttrib::~SpriteAttrib(void) {

        gyVidTexDestroy(_tex);

    }

    inline void SpriteAttrib::submitHeader(void) {
        if(_color.a == 255) gyVidAlphaLevel(GY_ALPHA_NONE);
        else gyVidAlphaLevel(GY_ALPHA_FULL);

        //gyVidMaterialBind(nullptr);
        gyVidTexBind(_tex);
        gyVidQuadListBegin(&_color);
        if(_frame == -1) gyVidTexCoordArray(_texCoords);

    }

    inline void SpriteAttrib::submitFinish(void) {
        gyVidQuadListEnd();
    }


    inline void SpriteAttrib::submitVerts(GYVector4* verts) {
        (void)verts;

        gyVidRect(_frame);

    }

    inline void SpriteAttrib::setTexCoord(const unsigned index, const float u, const float v) {
        _texCoords[index].u = u; _texCoords[index].v = v;
        _frame = -1;
    }

    inline void SpriteAttrib::getTexCoord(const unsigned index, float &u, float &v) {
        u = _texCoords[index].u; v = _texCoords[index].v;
    }

    inline Color4 SpriteAttrib::getColor4(void) { return Color4(&_color); }
    inline void SpriteAttrib::setColor4(const Color4& color) { _color = color; }

    inline void SpriteAttrib::setColor(const unsigned char r, const unsigned char g, const unsigned char b, const unsigned char a) { _color.r = r; _color.g = g; _color.b = b; _color.a = a; }
    inline void SpriteAttrib::setColor(const GYColor4& color) { _color = color; }

    inline void SpriteAttrib::getColor(unsigned char& r, unsigned char& g, unsigned char& b, unsigned char& a) const { r = _color.r; g = _color.g; b = _color.b; a = _color.a; }
    inline void SpriteAttrib::getColor(GYColor4& color) const { color = _color; }
    inline GYColor4 SpriteAttrib::getColor(void) const { return _color; }

    inline void SpriteAttrib::setAlpha(const unsigned char a) { _color.a = a; }
    inline unsigned char SpriteAttrib::getAlpha(void) const { return _color.a; }

    inline PrimAttrib::TYPE SpriteAttrib::getType(void) const { return PrimAttrib::SPRITE; }

    //Gonna shit their pants if there is no texture allocated yet...
    inline void SpriteAttrib::getSheetSize(unsigned& sheetWidth, unsigned& sheetHeight) const { sheetWidth = _tex->_coord->sheetWidth; sheetHeight = _tex->_coord->sheetHeight; }
    inline void SpriteAttrib::getSpriteSize(unsigned& spriteWidth, unsigned& spriteHeight) const { spriteWidth = _tex->_coord->spriteWidth; spriteHeight = _tex->_coord->spriteHeight; }
    inline const char* SpriteAttrib::getTexPath(void) const { return _tex? _tex->filename : "none"; }
    inline void SpriteAttrib::setTexPath(const char*const path) {
        delete _tex;
        _tex = gyVidTexCreate(path);

    }

    inline void SpriteAttrib::loadTexture(const char* filename, const unsigned sheetWidth, const unsigned sheetHeight, const unsigned spriteWidth, const unsigned spriteHeight) {
        if(_tex) {
            gyVidTexDestroy(_tex);
        }
        _tex = gyVidTexCreateSheetUniform(filename, sheetWidth, sheetHeight, spriteWidth, spriteHeight);
    }

    inline void SpriteAttrib::loadSheet(const char* filename, const unsigned sheetWidth, const unsigned sheetHeight) {
        if(_tex) {
            gyVidTexDestroy(_tex);
        }
        _tex = gyVidTexCreateSheet(filename, sheetWidth, sheetHeight);
    }

    inline void SpriteAttrib::setFrame(const unsigned fr) { _frame = fr; }
    inline int SpriteAttrib::getFrame(void) const { return _frame; }



}

#endif

#ifndef ELYSIAN_SPRITE_ATTRIB_BASE_HPP
#define ELYSIAN_SPRITE_ATTRIB_BASE_HPP

#include <AssetIO/primitive/elysian_prim_attrib.hpp>
#include <AssetIO/crtp/elysian_lua_table_proxy.hpp>
#include <AssetIO/crtp/elysian_color.hpp>
#include <AssetIO/crtp/elysian_blend_mode.hpp>

ELYSIAN_ASSET_TYPEDEF(Renderable)
ELYSIAN_ASSET_TYPEDEF(SpriteAttrib)

namespace elysian {

    class Material;

    class SpriteAttribBase:
            public PrimAttrib,
            public LuaTableProxy<SpriteAttribBase, SpriteAttrib>
    {
        friend SpriteAttrib;
        friend Renderable;
        friend class RenderableBase;
	protected:
        GYTexCoord2	_texCoords[4];
        BlendMode   _blendMode      = BlendMode(GY_BLEND_SRC_ALPHA, GY_BLEND_ONE_MINUS_SRC_ALPHA); //"additive" by default
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
            ELYSIAN_LUA_PROXY_FIELD(texture,        getTexPath,         setTexPath)
            ELYSIAN_LUA_PROXY_FIELD(normalMap,      getNormalMapPath,   setNormalMapPath)
            ELYSIAN_LUA_PROXY_FIELD(specularMap,    getSpecularMapPath, setSpecularMapPath)
            ELYSIAN_LUA_PROXY_FIELD(material,       getMaterial,        setMaterial)
            ELYSIAN_LUA_PROXY_FIELD(color,          getColor4,          setColor4)
            ELYSIAN_LUA_PROXY_FIELD(blendMode,      getBlendMode,       setBlendMode)
            ELYSIAN_LUA_PROXY_FIELD(frame,          getFrame,           setFrame)
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

        virtual ~SpriteAttribBase(void) override;

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

        const BlendMode&    getBlendMode(void) const;
        BlendMode&          getBlendMode(void);
        void                setBlendMode(const BlendMode& mode);

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

    inline void SpriteAttribBase::setNormalMapPath(const char * const path) {
        if(_normalMap) gyVidTexDestroy(_normalMap);
        if(strcmp(path, "") != 0) _normalMap = gyVidTexCreate(path);
    }
    inline void SpriteAttribBase::setSpecularMapPath(const char * const path) {
        if(_specularMap) gyVidTexDestroy(_specularMap);
        if(strcmp(path, "") != 0) _specularMap = gyVidTexCreate(path);
    }

    inline void SpriteAttribBase::setMaterial(Material*const material) { _material = material; }
    inline Material* SpriteAttribBase::getMaterial(void) const { return _material;}

    inline const char* SpriteAttribBase::getNormalMapPath(void) const {
        return _normalMap ? _normalMap->filename : "";
    }

    inline const char* SpriteAttribBase::getSpecularMapPath(void) const {
        return _specularMap ? _specularMap->filename : "";
    }

    inline void SpriteAttribBase::submit(void) {

        submitHeader();
        gyVidRect(_frame);
        submitFinish();

    }

    inline GYTexture* SpriteAttribBase::getSpecularMap(void) const { return _specularMap; }
    inline GYTexture* SpriteAttribBase::getNormalMap(void) const { return _normalMap; }
    inline GYTexture* SpriteAttribBase::getTexture(void) const { return _tex; }
    inline const BlendMode& SpriteAttribBase::getBlendMode(void) const { return _blendMode; }
    inline BlendMode& SpriteAttribBase::getBlendMode(void) { return _blendMode; }
    inline void SpriteAttribBase::setBlendMode(const BlendMode& mode) { _blendMode = mode; }

    inline SpriteAttribBase::~SpriteAttribBase(void) {
        gyVidTexDestroy(_tex);
        gyVidTexDestroy(_normalMap);
        gyVidTexDestroy(_specularMap);
    }

    inline void SpriteAttribBase::submitHeader(void) {
        if(_color.a == 255) gyVidAlphaLevel(GY_ALPHA_NONE);
        else gyVidAlphaLevel(GY_ALPHA_FULL);
        gyVidBlendMode(_blendMode.getSrcFactor(), _blendMode.getDstFactor());
        //gyVidMaterialBind(nullptr);
        gyVidTexBind(_tex);
        gyVidQuadListBegin(&_color);
        if(_frame == -1) gyVidTexCoordArray(_texCoords);

    }

    inline void SpriteAttribBase::submitFinish(void) {
        gyVidQuadListEnd();
    }


    inline void SpriteAttribBase::submitVerts(GYVector4* verts) {
        (void)verts;

        gyVidRect(_frame);

    }

    inline void SpriteAttribBase::setTexCoord(const unsigned index, const float u, const float v) {
        _texCoords[index].u = u; _texCoords[index].v = v;
        _frame = -1;
    }

    inline void SpriteAttribBase::getTexCoord(const unsigned index, float &u, float &v) {
        u = _texCoords[index].u; v = _texCoords[index].v;
    }

    inline Color4 SpriteAttribBase::getColor4(void) { return Color4(&_color); }
    inline void SpriteAttribBase::setColor4(const Color4& color) { _color = color; }

    inline void SpriteAttribBase::setColor(const unsigned char r, const unsigned char g, const unsigned char b, const unsigned char a) { _color.r = r; _color.g = g; _color.b = b; _color.a = a; }
    inline void SpriteAttribBase::setColor(const GYColor4& color) { _color = color; }

    inline void SpriteAttribBase::getColor(unsigned char& r, unsigned char& g, unsigned char& b, unsigned char& a) const { r = _color.r; g = _color.g; b = _color.b; a = _color.a; }
    inline void SpriteAttribBase::getColor(GYColor4& color) const { color = _color; }
    inline GYColor4 SpriteAttribBase::getColor(void) const { return _color; }

    inline void SpriteAttribBase::setAlpha(const unsigned char a) { _color.a = a; }
    inline unsigned char SpriteAttribBase::getAlpha(void) const { return _color.a; }

    inline PrimAttrib::TYPE SpriteAttribBase::getType(void) const { return PrimAttrib::SPRITE; }

    //Gonna shit their pants if there is no texture allocated yet...
    inline void SpriteAttribBase::getSheetSize(unsigned& sheetWidth, unsigned& sheetHeight) const { sheetWidth = _tex->_coord->sheetWidth; sheetHeight = _tex->_coord->sheetHeight; }
    inline void SpriteAttribBase::getSpriteSize(unsigned& spriteWidth, unsigned& spriteHeight) const { spriteWidth = _tex->_coord->spriteWidth; spriteHeight = _tex->_coord->spriteHeight; }
    inline const char* SpriteAttribBase::getTexPath(void) const { return _tex? _tex->filename : ""; }
    inline void SpriteAttribBase::setTexPath(const char*const path) {
        gyVidTexDestroy(_tex);
        if(strcmp(path, "") != 0) _tex = gyVidTexCreate(path);

    }

    inline void SpriteAttribBase::loadTexture(const char* filename, const unsigned sheetWidth, const unsigned sheetHeight, const unsigned spriteWidth, const unsigned spriteHeight) {
        if(_tex) {
            gyVidTexDestroy(_tex);
        }
        _tex = gyVidTexCreateSheetUniform(filename, sheetWidth, sheetHeight, spriteWidth, spriteHeight);
    }

    inline void SpriteAttribBase::loadSheet(const char* filename, const unsigned sheetWidth, const unsigned sheetHeight) {
        if(_tex) {
            gyVidTexDestroy(_tex);
        }
        _tex = gyVidTexCreateSheet(filename, sheetWidth, sheetHeight);
    }

    inline void SpriteAttribBase::setFrame(const unsigned fr) { _frame = fr; }
    inline int SpriteAttribBase::getFrame(void) const { return _frame; }



}

#endif

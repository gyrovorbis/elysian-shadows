#ifndef ELYSIAN_SPRITE_ATTRIB_HPP
#define ELYSIAN_SPRITE_ATTRIB_HPP

#include "elysian_prim_attrib.hpp"
#include "elysian_lua_table_proxy.hpp"
#include "elysian_color.hpp"
#include <gyro_video_api.h>

ELYSIAN_ASSET_TYPEDEF(Renderable)

namespace elysian {

    class SpriteAttrib:
            public PrimAttrib,
            public LuaTableProxy<SpriteAttrib>
    {
        friend Renderable;
        friend class RenderableBase;
	protected:
        GYTexCoord2	_texCoords[4];
		GYTexture*	_tex			= nullptr;
		unsigned	_spriteWidth	= 0;
		unsigned	_spriteHeight	= 0;
		int			_frame			= 0;
		GYColor4	_color			= {255, 255, 255, 255};
	public:

        ELYSIAN_LUA_PROXY_FIELDS_BEGIN()
            ELYSIAN_LUA_PROXY_FIELD(frame, getFrame, setFrame)
            ELYSIAN_LUA_PROXY_FIELD(texture, getTexPath)
            ELYSIAN_LUA_PROXY_FIELD(color, getColor4)
        ELYSIAN_LUA_PROXY_FIELDS_END()

		//Not super efficient, but since each rect needs its own color, header must be resubmitted...
		virtual void submit(void) override {

			submitHeader();
			gyVidRect(_frame);
			submitFinish();

		}

		inline GYTexture* getTexture(void) const { return _tex; }

        inline unsigned char getColorR(void) const { return _color.r; }
        inline unsigned char getColorG(void) const { return _color.g; }
        inline unsigned char getColorB(void) const { return _color.b; }
        inline unsigned char getColorA(void) const { return _color.a; }
        inline void setColorR(const unsigned char r) { _color.r = r; }
        inline void setColorG(const unsigned char g) { _color.g = g; }
        inline void setColorB(const unsigned char b) { _color.b = b; }
        inline void setColorA(const unsigned char a) { _color.a = a; }

		virtual ~SpriteAttrib(void) {

			gyVidTexDestroy(_tex);

		}

		inline void submitHeader(void) {
			if(_color.a == 255) gyVidAlphaLevel(GY_ALPHA_NONE);
			else gyVidAlphaLevel(GY_ALPHA_FULL);

			//gyVidMaterialBind(nullptr);
			gyVidTexBind(_tex);
			gyVidQuadListBegin(&_color);
			if(_frame == -1) gyVidTexCoordArray(_texCoords);

        }

		inline void submitFinish(void) {
			gyVidQuadListEnd();
		}


		virtual void submitVerts(GYVector4* verts = nullptr) {
			(void)verts;

			gyVidRect(_frame);

		}

        inline void setTexCoord(const unsigned index, const float u, const float v) {
			_texCoords[index].u = u; _texCoords[index].v = v;
			_frame = -1;
		}

		inline void getTexCoord(const unsigned index, float &u, float &v) {
			u = _texCoords[index].u; v = _texCoords[index].v;
        }

        Color4 getColor4(void) { return Color4(&_color); }

		void setColor(const unsigned char r, const unsigned char g, const unsigned char b, const unsigned char a) { _color.r = r; _color.g = g; _color.b = b; _color.a = a; }
		void setColor(const GYColor4& color) { _color = color; }

		void getColor(unsigned char& r, unsigned char& g, unsigned char& b, unsigned char& a) const { r = _color.r; g = _color.g; b = _color.b; a = _color.a; }
		void getColor(GYColor4& color) const { color = _color; }
		GYColor4 getColor(void) const { return _color; }

		void setAlpha(const unsigned char a) { _color.a = a; }
		unsigned char getAlpha(void) const { return _color.a; }

		PrimAttrib::TYPE getType(void) const { return PrimAttrib::SPRITE; }

		//Gonna shit their pants if there is no texture allocated yet...
		void getSheetSize(unsigned& sheetWidth, unsigned& sheetHeight) const { sheetWidth = _tex->_coord->sheetWidth; sheetHeight = _tex->_coord->sheetHeight; }
		void getSpriteSize(unsigned& spriteWidth, unsigned& spriteHeight) const { spriteWidth = _tex->_coord->spriteWidth; spriteHeight = _tex->_coord->spriteHeight; }
        const char* getTexPath(void) const { return _tex? _tex->filename : ""; }

		void loadTexture(const char* filename, const unsigned sheetWidth, const unsigned sheetHeight, const unsigned spriteWidth, const unsigned spriteHeight) {
			if(_tex) {
				gyVidTexDestroy(_tex);
			}
			_tex = gyVidTexCreateSheetUniform(filename, sheetWidth, sheetHeight, spriteWidth, spriteHeight);
		}

		void loadSheet(const char* filename, const unsigned sheetWidth, const unsigned sheetHeight) {
			if(_tex) {
				gyVidTexDestroy(_tex);
			}
			_tex = gyVidTexCreateSheet(filename, sheetWidth, sheetHeight);
		}

		void setFrame(const unsigned fr) { _frame = fr; }
		int getFrame(void) const { return _frame; }

	};

}

#endif

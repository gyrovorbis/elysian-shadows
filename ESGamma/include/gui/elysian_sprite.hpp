#ifndef ELYSIAN_SPRITE_HPP
#define ELYSIAN_SPRITE_HPP

#include <AssetIO/primitive/elysian_renderable_object.hpp>
#include <AssetIO/crtp/elysian_sprite_attrib.hpp>
#include <libGyro/gyro_video_api.h>

namespace elysian {
#if 0
	//Basically an abstraction of GYSprite/Quad attributes
	class SpriteAttrib { //Essentially prefab-able data for a Renderable
		friend class Renderable;
		friend class Sprite;
	protected:
		GYTexture *_tex; //already shared at the libGyro level!
		GYColor4 _color;
		unsigned _frame;
	public:
		inline SpriteAttrib(void): _tex(NULL), _frame(0) { 
			_color.a = _color.r = _color.g = _color.b = 255; 
			//_tex = gyTexDefault();
		}

		inline ~SpriteAttrib(void) { gyVidTexDestroy(_tex); }

		inline void loadTexture(const char *const fn, const unsigned shw, const unsigned shh, const unsigned sprw, const unsigned sprh) {
			gyVidTexDestroy(_tex);
			_tex = gyVidTexCreateSheetUniform(fn, shw, shh, sprw, sprh);
		}

		inline void loadTexture(const char *const fn, const unsigned shw, const unsigned shh) {
			gyVidTexDestroy(_tex);
			_tex = gyVidTexCreateSheet(fn, shw, shh);
		}

		inline void setColor(const unsigned char r, const unsigned char g, const unsigned char b, const unsigned char a) { _color.r = r; _color.g = g; _color.b = b; _color.a = a; }
		inline void setColor(const GYColor4& color) { _color = color; }
		inline void getColor(unsigned char& r, unsigned char& g, unsigned char& b, unsigned char& a) const { r = _color.r; g = _color.g; b = _color.b; a = _color.a; }
		inline void getColor(GYColor4& color) const { color = _color; }
		inline void setAlpha(const unsigned char a) { _color.a = a; }

		inline void getSheetSize(unsigned& sheetWidth, unsigned& sheetHeight) const { sheetWidth = _tex->_coord->sheetWidth; sheetHeight = _tex->_coord->sheetHeight; }
		inline void getSpriteSize(const unsigned frame, unsigned& spriteWidth, unsigned& spriteHeight) const { spriteWidth = _tex->_coord->texCoord[frame].spriteWidth; spriteHeight = _tex->_coord->texCoord[frame].spriteHeight; }
		inline const char* getTexPath(void) const { return _tex->filename; }
		inline unsigned getFrameCount(void) const { return _tex->_coord->frames; }

		inline void setFrame(const unsigned fr) { _frame = fr; }
		inline unsigned getFrame(void) const { return _frame; }

		//Load(); Save();
	};
#endif

	class Sprite: public RenderableObject, public SpriteAttrib {
	public:

		void render(void) {
			gyMatPush();
            _transform.apply();

			submit();

			gyMatPop();
		}

	};

}

#endif

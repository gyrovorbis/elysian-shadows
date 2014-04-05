#ifndef ELYSIAN_SPRITE_ATTRIB_HPP
#define ELYSIAN_SPRITE_ATTRIB_HPP

#include "elysian_prim_attrib.hpp"
#include <gyro.h>

namespace elysian {

	class SpriteAttrib: public PrimAttrib {
		friend class Renderable;
	protected:
		GYTexCoord2	_texCoords[4];
		GYTexture*	_tex			= nullptr;
		unsigned	_spriteWidth	= 0;
		unsigned	_spriteHeight	= 0;
		int			_frame			= 0;
		GYColor4	_color			= {255, 255, 255, 255};
	public:
		//Not super efficient, but since each rect needs its own color, header must be resubmitted...
		virtual void submit(void) override {
	#ifdef ELYSIAN_ENGINE
			submitHeader();
			gyVidRect(_frame);
			submitFinish();
	#endif
		}

		inline GYTexture* getTexture(void) const { return _tex; }

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
		const char* getTexPath(void) const { return _tex->filename; }

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

		virtual bool load(FILE* /*file*/) override {
			return true;
		}

		virtual bool save(FILE* /*file*/) override {
			return true;
		}
	};

}

#endif

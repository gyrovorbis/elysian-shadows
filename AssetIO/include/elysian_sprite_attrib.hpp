#ifndef ELYSIAN_SPRITE_ATTRIB_HPP
#define ELYSIAN_SPRITE_ATTRIB_HPP

#include "elysian_prim_attrib.hpp"
#include <gyro.h>

namespace elysian {

	class SpriteAttrib: public PrimAttrib {
		friend class Renderable;
	protected:
		GYTexture* _tex			= nullptr;
		unsigned _spriteWidth	= 0;
		unsigned _spriteHeight	= 0;
		unsigned _frame			= 0;
		GYColor4 _color			= {255, 255, 255, 255};
	public:
		//Not super efficient, but since each rect needs its own color, header must be resubmitted...
		virtual void submit(void) override {
	#ifdef ELYSIAN_ENGINE
			submitVerts();
	#endif
		}

		virtual void submitVerts(GYVector4* verts = nullptr) {
			if(_color.a == 255) gyVidAlphaLevel(GY_ALPHA_NONE);
			else gyVidAlphaLevel(GY_ALPHA_FULL);

			gyVidTexBind(_tex);
			gyVidQuadListBegin(&_color);
			gyVidRect(_frame);
			gyVidQuadListEnd();
		}

		void setColor(const unsigned char r, const unsigned char g, const unsigned char b, const unsigned char a) { _color.r = r; _color.g = g; _color.b = b; _color.a = a; }
		void setColor(const GYColor4& color) { _color = color; }

		void getColor(unsigned char& r, unsigned char& g, unsigned char& b, unsigned char& a) const { r = _color.r; g = _color.g; b = _color.b; a = _color.a; }
		void getColor(GYColor4& color) const { color = _color; }

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
		unsigned getFrame(void) const { return _frame; }

		bool load(std::ifstream& stream) {
			return true;
		}

		bool save(std::ofstream& stream) {
			return true;
		}
	};





#if 0
	class SpriteAttrib: public virtual RectAttrib, public TexturedAttrib {
		friend class Renderable;
	protected:
		unsigned _frame;

	public:
		inline SpriteAttrib(void): _frame(0) {}
		inline void setFrame(const unsigned fr) { _frame = fr; }
		inline unsigned getFrame(void) const { return _frame; }

		virtual void submit(void) override { //Resubmits header every time...
#ifdef ELYSIAN_ENGINE
			submitVerts();
#endif
		}

		virtual void submitVerts(GYVector4 *verts = nullptr) override {
			if(!_tex) return RectAttrib::submit();
			if(_color.a == 255) gyVidAlphaLevel(GY_ALPHA_PUNCHTHRU);
			else gyVidAlphaLevel(GY_ALPHA_FULL);
			gyVidTexBind(_tex);
			gyVidQuadListBegin(&_color);

			if(verts) gyVidRect(_frame);
			else gyVidQuad(verts, _frame);

			gyVidQuadListEnd();
		}

		PrimAttrib::TYPE getType(void) const { return PrimAttrib::SPRITE; }

		bool load(std::ifstream& stream) {
			bool success = true;
			dbgprintf(LOG_VERBOSE, "SpritePrim::Load(ifstream&)");
			dbgPush();
			success &= RectAttrib::load(stream);
			success &= TexturedAttrib::load(stream);
			dbgPop();
			return success;
		}

		bool save(std::ofstream& stream) {
			bool success = true;
			dbgprintf(LOG_VERBOSE, "SpritePrim::Save(ofstream&)");
			dbgPush();
			success &= RectAttrib::save(stream);
			success &= TexturedAttrib::save(stream);
			dbgPop();
			return success;
		}
	};


#endif

}

#endif

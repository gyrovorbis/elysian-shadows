#ifndef ELYSIAN_SPRITE_ATTRIB_HPP
#define ELYSIAN_SPRITE_ATTRIB_HPP

#include "elysian_rect_attrib.hpp"
#include "elysian_textured_attrib.hpp"

namespace elysian {

	class SpriteAttrib: public RectAttrib, public TexturedAttrib {
		friend class Renderable;
	protected:
		unsigned _frame;

	public:
		inline SpriteAttrib(void): _frame(0) {}
		inline void setFrame(const unsigned fr) { _frame = fr; }
		inline unsigned getFrame(void) const { return _frame; }
		void Submit(void) { //Resubmits header every time...
#ifdef ELYSIAN_ENGINE
			if(!_tex) return RectAttrib::submit();
            if(_color.a == 255) gyVidAlphaLevel(GY_ALPHA_PUNCHTHRU);
            else gyVidAlphaLevel(GY_ALPHA_FULL);
			gyVidTexBind(_tex);
			gyVidQuadListBegin(&_color);
			
			gyVidRect(_frame);
			
			gyVidQuadListEnd();
#endif
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

}

#endif

#ifndef ELYSIAN_QUAD_SPRITE_ATTRIB_HPP
#define ELYSIAN_QUAD_SPRITE_ATTRIB_HPP

#include "elysian_quad_attrib.hpp"
#include "elysian_textured_attrib.hpp"

namespace elysian {

	class QuadSpriteAttrib: public QuadAttrib, public TexturedAttrib {
	public:

		void Submit(void) {
#ifdef ELYSIAN_ENGINE
			//Requires libGyro work
#endif
		}

		PrimAttrib::TYPE getType(void) const { return PrimAttrib::QUAD_SPRITE; }

        bool load(std::ifstream& /*stream*/) {
			dbgprintf(LOG_VERBOSE, "QuadSpritePrim::Load(ifstream&)");
			dbgPush();
			dbgprintf(LOG_CRITICAL, "Not implemented.");
			dbgPop();
			return false;
		}

        bool save(std::ofstream& /*stream*/) {
			dbgprintf(LOG_VERBOSE, "QuadSpritePrim::Save(ofstream&)");
			dbgPush();
			dbgprintf(LOG_CRITICAL, "Not implemented.");
			dbgPop();
			return false;
		}
	};
}

#endif

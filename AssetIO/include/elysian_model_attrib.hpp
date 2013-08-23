#ifndef ELYSIAN_MODEL_ATTRIB_HPP
#define ELYSIAN_MODEL_ATTRIB_HPP

#include "elysian_prim_attrib.hpp"

namespace elysian {

	class ModelAttrib: public PrimAttrib {

		PrimAttrib::TYPE getType(void) const { return PrimAttrib::MODEL; }

		void submit(void) {
#ifdef ELYSIAN_ENGINE

#endif
		}

        bool load(std::ifstream& /*stream*/) {
			dbgprintf(LOG_VERBOSE, "ModelPrim::Load(ifstream&)");
			dbgPush();
			dbgprintf(LOG_CRITICAL, "Not implemented.");
			dbgPop();
			return false;
		}

        bool save(std::ofstream& /*stream*/) {
			dbgprintf(LOG_VERBOSE, "ModelPrim::Save(ofstream&)");
			dbgPush();
			dbgprintf(LOG_CRITICAL, "Not implemented.");
			dbgPop();
			return false;
		}
	};

}

#endif

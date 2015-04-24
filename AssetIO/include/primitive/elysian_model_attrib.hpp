#ifndef ELYSIAN_MODEL_ATTRIB_HPP
#define ELYSIAN_MODEL_ATTRIB_HPP

#include "elysian_prim_attrib.hpp"

namespace elysian {

	class ModelAttrib: public PrimAttrib {

		PrimAttrib::TYPE getType(void) const { return PrimAttrib::MODEL; }

		void submit(void) {

		}

		bool load(FILE* file) {
			dbgprintf(LOG_VERBOSE, "ModelPrim::Load(ifstream&)");
			dbgPush();
			dbgprintf(LOG_CRITICAL, "Not implemented.");
			dbgPop();
			return false;
		}

		bool save(FILE* file) {
			dbgprintf(LOG_VERBOSE, "ModelPrim::Save(ofstream&)");
			dbgPush();
			dbgprintf(LOG_CRITICAL, "Not implemented.");
			dbgPop();
			return false;
		}
	};

}

#endif

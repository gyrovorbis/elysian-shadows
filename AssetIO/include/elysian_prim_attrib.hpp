#ifndef ELYSIAN_PRIM_ATTRIB_HPP
#define ELYSIAN_PRIM_ATTRIB_HPP

#include <fstream>
#include "elysian_asset.hpp"

namespace elysian {

/*
	Base class for all supported renderable primitives in the AssetIO.
	These can be serialized/deserialized and encapsulated within a
	Renderable component, hence the required Load/Save functionality.
*/
	class PrimAttrib: public Asset {
	public:
		enum TYPE { SPRITE, QUAD_SPRITE, MODEL, NONE };

		virtual TYPE getType(void) const = 0;
		virtual void submit(void) = 0;
		virtual bool load(std::ifstream &stream) = 0;
		virtual bool save(std::ofstream &stream) = 0;
	};

}

#endif

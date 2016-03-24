#ifndef ELYSIAN_MODEL_ATTRIB_HPP
#define ELYSIAN_MODEL_ATTRIB_HPP

#include <elysian_prim_attrib.hpp>

namespace elysian {

    class ModelAttrib: public PrimAttrib {

        PrimAttrib::TYPE getType(void) const { return PrimAttrib::MODEL; }

        void submit(void) {

        }


    };

}

#endif

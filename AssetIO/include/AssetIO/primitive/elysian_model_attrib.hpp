/*
 * Copyright (c) 2013-2026 Elysian Shadows Team
 * SPDX-License-Identifier: MIT
 * Additional permission for Elysian Shadows Kickstarter backers: see LICENSE.
 */

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

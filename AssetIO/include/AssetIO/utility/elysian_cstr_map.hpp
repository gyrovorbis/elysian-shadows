/*
 * Copyright (c) 2013-2026 Elysian Shadows Team
 * SPDX-License-Identifier: MIT
 * Additional permission for Elysian Shadows Kickstarter backers: see LICENSE.
 */

#ifndef ELYSIAN_CSTR_MAP_HPP
#define ELYSIAN_CSTR_MAP_HPP

//Replacement for STL std::map<const char *, T>

namespace elysian {

    struct cstr_map {
        bool operator()(const char* a, const char* b) const {
            return (strcmp(a, b) == 0);
        }
    };

}

#endif

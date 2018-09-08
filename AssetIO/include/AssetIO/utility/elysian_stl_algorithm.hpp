#ifndef ELYSIAN_STL_ALGORITHM_HPP
#define ELYSIAN_STL_ALGORITHM_HPP

#include <string.h>

//GET RID OF ME!

namespace elysian {
	
    //We don't fucking like std::string!
    struct ltstr {
        bool operator()(const char* s1, const char* s2) const {
            return strcmp(s1, s2) < 0;
        }
    };

}

#endif

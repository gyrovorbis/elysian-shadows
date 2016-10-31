#ifndef ELYSIAN_SKELETON_SYSTEM_HPP
#define ELYSIAN_SKELETON_SYSTEM_HPP

#include <system/elysian_system.hpp>

struct spAttachment;

namespace elysian {

    class SkeletonSystem: public System {
    protected:

        static void    _renderEntityAttachment(spAttachment* attachment);
    public:
                        SkeletonSystem(void);
        void            update(void);


    };

}

#endif // ELYSIAN_SKELETON_SYSTEM_HPP

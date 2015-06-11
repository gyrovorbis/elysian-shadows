#ifndef ELYSIAN_SKELETON_HPP
#define ELYSIAN_SKELETON_HPP


#ifdef ELYSIAN_ENGINE
#   include <elysian_skeleton_base.hpp>
namespace elysian {
    typedef SkeletonBase Skeleton;
}
#else
#   include <tk_skeleton.h>
namespace elysian {
    typedef TkSkeleton Skeleton;
}
typedef TkSkeleton Skeleton;

#endif

#endif // ELYSIAN_SKELETON_HPP


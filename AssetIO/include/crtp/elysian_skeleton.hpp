#ifndef ELYSIAN_SKELETON_HPP
#define ELYSIAN_SKELETON_HPP


#ifdef ELYSIAN_ENGINE
#   include <elysian_skeleton_base.hpp>
namespace elysian {
    typedef SkeletonBase Skeleton;
}
#else
#   include <tkskeleton.h>
namespace elysian {
    typedef TkSkeleton Skeleton;
}
typedef TkSkeleton Skeleton;

#endif

#endif // ELYSIAN_SKELETON_HPP


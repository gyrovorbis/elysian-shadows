#ifndef ELYSIAN_SKELETON_HPP
#define ELYSIAN_SKELETON_HPP


#ifdef ELYSIAN_ENGINE
#   include <component/elysian_skeleton_base.hpp>
namespace elysian {
    typedef SkeletonBase        Skeleton;
    typedef SkeletonAttribBase  SkeletonAttrib;
    typedef AnimationTrackBase  AnimationTrack;
}
#else
#   include <tk_skeleton.h>
namespace elysian {
    typedef TkSkeleton          Skeleton;
    typedef TkSkeletonAttrib    SkeletonAttrib;
    typedef TkAnimationTrack    AnimationTrack;
}
typedef TkSkeleton              Skeleton;
typedef TkSkeletonAttrib        SkeletonAttrib;
typedef TkAnimationTrack        AnimationTrack;

#endif

#endif // ELYSIAN_SKELETON_HPP


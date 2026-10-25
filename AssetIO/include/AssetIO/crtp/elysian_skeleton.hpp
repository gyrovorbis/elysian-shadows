/*
 * Copyright (c) 2013-2026 Elysian Shadows Team
 * SPDX-License-Identifier: MIT
 * Additional permission for Elysian Shadows Kickstarter backers: see LICENSE.
 */

#ifndef ELYSIAN_SKELETON_HPP
#define ELYSIAN_SKELETON_HPP


#ifdef ELYSIAN_ENGINE
#   include <component/elysian_skeleton_base.hpp>
namespace elysian {
    typedef SkeletonBase        Skeleton;
    typedef SkeletonAttribBase  SkeletonAttrib;
    typedef AnimationBase       Animation;
    typedef AnimationTrackBase  AnimationTrack;
}
#else
#   include <ESToolkit/AssetIO/tk_skeleton.h>
namespace elysian {
    typedef TkSkeleton          Skeleton;
    typedef TkSkeletonAttrib    SkeletonAttrib;
    typedef TkAnimation         Animation;
    typedef TkAnimationTrack    AnimationTrack;
}
typedef TkSkeleton              Skeleton;
typedef TkSkeletonAttrib        SkeletonAttrib;
typedef TkAnimation             Animation;
typedef TkAnimationTrack        AnimationTrack;

#endif

#endif // ELYSIAN_SKELETON_HPP


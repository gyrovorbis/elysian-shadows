#ifndef TKSKELETON_H
#define TKSKELETON_H

#include <elysian_skeleton_base.hpp>

class TkSkeleton: public elysian::SkeletonBase {
public:
    TkSkeleton(TkEntity& parent);
    TkSkeleton(void) = default;
};


inline TkSkeleton::TkSkeleton(TkEntity &parent): elysian::SkeletonBase(parent) {}

#endif // TKSKELETON_H


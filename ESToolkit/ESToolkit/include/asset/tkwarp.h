#ifndef TKWARP_H
#define TKWARP_H

#include <elysian_warp_base.hpp>

class TkWarp: public elysian::WarpBase {
public:
    TkWarp(TkEntity& parent);
    TkWarp(void) = default;

   // ~TkWarp(void) = default;
};


inline TkWarp::TkWarp(TkEntity &parent): elysian::WarpBase(parent) {}

#endif // TKWARP_H


#ifndef WARP_H
#define WARP_H

#include "component.h"

namespace elysian {
    class Warp;
}

class Warp: public Component {
private:
    elysian::Warp *_elysianWarp;
public:
    Warp(elysian::Warp *const comp);
	elysian::Component::TYPE getType(void) const { return elysian::Component::WARP; }
};

#endif // WARP_H

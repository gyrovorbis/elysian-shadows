#ifndef FRIENDLY_H
#define FRIENDLY_H

#include "component.h"

namespace elysian {
    class Friendly;
}

class Friendly: public Component {
private:
    elysian::Friendly *_elysianFriendly;
public:
    Friendly(elysian::Friendly *const comp);
	elysian::Component::TYPE getType(void) const { return elysian::Component::FRIENDLY; }
};

#endif // FRIENDLY_H

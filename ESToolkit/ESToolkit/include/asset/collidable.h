#ifndef COLLIDABLE_H
#define COLLIDABLE_H

#include "component.h"

namespace elysian {
    class Collidable;
}

class Collidable: public Component {
private:
	//elysian::Collidable *_elysianCollid;
public:
    Collidable(elysian::Collidable *const comp);

	elysian::Component::TYPE getType(void) const { return elysian::Component::COLLIDABLE; }
};

#endif // COLLIDABLE_H

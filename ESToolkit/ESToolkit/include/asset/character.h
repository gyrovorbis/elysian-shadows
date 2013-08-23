#ifndef CHARACTER_H
#define CHARACTER_H

#include "component.h"

namespace elysian {
    class Character;
}

class Character: public Component {
private:
    elysian::Character *_elysianChar;
public:
    Character(elysian::Character *const comp);
		elysian::Component::TYPE getType(void) const { return elysian::Component::CHARACTER; }
};

#endif // CHARACTER_H

#ifndef ANIMATION_H
#define ANIMATION_H

#include "component.h"

namespace elysian {
    class Animation;
}

class Animation: public Component {
private:
	//elysian::Animation *_elysianAnim;
public:
    Animation(elysian::Animation *const anim);
	elysian::Component::TYPE getType(void) const { return elysian::Component::ANIMATION; }
};

#endif // ANIMATION_H

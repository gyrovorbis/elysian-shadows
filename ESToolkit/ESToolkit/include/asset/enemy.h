#ifndef ENEMY_H
#define ENEMY_H

#include "component.h"

namespace elysian {
    class Enemy;
}

class Enemy: public Component {
private:
	//elysian::Enemy *_elysianEnemy;
public:
    Enemy(elysian::Enemy *const comp);
	elysian::Component::TYPE getType(void) const { return elysian::Component::ENEMY; }

};

#endif // ENEMY_H

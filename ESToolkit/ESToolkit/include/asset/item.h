#ifndef ITEM_H
#define ITEM_H

#include "component.h"

namespace elysian {
    class Item;
}

class Item: public Component {
private:
    elysian::Item *_elysianItem;
public:
    Item(elysian::Item *const comp);
	elysian::Component::TYPE getType(void) const { return elysian::Component::ITEM; }
};


#endif // TKITEM_H

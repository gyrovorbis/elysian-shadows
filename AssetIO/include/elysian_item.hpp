#ifndef ELYSIAN_ITEM_HPP
#define ELYSIAN_ITEM_HPP

#include "elysian_component.hpp"

namespace elysian {
	
	class Item: public Component { //tolua_export
            friend class Entity;
	private:
		//static const bool _statConst;
		
	protected:
		Item(Entity& parent);
		Item(void);
		virtual ~Item(void);
		
	public:
		
		bool load(std::ifstream& stream);
		bool save(std::ofstream& stream);
		bool canAddTo(Entity& parent);
		bool addToLevel(void);
		bool removeFromLevel(void);

		Component::TYPE getType(void) const { return Component::ITEM; }
	}; //tolua_export
	
}

#endif

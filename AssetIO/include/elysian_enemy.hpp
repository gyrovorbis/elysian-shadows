#ifndef ELYSIAN_ENEMY_HPP
#define ELYSIAN_ENEMY_HPP

#include "elysian_component.hpp"

namespace elysian {
	
	class Enemy: public Component { //tolua_export
        friend class Entity;
	private:
		
	protected:
		Enemy(Entity& parent): Component(parent) {}
		Enemy(void) {}
		virtual ~Enemy(void) { }
		
	public:
		bool load(std::ifstream& stream);
		bool save(std::ofstream& stream);
		bool canAddTo(Entity& parent);
		bool addToLevel(void);
		bool removeFromLevel(void);
		Component::TYPE getType(void) const { return Component::ENEMY; }
	}; //tolua_export
	
}
#endif

#ifndef ELYSIAN_FRIENDLY_HPP
#define ELYSIAN_FRIENDLY_HPP

#include "elysian_component.hpp"

namespace elysian {
	
	class Friendly: public Component { //tolua_export
        friend class Entity;
	private:
		//static const bool _statConst;
		
	protected:
		Friendly(Entity& parent);
		Friendly(void);
		virtual ~Friendly(void);
		
	public:
		//static bool StaticConstructor();
		//static Component *CreateComponent(Entity &parent);
		
		bool load(std::ifstream& stream);
		bool save(std::ofstream& stream);
		bool canAddTo(Entity& parent);
		bool addToLevel(void);
		bool removeFromLevel(void);
		Component::TYPE getType(void) const { return Component::FRIENDLY; }

	}; //tolua_export
	
}

#endif

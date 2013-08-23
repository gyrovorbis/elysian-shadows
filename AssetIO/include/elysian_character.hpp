#ifndef ELYSIAN_CHARACTER_HPP
#define ELYSIAN_CHARACTER_HPP

#include <fstream>
#include "elysian_component.hpp"
#include "elysian_direction.hpp"
#include "elysian_entity.hpp"

namespace elysian {
	
	class Character: public Component { //tolua_export
        friend class Entity;
	private:
		static const Component::TYPE _type = Component::CHARACTER;
		
	protected:
		Character(Entity& entity);
		virtual ~Character(void) { }
		//walkingSpeed?

		bool _movedThisFrame;
		DIR4 _moveDir;
		//gyro::Vector2::DIRECTION _moveDir; Uh, yeaaaah...
		
	public:
		bool load(std::ifstream& stream);
		bool save(std::ofstream& stream);
		bool canAddTo(Entity& entity) { return (entity.getAnimation() && entity.getAnimation()); }
		bool addToLevel(void);
		bool removeFromLevel(void);
		Component::TYPE getType(void) const { return Component::CHARACTER; }

		//static Component *CreateComponent(Entity &entity);
#ifdef ELYSIAN_ENGINE
		void update(void);
//tolua_begin
		void move(const GYVector2& vec);
		void face(const DIR4 dir);
		void face(Entity& entity);
#endif
		//void Walk();
		//void Pickup();
	}; //tolua_export	
}

#endif

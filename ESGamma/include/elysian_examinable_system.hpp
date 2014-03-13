#ifndef ELYSIAN_EXAMINABLE_SYSTEM_HPP
#define ELYSIAN_EXAMINABLE_SYSTEM_HPP

#include "elysian_system.hpp"

namespace elysian {

	class GameInput;

	class ExaminableSystem: public System {
	private:


	public:

		void processInput(GameInput& input);


	};

}

#endif // ELYSIAN_EXAMINABLE_SYSTEM_HPP

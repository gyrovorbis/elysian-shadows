#ifndef ELYSIAN_INPUT_SYSTEM_HPP
#define ELYSIAN_INPUT_SYSTEM_HPP


#ifdef ELYSIAN_ENGINE
#   include <system/elysian_input_system_base.hpp>
namespace elysian {
    using InputSystem = InputSystemBase;
}
#else

#   include <estk/tk_input_system.h>

namespace elysian {
    using InputSystem = TkInputSystem;
}
using InputSystem = TkInputSystem;

#endif




#endif // ELYSIAN_INPUT_SYSTEM_HPP


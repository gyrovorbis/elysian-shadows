#ifndef ELYSIAN_CHARACTER_HPP
#define ELYSIAN_CHARACTER_HPP

#ifdef ELYSIAN_ENGINE
#   include <elysian_character_base.hpp>
namespace elysian {
    typedef CharacterBase Character;
}
#else
#   include <tkCharacter.h>
namespace elysian {
    typedef TkCharacter Character;
}
typedef TkCharacter Character;

#endif


#endif // ELYSIAN_CHARACTER_HPP


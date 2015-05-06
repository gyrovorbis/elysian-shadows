#ifndef TKCHARACTER_H
#define TKCHARACTER_H

#include <elysian_character_base.hpp>

class TkCharacter: public elysian::CharacterBase {
public:
    TkCharacter(TkEntity& parent);
    TkCharacter(void) = default;
    //virtual ~TkCharacter(void) = default;

};

inline TkCharacter::TkCharacter(TkEntity& parent): elysian::CharacterBase(parent) {}

#endif // TKCHARACTER_H


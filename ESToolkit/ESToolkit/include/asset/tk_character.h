#ifndef TKCHARACTER_H
#define TKCHARACTER_H

#include <component/elysian_character_base.hpp>

class TkCharacter: public elysian::CharacterBase {
public:
    TkCharacter(TkEntity& parent);
    TkCharacter(void) = default;

    virtual bool validate(QString &msg) const override;
    //virtual ~TkCharacter(void) = default;

};

inline TkCharacter::TkCharacter(TkEntity& parent): elysian::CharacterBase(parent) {}

inline bool TkCharacter::validate(QString& /*msg*/) const { return true; }

#endif // TKCHARACTER_H


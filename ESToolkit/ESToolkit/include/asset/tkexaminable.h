#ifndef TKEXAMINABLE_H
#define TKEXAMINABLE_H

#include <elysian_examinable_base.hpp>

class TkExaminable: public elysian::ExaminableBase {
public:
    TkExaminable(TkEntity& parent);
    TkExaminable(void) = default;
};



inline TkExaminable::TkExaminable(TkEntity &parent): elysian::ExaminableBase(parent) {}

#endif // TKEXAMINABLE_H


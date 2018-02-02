#ifndef TKEXAMINABLE_H
#define TKEXAMINABLE_H

#include <component/elysian_examinable_base.hpp>

class TkExaminable: public elysian::ExaminableBase {
public:
    TkExaminable(TkEntity& parent, TkExaminable*const prefab=nullptr);
    TkExaminable(void) = default;


    static void initializeDelegates(void);
};



inline TkExaminable::TkExaminable(TkEntity &parent, TkExaminable*const prefab): elysian::ExaminableBase(parent, prefab) {}

#endif // TKEXAMINABLE_H


#ifndef TKCOMBAT_H
#define TKCOMBAT_H

#include <component/elysian_combat_base.hpp>

class TkCombat: public elysian::CombatBase {
public:
                    TkCombat(TkEntity& parent);
                    TkCombat(void) = default;

    virtual bool    validate(QString &) const override;
    virtual QMenu*  createContextMenu(void) override;

    static void     initializeDelegates(void);
};

inline TkCombat::TkCombat(TkEntity &parent): elysian::CombatBase(parent) {}

#endif // TKCOMBAT_H


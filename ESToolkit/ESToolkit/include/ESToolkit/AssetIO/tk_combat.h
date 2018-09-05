#ifndef TKCOMBAT_H
#define TKCOMBAT_H

#include <AssetIO/component/elysian_combat_base.hpp>

class TkCombat: public elysian::CombatBase {
public:
                    TkCombat(void) = default;
                    TkCombat(TkEntity& parent, TkCombat*const prefab=nullptr);

    virtual bool    validate(QString &) const override;
    virtual QMenu*  createContextMenu(void) override;

    static void     initializeDelegates(void);
};

inline TkCombat::TkCombat(TkEntity &parent, TkCombat*const prefab): elysian::CombatBase(parent, prefab) {}

#endif // TKCOMBAT_H


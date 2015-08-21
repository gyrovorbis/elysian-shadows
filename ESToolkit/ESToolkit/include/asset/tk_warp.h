#ifndef TKWARP_H
#define TKWARP_H

#include <component/elysian_warp_base.hpp>

class TkWarp: public elysian::WarpBase {
public:
            TkWarp(TkEntity& parent);
            TkWarp(void) = default;

    virtual bool    canAddTo(const Entity &entity) const override;
    virtual QMenu*  createContextMenu(void) override;

    static void     initializeDelegates(void);
};


#endif // TKWARP_H


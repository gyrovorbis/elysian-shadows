/*
 * Copyright (c) 2013-2026 Elysian Shadows Team
 * SPDX-License-Identifier: MIT
 * Additional permission for Elysian Shadows Kickstarter backers: see LICENSE.
 */

#ifndef TKWARP_H
#define TKWARP_H

#include <AssetIO/component/elysian_warp_base.hpp>

class TkWarp: public elysian::WarpBase {
public:
            TkWarp(TkEntity& parent, const TkWarp*const prefab=nullptr);
            TkWarp(void) = default;

    virtual bool    canAddTo(const Entity &entity) const override;
    virtual QMenu*  createContextMenu(void) override;

    static void     initializeDelegates(void);
};



 //==== INLINEZ =====
inline TkWarp::TkWarp(TkEntity &parent, const TkWarp*const prefab): elysian::WarpBase(parent, prefab) {}


#endif // TKWARP_H


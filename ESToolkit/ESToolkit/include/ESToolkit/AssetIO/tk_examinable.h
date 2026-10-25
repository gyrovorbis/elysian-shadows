/*
 * Copyright (c) 2013-2026 Elysian Shadows Team
 * SPDX-License-Identifier: MIT
 * Additional permission for Elysian Shadows Kickstarter backers: see LICENSE.
 */

#ifndef TKEXAMINABLE_H
#define TKEXAMINABLE_H

#include <AssetIO/component/elysian_examinable_base.hpp>

class TkExaminable: public elysian::ExaminableBase {
public:
    TkExaminable(TkEntity& parent, TkExaminable*const prefab=nullptr);
    TkExaminable(void) = default;


    static void initializeDelegates(void);
};



inline TkExaminable::TkExaminable(TkEntity &parent, TkExaminable*const prefab): elysian::ExaminableBase(parent, prefab) {}

#endif // TKEXAMINABLE_H


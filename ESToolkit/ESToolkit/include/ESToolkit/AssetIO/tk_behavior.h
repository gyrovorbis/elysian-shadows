/*
 * Copyright (c) 2013-2026 Elysian Shadows Team
 * SPDX-License-Identifier: MIT
 * Additional permission for Elysian Shadows Kickstarter backers: see LICENSE.
 */

#ifndef TKBEHAVIOR_H
#define TKBEHAVIOR_H

#include <AssetIO/component/elysian_behavior_base.hpp>
#include <QString>

class TkBehavior: public elysian::BehaviorBase {
public:
    TkBehavior(Entity& entity);

    bool isLuaBehavior(void) const;

};






// ====== INLINEZ ======

inline TkBehavior::TkBehavior(Entity& entity): elysian::BehaviorBase(entity) {}


inline bool TkBehavior::isLuaBehavior(void) const {
    QString name = getBehavName();
    return name.contains(".lua");
}

#endif // TKBEHAVIOR_H


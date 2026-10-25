/*
 * Copyright (c) 2013-2026 Elysian Shadows Team
 * SPDX-License-Identifier: MIT
 * Additional permission for Elysian Shadows Kickstarter backers: see LICENSE.
 */

#ifndef TKCHARACTER_H
#define TKCHARACTER_H

#include <AssetIO/component/elysian_character_base.hpp>

class TkCharacter: public elysian::CharacterBase {
public:
                    TkCharacter(TkEntity& parent, const TkCharacter*const prefab=nullptr);
                    TkCharacter(void) = default;

    virtual bool    validate(QString &msg) const override;
    virtual bool    canAddTo(const Entity &) const override;
    virtual bool    canRemoveFrom(const Entity&) const override;
    virtual QMenu*  createContextMenu(void) override;

};

inline TkCharacter::TkCharacter(TkEntity& parent, const TkCharacter*const prefab): elysian::CharacterBase(parent, prefab) {}

inline bool TkCharacter::validate(QString& /*msg*/) const { return true; }

#endif // TKCHARACTER_H


/*
 * Copyright (c) 2013-2026 Elysian Shadows Team
 * SPDX-License-Identifier: MIT
 * Additional permission for Elysian Shadows Kickstarter backers: see LICENSE.
 */

#ifndef ELYSIAN_PLAYABLE_PANE_HPP
#define ELYSIAN_PLAYABLE_PANE_HPP

#include <ESGamma/gui/elysian_entity_widget.hpp>
#include <AssetIO/utility/elysian_asset_typedefs.hpp>
#include <ESGamma/crtp/elysian_lua_manager.hpp>

#define ELYSIAN_PLAYABLE_PANE_COMBAT_BEHAV      "/assets/scripts/combat/_player_combat_behav.lua"
#define ELYSIAN_PLAYABLE_PANE_PORTRAIT_PREFAB   "menu_partyMemberPane_portrait"
#define ELYSIAN_PLAYABLE_PANE_HP_BG_PREFAB      "menu_partyMemberPane_hpBg"
#define ELYSIAN_PLAYABLE_PANE_HP_FG_PREFAB      "menu_partyMemberPane_hpFg"
#define ELYSIAN_PLAYABLE_PANE_MP_BG_PREFAB      "menu_partyMemberPane_mpBg"
#define ELYSIAN_PLAYABLE_PANE_MP_FG_PREFAB      "menu_partyMemberPane_mpFg"
#define ELYSIAN_PLAYABLE_PANE_ECHO_ICON_PREFAB  "menu_partyMemberPane_echoIcon"
#define ELYSIAN_PLAYABLE_PANE_INDENT_SIZE       48


ELYSIAN_ASSET_TYPEDEF(Entity)

namespace elysian {
    class LabelWidget;
    class HorizontalLayout;
    class VerticalLayout;
}

namespace ElysianShadows {

    using namespace elysian;

    class HpMpGaugeWidget;

    class PlayablePane: public elysian::EntityWidget, public elysian::LuaManager {
    private:
        Entity*             _playable         = nullptr;
        LuaVariant          _combatTable;

        VerticalLayout*     _vLayoutLeft;
        VerticalLayout*     _vLayoutRight;
        HorizontalLayout*   _hLayoutEcho;

        GroupWidget*         _groupLeft;
        GroupWidget*         _groupRight;
        GroupWidget*         _groupEcho;

        LabelWidget*        _levelText;
        LabelWidget*        _nameText;
        LabelWidget*        _hpText;
        LabelWidget*        _hpMaxText;
        LabelWidget*        _echoLevelText;
        LabelWidget*        _echoNameText;
        LabelWidget*        _mpText;
        LabelWidget*        _mpMaxText;

        GroupWidget*        _indentWidget;
        EntityWidget*       _portrait;
        HpMpGaugeWidget*    _hpGauge;
        HpMpGaugeWidget*    _mpGauge;
        EntityWidget*       _echoIcon;

    public:
                        PlayablePane(void);

        virtual void    update(void) override;

        void            setPartyMember(Entity* entity);
    };


}

#endif // ELYSIAN_CHARACTER_PANE_HPP

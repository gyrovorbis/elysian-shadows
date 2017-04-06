#ifndef ELYSIAN_PLAYABLE_PANE_HPP
#define ELYSIAN_PLAYABLE_PANE_HPP

#include <gui/elysian_entity_widget.hpp>
#include <utility/elysian_asset_typedefs.hpp>
#include <gui/elysian_entity_widget.hpp>
#include <crtp/elysian_lua_manager.hpp>

#define ELYSIAN_PLAYABLE_PANE_COMBAT_BEHAV      "/assets/scripts/combat/player_combat_behav.lua"
#define ELYSIAN_PLAYABLE_PANE_PORTRAIT_PREFAB   "menu_partyMemberPane_portrait"
#define ELYSIAN_PLAYABLE_PANE_HP_BG_PREFAB      "menu_partyMemberPane_hpBg"
#define ELYSIAN_PLAYABLE_PANE_HP_FG_PREFAB      "menu_partyMemberPane_hpFg"
#define ELYSIAN_PLAYABLE_PANE_MP_BG_PREFAB      "menu_partyMemberPane_mpBg"
#define ELYSIAN_PLAYABLE_PANE_MP_FG_PREFAB      "menu_partyMemberPane_mpFg"
#define ELYSIAN_PLAYABLE_PANE_ECHO_ICON_PREFAB  "menu_partyMemberPane_echoIcon"


ELYSIAN_ASSET_TYPEDEF(Entity)

namespace elysian {
    class LabelWidget;
    class HorizontalLayout;
    class VerticalLayout;
}

namespace game {

    using namespace elysian;

    class PlayablePane: public elysian::EntityWidget, public elysian::LuaManager {
    private:
        Entity*             _playable         = nullptr;
        LuaVariant          _combatTable;

        VerticalLayout*     _vLayoutLeft;
        VerticalLayout*     _vLayoutRight;
        HorizontalLayout*   _hLayoutEcho;

        EntityWidget*        _groupLeft;
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

        EntityWidget*       _portrait;
        EntityWidget*       _hpBg;
        EntityWidget*       _mpBg;
        EntityWidget*       _hpFg;
        EntityWidget*       _mpFg;
        EntityWidget*       _echoIcon;

    public:
                        PlayablePane(void);

        virtual void    update(void) override;

        void            setPartyMember(Entity* entity);
    };


}

#endif // ELYSIAN_CHARACTER_PANE_HPP

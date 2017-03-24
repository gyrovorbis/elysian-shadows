#ifndef ELYSIAN_CHARACTER_PANE_HPP
#define ELYSIAN_CHARACTER_PANE_HPP

#include <gui/elysian_entity_widget.hpp>
#include <utility/elysian_asset_typedefs.hpp>
#include <gui/elysian_entity_widget.hpp>
#include <crtp/elysian_lua_manager.hpp>

#define ELYSIAN_CHARACTER_PANE_COMBAT_BEHAV "/assets/scripts/combat/player_combat_behav.lua"

ELYSIAN_ASSET_TYPEDEF(Entity)

namespace elysian {
    class LabelWidget;
    class HorizontalLayout;
    class VerticalLayout;
}

namespace game {

    using namespace elysian;

    class CharacterPane: public elysian::EntityWidget, public elysian::LuaManager {
    private:
        Entity*             _playable         = nullptr;
        LuaVariant          _combatTable;

        VerticalLayout*     _vLayoutLeft;
        VerticalLayout*     _vLayoutRight;
        HorizontalLayout*   _hLayoutEcho;

        GroupWidget*        _groupLeft;
        GroupWidget*        _groupRight;
        GroupWidget*        _groupEcho;

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
                        CharacterPane(void);

        virtual void    render(void) override;

        void            setCharacter(Entity* entity);
    };


}

#endif // ELYSIAN_CHARACTER_PANE_HPP

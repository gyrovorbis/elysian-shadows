#ifndef ELYSIAN_MENU_HPP
#define ELYSIAN_MENU_HPP

#include <elysian_entity_widget.hpp>
#include <system/elysian_system.hpp>
#include <system/elysian_party_system.hpp>

namespace game {

    class PlayablePane;

    class MenuWidget: public elysian::EntityWidget, public elysian::System {
    private:
        PlayablePane*  _playablePane[ELYSIAN_PARTY_SLOTS_MAX];

    public:
                        MenuWidget(void);
    virtual void        update(void) override;

    };

}


#endif // GAME_MENU_HPP

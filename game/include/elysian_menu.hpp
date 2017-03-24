#ifndef ELYSIAN_MENU_HPP
#define ELYSIAN_MENU_HPP

#include <elysian_entity_widget.hpp>
#include <system/elysian_system.hpp>

namespace game {

    class CharacterPane;

    class MenuWidget: public elysian::EntityWidget, public elysian::System {
    private:
        CharacterPane*  _charPane;

    public:
                        MenuWidget(void);
    virtual void        render(void) override;

    };

}


#endif // GAME_MENU_HPP

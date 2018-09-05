#ifndef ES_INVENTORY_MENU_HPP
#define ES_INVENTORY_MENU_HPP

#include <game/pause_menu/es_pause_menu.hpp>

using namespace elysian;

namespace ElysianShadows {

    class InventoryPauseMenuScreen: public PauseMenuScreen {

    private:
        char            _helpInfoText[ELYSIAN_PAUSE_MENU_HELP_INFO_TEXT_SIZE_MAX]
                                                = "Inventory";

        GroupWidget*    _centralWidget          = nullptr;
        GroupWidget*    _detailsWidget          = nullptr;
        GroupWidget*    _helpWidget             = nullptr;

        MenuWidget*     _itemTypeMenu           = nullptr; //optionsPaneWidget
        MenuWidget*     _itemSubtypeMenu        = nullptr;
        MenuWidget*     _itemMenu               = nullptr;
        MenuWidget*     _itemSelectionMenu      = nullptr;

    public:
                        InventoryPauseMenuScreen(void);

        virtual Widget* getCentralPaneWidget(void) const override;
        virtual Widget* getOptionsPaneWidget(void) const override;
        virtual Widget* getDetailsPaneWidget(void) const override;
        virtual Widget* getHelpPaneWidget(void) const override;
        virtual auto    getHelpInfoText(void) const -> const char* override;

    };

}


#endif // ES_INVENTORY_MENU_HPP

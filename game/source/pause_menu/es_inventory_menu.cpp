#include <game/pause_menu/es_inventory_menu.hpp>
#include <ESGamma/gui/elysian_menu_widget.hpp>
#include <ESGamma/gui/elysian_layout.hpp>

using namespace elysian;

namespace ElysianShadows {

InventoryPauseMenuScreen::InventoryPauseMenuScreen(void) {
    _itemTypeMenu = new elysian::MenuWidget();
   /* auto* itemTypeLayout = VerticalLayout({


                                          });*/
}

Widget* InventoryPauseMenuScreen::getCentralPaneWidget(void) const  {
    return _centralWidget;
}

Widget* InventoryPauseMenuScreen::getOptionsPaneWidget(void) const {
    return _itemTypeMenu;
}

Widget* InventoryPauseMenuScreen::getDetailsPaneWidget(void) const {
    return _detailsWidget;
}

Widget* InventoryPauseMenuScreen::getHelpPaneWidget(void) const {
    return _helpWidget;
}

auto InventoryPauseMenuScreen::getHelpInfoText(void) const -> const char* {
    return _helpInfoText;
}


}

#include <pause_menu/es_inventory_menu.hpp>

namespace ElysianShadows {

InventoryPauseMenuScreen::InventoryPauseMenuScreen(void) {

}

Widget* InventoryPauseMenuScreen::getCentralPaneWidget(void) const  {
    return _centralWidget;
}

Widget* InventoryPauseMenuScreen::getOptionsPaneWidget(void) const {
    return _optionsWidget;
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

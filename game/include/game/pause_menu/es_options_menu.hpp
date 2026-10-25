/*
 * Copyright (c) 2013-2026 Elysian Shadows Team
 * SPDX-License-Identifier: MIT
 * Additional permission for Elysian Shadows Kickstarter backers: see LICENSE.
 */

#ifndef ELYSIAN_OPTIONS_MENU_HPP
#define ELYSIAN_OPTIONS_MENU_HPP

#include <ESGamma/gui/elysian_menu_widget.hpp>

namespace elysian {
    class VerticalLayout;
}

namespace ElysianShadows {

    class PauseMenuOptionsMenu: public elysian::MenuWidget {
    public:
        PauseMenuOptionsMenu(void);
    private:
        elysian::VerticalLayout*    _mainLayout;
      //  elysian::LabelWidget*       _optionsLabels[ELYSIAN_MENU_OPTIONS_PANE_OPTION_COUNT];

        static const constexpr char* const optionStrings[] = {
            "Party",
            "Inventory",
            "Echoes",
            "Skills",
            "Options"
        };

        virtual void    onCurrentChange(Widget* pev, Widget* next) override;
    };


}

#endif // ELYSIAN_OPTIONS_MENU_HPP

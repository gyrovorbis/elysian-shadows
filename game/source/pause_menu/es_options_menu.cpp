/*
 * Copyright (c) 2013-2026 Elysian Shadows Team
 * SPDX-License-Identifier: MIT
 * Additional permission for Elysian Shadows Kickstarter backers: see LICENSE.
 */

#include <game/pause_menu/es_options_menu.hpp>
#include <ESGamma/gui/elysian_layout.hpp>
#include <ESGamma/gui/elysian_label_widget.hpp>

using namespace elysian;

namespace ElysianShadows {

const constexpr char* const PauseMenuOptionsMenu::optionStrings[];

PauseMenuOptionsMenu::PauseMenuOptionsMenu(void) {
    std::vector<Widget*> options;

    for(unsigned i = 0; i < sizeof(optionStrings)/sizeof(optionStrings[0]); ++i) {
        options.push_back(new LabelWidget(optionStrings[i]));
    }

    _mainLayout = new VerticalLayout(std::move(options));
    setLayout(_mainLayout);
}


void PauseMenuOptionsMenu::onCurrentChange(Widget* prev, Widget* next) {
    auto* prevLabel = static_cast<LabelWidget*>(prev);
    auto* nextLabel = static_cast<LabelWidget*>(next);

    if(prevLabel) prevLabel->setColor(Color4::white);
    if(nextLabel) nextLabel->setColor(Color4::aqua);
}

}

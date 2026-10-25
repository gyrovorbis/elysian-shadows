/*
 * Copyright (c) 2013-2026 Elysian Shadows Team
 * SPDX-License-Identifier: MIT
 * Additional permission for Elysian Shadows Kickstarter backers: see LICENSE.
 */

#include <AssetIO/proxy/elysian_entity_proxy.hpp>
#include <AssetIO/crtp/elysian_dialog.hpp>
#include <cstring>

namespace elysian {

    ELYSIAN_LUA_PROXY_FIELDS_EXPORT(DialogTextBase)
    ELYSIAN_LUA_PROXY_FIELDS_EXPORT(DialogOptionBase)
    ELYSIAN_LUA_PROXY_FIELDS_EXPORT(DialogPromptBase)
    ELYSIAN_LUA_PROXY_FIELDS_EXPORT(DialogBase)

// ====== DIALOG SCREEN ======



// ====== DIALOG TEXT =======


// ====== DIALOG PROMPT =======
DialogPromptBase& DialogPromptBase::operator =(const DialogPromptBase& rhs) {
    DialogScreen::operator=(rhs);
    for(auto& it : _options) delete it;
    _options.clear();
    for(auto& it : rhs._options) _options.push_back(new DialogOption(*it));
    return *this;
}

DialogOption* DialogPromptBase::getOption(const unsigned index) const {
    return index < _options.size()? _options[index] : nullptr;
}

void DialogPromptBase::addOption(const char*const text, const int nextIndex) { _options.push_back(new DialogOption(text, nextIndex)); }
void DialogPromptBase::setOption(const unsigned index, DialogOption* option) {
    if(index == _options.size()) {                       //insert
        _options.push_back(option);
    } else if(index < _options.size()) {
        delete _options[index];
        if(!option) {
            _options.erase(_options.begin()+index);   //remove
        } else {
            _options[index] = option;
        }
    }
}
DialogPromptBase::~DialogPromptBase(void) { for(auto it: _options) delete it; }
// ====== DIALOG =======

DialogBase::~DialogBase(void) { for(auto it : _screens) delete it; }

void DialogBase::setScreen(const unsigned index, DialogScreen*const screen) {
    if(index == _screens.size()) {                       //insert
        _screens.push_back(screen);
    } else if(index < _screens.size()) {
        delete _screens[index];
        if(!screen) {
            _screens.erase(_screens.begin()+index);   //remove
        } else {
            _screens[index] = screen;
        }
    }
}

LuaVariant DialogBase::getScreenLua(const unsigned index) const {
    LuaVariant v;

    if(index < _screens.size()) {
        auto* screen = getScreen(index);
        if(screen) {
            switch(screen->getScreenType()) {
            case DialogScreen::SCREEN_TEXT:
                v.setValue<DialogText>(static_cast<DialogText*>(screen));
                break;
            case DialogScreen::SCREEN_PROMPT:
                v.setValue<DialogPrompt>(static_cast<DialogPrompt*>(screen));
                break;
            default:
            GY_ASSERT(0);
            }
        }
    }

    return v;
}


DialogText*	DialogBase::addTextScreen(const char*const text) {
    DialogText* screen = new DialogText(text);
    screen->setIndex(_screens.size());
    _screens.push_back(screen);
    screen->setNextIndex(_screens.size());
    return screen;
}

DialogPrompt* DialogBase::addPromptScreen(const char*const text) {
    DialogPrompt* screen = new DialogPrompt(text);
    screen->setIndex(_screens.size());
    _screens.push_back(screen);
    return screen;
}

DialogBase& DialogBase::operator =(const DialogBase& rhs) {
    for(auto& it: _screens) delete it;
    _screens.clear();
    for(auto& it : rhs._screens) {
        if(it->getScreenType() == DialogScreen::SCREEN_TEXT) {
            _screens.push_back(new DialogText(*static_cast<DialogText*>(it)));
        } else if(it->getScreenType() == DialogScreen::SCREEN_PROMPT) {
            _screens.push_back(new DialogPrompt(*static_cast<DialogPrompt*>(it)));
        } else GY_ASSERT(false);
    }
    return *this;
}

}



/*
 * Copyright (c) 2013-2026 Elysian Shadows Team
 * SPDX-License-Identifier: MIT
 * Additional permission for Elysian Shadows Kickstarter backers: see LICENSE.
 */

#ifndef ES_PARTY_MENU_HPP
#define ES_PARTY_MENU_HPP

#include <ESGamma/gui/elysian_menu_widget.hpp>
#include <ESGamma/system/elysian_system.hpp>
#include <ESGamma/system/elysian_party_system.hpp>

struct GYWav;

namespace ElysianShadows {

    class PlayablePane;

    class PartyMenu: public elysian::MenuWidget, public elysian::System {
        PlayablePane*               _playablePane[ELYSIAN_PARTY_SLOTS_MAX];
        GYWav*                      _swapWav        = nullptr;
        GYWav*                      _activateWav    = nullptr;
        GYWav*                      _deactivateWav  = nullptr;

        enum SELECT_STATE {
            SELECT_BULLSHIT,
            SELECT_FIRST,
            SELECT_SECOND
        } _selectState = SELECT_BULLSHIT;

        unsigned        _firstPartyIndex = 0;

        virtual void    update(void) override;
        virtual void    onSelectionChange(const elysian::MenuSelection* sel) override;
        virtual void    onCurrentChange(Widget* prev, Widget* next) override;
        virtual void    onCancel(void) override;
        virtual void    deactivate(void) override;
        //virtual void activate(void) override;
    public:
                        PartyMenu(void);
                        ~PartyMenu(void);

    };

}

#endif // ES_PARTY_MENU_HPP

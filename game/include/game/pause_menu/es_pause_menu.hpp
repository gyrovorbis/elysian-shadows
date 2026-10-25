/*
 * Copyright (c) 2013-2026 Elysian Shadows Team
 * SPDX-License-Identifier: MIT
 * Additional permission for Elysian Shadows Kickstarter backers: see LICENSE.
 */

#ifndef ELYSIAN_MENU_HPP
#define ELYSIAN_MENU_HPP

#include <ESGamma/gui/elysian_menu_widget.hpp>
#include <ESGamma/gui/elysian_entity_widget.hpp>
#include <ESGamma/gui/elysian_frame_widget.hpp>
#include <ESGamma/system/elysian_system.hpp>
#include <ESGamma/system/elysian_event_system.hpp>

#define ELYSIAN_MENU_OPTIONS_PANE_OPTION_COUNT          10
#define ELYSIAN_PAUSE_MENU_SFX_OPEN_PATH                "/assets/audio/sfx/UI/menu_select_01.wav"
#define ELYSIAN_PAUSE_MENU_SFX_CLOSE_PATH               "/assets/audio/sfx/UI/menu_select_00.wav"
#define ELYSIAN_PARTY_MENU_SFX_SWAP_PATH                "/assets/audio/sfx/UI/menu_select_02.wav"
#define ELYSIAN_PARTY_MENU_SFX_ACTIVATE_PATH            "/assets/audio/sfx/UI/menu_select_04.wav"
#define ELYSIAN_PARTY_MENU_SFX_DEACTIVATE_PATH          "/assets/audio/sfx/UI/menu_select_03.wav"

#define ELYSIAN_PAUSE_MENU_INFO_PANE_HELP_ICON_PREFAB  "menu_pauseMenuInfoPane_helpIcon"
#define ELYSIAN_PAUSE_MENU_HELP_INFO_TEXT_SIZE_MAX      50

struct GYWav;

namespace elysian {
    class Widget;
    class VerticalLayout;
    class HorizontalLayout;
    class LabelWidget;
    class GroupWidget;
    class MenuSelection;
    class KeyEvent;
}

namespace ElysianShadows {
    class PartyMenu;
    class PauseMenuOptionsMenu;


    enum MAIN_MENU_OPTION_TYPE {
        MAIN_MENU_OPTION_PARTY,
        MAIN_MENU_OPTION_INVENTORY,
        MAIN_MENU_OPTION_ECHOES,
        MAIN_MENU_OPTION_SKILLS,
        MAIN_MENU_OPTION_OPTIONS,
        MAIN_MENU_OPTION_COUNT
    };

    class PauseMenuScreen {
    public:
        virtual elysian::Widget*    getCentralPaneWidget(void) const {return nullptr;}
        virtual elysian::Widget*    getOptionsPaneWidget(void) const {return nullptr;}
        virtual elysian::Widget*    getDetailsPaneWidget(void) const {return nullptr;}
        virtual elysian::Widget*    getHelpPaneWidget(void) const {return nullptr;}
        virtual const char*         getHelpInfoText(void) const {return nullptr;}
    };

    class PauseMenuInfoPane: public elysian::GroupWidget {
    private:
        elysian::HorizontalLayout*  _helpLayout         = nullptr;
        elysian::VerticalLayout*    _textLayout         = nullptr;
        elysian::EntityWidget*      _helpIcon           = nullptr;
        elysian::LabelWidget*       _helpInfo           = nullptr;
        elysian::LabelWidget*       _title              = nullptr;
        elysian::LabelWidget*       _subTitle           = nullptr;
        elysian::FrameWidget*       _customWidgetFrame  = nullptr;
    public:
                                    PauseMenuInfoPane(void);

        void                        setHelpInfo(const char* text) const;
        void                        setTitle(const char* text) const;
        void                        setSubTitle(const char* text) const;
        void                        setCustomWidgetFrameCentralWidget(elysian::Widget* widget);
    };

    class PauseMenuDetailsPane: public elysian::GroupWidget {
    private:
        elysian::LabelWidget* _textLines[2] = { nullptr };
    public:
        PauseMenuDetailsPane(void);

    };

    class PauseMenuSubOptionsFrame: public elysian::FrameWidget {
    public:
                PauseMenuSubOptionsFrame(void);
    };

    class GridLayoutTestWidget: public elysian::EntityWidget {
    public:
       GridLayoutTestWidget(void);
    };


    class PauseMenu: public elysian::EntityWidget {
    private:

        enum FRAME_INDEX {
            FRAME_LEFT,
            FRAME_RIGHT
        };

        PauseMenuInfoPane*          _infoPane;
        PauseMenuDetailsPane*       _detailsPane;
        PauseMenuOptionsMenu*       _optionsPane;
        PauseMenuSubOptionsFrame*   _subOptionsFrame;
        PartyMenu*                  _partyMenu;
        elysian::GroupWidget*       _centralGroupWidget;
        elysian::GroupWidget*       _optionsGroupWidget;
        elysian::HorizontalLayout*  _centralLayout;

        GYWav*                      _openWav    = nullptr;
        GYWav*                      _closeWav   = nullptr;


        elysian::Widget*             _mainFrameWidgets[ELYSIAN_MENU_OPTIONS_PANE_OPTION_COUNT] = { nullptr };


    public:
                        PauseMenu(void);
    virtual             ~PauseMenu(void);

    void                setCurrentMenuOption(MAIN_MENU_OPTION_TYPE type);

    //overridden from EntityWidget
    virtual void        activate(void) override;
    virtual void        deactivate(void) override;
    virtual void        controllerEvent(const elysian::Controller& cont) override;
    virtual void        onKeyPressEvent(elysian::KeyEvent* keyEvent);
    virtual void        onKeyReleaseEvent(elysian::KeyEvent* keyEvent);

    //overridden from EventMonitor
    virtual void        onObserveEvent(elysian::Event* event, elysian::EventTarget* receiver);

    //SLOTS
// LISTEN TO EVENT, ONPARTYCHANGE!!!!



    void                onOptionsPaneSelect(const elysian::MenuSelection* selection);
    void                onOptionsPaneCancel(void);
    void                onMainFrameCancel(void);


    };

}


#endif // GAME_MENU_HPP

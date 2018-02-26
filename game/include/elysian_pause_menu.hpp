#ifndef ELYSIAN_MENU_HPP
#define ELYSIAN_MENU_HPP

#include <gui/elysian_menu_widget.hpp>
#include <gui/elysian_entity_widget.hpp>
#include <system/elysian_system.hpp>
#include <system/elysian_party_system.hpp>
#include <system/elysian_event_system.hpp>

#define ELYSIAN_MENU_OPTIONS_PANE_OPTION_COUNT          10
#define ELYSIAN_PAUSE_MENU_SFX_OPEN_PATH                "/assets/audio/sfx/UI/menu_select_01.wav"
#define ELYSIAN_PAUSE_MENU_SFX_CLOSE_PATH               "/assets/audio/sfx/UI/menu_select_00.wav"
#define ELYSIAN_PARTY_MENU_SFX_SWAP_PATH                "/assets/audio/sfx/UI/menu_select_02.wav"
#define ELYSIAN_PARTY_MENU_SFX_ACTIVATE_PATH            "/assets/audio/sfx/UI/menu_select_04.wav"
#define ELYSIAN_PARTY_MENU_SFX_DEACTIVATE_PATH          "/assets/audio/sfx/UI/menu_select_03.wav"

#define ELYSIAN_PAUSE_MENU_INFO_PANE_HELP_ICON_PREFAB  "menu_pauseMenuInfoPane_helpIcon"

namespace elysian {
    class VerticalLayout;
    class HorizontalLayout;
    class LabelWidget;
    class GroupWidget;
    class MenuSelection;
}


namespace game {
    class PlayablePane;


    enum MAIN_MENU_OPTION_TYPE {
        MAIN_MENU_OPTION_PARTY,
        MAIN_MENU_OPTION_INVENTORY,
        MAIN_MENU_OPTION_ECHOES,
        MAIN_MENU_OPTION_SKILLS,
        MAIN_MENU_OPTION_OPTIONS,
        MAIN_MENU_OPTION_COUNT
    };

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

        GYWav*              _openWav    = nullptr;
        GYWav*              _closeWav   = nullptr;


        elysian::Widget*             _mainFrameWidgets[ELYSIAN_MENU_OPTIONS_PANE_OPTION_COUNT] = { nullptr };


    public:
                        PauseMenu(void);
    virtual             ~PauseMenu(void);

    void                setCurrentMenuOption(MAIN_MENU_OPTION_TYPE type);

    //overridden from EntityWidget
    virtual void        activate(void) override;
    virtual void        deactivate(void) override;
    virtual void        controllerEvent(const elysian::Controller& cont) override;
    virtual void        onKeyboardEvent(const elysian::Keyboard& keybd) override;

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

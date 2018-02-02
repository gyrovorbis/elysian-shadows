#ifndef ELYSIAN_MENU_HPP
#define ELYSIAN_MENU_HPP

#include <gui/elysian_menu_widget.hpp>
#include <gui/elysian_entity_widget.hpp>
#include <system/elysian_system.hpp>
#include <system/elysian_party_system.hpp>

#define ELYSIAN_MENU_OPTIONS_PANE_OPTION_COUNT  10

namespace elysian {
    class VerticalLayout;
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

    class OptionsMenu: public elysian::MenuWidget {
    public:
        OptionsMenu(void);
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

        virtual void    onSelectionChange(elysian::MenuSelection* sel) override;
        virtual void    onCurrentChange(Widget* pev, Widget* next) override;
        virtual void    onCancel(void) override;
    };

    class PartyMenu: public elysian::MenuWidget, public elysian::System {
        PlayablePane*               _playablePane[ELYSIAN_PARTY_SLOTS_MAX];

        enum SELECT_STATE {
            SELECT_BULLSHIT,
            SELECT_FIRST,
            SELECT_SECOND
        } _selectState = SELECT_BULLSHIT;

        unsigned _firstPartyIndex = 0;

        virtual void update(void) override;
        virtual void onSelectionChange(elysian::MenuSelection* sel) override;
        virtual void onCurrentChange(Widget* prev, Widget* next) override;
        virtual void onCancel(void) override;
        //virtual void activate(void) override;
    public:
        PartyMenu(void);

    };

    class PauseMenu: public elysian::EntityWidget {
    private:

        enum FRAME_INDEX {
            FRAME_LEFT,
            FRAME_RIGHT
        };

        OptionsMenu*                _optionsPane;


        PartyMenu*       _partyMenu;



        elysian::Widget*             _mainFrameWidgets[ELYSIAN_MENU_OPTIONS_PANE_OPTION_COUNT] = { nullptr };


    public:
                        PauseMenu(void);
    void                setCurrentMenuOption(MAIN_MENU_OPTION_TYPE type);

    virtual void        activate(void) override;

    virtual void        controllerEvent(const elysian::Controller& cont) override;
    virtual void        keyboardEvent(const elysian::Keyboard& keybd) override;

    //SLOTS
// LISTEN TO EVENT, ONPARTYCHANGE!!!!



    void                onOptionsPaneSelect(elysian::MenuSelection* selection);
    void                onOptionsPaneCancel(void);
    void                onMainFrameCancel(void);


    };

}


#endif // GAME_MENU_HPP

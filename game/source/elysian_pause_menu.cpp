#include <elysian_pause_menu.hpp>
#include <elysian_playable_pane.hpp>
#include <gui/elysian_layout.hpp>
#include <gui/elysian_label_widget.hpp>
#include <system/elysian_party_system.hpp>
#include <crtp/elysian_entity.hpp>
#include <crtp/elysian_renderable.hpp>
#include <crtp/elysian_sprite_attrib.hpp>
#include <crtp/elysian_input_system.hpp>
#include <gyro_input_api.h>

namespace game {

using namespace elysian;

PauseMenu::PauseMenu(void):
    _optionsPane(new OptionsMenu),
    _partyMenu(new PartyMenu)
{

    HorizontalLayout* mainLayout = new HorizontalLayout(std::vector<Widget*>{
        _optionsPane,
        _partyMenu
    });
    setLayout(mainLayout);

    //Store Main Widgets for each Menu Option
    _mainFrameWidgets[MAIN_MENU_OPTION_PARTY] = _partyMenu;

    Entity* entity = new Entity;
    //entity->getTransform().setSize(300, 800, 0);
    entity->getTransform().setPos(400, 400);
    Renderable* renderable = entity->addRenderable();
    SpriteAttrib* attrib = new SpriteAttrib;
    attrib->setColor(255, 0, 0, 200);
    renderable->setSpriteAttrib(attrib);
    setEntity(entity);
}

void PauseMenu::setCurrentMenuOption(MAIN_MENU_OPTION_TYPE type) {
    _layout->setWidget(FRAME_RIGHT, _mainFrameWidgets[type]);
    _optionsPane->setCurrentIndex(type);
}

void PauseMenu::activate(void) {
    //reset menu to default positions and shit!
    setCurrentMenuOption(MAIN_MENU_OPTION_PARTY);
    _optionsPane->setFocus();
    _optionsPane->setCurrentIndex(0);
    Widget::activate();
}

void PauseMenu::onOptionsPaneSelect(elysian::MenuSelection* selection) {
    if(selection) {
        GY_ASSERT(selection->getCount());
        setCurrentMenuOption((MAIN_MENU_OPTION_TYPE)selection->getEntry(0)->second.index);
        _layout->getWidget(FRAME_RIGHT)->setFocus(true);
        _optionsPane->setFocus(false);
    } else {
        setCurrentMenuOption(MAIN_MENU_OPTION_PARTY);
        _optionsPane->setFocus();
    }

}

void PauseMenu::onOptionsPaneCancel(void) {
    //CLOSE/EXIT/DEACTIVATE!!!! SAME THING AS PUSHING PAUSE!!!
}

void PauseMenu::controllerEvent(const Controller& cont) {
    if(cont.isTapped(GY_CONTROLLER_BUT_START)) {
         //quit
    }
}

void PauseMenu::keyboardEvent(const Keyboard& keybd) {
    if(keybd.isTapped(GY_KEY_RETURN)) {

        //quit
    }

}



const constexpr char* const OptionsMenu::optionStrings[];

OptionsMenu::OptionsMenu(void) {
    std::vector<Widget*> options;

    for(unsigned i = 0; i < sizeof(optionStrings)/sizeof(optionStrings[0]); ++i) {
        options.push_back(new LabelWidget(optionStrings[i]));

    }

    _mainLayout = new VerticalLayout(std::move(options));
    setLayout(_mainLayout);
}


//GET RID OF THIS BULLSHIT AND CHANGE TO
//ON PARTY CHANGED EVENT!!!
void PartyMenu::update(void) {
    _layout->clearWidgets();
    for(unsigned i = 0; i < getPartySys()->getPartySize(); ++i) {
        _playablePane[i]->setPartyMember(getPartySys()->getPartyMember(i));
        _layout->addWidget(_playablePane[i]);
    }
    MenuWidget::update();
}

void PartyMenu::onSelectionChange(MenuSelection *next) {
    unsigned secondPartyIndex = 0;
    if(!next) return;

    switch(_selectState) {
    case SELECT_FIRST: //select a party member!
        _firstPartyIndex = next->getEntry(0)->second.index;
        //Do something to render this fucker differently!!!!11
        _selectState = SELECT_SECOND;
        break;
    case SELECT_SECOND:
        secondPartyIndex = next->getEntry(0)->second.index;
        if(secondPartyIndex == _firstPartyIndex) {
            //activate/deactivate character!!!
        } else {
            //swap character positions
        }
        _selectState = SELECT_FIRST;
        break;
    default: GY_ASSERT(false);

    }
}

void PartyMenu::onCancel(void) {

    switch(_selectState) {
    case SELECT_SECOND: //select a party member!
        //stop rendering firstPartyIndex as selection!
        _selectState = SELECT_FIRST;
        break;
    case SELECT_FIRST:
        //exit();
        break;
    default: GY_ASSERT(false);
    }


}



}

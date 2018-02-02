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
    auto attrib = std::make_shared<SpriteAttrib>();
    attrib->setColor(0, 0, 0, 100);
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
    setFocus(false);
    //CLOSE/EXIT/DEACTIVATE!!!! SAME THING AS PUSHING PAUSE!!!
}

void PauseMenu::controllerEvent(const Controller& cont) {
    if(cont.isTapped(GY_CONTROLLER_BUT_START)) {
        // setFocus(false);
         return;
    }
    EntityWidget::controllerEvent(cont);
}

void PauseMenu::keyboardEvent(const Keyboard& keybd) {
    if(keybd.isTapped(GY_KEY_RETURN)) {

        //quit
    }

}

void PauseMenu::onMainFrameCancel(void) {
    _layout->getWidget(FRAME_RIGHT)->setFocus(false);
    _optionsPane->setFocus();
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

void OptionsMenu::onSelectionChange(elysian::MenuSelection* sel) {
    static_cast<PauseMenu*>(getParentWidget())->onOptionsPaneSelect(sel);
}

void OptionsMenu::onCancel(void) {
    static_cast<PauseMenu*>(getParentWidget())->onOptionsPaneCancel();
}

void OptionsMenu::onCurrentChange(Widget* prev, Widget* next) {
    auto* prevLabel = static_cast<LabelWidget*>(prev);
    auto* nextLabel = static_cast<LabelWidget*>(next);

    if(prevLabel) prevLabel->setColor(Color4::white);
    if(nextLabel) nextLabel->setColor(Color4::aqua);
}

PartyMenu::PartyMenu(void) {
    for(unsigned i = 0; i < ELYSIAN_PARTY_SLOTS_MAX; ++i) _playablePane[i] = new PlayablePane();
    setLayout(new VerticalLayout);
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

void PartyMenu::onCurrentChange(Widget* prev, Widget* next) {
    auto* prevPlayablePane = static_cast<PlayablePane*>(prev);
    auto* nextPlayablePane = static_cast<PlayablePane*>(next);

    for(unsigned i = 0; i < getLayout()->getWidgetCount(); ++i) {
        auto* widget = getLayout()->getWidget(i);
        auto* entity = static_cast<EntityWidget*>(widget)->getEntity();
        auto sprAttrib = entity->getRenderable()->getSpriteAttrib();
        char alpha = 0;

        if(hasFocus()) {
            if(widget == next) {
                if(_selectState == SELECT_FIRST) alpha = 100;
                else if(_selectState == SELECT_SECOND) alpha = 200;
            } else if(_selectState == SELECT_SECOND && i == _firstPartyIndex) {
                alpha = 100;
            }

        }
#if 0
            if(_selectState == SELECT_FIRST) {
               if(next == widget) alpha += 100;
            } else {
                if(i == _firstPartyIndex && _selectState != SELECT_BULLSHIT) {
                    alpha += 100;
                }
                if(widget == next && _selectState == SELECT_SECOND) {
                    alpha += 100;
                }
            }
        }
#endif

        sprAttrib->setAlpha(alpha);
    }

}

void PartyMenu::onSelectionChange(MenuSelection *next) {
    unsigned secondPartyIndex = 0;
    if(!next) return;

    switch(_selectState) {
    case SELECT_BULLSHIT:
        _selectState = SELECT_FIRST;
        setCurrentIndex(0);
         onCurrentChange(nullptr, getCurrentWidget());
        break;
    case SELECT_FIRST: //select a party member!
        _firstPartyIndex = next->getEntry(0)->second.index;
        //Do something to render this fucker differently!!!!11
        _selectState = SELECT_SECOND;
        onCurrentChange(nullptr, getCurrentWidget());
        break;
    case SELECT_SECOND:
        secondPartyIndex = next->getEntry(0)->second.index;
        if(secondPartyIndex == _firstPartyIndex) {
            dbgprintf("Just tried to do shit to same shit [%d]", secondPartyIndex);
            //activate/deactivate character!!!
        } else {
            dbgprintf("Just tried to swap slots [%d, %d]", _firstPartyIndex, secondPartyIndex);
            //swap character positions
        }
        _selectState = SELECT_FIRST;
        _firstPartyIndex = secondPartyIndex;
        onCurrentChange(nullptr, getCurrentWidget());
        break;
    default: GY_ASSERT(false);

    }
}

void PartyMenu::onCancel(void) {

    switch(_selectState) {
    case SELECT_SECOND: //select a party member!
        //stop rendering firstPartyIndex as selection!
        _selectState = SELECT_FIRST;
        onCurrentChange(nullptr, getCurrentWidget());
        break;
    case SELECT_FIRST:
        _selectState = SELECT_BULLSHIT;
        setFocus(false);
        onCurrentChange(nullptr, nullptr);
        static_cast<PauseMenu*>(getParentWidget())->onMainFrameCancel();
        break;
    default: GY_ASSERT(false);
    }


}



}

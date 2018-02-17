#include <elysian_pause_menu.hpp>
#include <elysian_playable_pane.hpp>
#include <gui/elysian_layout.hpp>
#include <gui/elysian_label_widget.hpp>
#include <system/elysian_party_system.hpp>
#include <crtp/elysian_entity.hpp>
#include <crtp/elysian_renderable.hpp>
#include <crtp/elysian_sprite_attrib.hpp>
#include <crtp/elysian_input_system.hpp>
#include <system/elysian_audio_system.hpp>
#include <proxy/elysian_entity_proxy.hpp>
#include <crtp/elysian_gui_system.hpp>
#include <gyro_input_api.h>

namespace game {

using namespace elysian;

PauseMenu::PauseMenu(void):
    _infoPane(new PauseMenuInfoPane),
    _detailsPane(new PauseMenuDetailsPane),
    _optionsPane(new PauseMenuOptionsMenu),
    _subOptionsFrame(new PauseMenuSubOptionsFrame),
    _partyMenu(new PartyMenu),
    _centralGroupWidget(new elysian::GroupWidget),
    _openWav(gyAudWavCreate(ELYSIAN_PAUSE_MENU_SFX_OPEN_PATH)),
    _closeWav(gyAudWavCreate(ELYSIAN_PAUSE_MENU_SFX_CLOSE_PATH))
{

    _optionsGroupWidget = new elysian::GroupWidget(
        new elysian::VerticalLayout({
            _optionsPane,
            _subOptionsFrame
        })
    );

    _centralLayout = new HorizontalLayout({
        _optionsGroupWidget,
        _partyMenu
    });
    _centralGroupWidget->setLayout(_centralLayout);

    //Store Main Widgets for each Menu Option
    _mainFrameWidgets[MAIN_MENU_OPTION_PARTY] = _partyMenu;

    Entity* entity = new Entity;
    entity->getTransform().setSize(600, 440, 0);
    //setSizePolicy(Widget::FIXED, Widget::FIXED);
    //entity->getTransform().setPos(400, 400);
    Renderable* renderable = entity->addRenderable();
    auto attrib = std::make_shared<SpriteAttrib>();
    attrib->setColor(0, 0, 0, 100);
    renderable->setSpriteAttrib(attrib);
    setEntity(entity);

    setLayout(new VerticalLayout({
         _infoPane,
         _centralGroupWidget,
         _detailsPane
     }));

    //SIGNALS/SLOTS
    System::getEventSys()->addEventObserver(this, EVENT_WIDGET_MENU_CANCEL,     _optionsPane);
    System::getEventSys()->addEventObserver(this, EVENT_WIDGET_MENU_SELECTION,  _optionsPane);
    System::getEventSys()->addEventObserver(this, EVENT_WIDGET_MENU_CANCEL,     _partyMenu);
#if 1 //TESTING
    System::getEventSys()->addEventObserver(this, EVENT_MOUSE_BUTTON);
    System::getEventSys()->addEventObserver(this, EVENT_MOUSE_MOVE);
    System::getEventSys()->addEventObserver(this, EVENT_MOUSE_WHEEL);
#endif


}

PauseMenu::~PauseMenu(void) {
    gyAudWavDestroy(_openWav);
    gyAudWavDestroy(_closeWav);
}


void PauseMenu::setCurrentMenuOption(MAIN_MENU_OPTION_TYPE type) {
    _centralLayout->setWidget(FRAME_RIGHT, _mainFrameWidgets[type]);
    _optionsPane->setCurrentIndex(type);
}

void PauseMenu::activate(void) {
    //reset menu to default positions and shit!
    setCurrentMenuOption(MAIN_MENU_OPTION_PARTY);
    _centralGroupWidget->setFocus();
    _optionsGroupWidget->setFocus();
    _optionsPane->setFocus();
    _optionsPane->setCurrentIndex(0);
    System::getAudioSys()->playSfxChannel(-1, *_openWav);
    Widget::activate();
}

void PauseMenu::deactivate(void) {
    _centralLayout->getWidget(FRAME_RIGHT)->setFocus(false);
    _optionsGroupWidget->setFocus(false);
    _centralGroupWidget->setFocus(false);
    System::getAudioSys()->playSfxChannel(-1, *_closeWav);
    Widget::deactivate();
}

void PauseMenu::onObserveEvent(elysian::Event* event, elysian::EventReceiver* receiver) {
    if(receiver) {
        if(receiver == _optionsPane) {
            switch(event->getEventType()) {
            case EVENT_WIDGET_MENU_CANCEL:
                onOptionsPaneCancel();
                return;
            case EVENT_WIDGET_MENU_SELECTION:
                onOptionsPaneSelect(_optionsPane->getSelection());
                return;
            default: break;
            }
        } else if(receiver == _centralLayout->getWidget(FRAME_RIGHT)) {
            switch(event->getEventType()) {
            case EVENT_WIDGET_MENU_CANCEL:
                onMainFrameCancel();
            default: break;
            }
        } else { //dis some global event shit

        }
    }
    EntityWidget::onObserveEvent(event, receiver);
}

void PauseMenu::onOptionsPaneSelect(const elysian::MenuSelection* selection) {
    if(selection) {
        GY_ASSERT(selection->getCount());
        const int index = selection->getEntry(0)->second.index;
        if(_mainFrameWidgets[index]) {
            setCurrentMenuOption((MAIN_MENU_OPTION_TYPE)index);
            _centralLayout->getWidget(FRAME_RIGHT)->setFocus(true);
            _optionsPane->setFocus(false);
            _optionsGroupWidget->setFocus(false);
            System::getAudioSys()->playSfxChannel(-1, *_openWav);
        }
    } else {
        setCurrentMenuOption(MAIN_MENU_OPTION_PARTY);
        _optionsPane->setFocus();
        _optionsGroupWidget->setFocus(false);
    }

}

void PauseMenu::onOptionsPaneCancel(void) {
    setFocus(false);
    _optionsGroupWidget->setFocus(false);
    _optionsPane->setFocus(false);
    //CLOSE/EXIT/DEACTIVATE!!!! SAME THING AS PUSHING PAUSE!!!
}

void PauseMenu::controllerEvent(const Controller& cont) {
    if(cont.isTapped(GY_CONTROLLER_BUT_START)) {
        // setFocus(false);
         return;
    }
    EntityWidget::controllerEvent(cont);
}

void PauseMenu::onKeyboardEvent(const Keyboard& keybd) {
    if(keybd.isTapped(GY_KEY_RETURN)) {

        //quit
    }

}

void PauseMenu::onMainFrameCancel(void) {
    _centralLayout->getWidget(FRAME_RIGHT)->setFocus(false);
    System::getAudioSys()->playSfxChannel(-1, *_closeWav);
    _optionsPane->setFocus();
    _optionsGroupWidget->setFocus(true);
}

PauseMenuInfoPane::PauseMenuInfoPane(void):
    _helpInfo(new elysian::LabelWidget("Help Information")),
    _title(new elysian::LabelWidget("Title")),
    _subTitle(new elysian::LabelWidget("Subtitle")),
    _customWidgetFrame(new elysian::FrameWidget(nullptr))
{
    _helpInfo->setAlignment(LabelWidget::LEFT);
    _title->setAlignment(LabelWidget::LEFT);
    //_title->setFont(&System::getGuiSys()->verdana);
    _subTitle->setAlignment(LabelWidget::LEFT);
    LuaVariant prefab = LuaManager::getPrefab(ELYSIAN_PAUSE_MENU_INFO_PANE_HELP_ICON_PREFAB);
    elysian::Entity* helpIconEntity = nullptr;
    if(!prefab.isNil()) {
        helpIconEntity = prefab.getValue<elysian::Entity*>();
    }
    _helpIcon = new elysian::EntityWidget(helpIconEntity);
    _helpIcon->setSizePolicy(Widget::FIXED, Widget::FIXED);

    _helpLayout = new HorizontalLayout({ _helpIcon, _helpInfo });
    auto* helpGroupWidget = new elysian::GroupWidget();
    helpGroupWidget->setLayout(_helpLayout);

    _textLayout = new VerticalLayout({ helpGroupWidget, _title, _subTitle });
    auto* textGroupWidget = new elysian::GroupWidget();
    textGroupWidget->setLayout(_textLayout);

    _customWidgetFrame->setSizePolicy(Widget::X, Widget::EXPANDING);

    elysian::Layout* mainLayout = new HorizontalLayout({textGroupWidget, _customWidgetFrame});
    setLayout(mainLayout);
}

void PauseMenuInfoPane::setHelpInfo(const char* text) const {
    _helpInfo->setText(text);
}
void PauseMenuInfoPane::setTitle(const char* text) const {
    _title->setText(text);
}
void PauseMenuInfoPane::setSubTitle(const char* text) const {
    _subTitle->setText(text);
}
void PauseMenuInfoPane::setCustomWidgetFrameCentralWidget(elysian::Widget* widget) {
    _customWidgetFrame->setCentralWidget(widget);
}

PauseMenuDetailsPane::PauseMenuDetailsPane(void) {
    _textLines[0] = new elysian::LabelWidget("Line 1");
    _textLines[1] = new elysian::LabelWidget("Line 2");
    setLayout(new VerticalLayout({
         _textLines[0],
         _textLines[1]
     }));
}

PauseMenuSubOptionsFrame::PauseMenuSubOptionsFrame(void):
    FrameWidget(nullptr)
{
    setSizePolicy(Widget::Y, Widget::EXPANDING);
}

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

PartyMenu::PartyMenu(void):
    _swapWav(gyAudWavCreate(ELYSIAN_PARTY_MENU_SFX_SWAP_PATH)),
    _activateWav(gyAudWavCreate(ELYSIAN_PARTY_MENU_SFX_ACTIVATE_PATH)),
    _deactivateWav(gyAudWavCreate(ELYSIAN_PARTY_MENU_SFX_DEACTIVATE_PATH))
{
    for(unsigned i = 0; i < ELYSIAN_PARTY_SLOTS_MAX; ++i) _playablePane[i] = new PlayablePane();
    setLayout(new VerticalLayout);
}

PartyMenu::~PartyMenu(void) {
    gyAudWavDestroy(_swapWav);
    gyAudWavDestroy(_activateWav);
    gyAudWavDestroy(_deactivateWav);
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

void PartyMenu::onSelectionChange(const MenuSelection *next) {
    unsigned secondPartyIndex = 0;
    if(!next) return;

    switch(_selectState) {
    case SELECT_BULLSHIT:
        _selectState = SELECT_FIRST;
        setCurrentIndex(0);
         onCurrentChange(nullptr, getCurrentWidget());
        break;
    case SELECT_FIRST: //select a party member!
        System::getAudioSys()->playSfxChannel(-1, *_swapWav);
        _firstPartyIndex = next->getEntry(0)->second.index;
        //Do something to render this fucker differently!!!!11
        _selectState = SELECT_SECOND;
        onCurrentChange(nullptr, getCurrentWidget());
        break;
    case SELECT_SECOND:
        secondPartyIndex = next->getEntry(0)->second.index;
        if(secondPartyIndex == _firstPartyIndex) {
            bool active = System::getPartySys()->isPartyMemberActive(secondPartyIndex);
            System::getAudioSys()->playSfxChannel(-1, active? *_deactivateWav : *_activateWav);
            System::getPartySys()->setPartyMemberActive(secondPartyIndex, !active);
        } else {
            System::getAudioSys()->playSfxChannel(-1, *_swapWav);
            System::getPartySys()->swapPartyMemberSlots(_firstPartyIndex, secondPartyIndex);
        }
        _selectState = SELECT_FIRST;
        _firstPartyIndex = secondPartyIndex;
        onCurrentChange(nullptr, getCurrentWidget());
        break;
    default: GY_ASSERT(false);

    }
}

void PartyMenu::deactivate(void) {
    _selectState = SELECT_BULLSHIT;
    onCurrentChange(nullptr, nullptr);
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
        //static_cast<PauseMenu*>(getParentWidget()->getParentWidget())->onMainFrameCancel();
        break;
    default: GY_ASSERT(false);
    }


}



}

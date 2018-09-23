#include <game/pause_menu/es_pause_menu.hpp>
#include <game/pause_menu/es_options_menu.hpp>
#include <game/pause_menu/es_playable_pane.hpp>
#include <game/pause_menu/es_party_menu.hpp>
#include <ESGamma/gui/elysian_layout.hpp>
#include <ESGamma/gui/elysian_label_widget.hpp>
#include <AssetIO/crtp/elysian_entity.hpp>
#include <AssetIO/crtp/elysian_renderable.hpp>
#include <AssetIO/crtp/elysian_sprite_attrib.hpp>
#include <ESGamma/crtp/elysian_input_system.hpp>
#include <ESGamma/system/elysian_audio_system.hpp>
#include <AssetIO/proxy/elysian_entity_proxy.hpp>
#include <ESGamma/crtp/elysian_gui_system.hpp>
#include <AssetIO/crtp/elysian_sprite_attrib.hpp>
#include <libGyro/gyro_input_api.h>
#include <ESGamma/gui/elysian_item_delegate.hpp>

namespace ElysianShadows {

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
    _optionsGroupWidget->setSizePolicy(Widget::PREFERRED, Widget::PREFERRED);

    _centralLayout = new HorizontalLayout({
        _optionsGroupWidget,
        _partyMenu
    });
    _centralGroupWidget->setLayout(_centralLayout);

    //Store Main Widgets for each Menu Option
    _mainFrameWidgets[MAIN_MENU_OPTION_PARTY] = _partyMenu;

    Entity* entity = new Entity;
    entity->getTransform().setSize(600, 500, 0);
    setSizeHint(Vector2(600, 500));
    setSizePolicy(Widget::MAXIMUM, Widget::MAXIMUM);
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
    System::getEventSys()->addEventObserver(this, EVENT_MOUSE_MOVED);
    System::getEventSys()->addEventObserver(this, EVENT_MOUSE_SCROLLED);
#endif


    //=======DELETE MEEEEE!!!!!!!=======
    auto* testWidget = new GridLayoutTestWidget();
    _mainFrameWidgets[1] = testWidget;

    Entity* e = new Entity();
    e->addRenderable();
    auto attr = std::make_shared<elysian::SpriteAttrib>();
    attr->setColor(255, 100, 50, 40);
    e->getRenderable()->setSpriteAttrib(attr);
    testWidget->setEntity(e);

    //MenuWidget synchronicity
    //1) virualize item count (return from total count)
    //2) virtualize current item change  (sync curItemIndex=>_topLeftIndex)

    LuaArrayModel* luaModel = new LuaArrayModel(LuaVariant::fromArray({
                "Test1",
                "Bitch",
                "Wench",
                "Franz Ferdinand",
                  "Test2",
                  "Bitch",
                  "Wench",
                  "Franz Ferdinand",
                  "Test3",
                  "Bitch",
                  "Wench",
                  "Franz Ferdinand",
                  "Test4",
                  "Bitch",
                  "Wench",
                  "Franz Ferdinand",
                  "Test5",
                  "Bitch",
                  "Wench",
                  "Franz Ferdinand",
                  "Franz Ferdinand",
                  "Test4",
                  "Bitch",
                  "Wench",
                  "Franz Ferdinand",
                  "Test5",
                  "Bitch",
                  "Wench",
                  "Franz Ferdinand"
        }));

    MenuItemView* menu = new MenuItemView(luaModel, new DefaultItemDelegate(luaModel));
    menu->setLayout(new VerticalLayout);
    //menu->setMaxSize(Vector2(600, 440));
   // menu->setMaxSize(Vector2(750, 600));
    _mainFrameWidgets[2] = menu;
    //_optionsGroupWidget->getLayout<VerticalLayout>()->insertWidget(0, menu);

    //===== END TEMP FUCKERY =====
}

PauseMenu::~PauseMenu(void) {
    gyAudWavDestroy(_openWav);
    gyAudWavDestroy(_closeWav);
}


void PauseMenu::setCurrentMenuOption(MAIN_MENU_OPTION_TYPE type) {
    _centralLayout->insertWidget(FRAME_RIGHT, _mainFrameWidgets[type]);
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

void PauseMenu::onObserveEvent(elysian::Event* event, elysian::EventTarget* target) {
    if(target) {
        if(target == _optionsPane) {
            switch(event->getEventType()) {
            case EVENT_WIDGET_MENU_CANCEL:
                onOptionsPaneCancel();
                return;
            case EVENT_WIDGET_MENU_SELECTION:
                onOptionsPaneSelect(_optionsPane->getSelection());
                return;
            default: break;
            }
        } else if(target == _centralLayout->getWidget(FRAME_RIGHT)) {
            switch(event->getEventType()) {
            case EVENT_WIDGET_MENU_CANCEL:
                onMainFrameCancel();
            default: break;
            }
        } else { //dis some global event shit

        }
    }
    EntityWidget::onObserveEvent(event, target);
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

void PauseMenu::onKeyPressEvent(KeyEvent* keyEvent) {
    if(keyEvent->getKey() == GY_KEY_RETURN) {

        //quit
    }

}

void PauseMenu::onKeyReleaseEvent(KeyEvent* /*keyEvent*/) {


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

    elysian::HorizontalLayout* mainLayout = new HorizontalLayout({textGroupWidget, _customWidgetFrame});
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
    //setSizePolicy(Widget::Y, Widget::EXPANDING);
}



}

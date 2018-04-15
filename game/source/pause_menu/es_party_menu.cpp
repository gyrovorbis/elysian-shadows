#include <pause_menu/es_party_menu.hpp>
#include <pause_menu/es_pause_menu.hpp>
#include <pause_menu/es_playable_pane.hpp>
#include <gui/elysian_label_widget.hpp>
#include <gui/elysian_layout.hpp>
#include <system/elysian_audio_system.hpp>
#include <crtp/elysian_entity.hpp>
#include <crtp/elysian_renderable.hpp>

namespace ElysianShadows {

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
    auto* vertLayout = static_cast<VerticalLayout*>(getLayout());
    vertLayout->clearWidgets();
    for(unsigned i = 0; i < getPartySys()->getPartySize(); ++i) {
        _playablePane[i]->setPartyMember(getPartySys()->getPartyMember(i));
        vertLayout->addWidget(_playablePane[i]);
    }
    MenuWidget::update();
}

void PartyMenu::onCurrentChange(Widget* prev, Widget* next) {
    auto* prevPlayablePane = static_cast<PlayablePane*>(prev);
    auto* nextPlayablePane = static_cast<PlayablePane*>(next);
    auto* vertLayout = static_cast<VerticalLayout*>(getLayout());

    for(unsigned i = 0; i < getLayout()->getWidgetCount(); ++i) {
        auto* widget = vertLayout->getWidget(i);
        auto* entity = static_cast<EntityWidget*>(widget)->getEntity();
        auto sprAttrib = entity->getRenderable()->getSpriteAttrib();
        unsigned char alpha = 0;

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

GridLayoutTestWidget::GridLayoutTestWidget(void) {
    auto* layout = new TableLayout();

    const int rowCount = 30;
    const int colCount = 25;

    for(unsigned r = 0; r < rowCount; ++r) {
        for(unsigned c = 0; c < colCount; ++c) {

            if(c == 14 && r == 15) {
                LabelWidget* label = new LabelWidget("FUCK OFF, BITCH");
                layout->insertWidget(r, c, label);
                continue;
            }

            auto attr = std::make_shared<elysian::SpriteAttrib>();
            attr->setColor(0, 0, 0, 255);
            Entity* entity = new Entity();
            if(c == 1 && r == 0) {
                entity->getTransform().setSize(32.0f, 16.0f, 1.0f);
            } else if(c == 5 && r == 7) {
                entity->getTransform().setSize(16.0f, 137.0f, 1.0f);
            } else if(c == 15 && r == 25) {
                entity->getTransform().setSize(48.0f, 92.0f, 1.0f);
            } else {
                entity->getTransform().setSize(16.0f, 16.0f, 1.0f);
                attr->setColor(r * (255/rowCount), 100, c * (255/colCount), 255);
            }


            entity->addRenderable();
            entity->getRenderable()->setSpriteAttrib(attr);
            layout->insertWidget(r, c, new EntityWidget(entity));
        }
    }

    setLayout(layout);

}


}

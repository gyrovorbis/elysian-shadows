#include <elysian_menu.hpp>
#include <elysian_playable_pane.hpp>
#include <gui/elysian_layout.hpp>
#include <system/elysian_party_system.hpp>
#include <crtp/elysian_entity.hpp>
#include <crtp/elysian_renderable.hpp>
#include <crtp/elysian_sprite_attrib.hpp>

namespace game {

using namespace elysian;

MenuWidget::MenuWidget(void) {
    for(unsigned i = 0; i < ELYSIAN_PARTY_SLOTS_MAX; ++i) {
        _playablePane[i] = new PlayablePane();
    }

    VerticalLayout* layout = new VerticalLayout();
    setLayout(layout);

    Entity* entity = new Entity;
    //entity->getTransform().setSize(300, 800, 0);
    entity->getTransform().setPos(400, 400);
    Renderable* renderable = entity->addRenderable();
    SpriteAttrib* attrib = new SpriteAttrib;
    attrib->setColor(255, 0, 0, 200);
    renderable->setSpriteAttrib(attrib);
    setEntity(entity);
}


void MenuWidget::update(void) {
    auto* layout = getLayout();
    layout->clearWidgets();
    for(unsigned i = 0; i < getPartySys()->getPartySize(); ++i) {
        _playablePane[i]->setPartyMember(getPartySys()->getPartyMember(i));
        layout->addWidget(_playablePane[i]);
    }
    Widget::update();
}


}

#include <elysian_menu.hpp>
#include <elysian_character_pane.hpp>
#include <gui/elysian_layout.hpp>
#include <system/elysian_party_system.hpp>
#include <crtp/elysian_entity.hpp>
#include <crtp/elysian_renderable.hpp>
#include <crtp/elysian_sprite_attrib.hpp>

namespace game {

using namespace elysian;

MenuWidget::MenuWidget(void) {
    _charPane = new CharacterPane();

    VerticalLayout* layout = new VerticalLayout();
    layout->addWidget(_charPane);
    setLayout(layout);

    Entity* entity = new Entity;
    entity->getTransform().setSize(400, 400, 400);
    entity->getTransform().setPos(400, 400);
    Renderable* renderable = entity->addRenderable();
    SpriteAttrib* attrib = new SpriteAttrib;
    attrib->setColor(100, 100, 100, 100);
    renderable->setSpriteAttrib(attrib);
    setEntity(entity);
}


void MenuWidget::render(void) {
    _charPane->setCharacter(getPartySys()->getPartyLeader());
    EntityWidget::render();
}


}

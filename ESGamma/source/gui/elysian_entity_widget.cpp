#include <gui/elysian_entity_widget.hpp>
#include <crtp/elysian_entity.hpp>

namespace elysian {

int EntityWidget::getPosition(AXIS axis) const {
    if(_entity) {
        return (axis == X)? _entity->getTransform().getPosX() :
                            _entity->getTransform().getPosY();
    }
    return 0;
}

void EntityWidget::setPosition(AXIS axis, int pos) {
    if(_entity) {
        if(axis == X) _entity->getTransform().setPosX(pos);
        else _entity->getTransform().setPosY(pos);
    }
}

int EntityWidget::getSize(AXIS axis) const {
    if(_entity) {
        return (axis == X)? _entity->getTransform().getSizeX() :
                            _entity->getTransform().getSizeY();
    }
    return 0;
}

void EntityWidget::setSize(AXIS axis, int size) {
    if(_entity) {
        if(axis == X) _entity->getTransform().setSizeX(size);
        else _entity->getTransform().setSizeY(size);
    }
}

void EntityWidget::render(void) const {
    if(_entity) _entity->render();
    EntityWidget::renderLayout();
}

}

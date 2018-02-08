#include <gui/elysian_entity_widget.hpp>
#include <crtp/elysian_entity.hpp>

namespace elysian {

float EntityWidget::getPosition(AXIS axis) const {
    if(_entity) {
        return (axis == X)? _entity->getTransform().getPosX() :
                            _entity->getTransform().getPosY();
    }
    return 0;
}

void EntityWidget::setPosition(AXIS axis, float pos) {
    if(_entity) {
        if(axis == X) _entity->getTransform().setPosX(pos);
        else _entity->getTransform().setPosY(pos);
    } else GY_ASSERT(false);
}

float EntityWidget::getSize(AXIS axis) const {
    if(_entity) {
        return (axis == X)? _entity->getTransform().getSizeX() :
                            _entity->getTransform().getSizeY();
    }
    return 0;
}

void EntityWidget::setSize(AXIS axis, float size) {
    if(_entity) {
        if(axis == X) _entity->getTransform().setSizeX(size);
        else _entity->getTransform().setSizeY(size);
    }
}

void EntityWidget::render(void) const {
    if(_entity) _entity->render();
    Widget::render();
}

float EntityWidget::getPositionZ(void) const {
    return _entity->getTransform().getPosZ();
}

void EntityWidget::setPositionZ(float pos) {
    _entity->getTransform().setPosZ(pos);
}

}

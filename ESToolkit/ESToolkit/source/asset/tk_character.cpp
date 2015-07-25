#include <asset/tk_entity.h>
#include <asset/tk_character.h>

bool TkCharacter::canAddTo(const Entity &entity) const {
    return entity.getCollidable() && entity.getRigidBody();
}

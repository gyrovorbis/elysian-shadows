#include <asset/tk_combat.h>
#include <core/tk_menu_stack.h>
#include <asset/tk_entity.h>

bool TkCombat::validate(QString& str) const {
    bool success = true;
    auto cAssert = [&](const bool val, const char* msg) {
        if(!val) {
            success = false;
            str += msg;
            str += "\n";
        }
    };

    cAssert(_attrib, "DBG_ERROR: Combat component has no attrib field.");
    return success;
}

QMenu* TkCombat::createContextMenu(void) {
    MenuStack menuStack("Combat");

    menuStack.insertCheckableAction("Attributes",
        [&](bool checked) {
            setAttrib(checked? new elysian::CombatAttrib : nullptr);
        },
        getAttrib(),
        !_entity->isActive()
    );

    menuStack.insertAction("Remove", [&](bool /*checked*/) {
            _entity->removeCombat();
        },
        canRemoveFrom(*_entity)
    );


    return menuStack.first();

}

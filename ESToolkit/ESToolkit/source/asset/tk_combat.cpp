#include <tk_combat.h>
#include <tk_menu_stack.h>

bool TkCombat::validate(QString& str) const {
    bool success = true;
    auto cAssert = [&](const bool val, const char* msg) {
        if(!val) {
            success = false;
            str += msg;
            str += "\n";
        }
    };

    cAssert(_attrib, "ERROR: Combat component has no attrib field.");
    return success;
}

QMenu* TkCombat::createContextMenu(void) {
    MenuStack menuStack("Combat");

    menuStack.insertCheckableAction("Attributes",
        [&](bool checked) {
            setAttrib(checked? new elysian::CombatAttrib : nullptr);
        },
        getAttrib()
    );

    return menuStack.first();

}

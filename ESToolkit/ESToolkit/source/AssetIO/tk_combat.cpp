#include <ESToolkit/AssetIO/tk_combat.h>
#include <QESTkUtils/tk_menu_stack.h>
#include <ESToolkit/AssetIO/tk_entity.h>
#include <QESTkUtils/ui/delegate/tk_enum_dropdown_property_item_delegate.h>
#include <ESToolkit/ESGamma/tk_lua_wrappers.h>

bool TkCombat::validate(QString& str) const {
    bool success = true;
    auto cAssert = [&](const bool val, const char* msg) {
        if(!val) {
            success = false;
            str += msg;
            str += "\n";
        }
    };

    cAssert(_attrib.get(), "LOG_ERROR: Combat component has no attrib field.");
    return success;
}

QMenu* TkCombat::createContextMenu(void) {
    MenuStack menuStack("Combat");

    menuStack.insertCheckableAction("Attributes",
        [&](bool checked) {
            setAttrib(checked? std::make_shared<elysian::CombatAttrib>() : nullptr);
        },
        getAttrib().get(),
        !_entity->isActive()
    );

    menuStack.insertAction("Remove", [&](bool /*checked*/) {
            _entity->removeCombat();
        },
        canRemoveFrom(*_entity)
    );


    return menuStack.takeTopLevel();

}

void TkCombat::initializeDelegates(void) {
    lua_newtable(*_mainState);
    lua_pushvalue(*_mainState, -1);
    lua_setfield(*_mainState, -3, TK_LUA_TABLE_DELEGATE_FIELD_KEY);

    auto* typeDelegate = new EnumDropdownPropertyItemDelegate({
        "DMG_NONE", "DMG_SOURCE", "DMG_TARGET", "DMG_SOURCE_TARGET"
    });
    OOLUA::push(*_mainState, typeDelegate);
    lua_setfield(*_mainState, -2, "dmgType");

    lua_pop(*_mainState, 1);
}

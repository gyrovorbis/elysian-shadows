#include <ESToolkit/AssetIO/tk_light_source.h>
#include <QESTkUtils/tk_menu_stack.h>
#include <ESToolkit/AssetIO/tk_entity.h>
#include <QESTkUtils/ui/delegate/tk_enum_dropdown_property_item_delegate.h>
#include <QESTkUtils/ui/delegate/tk_spinbox_property_item_delegate.h>
#include <ESToolkit/ESGamma/tk_lua_wrappers.h>

bool TkLightSource::validate(QString& str) const {
    bool success = true;
    auto cAssert = [&](const bool val, const char* msg) {
        if(!val) {
            success = false;
            str += msg;
            str += "\n";
        }
    };

    cAssert(properties, "LOG_ERROR: LightSource component has no attrib field.");
    return success;
}

QMenu* TkLightSource::createContextMenu(void) {
    MenuStack menuStack("LightSource");

    menuStack.insertCheckableAction("Attributes",
        [&](bool checked) {
            setAttrib(checked? std::make_shared<elysian::LightAttrib>() : nullptr);
        },
        getAttrib().get(),
        !_entity->isActive()
    );

    menuStack.insertAction("Remove", [&](bool /*checked*/) {
            _entity->removeLightSource();
        },
        canRemoveFrom(*_entity)
    );


    return menuStack.takeTopLevel();

}

void TkLightSource::initializeDelegates(void) {
    lua_newtable(*_mainState);
    lua_pushvalue(*_mainState, -1);
    lua_setfield(*_mainState, -3, TK_LUA_TABLE_DELEGATE_FIELD_KEY);

    auto* typeDelegate = new EnumDropdownPropertyItemDelegate({
       "LIGHT_DIRECTIONAL", "LIGHT_SPOT", "LIGHT_POINT"
    });
    OOLUA::push(*_mainState, typeDelegate);
    lua_setfield(*_mainState, -2, "type");

    auto* cutoffDelegate = new SpinboxPropertyItemDelegate(0, 360);
    OOLUA::push(*_mainState, cutoffDelegate);
    lua_setfield(*_mainState, -2, "spotCutoffAngle");

    lua_pop(*_mainState, 1);
}

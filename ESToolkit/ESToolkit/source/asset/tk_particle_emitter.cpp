#include <asset/tk_particle_emitter.h>
#include <core/tk_menu_stack.h>
#include <delegate/tk_dynamic_content_property_item_delegate.h>
#include <estk/tk_lua_wrappers.h>
#include <asset/tk_entity.h>
#include <system/elysian_system.hpp>
#include <estk/tk_particle_system.h>
#include <QApplication>

#include <proxy/elysian_entity_proxy.hpp>

bool TkParticleEmitter::validate(QString& str) const {
    bool success = true;
    auto cAssert = [&](const bool val, const char* msg) {
        if(!val) {
            success = false;
            str += msg;
            str += "\n";
        }
    };

    cAssert(_attrib, "LOG_ERROR: ParticleEmitter component has no attrib field.");
    return success;
}

QMenu* TkParticleEmitter::createContextMenu(void) {
    MenuStack menuStack("ParticleEmitter");

    menuStack.insertCheckableAction("Attributes",
        [&](bool checked) {
            setAttrib(checked? new elysian::ParticleEmitterAttrib : nullptr);
        },
        getAttrib(),
        !_entity->isActive()
    );

    menuStack.insertAction("Remove", [&](bool /*checked*/) {
            _entity->removeParticleEmitter();
        },
        canRemoveFrom(*_entity)
    );


    return menuStack.takeTopLevel();

}

void TkParticleEmitterAttrib::initializeDelegates(void) {
#if 0
    auto* typeDelegate = new ColorSwatchPropertyItemDelegate();

    OOLUA::push(*_mainState, typeDelegate);
    lua_setfield(*_mainState, -2, TK_LUA_TABLE_OUTER_DELEGATE_FIELD_KEY);

    auto* spinBox = new SpinboxPropertyItemDelegate(0, 255);
    LuaVariant fieldDelegate = {
        { "r", spinBox },
        { "g", spinBox },
        { "b", spinBox },
        { "a", spinBox }
    };
    fieldDelegate.push();
    lua_setfield(*_mainState, -2, TK_LUA_TABLE_DELEGATE_FIELD_KEY);

#endif

    DynamicContentPropertyItemDelegate* typeDelegate = new DynamicContentPropertyItemDelegate();
    OOLUA::push(*_mainState, typeDelegate);
    lua_setfield(*_mainState, -2, TK_LUA_TABLE_OUTER_DELEGATE_FIELD_KEY);

    typeDelegate->addButton(QApplication::style()->standardIcon(QStyle::SP_MediaPlay), [](ESTkStandardItem* item) {
        auto* luaItem = LuaTableFieldItem::fromStandardItem(item);
        if(luaItem) {
            auto* attrib = luaItem->getVariant().getValue<ParticleEmitterAttrib*>();
            if(attrib) {
                elysian::System::getPartSys()->emitPreview(attrib);
            }
        }
    });
    typeDelegate->addButton(QApplication::style()->standardIcon(QStyle::SP_MessageBoxInformation), [](ESTkStandardItem* item) {
        auto* luaItem = LuaTableFieldItem::fromStandardItem(item);
        if(luaItem) {
            elysian::System::getPartSys()->popDockWidget();
        }
    });
    typeDelegate->setReadOnly();

}



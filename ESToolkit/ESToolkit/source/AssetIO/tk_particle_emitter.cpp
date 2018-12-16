#include <ESToolkit/AssetIO/tk_particle_emitter.h>
#include <QESTkUtils/tk_menu_stack.h>
#include <QESTkUtils/ui/delegate/tk_dynamic_content_property_item_delegate.h>
#include <ESToolkit/ESGamma/tk_lua_wrappers.h>
#include <ESToolkit/AssetIO/tk_entity.h>
#include <ESGamma/system/elysian_system.hpp>
#include <ESToolkit/ESGamma/tk_particle_system.h>
#include <QApplication>
#include <AssetIO/proxy/elysian_entity_proxy.hpp>

using namespace QESTkUtils;

bool TkParticleEmitter::validate(QString& str) const {
    bool success = true;
    auto cAssert = [&](const bool val, const char* msg) {
        if(!val) {
            success = false;
            str += msg;
            str += "\n";
        }
    };

    cAssert(_attrib.get(), "LOG_ERROR: ParticleEmitter component has no attrib field.");
    return success;
}

QMenu* TkParticleEmitter::createContextMenu(void) {
    MenuStack menuStack("ParticleEmitter");

    menuStack.insertCheckableAction("Attributes",
        [&](bool checked) {
            setAttrib(checked? std::make_shared<elysian::ParticleEmitterAttrib>() : nullptr);
        },
        getAttrib().get(),
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
            auto attrib = luaItem->getVariant().getValue<std::shared_ptr<ParticleEmitterAttrib>>();
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



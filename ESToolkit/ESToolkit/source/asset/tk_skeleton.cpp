#include <asset/tk_skeleton.h>
#include <core/tk_menu_stack.h>
#include <asset/tk_entity.h>
#include <spine/AnimationState.h>
#include <model/tk_property_item_delegate.h>
#include <estk/tk_lua_wrappers.h>

void TkSkeleton::onAtlasChanged(void) {
    setAttrib(_attrib);
}

void TkSkeleton::onJsonChanged(void) {
    setAttrib(_attrib);
}

bool TkSkeleton::validate(QString &str) const {
    bool success = true;
    auto cAssert = [&](const bool val, const char* msg) {
        if(!val) {
            success = false;
            str += msg;
            str += "\n";
        }
    };

    cAssert(_attrib, "LOG_ERROR: Skeleton component has no attrib field.");
    return success;
}


QMenu* TkSkeleton::createContextMenu(void) {
    MenuStack menuStack("Skeleton");

    menuStack.insertCheckableAction("Attributes",
        [&](bool checked) {
            setAttrib(checked? new SkeletonAttrib : nullptr);
        },
        getAttrib(),
        !_entity->isActive()
    );

    menuStack.insertAction("Remove", [&](bool /*checked*/) {
            _entity->removeSkeleton();
        },
        canRemoveFrom(*_entity)
    );

    return menuStack.first();

}

void TkSkeleton::setAnimByName(const int trackIndex, const char*const animName, const bool loop) const {
    if(!spAnimationState_setAnimationByName(_animState, trackIndex, animName, loop)) {
        //qWarning() << "Skeleton::setAnimByName() - " << animName << "animation could not be found!";

    }

}

void TkSkeletonAttrib::initializeDelegates(void) {
    lua_newtable(*_mainState);
    lua_pushvalue(*_mainState, -1);
    lua_setfield(*_mainState, -3, TK_LUA_TABLE_DELEGATE_FIELD_KEY);

    auto* atlasDelegate = new FileBrowserPropertyItemDelegate;
    OOLUA::push(*_mainState, atlasDelegate);
    lua_setfield(*_mainState, -2, "atlasPath");

    auto* jsonDelegate = new FileBrowserPropertyItemDelegate;
    OOLUA::push(*_mainState, jsonDelegate);
    lua_setfield(*_mainState, -2, "jsonPath");

    lua_pop(*_mainState, 1);
}

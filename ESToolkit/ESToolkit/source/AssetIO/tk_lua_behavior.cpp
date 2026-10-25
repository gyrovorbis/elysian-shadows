/*
 * Copyright (c) 2013-2026 Elysian Shadows Team
 * SPDX-License-Identifier: MIT
 * Additional permission for Elysian Shadows Kickstarter backers: see LICENSE.
 */

#include <ESToolkit/AssetIO/tk_lua_behavior.h>
#include <QESTkUtils/tk_menu_stack.h>
#include <ESToolkit/AssetIO/tk_entity.h>

bool TkLuaBehavior::validate(QString& str) const {
    bool success = true;
    auto cAssert = [&](const bool val, const char* msg) {
        if(!val) {
            success = false;
            str += msg;
            str += "\n";
        }
    };

    cAssert(_script, "LOG_ERROR: LuaBehavior component doesn't have a valid script field.");
    return success;
}

QMenu* TkLuaBehavior::createContextMenu(void) {
#if 0
    MenuStack menuStack("LuaBehavior");

    menuStack.insertCheckableAction("Attributes",
        [&](bool checked) {
            //setAttrib(checked? new elysian::LightAttrib : nullptr);
        },
    false
        //getAttrib(),
        !_entity->isActive()
    );

    return menuStack.takeTopLevel();
#endif
    return nullptr;
}

QString TkLuaBehavior::getScriptName(void) const {
    if(!_script) return "none";

    auto tokens = QString(_script->getPath()).split("/");

    return tokens.last();

}

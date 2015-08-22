#include <asset/tk_sprite_attrib.h>
#include <estk/tk_lua_wrappers.h>


void TkSpriteAttrib::initializeDelegates(void) {
    auto* textureDelegate = new FileBrowserPropertyItemDelegate;
    OOLUA::push(*_mainState, textureDelegate);
    lua_setfield(*_mainState, -2, "texture");

    auto* normalDelegate = new FileBrowserPropertyItemDelegate;
    OOLUA::push(*_mainState, normalDelegate);
    lua_setfield(*_mainState, -2, "normalMap");

    auto* specularDelegate = new FileBrowserPropertyItemDelegate;
    OOLUA::push(*_mainState, specularDelegate);
    lua_setfield(*_mainState, -2, "specularMap");

}

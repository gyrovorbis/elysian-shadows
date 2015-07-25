#include <asset/tk_light_source.h>
#include <core/tk_menu_stack.h>
#include <asset/tk_entity.h>

bool TkLightSource::validate(QString& str) const {
    bool success = true;
    auto cAssert = [&](const bool val, const char* msg) {
        if(!val) {
            success = false;
            str += msg;
            str += "\n";
        }
    };

    cAssert(properties, "DBG_ERROR: LightSource component has no attrib field.");
    return success;
}

QMenu* TkLightSource::createContextMenu(void) {
    MenuStack menuStack("LightSource");

    menuStack.insertCheckableAction("Attributes",
        [&](bool checked) {
            setAttrib(checked? new elysian::LightAttrib : nullptr);
        },
        getAttrib(),
        !_entity->isActive()
    );

    return menuStack.first();

}

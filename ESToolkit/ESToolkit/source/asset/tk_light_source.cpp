#include <tk_light_source.h>
#include <tk_menu_stack.h>

bool TkLightSource::validate(QString& str) const {
    bool success = true;
    auto cAssert = [&](const bool val, const char* msg) {
        if(!val) {
            success = false;
            str += msg;
            str += "\n";
        }
    };

    cAssert(properties, "ERROR: LightSource component has no attrib field.");
    return success;
}

QMenu* TkLightSource::createContextMenu(void) {
    MenuStack menuStack("LightSource");

    menuStack.insertCheckableAction("Attributes",
        [&](bool checked) {
            setAttrib(checked? new elysian::LightAttrib : nullptr);
        },
        getAttrib()
    );

    return menuStack.first();

}

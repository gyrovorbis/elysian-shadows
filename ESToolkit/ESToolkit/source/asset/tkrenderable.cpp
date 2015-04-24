#include <tkRenderable.h>

bool TkRenderable::validate(QString &str) const {

    bool success = true;
    auto cAssert = [&](const bool val, const char* msg) {
        if(!val) {
            success = false;
            str += msg;
            str += "\n";
        }
    };

    cAssert(_spriteAttrib, "ERROR: Renderable component has no spriteAttrib.");

    return success;
}

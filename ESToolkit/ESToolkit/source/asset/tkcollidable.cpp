#include <tkCollidable.h>


bool TkCollidable::validate(QString &str) const {
    bool success = true;
    auto cAssert = [&](const bool val, const char* msg) {
        if(!val) {
            success = false;
            str += msg;
            str += "\n";
        }
    };

    cAssert(_attrib, "ERROR: Collidable component has no attrib field.");

    return success;

}

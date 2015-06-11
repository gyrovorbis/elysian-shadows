#include <tk_particle_emitter.h>
#include <tk_menu_stack.h>

bool TkParticleEmitter::validate(QString& str) const {
    bool success = true;
    auto cAssert = [&](const bool val, const char* msg) {
        if(!val) {
            success = false;
            str += msg;
            str += "\n";
        }
    };

    cAssert(_attrib, "ERROR: LightSource component has no attrib field.");
    return success;
}

QMenu* TkParticleEmitter::createContextMenu(void) {
    MenuStack menuStack("ParticleEmitter");

    menuStack.insertCheckableAction("Attributes",
        [&](bool checked) {
            setAttrib(checked? new elysian::ParticleEmitterAttrib : nullptr);
        },
        getAttrib()
    );

    return menuStack.first();

}

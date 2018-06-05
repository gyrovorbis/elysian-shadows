#include <ESGamma/tk_render_system.h>

void TkRenderSystem::_setParticleAccelerationAvailable(bool val) {
    _setParticleAccelerationAvailable(val);
    Q_EMIT particleAccelerationAvailableChanged(val);
}

void TkRenderSystem::setUseParticleAcceleration(bool val) {
    setUseParticleAcceleration(val);
    Q_EMIT useParticleAccelerationChanged(getUseParticleAcceleration());

}

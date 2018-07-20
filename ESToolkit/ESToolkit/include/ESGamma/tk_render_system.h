#ifndef TK_RENDER_SYSTEM_H
#define TK_RENDER_SYSTEM_H

#include <QObject>
#include <system/elysian_render_system_base.hpp>

class TkRenderSystem: public QObject, public elysian::RenderSystemBase {
    Q_OBJECT

protected:
    //void _setParticleAccelerationAvailable(bool val);
public:

    //void setUseParticleAcceleration(bool val);

Q_SIGNALS:

    //void useParticleAccelerationChanged(bool val);
    //void particleAccelerationAvailableChanged(bool val);



};

#endif // TK_RENDER_SYSTEM_H

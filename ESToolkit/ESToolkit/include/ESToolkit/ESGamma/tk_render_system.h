/*
 * Copyright (c) 2013-2026 Elysian Shadows Team
 * SPDX-License-Identifier: MIT
 * Additional permission for Elysian Shadows Kickstarter backers: see LICENSE.
 */

#ifndef TK_RENDER_SYSTEM_H
#define TK_RENDER_SYSTEM_H

#include <QObject>
#include <ESGamma/system/elysian_render_system_base.hpp>

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

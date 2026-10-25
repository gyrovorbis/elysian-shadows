/*
 * Copyright (c) 2013-2026 Elysian Shadows Team
 * SPDX-License-Identifier: MIT
 * Additional permission for Elysian Shadows Kickstarter backers: see LICENSE.
 */

#ifndef TK_INPUT_SYSTEM_H
#define TK_INPUT_SYSTEM_H

#include <ESGamma/system/elysian_input_system_base.hpp>
#include <QObject>

class TkInputSystem: public QObject, public elysian::InputSystemBase {
Q_OBJECT
public:

    void _onControllerAttached(int slot);
    void _onControllerRemoved(int slot);
    void _onVMUAttached(int slot);
    void _onVMURemoved(int slot);

Q_SIGNALS:

    void controllerAdded(const unsigned port, const char*const name);
    void controllerRemoved(const unsigned port);

};



#endif // TK_INPUT_SYSTEM_H


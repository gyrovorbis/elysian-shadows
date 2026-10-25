/*
 * Copyright (c) 2013-2026 Elysian Shadows Team
 * SPDX-License-Identifier: MIT
 * Additional permission for Elysian Shadows Kickstarter backers: see LICENSE.
 */

#include <ESToolkit/ESGamma/tk_input_system.h>
#include <ESToolkit/ui/tk_estoolkit.h>

void TkInputSystem::_onControllerAttached(int slot) {
    elysian::InputSystemBase::_onControllerAttached(slot);
    ESToolkit::showStatusBarMessage(QString("Controller Detected: [Slot %1]").arg(slot));
}

void TkInputSystem::_onControllerRemoved(int slot) {
    elysian::InputSystemBase::_onControllerRemoved(slot);
    ESToolkit::showStatusBarMessage(QString("Controller Removed: [Slot %1]").arg(slot));
}

void TkInputSystem::_onVMUAttached(int slot) {
    elysian::InputSystemBase::_onVMUAttached(slot);
    ESToolkit::showStatusBarMessage(QString("VMU Detected: [Slot %1]").arg(slot));
}

void TkInputSystem::_onVMURemoved(int slot) {
    elysian::InputSystemBase::_onVMURemoved(slot);
    ESToolkit::showStatusBarMessage(QString("Controller Removed: [Slot %1]").arg(slot));
}

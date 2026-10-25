/*
 * Copyright (c) 2013-2026 Elysian Shadows Team
 * SPDX-License-Identifier: MIT
 * Additional permission for Elysian Shadows Kickstarter backers: see LICENSE.
 */

#ifndef ELYSIAN_DIALOG_HPP
#define ELYSIAN_DIALOG_HPP

#ifdef ELYSIAN_ENGINE
#   include <level/elysian_dialog_base.hpp>
namespace elysian {
    using Dialog        = DialogBase;
    using DialogText    = DialogTextBase;
    using DialogOption  = DialogOptionBase;
    using DialogPrompt  = DialogPromptBase;
}
#else
#   include <ESToolkit/AssetIO/tk_dialog.h>
namespace elysian {
    using Dialog        = TkDialog;
    using DialogText    = TkDialogText;
    using DialogOption  = TkDialogOption;
    using DialogPrompt  = TkDialogPrompt;
}
using Dialog        = TkDialog;
using DialogText    = TkDialogText;
using DialogOption  = TkDialogOption;
using DialogPrompt  = TkDialogPrompt;

#endif

#endif // ELYSIAN_DIALOG_HPP


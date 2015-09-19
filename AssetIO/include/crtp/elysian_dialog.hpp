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
#   include <tk_dialog.h>
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


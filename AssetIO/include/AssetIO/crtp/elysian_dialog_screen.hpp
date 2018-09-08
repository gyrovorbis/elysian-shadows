#ifndef ELYSIAN_DIALOG_SCREEN_HPP
#define ELYSIAN_DIALOG_SCREEN_HPP


#ifdef ELYSIAN_ENGINE
#   include <level/elysian_dialog_screen_base.hpp>
namespace elysian {
    using DialogScreen  = DialogScreenBase;
}
#else
#   include <ESToolkit/AssetIO/tk_dialog_screen.h>
namespace elysian {
    using DialogScreen  = TkDialogScreen;
}
using DialogScreen      = TkDialogScreen;

#endif

#endif // ELYSIAN_DIALOG_SCREEN_HPP


#ifndef TK_DIALOG_SCREEN
#define TK_DIALOG_SCREEN

#include <level/elysian_dialog_screen_base.hpp>

class TkDialogScreen: public elysian::DialogScreenBase {
public:
    TkDialogScreen(const SCREEN_TYPE scrType);
    TkDialogScreen(const SCREEN_TYPE scrType, const char*const text);
};







// ======= INLINEZ =======
inline TkDialogScreen::TkDialogScreen(const SCREEN_TYPE scrType): DialogScreenBase(scrType) {}
inline TkDialogScreen::TkDialogScreen(const SCREEN_TYPE scrType, const char * const text): DialogScreenBase(scrType, text) {}


#endif // TK_DIALOG_SCREEN


#ifndef TK_DIALOG_SCREEN
#define TK_DIALOG_SCREEN

#include <ESToolkit/ESGamma/tk_lua_wrappers.h>
#include <AssetIO/level/elysian_dialog_screen_base.hpp>

class TkDialog;

class TkDialogScreen: public elysian::DialogScreenBase {
public:
                    TkDialogScreen(const SCREEN_TYPE scrType);
                    TkDialogScreen(const SCREEN_TYPE scrType, const char*const text);
    virtual bool    validate(const TkDialog* dialog, QString& errorMsg) const = 0;


    static void     initializeDelegates(void);
};







// ======= INLINEZ =======
inline TkDialogScreen::TkDialogScreen(const SCREEN_TYPE scrType): DialogScreenBase(scrType) {}
inline TkDialogScreen::TkDialogScreen(const SCREEN_TYPE scrType, const char * const text): DialogScreenBase(scrType, text) {}


#endif // TK_DIALOG_SCREEN


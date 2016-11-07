#ifndef TK_DIALOG_H
#define TK_DIALOG_H

#include <level/elysian_dialog_base.hpp>


class TkDialogText: public elysian::DialogTextBase {
public:
        using       LuaTableProxy<DialogTextBase, DialogText, DialogScreen>::initializeDelegates;
        using       LuaTableProxy<DialogTextBase, DialogText, DialogScreen>::initializeVirtualTableDelegates;

                    TkDialogText(void) = default;
                    TkDialogText(const char *str);
};

class TkDialogOption: public elysian::DialogOptionBase {
public:
                    TkDialogOption(void) = default;
                    TkDialogOption(const char* str, const int nextIndex);
};

class TkDialogPrompt: public elysian::DialogPromptBase {
public:
        using       LuaTableProxy<DialogPromptBase, DialogPrompt, DialogScreen>::initializeDelegates;
        using       LuaTableProxy<DialogPromptBase, DialogPrompt, DialogScreen>::initializeVirtualTableDelegates;

                    TkDialogPrompt(void) = default;
                    TkDialogPrompt(const char* str);

};

class TkDialog: public elysian::DialogBase {
public:

};






// ====== INLINEZ
inline TkDialogText::TkDialogText(const char *str): elysian::DialogTextBase(str) {}
inline TkDialogPrompt::TkDialogPrompt(const char *str): elysian::DialogPromptBase(str) {}
inline TkDialogOption::TkDialogOption(const char *str, const int nextIndex): elysian::DialogOptionBase(str, nextIndex) {}


#endif // TK_DIALOG_H


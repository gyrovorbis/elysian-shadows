/*
 * Copyright (c) 2013-2026 Elysian Shadows Team
 * SPDX-License-Identifier: MIT
 * Additional permission for Elysian Shadows Kickstarter backers: see LICENSE.
 */

#ifndef TK_DIALOG_H
#define TK_DIALOG_H

#include <AssetIO/level/elysian_dialog_base.hpp>


class TkDialogText: public elysian::DialogTextBase {
public:
    using           TkDialogScreen::initializeVirtualTableDelegates;

                    TkDialogText(void) = default;
                    TkDialogText(const char *str);

    virtual bool    validate(const TkDialog* dialog, QString& errorMsg) const override;
    static void     initializeDelegates(void);
};

class TkDialogOption: public elysian::DialogOptionBase {
public:
                    TkDialogOption(void) = default;
                    TkDialogOption(const char* str, const int nextIndex);

        bool        validate(const TkDialog* dialog, QString& errorMsg) const;
        static void initializeDelegates(void);
};

class TkDialogPrompt: public elysian::DialogPromptBase {
public:
    using           TkDialogScreen::initializeVirtualTableDelegates;

                    TkDialogPrompt(void) = default;
                    TkDialogPrompt(const char* str);

    static void     initializeDelegates(void);
    static void     initializeVirtualTableDelegates(QString key);

    virtual bool    validate(const TkDialog* dialog, QString& errorMsg) const override;

};

class TkDialog: public elysian::DialogBase {
public:

    static void initializeDelegates(void);
    static void initializeVirtualTableDelegates(QString key);

    bool        validate(QString& errorMsg) const;

};






// ====== INLINEZ
inline TkDialogText::TkDialogText(const char *str): elysian::DialogTextBase(str) {}
inline TkDialogPrompt::TkDialogPrompt(const char *str): elysian::DialogPromptBase(str) {}
inline TkDialogOption::TkDialogOption(const char *str, const int nextIndex): elysian::DialogOptionBase(str, nextIndex) {}


#endif // TK_DIALOG_H


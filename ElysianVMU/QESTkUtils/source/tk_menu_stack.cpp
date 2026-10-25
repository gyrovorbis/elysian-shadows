/*
 * Copyright (c) 2013-2026 Elysian Shadows Team
 * SPDX-License-Identifier: MIT
 * Additional permission for Elysian Shadows Kickstarter backers: see LICENSE.
 */

#include <QESTkUtils/tk_menu_stack.h>
#include <QMenu>

namespace QESTkUtils {

MenuStack::MenuStack(const QString name) { _stack.push_back(new QMenu(name)); }

MenuStack::MenuStack(QMenu *topLevelMenu, QString name) {
    _stack.push_back(topLevelMenu? topLevelMenu : new QMenu(name));
}

MenuStack::~MenuStack(void) {
    if(_ownsStack) {
        delete first();
    }
}

QMenu* MenuStack::push(const QString name) {
    auto* menu = _stack.size()? current()->addMenu(name) : new QMenu(name);
    _stack.push_back(menu);
    return menu;
}

QAction* MenuStack::insertAction(const QString text, const std::function<void(bool)>& callback, const bool enabled) {
    auto* action = new QAction(text, nullptr);
    //motherfucking C++11 lambda function Qt SLAWTS!!!
    QObject::connect(action, &QAction::triggered, callback);
    action->setEnabled(enabled);
    _stack.top()->addAction(action);
    return action;
}

QAction* MenuStack::insertCheckableAction(const QString text, const std::function<void(bool)>& callback, const bool checked, const bool enabled) {
    QAction* action = insertAction(text, callback, enabled);
    action->setCheckable(true);
    action->blockSignals(true);
    action->setChecked(checked);
    action->blockSignals(false);
    return action;
}

QAction* MenuStack::insertSeparator(void) {
    return _stack.top()->addSeparator();
}

void MenuStack::execDeleteLater(const QPoint &pos, QAction *at) {
    auto* topLevel = takeTopLevel();
    QObject::connect(topLevel, &QMenu::triggered, topLevel, &QMenu::deleteLater);
    topLevel->exec(pos, at);
}

}

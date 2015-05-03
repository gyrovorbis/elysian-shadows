#ifndef TK_MENU_STACK_H
#define TK_MENU_STACK_H

#include <QStack>
#include <functional>

class QMenu;
class QAction;

struct MenuStack {
    QStack<QMenu*> stack;

                MenuStack(void) = default;
                MenuStack(const QString name);
    QAction*    insertAction(const QString text, const std::function<void(bool)>& callback, const bool enabled=true);
    QAction*    insertCheckableAction(const QString text, const std::function<void(bool)>& callback, const bool checked=false, const bool enabled=true);

    QMenu*      push(const QString name);
    void        pop(void);

    QMenu*      current(void) const;
    QMenu*      first(void) const;

};







// ========= INLINEZ =======

inline QMenu* MenuStack::current(void) const { return stack.top(); }
inline QMenu* MenuStack::first(void) const { return stack.first(); }
inline void MenuStack::pop(void) { stack.pop(); }



#endif // TK_MENU_STACK_H


#ifndef TK_MENU_STACK_H
#define TK_MENU_STACK_H

#include <QStack>
#include <functional>

class QMenu;
class QAction;
class QPoint;

/* Once you've created the stack, calling takeTopLevel() relinquishes ownership of the menus on the stack.
 * If you do not call takeTopLevel() and assume ownership, the QMenus will be deleted with the MenuStack object.
 */
class MenuStack {
private:
    QStack<QMenu*>  _stack;
    bool            _ownsStack = true;

public:
                MenuStack(void) = default;
                MenuStack(const QString name);
                MenuStack(QMenu* topLevelMenu, QString name="");
                ~MenuStack(void);

    QAction*    insertAction(const QString text, const std::function<void(bool)>& callback, const bool enabled=true);
    QAction*    insertCheckableAction(const QString text, const std::function<void(bool)>& callback, const bool checked=false, const bool enabled=true);

    QMenu*      push(const QString name);
    void        pop(const unsigned count=1);

    QMenu*      current(void) const;
    QMenu*      first(void) const;
    QMenu*      takeTopLevel(void);

    void        execDeleteLater(const QPoint& pos, QAction *at = nullptr);

};







// ========= INLINEZ =======

inline QMenu* MenuStack::current(void) const { return _stack.top(); }
inline QMenu* MenuStack::takeTopLevel(void) { _ownsStack = false; return _stack.first(); }
inline QMenu* MenuStack::first(void) const { return _stack.first(); }
inline void MenuStack::pop(const unsigned count) { for(unsigned i = 0; i < count; ++i) _stack.pop(); }



#endif // TK_MENU_STACK_H


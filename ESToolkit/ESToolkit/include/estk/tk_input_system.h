#ifndef TK_INPUT_SYSTEM_H
#define TK_INPUT_SYSTEM_H

#include <system/elysian_input_system_base.hpp>
#include <QObject>

class TkInputSystem: public QObject, public elysian::InputSystemBase {
Q_OBJECT

Q_SIGNALS:

    void controllerAdded(const unsigned port, const char*const name);
    void controllerRemoved(const unsigned port);

};



#endif // TK_INPUT_SYSTEM_H


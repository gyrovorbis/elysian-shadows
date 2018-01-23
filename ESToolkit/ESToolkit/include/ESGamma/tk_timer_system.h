#ifndef TK_TIMER_SYSTEM_H
#define TK_TIMER_SYSTEM_H

#include <elysian_timer_system_base.hpp>
#include <QESTkUtils/model/tk_property_stack_model.h>

class TkTimerSystem:
    public PropertyStackModel,
    public elysian::TimerSystemBase
{
public:

    virtual void populateModel(void) override {}

    void    pushTaskMonitor(TaskMonitor*const task);
    void    popTaskMonitor(TaskMonitor*const task);

};





#endif


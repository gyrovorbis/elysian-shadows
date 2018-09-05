#include <ESToolkit/ESGamma/tk_timer_system.h>
#include <ESToolkit/ESGamma/tk_task_monitor.h>

void TkTimerSystem::pushTaskMonitor(TaskMonitor * const task) {
    return;
    //bool first = !_topLevelTask;

	if(elysian::TimerSystemBase::pushTaskMonitor(task)) {
		pushLabel(task->getName(), task);
	}

}

void TkTimerSystem::popTaskMonitor(TaskMonitor * const task) {
    return;
	elysian::TimerSystemBase::popTaskMonitor(task);
	if(task != _topLevelTask) popLabel();
}

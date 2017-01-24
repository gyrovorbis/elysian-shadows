#include "elysian_event_monitor.hpp"
#include <proxy/elysian_event_monitor_proxy.hpp>

namespace elysian {

EventMonitor EventMonitor::_nullMonitor;

void LuaEventCallback::onFinish(void) {
    EventMonitor::onFinish();
	_callback.push(*_mainState);
	_table.push(*_mainState);
	_protectedCall(1, 0, nullptr);
}

}
